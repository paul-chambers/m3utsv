# m3utsv

This tool provides a simple way to update a file of [tab-separated values](https://en.wikipedia.org/wiki/Tab-separated_values)
(`tsv`) from an M3U file, and then generate another M3U file from that `tsv` file.

## Why do I need this?

Some sources of M3U files contain information that is updated periodically. Usually not often,
but often enough that it's a nuisance if you're maintaining your own version of that file
manually.

A good example of this is the files provided at https://iptv-org.github.io/. This site
provides M3U files for free (and legal) streams from around the world. The downside is 
that the metadata it contains is generally sparse, and so it's not the greatest
experience when using them with the Channels DVR.

This tool makes that a little easier. It imports an M3U file and uses it to update an
existing `tsv` file with new or updated information - while leaving any information
you've previously added to the `tsv` file untouched. It merges in any new or updated 
information from this latest M3U file while preserving any tags you've added manually
that are not present in the source M3U file.

This means you can edit the `tsv` file to add any of the enhanced tags that Custom
Channels supports, and they will be preserved across updates. Only any new channels
added will need attention.

Documentation for the additional tags supported by the Custom Channels feature of
Channels DVR can be found here:

* https://getchannels.com/docs/channels-dvr-server/how-to/custom-channels/

A 'standard' M3U file will contain these tags (and perhaps others). Note that because
there's no formal specification for this type of M3U file, there's much inconsistency,
and it's unclear what tags are required, and which are optional.
* **tvg-name**: channel callsign
* **tvg-logo**: channel logo url (4x3 aspect ratio)
* **tvg-id**: identifier for finding electronic program guide (EPG) listings for this channel.
  Often missing or empty.
* **group-title**: groupings of channels that are related in some way (e.g. 'News' or 'USA')

In general, tags provided by the M3U file will be copied verbatim into the `tsv` file, with
the exception that an empty tag (e.g. tvg-id="", a common occurrence) will not replace one
that has been given a value already.

As of writing, there are several additional tags defined for Custom Channels:
* **channel-id**: unique ID for the channel (required)
* **channel-number**: display number for the channel
* **tvc-guide-stationid**: Gracenote station id (See 
  [extractChannelIDs](https://github.com/Channels-DVR-Goodies/extractChannelIDs)
  for one method of discovering these)

Plus there are additional tags for streams where there's little to no guide data available
directly from Gracenote. See the link above for more information.

## How does it do that?

This tool is a cousin of the [m3upipe](https://github.com/Channels-DVR-Goodies/m3upipe) tool, and the import side works in much the same way.
See https://github.com/Channels-DVR-Goodies/m3upipe for more information.

It can accept the output of [m3upipe](https://github.com/Channels-DVR-Goodies/m3upipe) directly, so you can use it to trim the list of
channels in your `tsv` file down to the ones you care about.

After importing the updated M3U file and merging it with the existing `tsv` file, it then
outputs the updated M3U file.

You can think of the `tsv` file as a poor man's database, one you can easily edit with a
text editor or spreadsheet.

## How do I use it?

_todo - I have to write it first :)_

## References

* https://iptv-org.github.io/
* https://getchannels.com/docs/channels-dvr-server/how-to/custom-channels/
* https://github.com/Channels-DVR-Goodies/m3upipe
* https://github.com/Channels-DVR-Goodies/extractChannelIDs
* https://en.wikipedia.org/wiki/M3U
