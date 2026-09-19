/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_SYSTEM_API_TAR_H
#define PT_SYSTEM_API_TAR_H

/** @addtogroup Pt-System-Tar

    @brief Reading and writing tar archives (Pax/UStar format).

    A tar archive is a sequence of entries. %TarReader parses that
    sequence from a std::istream. %TarWriter writes it to a
    std::ostream. %TarEntry is the current entry.

    %TarReader::advance() delivers one header or the next content
    chunk. A null return means the stream has no more bytes yet. The
    returned %TarEntry stays valid until the next advance(). Process
    the bytes at data() before calling advance() again. isEnd() on the
    entry means the content is complete. isEnd() on the reader means
    the archive is complete. A non-zero import size may block and is
    for file streams. A zero import size uses only buffered bytes and
    is for an event loop.

    %TarWriter writes complete entries with addFile(), addDirectory(),
    addSymlink(), and addHardlink(). Large files use beginFile(),
    writeFile(), and endFile(). finish() writes the end-of-archive
    marker. Paths are UTF-8. Pax extended headers are written when a
    path is long or not ASCII.
*/

#endif
