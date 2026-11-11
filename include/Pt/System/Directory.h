/*
 * Copyright (C) 2006-2008 Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PT_SYSTEM_DIRECTORY_H
#define PT_SYSTEM_DIRECTORY_H

#include <Pt/System/Api.h>
#include <Pt/System/FileInfo.h>
#include <iterator>

namespace Pt {

namespace System {

class Path;

/** @brief Iterates over entries of a directory.

    A %DirectoryIterator walks through the entries of a directory and
    dereferences to a %FileInfo for the current entry. That %FileInfo is
    the directory entry: its path is the full path of the node, and the
    instance queries such as type(), size() and exists() refer to that
    node. The special names `.` and `..` are skipped, so a loop over the
    iterator yields only the actual contents of the directory.

    @code
    try
    {
        Pt::System::Path path = "/tmp";
        Pt::System::DirectoryIterator it(path);
        Pt::System::DirectoryIterator end;

        for( ; it != end; ++it)
        {
            const Pt::System::FileInfo& entry = *it;
            std::cout << entry.path().toLocal();

            if(entry.type() == Pt::System::FileInfo::Directory)
                std::cout << " [dir]\n";
            else
                std::cout << " " << entry.size() << " bytes\n";
        }
    }
    catch(const Pt::System::AccessFailed& e)
    {
        std::cerr << "failed to access directory" << std::endl;
    }
    @endcode

    The iterator meets the requirements of an input iterator. Copies share
    the iteration position; incrementing one copy moves the others as well.
    When no entries remain, the iterator takes on the same state as a
    default-constructed iterator, which therefore serves as the past-the-end
    iterator. A directory that contains no entries besides `.` and `..`
    compares equal to that end iterator immediately after construction.

    The constructor throws %AccessFailed if the path is not an accessible
    directory. The exception can be avoided by checking the path with
    @link Pt::System::FileInfo::type() FileInfo::type()@endlink first.

    @ingroup Pt-System-FileSystem
*/
class PT_SYSTEM_API DirectoryIterator
{
    public:
        typedef FileInfo value_type;
        typedef std::ptrdiff_t difference_type;
        typedef std::input_iterator_tag iterator_category;
        typedef const FileInfo* pointer;
        typedef const FileInfo& reference;

    public:
        //! @brief Default constructor.
        DirectoryIterator()
        : _impl(0)
        { }

        //! @brief Constructs with directory path.
        explicit DirectoryIterator(const Path& path);

        //! @brief Constructs with directory.
        explicit DirectoryIterator(const FileInfo& fi);

        //! @brief Copy constructor.
        DirectoryIterator(const DirectoryIterator& it);

        //! @brief Destructor.
        ~DirectoryIterator();

        //! @brief Advances the iterator to the next file.
        DirectoryIterator& operator++();

        //! @brief Assignment operator.
        DirectoryIterator& operator=(const DirectoryIterator& it);

        //! @brief Equality comparison.
        bool operator==(const DirectoryIterator& it) const
        { return _impl == it._impl; }

        //! @brief Inequality comparison.
        bool operator!=(const DirectoryIterator& it) const
        { return _impl != it._impl; }

        //! @brief Returns the file the iterator points at.
        const FileInfo& operator*() const;

        //! @brief Returns the file the iterator points at.
        const FileInfo* operator->() const;

    private:
        //! @internal
        class DirectoryIteratorImpl* _impl;
};

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_DIRECTORY_H
