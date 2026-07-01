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

#ifndef PT_SYSTEM_TAR_WRITER_H
#define PT_SYSTEM_TAR_WRITER_H

#include <Pt/System/Path.h>
#include <Pt/System/FileInfo.h>

#include <ostream>
#include <cstddef>

namespace Pt {

/** @brief Blocking writer for tar archives (Pax/UStar format).

    TarWriter writes entries into a tar archive sequentially.  All write
    operations are synchronous and flush immediately to the attached
    std::ostream.

    Call finish() to write the end-of-archive marker (two 512-byte null
    blocks) before closing the stream.

    Path strings are interpreted as UTF-8.  Pax extended headers are
    written automatically for paths longer than 99 characters or containing
    non-ASCII characters.
*/
class PT_SYSTEM_API TarWriter
{
  public:
    TarWriter();

    explicit TarWriter(std::ostream& os);

    ~TarWriter();

    /** @brief Attach to an output stream. */
    void attach(std::ostream& os);

    /** @brief Detach from the current output stream. */
    void detach();

    /** @brief Detach from the current output stream and reset state. */
    void reset();

    /** @brief Write a regular file entry.

        @param path        Archive path (UTF-8).
        @param data        File content buffer.
        @param size        Number of bytes in @a data.
        @param permissions POSIX permission bits.
    */
    void addFile(const Pt::System::Path& path,
                 const char* data,
                 std::size_t size,
                 Pt::System::FileInfo::Perms permissions);

    /** @brief Write a directory entry.

        @param path        Archive path (UTF-8).
        @param permissions POSIX permission bits.
    */
    void addDirectory(const Pt::System::Path& path,
                      Pt::System::FileInfo::Perms permissions);

    /** @brief Write a symbolic-link entry.

        @param path   Archive path (UTF-8).
        @param target Link target path (UTF-8).
    */
    void addSymlink(const Pt::System::Path& path,
                    const Pt::System::Path& target);

    /** @brief Write a hard-link entry.

        @param path   Archive path of the new link (UTF-8).
        @param target Archive path of the existing file to link to (UTF-8).
    */
    void addHardlink(const Pt::System::Path& path,
                     const Pt::System::Path& target);

    /** @brief Write the end-of-archive marker (two 512-byte null blocks). */
    void finish();

  private:
    TarWriter(const TarWriter&);

    TarWriter& operator=(const TarWriter&);

  private:
    class TarWriterImpl* _impl;
};


} // namespace

#endif // include guard
