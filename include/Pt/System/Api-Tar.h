/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_SYSTEM_API_TAR_H
#define PT_SYSTEM_API_TAR_H

/** @addtogroup Pt-System-Tar

    @brief Reading and writing tar archives.

    A tar archive is a sequence of members laid out in fixed 512-byte
    blocks. Each member is a header followed, for a regular file, by
    the file's bytes padded out to the next block. Two blocks of zeros
    mark the end. This group reads and writes that sequence in the
    POSIX Pax/UStar form: a UStar header for the fields that fit, and a
    Pax extended header in front of the member for the fields that do
    not.

    Three types cover the task. %TarEntry is one member: its path, its
    kind, its size, its link target, its permissions, its modification
    time, and its owner and group. %TarReader pulls entries out of a
    std::istream, one header or one content chunk per call, and can
    stop when the stream has nothing further buffered. %TarWriter
    appends entries to a std::ostream. The entry is the value that
    crosses both boundaries, so metadata read from an archive can be
    handed to a writer and stored again.

    The on-disk form is an implementation choice the caller does not
    make. A path or a link target of at most 99 bytes, an ASCII owner
    or group name of at most 31 bytes, a numeric id up to 07777777, a
    size of at most eleven octal digits, and a modification time in
    whole seconds all live in the UStar header. Anything beyond that
    — a longer path, a name that is not ASCII, a larger id, a larger
    size — is written as a Pax record and applied by the reader before
    the entry is published. Pax wins where both forms carry the same
    field. A global Pax header, which would change the defaults for
    every following member, is skipped. There is no writer-wide default
    identity either: each entry carries its own owner and group, and an
    entry that leaves them unset stores neither an id nor a name.

    Names are Unicode in the API and UTF-8 in the archive. Numeric
    owner and group ids use %TarEntry::NoId for "not present", which is
    distinct from id 0. Id 0 is the root user or the root group and is
    a real value. Permissions use the POSIX mode bits from
    %FileInfo::Perms, including set-user-ID, set-group-ID, and the
    sticky bit. The modification time is a %Pt::DateTime in UTC. A
    default-constructed time is the Unix epoch, and that is what gets
    stored when the caller does not set one.

    The archive describes ownership. It does not apply it. Nothing in
    this group creates a directory, writes a file, or changes the owner
    of a file-system object. A caller who wants that uses the path,
    permissions, and timestamps from the entry with the file-system
    API, and decides separately what to do with an owner id that the
    local system may not have.

    @code
    std::ofstream out("tree.tar", std::ios::binary);
    TarWriter writer(out);

    TarEntry dir;
    dir.setPath(Path("docs"));
    dir.setPermissions(FileInfo::OwnerAll);
    dir.setOwnerId(1000);
    dir.setOwnerName(String("marc"));
    writer.addDirectory(dir);

    TarEntry page;
    page.setPath(Path("docs/index.txt"));
    page.setSize(body.size());
    page.setPermissions(FileInfo::OwnerRead);
    page.setMtime(DateTime(2024, 6, 1, 12, 0, 0));
    page.setOwnerId(dir.ownerId());
    page.setOwnerName(dir.ownerName());
    writer.addFile(page, body.data(), body.size());

    writer.finish();
    out.close();

    std::ifstream in("tree.tar", std::ios::binary);
    TarReader reader(in);
    while( ! reader.isEnd() )
    {
        const TarEntry* entry = reader.advance(4096);
        if( ! entry )
            break;

        // entry->ownerId() and entry->path() describe the member.
        // The content window is entry->data() / entry->avail().
    }
    @endcode
*/

#endif
