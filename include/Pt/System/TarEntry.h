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

#ifndef PT_SYSTEM_TAR_ENTRY_H
#define PT_SYSTEM_TAR_ENTRY_H

#include <Pt/System/Api.h>
#include <Pt/System/FileInfo.h>
#include <Pt/System/Path.h>
#include <Pt/DateTime.h>

#include <cstddef>

namespace Pt {

namespace System {

/** @brief Represents a single entry returned by TarReader.

    A %TarEntry holds the metadata and provides access to the content of
    one entry in a tar archive.  Instances are produced by
    %TarReader::advance() and remain valid until the next advance() call.

    Use %type() to distinguish files, directories, symbolic links, and hard
    links.  Use %path() for the archive path, %mtime() for the modification
    time, and %permissions() for the POSIX permission bits.

    File content is delivered in one or more chunks.  After each advance()
    call, up to %avail() bytes are readable at %data().  Process those bytes
    before calling advance() again — the buffer is reused on the next call.
    Repeat until %isEnd() returns true, which means all %size() bytes have
    been delivered.

    @ingroup Pt-System-Tar
*/
class TarEntry
{
  public:
    /** @brief Entry type in the tar archive.
    */
    enum Type
    {
        Invalid   = 0, //!< Not yet set
        File      = 1, //!< Regular file
        Directory = 2, //!< Directory
        Link      = 3, //!< Symbolic link
        Hardlink  = 4  //!< Hard link
    };

    /** @brief Default constructor.
    */
    TarEntry() = default;

    /** @brief Move constructor.
    */
    TarEntry(TarEntry&&) = default;

    /** @brief Move assignment.
    */
    TarEntry& operator=(TarEntry&&) = default;

    /** @brief Destructor.
    */
    ~TarEntry() = default;

    /** @brief Resets all fields to their default values.
    */
    void clear()
    { *this = TarEntry(); }

    /** @brief Returns true when the entire content has been delivered.

        Once this returns true, the next %TarReader::advance() call moves
        to the following archive entry.
    */
    bool isEnd() const
    { return _avail == _remaining; }

    /** @brief Archive path of this entry (UTF-8 encoded).
    */
    const Pt::System::Path& path() const
    { return _path; }

    /** @brief Total content size in bytes as stored in the archive header.
    */
    std::size_t size() const
    { return _size; }

    /** @brief Content bytes not yet delivered by TarReader::advance().
    */
    std::size_t remaining() const
    { return _remaining; }

    /** @brief Number of bytes readable at the current data() pointer.
    */
    std::size_t avail() const
    { return _avail; }

    /** @brief Pointer to the current content chunk of avail() bytes.

        Process these bytes before calling %TarReader::advance() again —
        the pointer becomes invalid on the next advance() call.
    */
    const char* data() const
    { return _data; }

    /** @brief Type of this archive entry (file, directory, link, or hard link).
    */
    Type type() const
    { return _type; }

    /** @brief Target path for symbolic and hard links (UTF-8 encoded).
    */
    const Pt::System::Path& linkTarget() const
    { return _linkTarget; }

    /** @brief POSIX permission bits for this entry.
    */
    Pt::System::FileInfo::Perms permissions() const
    { return _permissions; }

    /** @brief Last modification time of this entry.
    */
    const Pt::DateTime& mtime() const
    { return _mtime; }

    /** @brief Sets the archive path.
    */
    void setPath(const Pt::System::Path& path)
    { _path = path; }

    /** @brief Sets the total content size in bytes.
    */
    void setSize(std::size_t size)
    { _size = size; }

    /** @brief Sets the number of content bytes not yet delivered.
    */
    void setRemaining(std::size_t remaining)
    { _remaining   = remaining; }

    /** @brief Sets the current data chunk pointer and the number of available bytes.
    */
    void setData(const char* data, std::size_t avail)
    { _data = data; _avail = avail; }

    /** @brief Sets the entry type.
    */
    void setType(Type type)
    { _type = type; }

    /** @brief Sets the link target path.
    */
    void setLinkTarget(const Pt::System::Path& target)
    { _linkTarget = target; }

    /** @brief Sets the POSIX permission bits.
    */
    void setPermissions(Pt::System::FileInfo::Perms p)
    { _permissions = p; }

    /** @brief Sets the modification time.
    */
    void setMtime(const Pt::DateTime& mtime)
    { _mtime = mtime; }

  private:
    TarEntry(const TarEntry&) = delete;

    TarEntry& operator=(const TarEntry&) = delete;

  private:
    Pt::System::Path             _path;
    std::size_t                  _size        = 0;
    std::size_t                  _remaining   = 0;
    std::size_t                  _avail       = 0;
    const char*                  _data        = 0;
    Type                         _type        = Invalid;
    Pt::System::Path             _linkTarget;
    Pt::System::FileInfo::Perms  _permissions;
    Pt::DateTime                 _mtime;
};

} // namespace System

} // namespace Pt

#endif // include guard
