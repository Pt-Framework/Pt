/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_API_STREAMS_H
#define PT_API_STREAMS_H

/** @addtogroup Pt-Streams

    @brief Stream buffers and iostreams with peek and partial write.

    These types are the framework's iostream layer. They sit on the
    standard stream hierarchy and add the operations that a buffer
    which knows its get and put areas can offer: peek several
    characters without consuming them, and write only as many
    characters as still fit.

    %BasicStreamBuffer is a @c std::basic_streambuf. Derived buffers
    implement underflow, overflow, and the rest of the streambuf
    protocol as usual. On top of that, %speekn() copies up to a
    requested number of characters from the get area, calling
    underflow once if the get area is empty, and does not advance the
    get pointer. %out_avail() is the number of characters waiting in
    the put area. A derived buffer that is unbuffered overrides
    %showfull() so %out_avail() can still report space.

    %BasicIStream, %BasicOStream and %BasicIOStream are
    @c std::basic_istream, @c std::basic_ostream and
    @c std::basic_iostream that hold a %BasicStreamBuffer pointer.
    %buffer() and %setBuffer() get and replace that pointer and keep
    @c rdbuf() in sync. %peeksome() forwards to %speekn() when the
    stream's rdbuf is that buffer, so a caller can look ahead by more
    than one character. %writesome() forwards to @c sputn() for as
    many characters as %out_avail() allows, and writes nothing when
    the put area is empty. That is the difference from @c write(): a
    partial write that does not block on a flush.

    The character type is a template argument, with
    @c std::char_traits as the default traits. Byte streams use
    @c char. Text streams in this module use %Pt::Char. A buffer and
    a stream that work together must use the same character type.

    Construct a stream with a buffer, or with a null buffer and call
    %setBuffer() later. The stream does not own the buffer. Destroy
    the buffer only after the stream has been destroyed or given
    another buffer. Replacing the buffer while formatted operations
    are in progress leaves the stream's locale and error state in
    place and redirects subsequent extraction or insertion.

    @code
    Pt::BasicStreamBuffer<char>* buf = ...;
    Pt::BasicIStream<char> in(buf);
    char ahead[16];
    std::streamsize n = in.peeksome(ahead, 16);
    @endcode

    @ref Pt-ZStreams uses this model for zlib compression. I/O that
    fails at this layer is a %IOError.
*/

#endif
