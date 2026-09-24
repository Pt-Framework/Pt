/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_SYSTEM_TAR_WRITER_H
#define PT_SYSTEM_TAR_WRITER_H

#include <Pt/System/Api.h>
#include <Pt/System/TarEntry.h>

#include <ostream>
#include <cstddef>

namespace Pt {

namespace System {

/** @brief Writes a tar archive to a stream.

    A %TarWriter appends archive members to a std::ostream, in order,
    and blocks on each write. The caller builds a %TarEntry for the
    metadata and passes that entry to the writer. The writer does not
    invent an owner, a group, or a modification time. Whatever the
    entry carries is what the archive stores, and whatever the entry
    leaves unset stays unset.

    A complete member is one call. %addFile() writes a regular file
    whose bytes are already in memory. %addDirectory() writes a
    directory. %addSymlink() and %addHardlink() write a link, and they
    take the link target, the permissions, the modification time, and
    the owner and group from the entry, the same fields a file carries.
    The type flag in the archive comes from the method, so the caller
    does not have to set %TarEntry::type(). The path, the size, and the
    link target do have to be set, because those are the member's
    identity.

    A file too large to hold in one buffer is written in three steps.
    %beginFile() writes the header and declares %TarEntry::size() as
    the total number of content bytes. %writeFile() delivers those
    bytes in as many chunks as needed. %endFile() writes the padding
    that rounds the member up to a 512-byte block, and it fails if the
    chunks so far do not add up to the declared size. Starting another
    member, or calling %finish(), while a streamed file is still open
    fails the same way: the open member has to be closed first.

    %finish() writes the two zero blocks that mark the end of the
    archive. A reader treats an archive without that marker as
    truncated, so finish() belongs before the stream is closed.

    Names, ids, sizes, and times that fit in a UStar header are stored
    there. A path or a link target longer than 99 bytes, a name that is
    not ASCII, an owner or group name longer than 31 bytes, a numeric
    id above 07777777, or a size above eleven octal digits is stored in
    a Pax extended header that precedes the member. The caller does not
    choose the form. Reading the result back yields the same %TarEntry
    fields either way.

    The writer throws %Pt::IOError when the stream is missing or a
    write fails, when %writeFile() is asked for more bytes than the
    header declared, when %endFile() is called before those bytes have
    all been written, and when a new member is started while a streamed
    file is still open.

    @code
    std::ofstream out("archive.tar", std::ios::binary);
    TarWriter writer(out);

    TarEntry dir;
    dir.setPath(Path("src"));
    dir.setPermissions(FileInfo::OwnerAll);
    writer.addDirectory(dir);

    TarEntry file;
    file.setPath(Path("src/main.cpp"));
    file.setSize(source.size());
    file.setPermissions(FileInfo::OwnerRead);
    file.setMtime(DateTime(2024, 6, 1, 12, 0, 0));
    file.setOwnerId(1000);
    file.setOwnerName(String("marc"));
    writer.addFile(file, source.data(), source.size());

    writer.finish();
    @endcode

    @ingroup Pt-System-Tar
*/
class PT_SYSTEM_API TarWriter
{
  public:
    /** @brief Creates a writer with no stream attached.
    */
    TarWriter();

    /** @brief Creates a writer that appends to @a os.
    */
    explicit TarWriter(std::ostream& os);

    /** @brief Destructor.
    */
    ~TarWriter();

    /** @brief Attaches @a os as the destination.

        A streamed file that is still open on the previous stream stays
        open. Call %endFile() before detaching if that member should be
        finished.
    */
    void attach(std::ostream& os);

    /** @brief Detaches the current destination.
    */
    void detach();

    /** @brief Detaches the destination and clears the writer state.

        An open streamed file is abandoned. Its header may already have
        been written to the previous stream.
    */
    void reset();

    /** @brief Writes a regular file from a memory buffer.

        @a size is the number of content bytes taken from @a data. The
        header declares %TarEntry::size(). For a member that should
        round-trip, the two lengths are equal.

        @param entry Metadata of the file. %TarEntry::path() and
                     %TarEntry::size() are written into the header.
        @param data  Content bytes. May be null when @a size is 0.
        @param size  Number of bytes to write from @a data.
    */
    void addFile(const TarEntry& entry, const char* data, std::size_t size);

    /** @brief Writes a directory member.

        @param entry Metadata of the directory. %TarEntry::path() is the
                     archive path. A trailing slash is added in the
                     archive when the path does not already end with one.
    */
    void addDirectory(const TarEntry& entry);

    /** @brief Writes a symbolic link.

        @param entry Metadata of the link. %TarEntry::linkTarget() is
                     the path the link points at. Permissions,
                     modification time, owner, and group are stored the
                     same way as for a file.
    */
    void addSymlink(const TarEntry& entry);

    /** @brief Writes a hard link.

        @param entry Metadata of the link. %TarEntry::linkTarget() is
                     the archive path of the existing member. Permissions,
                     modification time, owner, and group are stored the
                     same way as for a file.
    */
    void addHardlink(const TarEntry& entry);

    /** @brief Writes the header of a file whose content follows in chunks.

        %TarEntry::size() is the total number of bytes %writeFile() must
        deliver before %endFile(). The header is written immediately, so
        the size cannot be changed afterwards.

        @param entry Metadata of the file, including the total content
                     length.
        @throw Pt::IOError if a previous streamed file was not closed
               with %endFile().
    */
    void beginFile(const TarEntry& entry);

    /** @brief Writes the next content chunk of the open streamed file.

        The sum of every @a size passed between %beginFile() and
        %endFile() must equal %TarEntry::size().

        @param data Content bytes. May be null when @a size is 0.
        @param size Number of bytes to write from @a data.
        @throw Pt::IOError if @a size exceeds the number of bytes still
               expected for this member.
    */
    void writeFile(const char* data, std::size_t size);

    /** @brief Finishes the streamed file opened by beginFile().

        Writes the padding that rounds the member up to a 512-byte
        block.

        @throw Pt::IOError if %writeFile() has not yet delivered every
               byte declared by %TarEntry::size().
    */
    void endFile();

    /** @brief Writes the end-of-archive marker.

        The marker is two 512-byte blocks of zeros. Call this before
        closing the stream.

        @throw Pt::IOError if a streamed file is still open.
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

#endif // PT_SYSTEM_TAR_WRITER_H
