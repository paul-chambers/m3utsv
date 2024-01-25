//
// Created by paul on 12/27/23.
//

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdint.h>
#include <stdbool.h>

#include <string.h>
#include <ctype.h>

#ifndef M3U2JSON_H
#define M3U2JSON_H

typedef unsigned long tHash;

typedef struct {
    struct {
        pid_t   pid;
        const char * name;
    };
} tGlobals;

extern tGlobals g;


#endif //M3U2JSON_H
