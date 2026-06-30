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

/** @brief Incremental reader for tar archives (Pax/UStar format).

    TarReader parses a tar archive from a std::istream incrementally.  It
    is designed for use with non-blocking streams: advance() consumes only
    the bytes that are currently available in the stream buffer and returns
    nullptr when more data is needed.

    ### Reading a complete archive

    @code
    TarReader reader(stream);

    while( ! reader.atEnd() )
    {
        const TarEntry* entry = reader.advance();
        if( ! entry )
            break; // starved, call advance() again when more data arrives

        if(entry->type == Pt::System::FileInfo::File)
        {
            char buf[4096];
            std::size_t n = 0;
            while( (n = reader.read(buf, sizeof(buf))) > 0 )
                outFile.write(buf, n);

            if(entry->remaining == 0)
                onEntryComplete();  // all bytes consumed
            else
                waitForMoreData();  // call advance()/read() later
        }
    }
    @endcode

    ### Non-blocking event-loop usage

    Call advance() each time new data arrives.  The @a avail member shows
    how many bytes can be read immediately.  @a remaining tracks how many
    bytes of the current entry have not yet been delivered.

    ### Pax extended headers

    Long paths (> 100 characters), UTF-8 paths and extended modification
    times are handled via Pax extended headers.  Hard links are not
    supported and throw Pt::IOError.
*/
class PT_SYSTEM_API TarReader
{
  public:
    using Entry = TarEntry;

  public:
    TarReader();

    explicit TarReader(std::istream& is);

    ~TarReader();

    /** @brief Attach to an input stream. */
    void attach(std::istream& is);

    /** @brief Detach from the current input stream. */
    void detach();

    /** @brief Reset state and detach from the input stream. */
    void reset();

    /** @brief Reset state and attach to a new input stream. */
    void reset(std::istream& is);

    /** @brief Advance the parser.

        Consumes bytes from the stream, parses headers and updates the
        current TarEntry.

        On each call while in data state, the bytes previously exposed via
        TarEntry::data (TarEntry::avail bytes) are implicitly consumed before
        new data is fetched.  The caller must therefore process TarEntry::data
        before calling advance() again.

        When @a importSize is 0 (default), only bytes already available in
        the stream buffer are consumed (non-blocking, suitable for
        event-loop use).  When @a importSize is greater than 0, up to that
        many bytes are read from the underlying stream via sgetn(), which
        may block until data arrives (suitable for file or thread use).

        @returns Pointer to the current TarEntry once a header has been
                 parsed.  Returns nullptr only when no entry header could be
                 parsed yet or when isEnd() is true.
    */
    const TarEntry* advance(std::streamsize importSize = 0);

    /** @brief Returns true after two consecutive null blocks have been read. */
    bool isEnd() const;

  private:
    TarReader(const TarReader&);

    TarReader& operator=(const TarReader&);

  private:
    class TarReaderImpl* _impl;
};

} // namespace

#endif // include guard
