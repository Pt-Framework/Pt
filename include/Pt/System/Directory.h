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
#include <string>
#include <iterator>

namespace Pt {

namespace System {

/** @brief Iterates over entries of a directory.

    A %DirectoryIterator is created by the Directory class and
    can be used as follows:

    \code
    Directory::iterator it("/usr");
    Directory::iterator end;

    while(it != end)
    {
        std::cout << "name : " << *it << std::endl;
        ++it;
    }
    \endcode

    @ingroup FileSystem
*/
class PT_SYSTEM_API DirectoryIterator
{
    public:
        typedef std::string value_type;
        typedef std::ptrdiff_t difference_type;
        typedef std::forward_iterator_tag iterator_category;
        typedef const std::string* pointer;
        typedef const std::string& reference;

    public:
        //! @brief Default constructor
        DirectoryIterator()
        : _impl(0)
        { }

        //! @brief Constructs an iterator pointing at the file given by @a path
        explicit DirectoryIterator(const std::string& path);

        //! @brief Constructs an iterator pointing at the file given by @a path
        explicit DirectoryIterator(const char* path);

        //! @brief Constructs an iterator pointing at @a fi
        explicit DirectoryIterator(const FileInfo& fi);

        //! @brief Copy constructor
        DirectoryIterator(const DirectoryIterator& it);

        //! @brief Destructor
        ~DirectoryIterator();

        //! @brief Advances the iterator to the next file
        DirectoryIterator& operator++();

        //! @brief Assignment operator
        DirectoryIterator& operator=(const DirectoryIterator& it);

        //! @brief Equality comparison
        bool operator==(const DirectoryIterator& it) const
        { return _impl == it._impl; }

        //! @brief Inequality comparison
        bool operator!=(const DirectoryIterator& it) const
        { return _impl != it._impl; }

        //! @brief Returns the name of the file the iterator points at
        const FileInfo& operator*() const;

        //! @brief Returns the name of the file the iterator points at
        const FileInfo* operator->() const;

    private:
        //! @internal
        class DirectoryIteratorImpl* _impl;
};

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_DIRECTORY_H
