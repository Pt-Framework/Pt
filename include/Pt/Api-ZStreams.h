/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_API_ZSTREAMS_H
#define PT_API_ZSTREAMS_H

/** @addtogroup Pt-ZStreams

    @brief zlib compression through stream buffers and iostreams.

    These types put zlib in the stream model of @ref Pt-Streams. A
    %ZBuffer is a %BasicStreamBuffer of @c char attached to a target
    @c std::ios. Reading the buffer inflates compressed bytes from
    the target. Writing the buffer deflates uncompressed bytes into
    the target. The stream wrappers own a %ZBuffer and present it as
    an input stream, an output stream, or both.

    %ZBuffer::Format selects the on-wire layout. %ZBuffer::Zlib is
    the zlib wrapper with an Adler-32 checksum. %ZBuffer::Gzip is
    the gzip wrapper with a CRC-32 checksum, as in RFC 1952. The
    format is fixed at construction. It must match the data on the
    target; a gzip file attached as zlib, or the reverse, fails
    during inflate.

    %attach() sets the target stream. %detach() drops it without
    finishing a compressed frame. %reset() discards buffered data
    and returns the zlib state to the start of a new stream, and
    can take a new target at the same time. %discard() throws away
    the buffer contents and resets the state while keeping the
    target. %finish() flushes the remaining compressed bytes to the
    target and ends the zlib stream. After %finish(), start a new
    stream with %reset() before writing more.

    %zcount() is the total number of uncompressed bytes produced by
    inflate so far. It does not count compressed bytes written on
    the deflate path.

    %ZIStream, %ZOStream and %ZIOStream construct a %ZBuffer, install
    it with %setBuffer(), and forward %attach(), %detach(), %reset()
    and %finish() to it. %ZIStream reads uncompressed bytes from a
    compressed @c std::istream. %ZOStream writes uncompressed bytes
    to a compressed @c std::ostream. %ZIOStream does both on a
    @c std::iostream. %zBuffer() returns the owned buffer when a
    caller needs the buffer API directly.

    @code
    std::ifstream file("data.gz", std::ios::binary);
    Pt::ZIStream in(file, Pt::ZBuffer::Gzip);
    std::string text;
    in >> text;
    @endcode

    The stream wrappers do not own the target. Keep the target alive
    until %detach() or destruction. Call %finish() on an output
    stream before relying on the target to contain a complete zlib
    or gzip frame. A format mismatch or a truncated frame surfaces
    as a stream error, typically a %IOError.
*/

#endif
