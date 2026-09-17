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
