/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_SYSTEM_TAR_ENTRY_H
#define PT_SYSTEM_TAR_ENTRY_H

#include <Pt/System/Api.h>
#include <Pt/System/FileInfo.h>
#include <Pt/System/Path.h>
#include <Pt/DateTime.h>
#include <Pt/String.h>
#include <Pt/Types.h>

#include <cstddef>

namespace Pt {

namespace System {

/** @brief One entry in a tar archive.

    A %TarEntry is the metadata of one member of a tar archive, together
    with a window onto that member's content. %TarReader publishes an
    entry after it has parsed the header. %TarWriter consumes an entry
    the caller has filled in. The same type travels in both directions,
    so a value read from one archive can be handed to a writer and
    reproduced, including owner, group, permissions, and modification
    time.

    The metadata is the part the caller owns. %path() is the name inside
    the archive. %type() distinguishes a regular file, a directory, a
    symbolic link, and a hard link. A link also has %linkTarget().
    %size() is the content length stored in the header. %permissions()
    holds the POSIX permission bits, including the set-user-ID,
    set-group-ID, and sticky bits. %mtime() is the modification time in
    UTC. A default-constructed %Pt::DateTime means the Unix epoch, and
    that is what the writer stores when the caller never sets a time.

    Owner and group are each an optional numeric id plus an optional
    name. %NoId means the archive did not carry an id, or the caller has
    not set one. Numeric id 0 is a real value: it is the root user or
    the root group, and it is written and read as such. An empty
    %Pt::String means the archive did not carry a name. A name that
    happens to be @c root is a real name and round-trips. The writer
    omits an unset id and an empty name. It does not invent @c root or
    id 0. Names are Unicode. The archive stores them as UTF-8, in the
    UStar header when they fit and in a Pax extended header otherwise.

    The content window is the part %TarReader fills in. After
    %TarReader::advance() returns an entry, up to %avail() bytes are
    readable at %data(). Those bytes must be consumed before the next
    call to advance(), because the reader reuses its buffer and the
    pointer then refers to a different chunk, or to nothing. %isEnd()
    becomes true once every byte of %size() has been delivered. A
    directory, a symbolic link, and a hard link have no content, so
    %isEnd() is already true when the entry is first published.
    %TarWriter does not read the content window. The bytes of a file
    are the buffer passed to %TarWriter::addFile(), or the chunks passed
    to %TarWriter::writeFile(). %size() on the entry is the length the
    writer declares in the header, and for a streamed file it is the
    total that %writeFile() must deliver before %endFile().

    An entry returned by the reader stays valid until the next
    advance() call, which replaces its metadata and its content window.
    A caller who wants to keep an entry copies the fields out. The
    content pointer cannot be kept: it addresses the reader's buffer.

    @ingroup Pt-System-Tar
*/
class TarEntry
{
  public:
    /** @brief Kind of archive member.
    */
    enum Type
    {
        Invalid   = 0, //!< Not set
        File      = 1, //!< Regular file
        Directory = 2, //!< Directory
        Link      = 3, //!< Symbolic link
        Hardlink  = 4  //!< Hard link
    };

    /** @brief Id value meaning that no numeric id is set.

        Distinct from numeric id 0, which is the root user or the root
        group.
    */
    static const Pt::uint32_t NoId = static_cast<Pt::uint32_t>(-1);

  public:
    /** @brief Creates an entry with no metadata and an empty content window.
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

    /** @brief Resets every field to the default-constructed state.
    */
    void clear()
    { *this = TarEntry(); }

    /** @brief Returns true when every content byte has been delivered.

        For a directory or a link this is true as soon as the reader
        publishes the entry. For a file it becomes true on the advance()
        call that delivers the last byte. The following advance() call
        moves to the next archive member.
    */
    bool isEnd() const
    { return _avail == _remaining; }

    /** @brief Returns the archive path.
    */
    const Pt::System::Path& path() const
    { return _path; }

    /** @brief Returns the content length declared in the archive header.
    */
    std::size_t size() const
    { return _size; }

    /** @brief Returns the number of bytes readable at data().

        Valid until the next %TarReader::advance() call.
    */
    std::size_t avail() const
    { return _avail; }

    /** @brief Returns the current content chunk of avail() bytes.

        The pointer addresses the reader's buffer and becomes invalid
        on the next %TarReader::advance() call. A chunk of length zero
        returns a null pointer.
    */
    const char* data() const
    { return _data; }

