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

namespace System {

/** @brief Blocking writer for tar archives (Pax/UStar format).

    %TarWriter writes entries sequentially into a tar archive.  All
    operations are synchronous and write directly to the attached
    std::ostream.

    Use addFile(), addDirectory(), addSymlink(), or addHardlink() to write
    complete entries in a single call.  For large files, use the streaming
    API: beginFile() writes the header, writeFile() delivers the content in
    one or more chunks, and endFile() closes the entry.

    Always call finish() before closing the stream to write the mandatory
    end-of-archive marker.  Paths are interpreted as UTF-8; Pax extended
    headers are written automatically for long or non-ASCII paths.

    @code
    std::ofstream ofs("archive.tar", std::ios::binary);
    TarWriter writer(ofs);

    // complete entries
    writer.addDirectory(Pt::System::Path("src/"), dirPerms);
    writer.addFile(Pt::System::Path("src/main.cpp"),
                   src.data(), src.size(), filePerms);

    // streaming a large file
    writer.beginFile(Pt::System::Path("data.bin"), totalSize, filePerms);
    writer.writeFile(chunk1, size1);
    writer.writeFile(chunk2, size2);
    writer.endFile();

    writer.finish();
    @endcode

    @ingroup Pt-System-Tar
*/
class PT_SYSTEM_API TarWriter
{
  public:
    /** @brief Default constructor.
    */
    TarWriter();

    /** @brief Constructor attaching to @a os.
    */
    explicit TarWriter(std::ostream& os);

    /** @brief Destructor.
    */
    ~TarWriter();

    /** @brief Attach to an output stream.
    */
    void attach(std::ostream& os);

    /** @brief Detach from the current output stream.
    */
    void detach();

    /** @brief Detach from the current output stream and reset state.
    */
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

    /** @brief Begin writing a regular file entry for streaming.

        Writes the archive header for a file of the given total size.
        Subsequent calls to writeFile() deliver the content bytes;
        endFile() must be called once all data has been written.

        @param path        Archive path (UTF-8).
        @param totalSize   Total byte count that will be written via writeFile().
        @param permissions POSIX permission bits.
    */
    void beginFile(const Pt::System::Path& path,
                   std::size_t totalSize,
                   Pt::System::FileInfo::Perms permissions);

    /** @brief Write a chunk of data for the current streaming file entry.

        May be called multiple times after beginFile() until all totalSize
        bytes have been written.  The sum of all @a size arguments must equal
        the @a totalSize passed to beginFile().

        @param data   Pointer to data bytes.
        @param size   Number of bytes to write.
        @throws Pt::IOError if @a size exceeds the remaining byte count
                declared in beginFile().
    */
    void writeFile(const char* data, std::size_t size);

    /** @brief Finish the current streaming file entry started by beginFile().

        Writes the 512-byte block-alignment padding.

        @throws Pt::IOError if not all bytes declared in beginFile() have
                been written via writeFile().
    */
    void endFile();

    /** @brief Write the end-of-archive marker (two 512-byte null blocks).
    */
    void finish();

  private:
    TarWriter(const TarWriter&);

    TarWriter& operator=(const TarWriter&);

  private:
    class TarWriterImpl* _impl;
};


} // namespace System

} // namespace Pt

#endif // include guard
