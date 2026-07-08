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

/** @brief Describes a single entry in a tar archive.

    Path strings are always UTF-8 encoded.

    The %size() method holds the total byte count of the entry's content as
    read from the archive header and remains constant for the lifetime of the
    entry.

    The %remaining() method reflects how many content bytes have not yet been
    consumed.  It is decremented by TarReader::advance() as data is consumed.

    The %avail() method reflects how many bytes are currently readable via
    the %data() pointer.  It is updated by TarReader::advance().

    The %data() pointer points into TarReader's internal buffer.  Exactly
    %avail() bytes are valid starting at %data().  The pointer is
    invalidated on the next call to TarReader::advance().

    Call %isEnd() to check whether all content bytes have been delivered.
*/
class TarEntry
{
  public:
    /** @brief Entry type in the tar archive. */
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

    /** @brief Returns true when all content bytes have been delivered.

        Equivalent to <tt>avail() == remaining()</tt>.  When true after
        writing data(), no further advance() call is needed for this entry's
        data; the next outer advance() call will proceed to the next entry.
    */
    bool isEnd() const
    { return _avail == _remaining; }

    /** @brief Path of this entry. */
    const Pt::System::Path& path() const
    { return _path; }

    /** @brief Total content size in bytes as declared in the archive header. */
    std::size_t size() const
    { return _size; }

    /** @brief Content bytes not yet consumed. */
    std::size_t remaining() const
    { return _remaining; }

    /** @brief Bytes currently readable via data(). */
    std::size_t avail() const
    { return _avail; }

    /** @brief Pointer to avail() bytes in TarReader's internal buffer.

        Valid until the next call to TarReader::advance().
    */
    const char* data() const
    { return _data; }

    /** @brief Entry type (file, directory, symbolic link, or hard link). */
    Type type() const
    { return _type; }

    /** @brief Link target path for symbolic links. */
    const Pt::System::Path& linkTarget() const
    { return _linkTarget; }

    /** @brief File permissions. */
    Pt::System::FileInfo::Perms permissions() const
    { return _permissions; }

    /** @brief Modification time. */
    const Pt::DateTime& mtime() const
    { return _mtime; }

    void setPath(const Pt::System::Path& path)
    { _path = path; }

    void setSize(std::size_t size)
    { _size = size; }

    void setRemaining(std::size_t remaining)
    { _remaining   = remaining; }

    void setData(const char* data, std::size_t avail)
    { _data = data; _avail = avail; }

    void setType(Type type)
    { _type = type; }

    void setLinkTarget(const Pt::System::Path& target)
    { _linkTarget = target; }

    void setPermissions(Pt::System::FileInfo::Perms p)
    { _permissions = p; }

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
