//
// Created by paul on 12/27/23.
//

#include "m3utsv.h"

tGlobals g;

enum {
    kM3uDuration  = 0xb02c12bd6f00739,
    kTvgName      = 0xb02c12bd6f00739,
    kTvgLogo      = 0xb02c12bd6f00739,
    kGroupTitle   = 0xb02c12bd6f00739,
    kM3uTitle     = 0xb02c12bd6f00739,
    kM3uUrl       = 0xb02c12bd6f00739
};

const unsigned int kMaxFields = 32;

char * removeQuotes( char * string )
{
    char * p = string;
    if ( *p == '"' ) {
        while ( *p != '\0' ) {
            p[0] = p[1];
            p++;
        }
        p -= 2;
        if ( *p == '"' ) {
            *p = '\0';
        }
    }
    return string;
}

char * wrapInQuotes( char * string )
{
    char * p = string;
    while ( *p != '\0' ) p++;
    p += 2;
    *p-- = '\0';
    *p-- = '"';
    *p-- = '!';
    while ( p >= string ) {
        p[1] = p[0];
        --p;
    }
    *string = '"';

    return string;
}

void outputStream( FILE * output, const char * string )
{
    const char * p = string;
    const char * m;
    char * name;
    char * value;

    while (*p != '\0') {
        while ( isspace( *p )) p++;
        m = p;
        while ( *p != '=' && *p != '\0' ) p++;
        name = strndup( m, p - m );
        while ( *p == '=' || isspace( *p )) p++;
        if ( *p == '\"' ) {
            p++;
            m = p;
            while ( *p != '"' && *p != '\0' ) p++;
            value = strndup( m, p - m );
            p++;
        }

        fprintf( output, " %s=\"%s\"", name, value );
    }
    fputs( "\n", output );
}

void processLine( char *lineBuffer, size_t lineSize, FILE *input, FILE *output )
{
    char eol[3];
    char * p;
    char * m;

    /* trim trailing line endings */
    p = strpbrk( lineBuffer, "\n\r" );
    if ( p != NULL ) {
        strncpy( eol, p, sizeof(eol));
        *p = '\0';
    }

    struct {
        tHash  hash;
        char * key;
        char * value;
    } kvtable[ kMaxFields ];

    unsigned int field = 0;

    enum { no, forward, reverse} mangle = no;

    if (memcmp( lineBuffer, "#EXTINF:", 8 ) == 0 ) {
        p = &lineBuffer[8];

        mangle = forward;

        m = strpbrk( p, " ,");
        if (m != NULL) {
            kvtable[ field ].key  = "m3u-duration";
            kvtable[ field ].hash = kM3uDuration;
            *m++ = '\0';
            kvtable[ field ].value = p;
            field++;
            p = m;
        }
    } else if (memcmp( lineBuffer, "#STREAM: ", 9 ) == 0 ) {
        p = &lineBuffer[9];
        mangle = reverse;
    }

    if ( mangle == no ) {
        fputs( lineBuffer, output );
        fputs( eol, output );
    } else {
        /* parse the key-value pairs into kvtable[] */
        char * lastSpace = NULL;
        while ( *p != '\0' && field < kMaxFields ) {
            /* be selective about what is acceptable as a key, otherwise
             * things like parameters in URLs can be mistaken for keys */
            kvtable[ field ].key = p;
            int keyLen = 0;
            tHash hash = 0xDeadBeef;
            while ( isalnum( *p ) || *p == '-' ) {
                hash = (hash * 43) ^ *p++;
                keyLen++;
            }
            /* it's only a key if it's terminated by an equals sign.
             * Note: multiple 'false starts' are expected if the value contains spaces */
            if ( *p == '=' && keyLen > 3 ) {
                if ( lastSpace != NULL ) {
                    *lastSpace = '\0';      /* terminate the previous value */
                }
                *p++ = '\0';                /* terminate this key */
                kvtable[ field ].hash  = hash;
                kvtable[ field ].value = p;       /* value starts after the equals sign
                                             * note: string has not been terminated yet */
                field++;
            }
            /* Scan forwards to find the next space. Which may
             * or may not be the one preceding the next key */
            while ( *p != ' ' && *p != '\0' ) p++;
            if ( *p == ' ' ) {
                lastSpace = p++;
            }
        }

        /* reached the end of the line */
        if ( mangle == forward )
        {
            /* the last key/value pair includes the ',<title>' suffix, so break that out */
            if ( field > 1 && field < kMaxFields ) {
                m = strstr( kvtable[ field - 1 ].value,"\"," );
                if ( m != NULL ) {
                    ++m;
                    *m++ = '\0';
                    kvtable[ field ].key   = "m3u-title";
                    kvtable[ field ].hash  = kM3uTitle;
                    kvtable[ field ].value = m;
                    field++;
                }
            }

            /* consume the next line as a 'url' parameter, with the same
             * formatting style as the parameters on the #EXTINF line. */
            while ( *p != '\0' ) p++;
            p++;
            fgets( p, (int)(lineSize - (p - lineBuffer) - 2), input );
            /* trim off the line endings */
            m = strpbrk( p, "\n\r" );
            if ( m != NULL) {
                strncpy( eol, m, sizeof(eol));
                *m = '\0';
            }

            if ( field < kMaxFields ) {
                kvtable[ field ].key   = "m3u-url";
                kvtable[ field ].hash  = kM3uUrl;
                kvtable[ field ].value = p;
                field++;
            }
        }

        for ( unsigned int j = 0; j < field; j++ ) {
            if ( kvtable[j].key != NULL ) {
                removeQuotes(kvtable[j].value);
                fprintf( output, " %s (0x%lx) = \'%s\'",
                         kvtable[j].key, kvtable[ field ].hash, kvtable[j].value );
            }
        }
        fputs( eol, output );
    }
}

