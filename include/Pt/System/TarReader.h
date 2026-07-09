/* Copyright (C) 2008 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
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

/** @brief Incremental reader for tar archives (Pax/UStar format).

    %TarReader parses a tar archive from a std::istream one entry at a time.
    It is designed for non-blocking use: advance() delivers only the bytes
    already in the stream buffer and returns nullptr when the stream is
    starved.

    Call advance() in a loop.  A non-null return value holds a %TarEntry
    with the current entry's metadata and the first content chunk.  Read
    %TarEntry::data() for %TarEntry::avail() bytes, then call advance()
    again to fetch the next chunk.  Repeat until %TarEntry::isEnd() is
    true, then call advance() once more to move to the next archive entry.
    The loop ends when isEnd() on the reader itself returns true.

    Pass a non-zero @a importSize to advance() to read more bytes from the
    stream per call — this may block and is suitable for file-based use.

    Pax extended headers handle long paths (> 99 characters), UTF-8 paths,
    and extended modification times automatically.

    @code
    TarReader reader(stream);

    while( ! reader.isEnd() )
    {
        const TarEntry* entry = reader.advance();
        if( ! entry )
            break; // not enough data, call advance() again when more arrives

        if(entry->type() == TarEntry::File)
        {
            do
            {
                outFile.write(entry->data(), entry->avail());
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
    /** @brief Default constructor.
    */
    TarReader();

    /** @brief Constructor attaching to @a is.
    */
    explicit TarReader(std::istream& is);

    /** @brief Destructor.
    */
    ~TarReader();

    /** @brief Attach to an input stream.
    */
    void attach(std::istream& is);

    /** @brief Detach from the current input stream.
    */
    void detach();

    /** @brief Reset state and detach from the input stream.
    */
    void reset();

    /** @brief Reset state and attach to a new input stream.
    */
    void reset(std::istream& is);

    /** @brief Advance to the next entry or deliver the next content chunk.

        Each call consumes the bytes previously exposed via %TarEntry::data()
        and fetches the next data from the stream.  Process %TarEntry::data()
        before calling advance() again — the buffer is reused on each call.

        When @a importSize is 0 (default), only bytes already in the stream
        buffer are used and no blocking I/O is performed, making this safe
        for event-loop use.  A value greater than 0 allows reading up to
        that many additional bytes from the stream, which may block.

        @param importSize Maximum bytes to read from the stream; 0 is non-blocking.

        @return Pointer to the current %TarEntry once a header has been parsed,
                or nullptr when the stream is starved and more data is needed.
    */
    const TarEntry* advance(std::streamsize importSize = 0);

    /** @brief Returns true when the end-of-archive marker has been read.
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

#endif // include guard
