/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_SYSTEM_TAR_READER_H
#define PT_SYSTEM_TAR_READER_H

#include <Pt/System/Api.h>
#include <Pt/System/TarEntry.h>

#include <istream>
#include <ios>
#include <cstddef>

namespace Pt {

namespace System {

/** @brief Reads a tar archive from a stream, one chunk at a time.

    A %TarReader walks a Pax/UStar archive held by a std::istream. Each
    call to %advance() does a bounded amount of work and then returns,
    so the same reader serves a file that is already complete and a
    socket that delivers the archive in pieces. The caller asks for the
    next step, processes what came back, and asks again.

    %advance() returns a %TarEntry once a member header has been parsed,
    and returns the same entry again for each following content chunk.
    The pointer is null when the stream has no more bytes yet. That is
    a pause, not the end of the archive. The caller waits for more input
    and calls %advance() again with the same reader. %isEnd() becomes
    true only after the two zero blocks that mark the end of the archive
    have been read. A null return together with %isEnd() means there is
    nothing further to read.

    The entry stays valid until the next %advance() call. Its metadata
    is complete on the first return for that member: path, type, size,
    link target, permissions, modification time, owner id and name, and
    group id and name. A Pax extended header that preceded the member is
    already applied, so the caller never sees the Pax header as an entry
    of its own. A global Pax header is skipped. A member type the reader
    does not recognise is skipped as well, including its content, and
    the next recognised member is what comes back.

    Content arrives through the entry's window. %TarEntry::data() points
    at %TarEntry::avail() bytes inside the reader's buffer. Copy those
    bytes out before the next %advance(), which reuses the buffer. For a
    file, repeat until %TarEntry::isEnd() is true, then call %advance()
    once more to move to the following member. A directory and a link
    have no content, so %isEnd() is already true on the first return.

    The @a importSize argument bounds how much %advance() may pull from
    the stream. Zero means: use only the bytes the stream buffer already
    holds, and do not block. That is the right call from an event loop,
    after the loop has been told that the stream is readable. A positive
    value allows %advance() to read up to that many additional bytes,
    which may block, and suits a file stream whose data is known to be
    available.

    A header whose checksum does not match throws %Pt::IOError. The
    archive is then unusable from the point of the bad header, because
    the reader can no longer trust the block boundaries.

    @code
    TarReader reader(stream);

    while( ! reader.isEnd() )
    {
        const TarEntry* entry = reader.advance();
        if( ! entry )
            break; // stream starved; call advance() again when more arrives

        if(entry->type() == TarEntry::File)
        {
            do
            {
                out.write(entry->data(), entry->avail());
                if(entry->isEnd())
                    break;
                entry = reader.advance();
            }
            while(entry);
        }
    }
    @endcode

    @ingroup Pt-System-Tar
*/
class PT_SYSTEM_API TarReader
{
  public:
    using Entry = TarEntry;

  public:
    /** @brief Creates a reader with no stream attached.
    */
    TarReader();

    /** @brief Creates a reader that parses @a is.
    */
    explicit TarReader(std::istream& is);

    /** @brief Destructor.
    */
    ~TarReader();

    /** @brief Attaches @a is as the source.

        Parsing continues with the next header. Call %reset() instead
        when the new stream is the start of an archive.
    */
    void attach(std::istream& is);

    /** @brief Detaches the current source.

        The parse state is kept. %advance() returns null until a stream
        is attached again.
    */
    void detach();

    /** @brief Clears the parse state and detaches the source.
    */
    void reset();

    /** @brief Clears the parse state and attaches @a is.

        The next %advance() call starts at the first header of @a is.
    */
    void reset(std::istream& is);

    /** @brief Parses the next header or delivers the next content chunk.

        Consumes the chunk previously exposed through %TarEntry::data()
        and then reads what it needs from the stream. Copy those bytes
        out before calling %advance() again.

        A return of null with %isEnd() false means the stream buffer
        ran out. Call %advance() again when more bytes are available.
        A return of null with %isEnd() true means the end-of-archive
        marker has been read.

        @param importSize Maximum number of additional bytes to pull
                          from the stream. Zero uses only bytes already
                          buffered and does not block.
        @return The current entry, or null when more input is required
                or the archive has ended.
        @throw Pt::IOError if a header checksum does not match.
    */
    const TarEntry* advance(std::streamsize importSize = 0);

    /** @brief Returns true after the end-of-archive marker has been read.
    */
    bool isEnd() const;

  private:
    TarReader(const TarReader&);

    TarReader& operator=(const TarReader&);

  private:
    class TarReaderImpl* _impl;
};

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_TAR_READER_H