int processM3U( FILE * input, FILE * output )
{
    char lineBuffer[65535];

    /* process a line at a time */
    while ( fgets( lineBuffer, sizeof(lineBuffer), input ) != NULL )
        processLine( lineBuffer, sizeof(lineBuffer), input, output );
}

int main( int argc, const char * argv[] )
{
#if 0
    if ( argc != 2 ) {
        fprintf( stderr,"%s requires a file to process\n", argv[0] );
        exit( -1 );
    }

    if ( access( argv[1], R_OK ) != 0 ) {
        fprintf( stderr, "%s: unable to read %s\n", argv[0], argv[1] );
        exit( -2 );
    }

    int fd = open( argv[1], O_RDONLY );
    if ( fd < 0 ) {
        fprintf( stderr, "%s: unable to open %s\n", argv[0], argv[1] );
        exit( -3 );
    }

    struct stat fileInfo;
    fstat( fd, &fileInfo );
    size_t fileLength = fileInfo.st_size;

    const char * fileBuffer = (const char *)mmap( NULL, fileLength,
                                                  PROT_READ, MAP_SHARED, fd, 0 );

    if ( strncmp( fileBuffer, "#EXTM3U", 7 ) == 0 ) {
        fprintf( stderr, "it's an M3U\n" );
        processM3U( fileBuffer, fileLength );
    } else {
        cJSON * root = cJSON_ParseWithLength( fileBuffer, fileLength );
        if ( root != NULL ) {
            fprintf( stderr, "parsed as JSON\n" );

            char * dump = cJSON_Print( root );
            fprintf( stderr, "dump: %s\n", dump );

        } else {
            fprintf( stderr, "%s: %s doesn't appear to be a valid JSON file\n", argv[0], argv[1] );
        }
    }
    fprintf( stdout, "%s\n", fileBuffer );
#endif

    processM3U( stdin, stdout );

    return 0;
}