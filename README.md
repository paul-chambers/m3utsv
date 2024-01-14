# m3upipe

Provides a simple translation between the funky 'M3U Plus' format and a single-line-per-stream
format, which is much easier to manipulate with text processing tools like `sed` and `awk`

## Why do I need this?

m3u files can be huge and unwieldy. Software like Channels DVR often puts a limit on the 
number of channels that can be contained in a single m3u file to keep things sane. In the
case of Channels DVR, this limit is 500 channels per m3u file. 

So there's a need to cut down an m3u file to just the channels you want. Linux provides
some excellent text manipulation tools, though they're generally line-oriented, whereas
the m3u file format uses two lines of text to describe each stream, making it difficult
to process them with those linux tools.

This tool converts m3u files into a single-line per stream format that makes it much
easier to process m3u files with linux text processing tools. It also provides conversion
back to the original m3u format, after the file has been processed.

## How does it do that?

It analyses the incoming stream of text, identifies the first line of the two-line-per-stream
format used by m3u, and converts the following line into another 'name="value"' parameter in
the same style as the rest of the first line. It also converts a couple of other idiosyncrasies
in the m3u format, so everything on that single line is consistent.

For example, and m3u might contain a stream like this, as an #EXTINF line with the metadata,
followed by a line with the URL for the stream itself:

```
#EXTINF:-1 tvg-id="CreaTVChannel28.us" tvg-logo="https://i.imgur.com/rvQdzL1.png" group-title="Education",CreaTV Classrooms Channel 28 (San Jose CA) (720p)
https://reflect-creatv.cablecast.tv/live-14/live/live.m3u8
```
Fed as input to `m3upipe`, it would be converted to:

```
#STREAM: m3u-duration=-1 tvg-id="CreaTVChannel28.us" tvg-logo="https://i.imgur.com/rvQdzL1.png" group-title="Education" m3u-title="CreaTV Classrooms Channel 28 (San Jose CA) (720p)" m3u-url="https://reflect-creatv.cablecast.tv/live-14/live/live.m3u8"
```
All the same information on a single line, with more consistent formatting, making it easier
to process with linux text processing tools.

Feeding this single line back through `m3upipe` will recreate the two-line-per-stream format
expected in an m3u file, with no information loss.

## How do I use it?

The trival example is:
```
m3upipe < original.m3u | m3upipe > copy.m3u
```
That will generate a second file `copy.m3u` that should be equivalent to `original.m3u`, if the
latter is formatted correctly as m3u (not always a safe assumption, malformatted m3u files are
common).

**Note:** the files may not be exactly identical, according to byte-wise comparison tools like
`diff`, but should be functionally the same.

A more useful example is:
```
m3upipe < us.m3u | sed -n -e "/group-title=\"General\"/ p" | m3upipe > us-general.m3u
```
where `us.m3u` is around 1178 streams, and `us-general.m3u` contains 113.

_(`us.m3u` was obtained from https://iptv-org.github.io/iptv/countries/us.m3u)_

## References

https://en.wikipedia.org/wiki/M3U

There isn't a true spec for the m3u format in common use today.

It started out as a simple list of URLs for music playback used by Winamp (a 'playlist' of MP3
URLs, hence the extension). It was later enhanced with duration and 'song title' metadata,
which was specified in the 'extended M3U' specification, among other information that doesn't
seem to be widely used.

Then the IPTV folks got hold of it, and made a complete dog's breakfast out of it. Rather than
adding a record to the m3u8 spec, they overloaded the meaning of an existing record (`#EXTINF`).
Since there's no formal spec for this extension of the original, the files you see 'in the wild'
are often formatted inconsistently, contain unescaped quotes within quoted fields, and numerous
other crimes. While it could be argued that these are malformed, since there's no definition of
what a correctly-formatted m3u file is...

While this tool may seem trivial in functionality, getting it to work reliably with a variety
of real-world m3u files has been a challenge.

I have a feeling there are more surprises on the way...
