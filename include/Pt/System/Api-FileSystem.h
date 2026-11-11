/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_SYSTEM_API_FILESYSTEM_H
#define PT_SYSTEM_API_FILESYSTEM_H

/** @addtogroup Pt-System-FileSystem

    @brief File and directory operations with iterator-based traversal.

    The systems programming library (Pt::System) provides functionality to
    identify, create, rename, move or remove files and directories. A
    %FileInfo identifies a node by path and queries or modifies that node.
    %DirectoryIterator traverses a directory and yields a %FileInfo for
    each entry, with the full path of that entry, so the same queries work
    during iteration. The iterator can be used with the iterator based
    algorithms in the C++ standard library. The FileDevice is an
    implementation of the IODevice to read and write files. 
*/

#endif