    /** @brief Returns the member kind.
    */
    Type type() const
    { return _type; }

    /** @brief Returns the target of a symbolic link or a hard link.

        Empty for a file or a directory.
    */
    const Pt::System::Path& linkTarget() const
    { return _linkTarget; }

    /** @brief Returns the POSIX permission bits.

        Includes the set-user-ID, set-group-ID, and sticky bits when
        the archive stored them.
    */
    Pt::System::FileInfo::Perms permissions() const
    { return _permissions; }

    /** @brief Returns the modification time in UTC.

        A default-constructed %Pt::DateTime is the Unix epoch. That is
        the value of an entry whose archive header carried time 0, and
        the value the writer stores when the caller never sets a time.
    */
    const Pt::DateTime& mtime() const
    { return _mtime; }

    /** @brief Returns the numeric owner id.

        %NoId means the archive carried no uid, or the caller has not
        set one. Numeric id 0 is the root user.
    */
    Pt::uint32_t ownerId() const
    { return _ownerId; }

    /** @brief Returns the owner name.

        Empty when the archive carried no owner name, or the caller has
        not set one.
    */
    const Pt::String& ownerName() const
    { return _ownerName; }

    /** @brief Returns the numeric group id.

        %NoId means the archive carried no gid, or the caller has not
        set one. Numeric id 0 is the root group.
    */
    Pt::uint32_t groupId() const
    { return _groupId; }

    /** @brief Returns the group name.

        Empty when the archive carried no group name, or the caller has
        not set one.
    */
    const Pt::String& groupName() const
    { return _groupName; }

    /** @brief Sets the archive path.
    */
    void setPath(const Pt::System::Path& path)
    { _path = path; }

    /** @brief Sets the content length declared in the header.

        For %TarWriter::beginFile() this is the total number of bytes
        that %TarWriter::writeFile() must deliver.
    */
    void setSize(std::size_t size)
    { _size = size; }

    /** @brief Sets the member kind.
    */
    void setType(Type type)
    { _type = type; }

    /** @brief Sets the target of a symbolic link or a hard link.
    */
    void setLinkTarget(const Pt::System::Path& target)
    { _linkTarget = target; }

    /** @brief Sets the POSIX permission bits.
    */
    void setPermissions(Pt::System::FileInfo::Perms permissions)
    { _permissions = permissions; }

    /** @brief Sets the modification time in UTC.
    */
    void setMtime(const Pt::DateTime& mtime)
    { _mtime = mtime; }

    /** @brief Sets the numeric owner id.

        Pass %NoId to clear a previously set id.
    */
    void setOwnerId(Pt::uint32_t id)
    { _ownerId = id; }

    /** @brief Sets the owner name.

        An empty name clears a previously set name and is omitted from
        the archive.
    */
    void setOwnerName(const Pt::String& name)
    { _ownerName = name; }

    /** @brief Sets the numeric group id.

        Pass %NoId to clear a previously set id.
    */
    void setGroupId(Pt::uint32_t id)
    { _groupId = id; }

    /** @brief Sets the group name.

        An empty name clears a previously set name and is omitted from
        the archive.
    */
    void setGroupName(const Pt::String& name)
    { _groupName = name; }

    /** @internal

        Publishes the next content chunk. Called by %TarReader only.
        @a remaining counts the content bytes still to be delivered,
        including the @a avail bytes now readable at @a data.
    */
    void setContent(const char* data, std::size_t avail, std::size_t remaining)
    {
        _data = data;
        _avail = avail;
        _remaining = remaining;
    }

  private:
    TarEntry(const TarEntry&) = delete;

    TarEntry& operator=(const TarEntry&) = delete;

  private:
    Pt::System::Path            _path;
    std::size_t                 _size = 0;
    std::size_t                 _remaining = 0;
    std::size_t                 _avail = 0;
    const char*                 _data = 0;
    Type                        _type = Invalid;
    Pt::System::Path            _linkTarget;
    Pt::System::FileInfo::Perms _permissions = Pt::System::FileInfo::NoPerms;
    Pt::DateTime                _mtime;
    Pt::uint32_t                _ownerId = NoId;
    Pt::String                  _ownerName;
    Pt::uint32_t                _groupId = NoId;
    Pt::String                  _groupName;
};

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_TAR_ENTRY_H
