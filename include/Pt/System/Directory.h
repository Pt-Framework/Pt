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
#include <iterator>

namespace Pt {

namespace System {

class Path;
class FileInfo;

/** @brief Iterates over entries of a directory.

    The Pt::System::DirectoryIterator can be used to iterate over the contents
    of directories. It is created with a path to a directory and satisfies
    the requirements for a forward iterator. The iterator successivly reads
    the contents of the assoziated directory and returns a %FileInfo when
    dereferenced. Like the stream iterators of the C++ standard library, it
    changes to a special state when the end of the directory is reached. This
    state is identical to a default constructed iterator, so instances thereof
    can serve as the iterator to the end of the directory. 

    @code
    try
    {
        Pt::System::Path path = "/tmp";
        Pt::System::DirectoryIterator it(path);
        Pt::System::DirectoryIterator end;

        for( ; it != end; ++it)
        {
            std::cout << "name : " << it->path().toLocal() << std::endl;
        }
    }
    catch(const Pt::System::AccessFailed& e)
    {
        std::cerr << "failed to access directory" << std::endl;
    }
    @endcode

    The constructor throws an %AccessFailed exception if the path is not a valid
    directory. The exception can be avoided by checking the path with @link
    Pt::System::FileInfo::type() FileInfo::type()@endlink first, to make sure
    it is valid. The DirectoryIterator can then be advanced to get the %FileInfo
    for the next file in the directory, until the end of the directory contents
    is reached.

    @ingroup FileSystem
*/
class PT_SYSTEM_API DirectoryIterator
{
    public:
        typedef FileInfo value_type;
        typedef std::ptrdiff_t difference_type;
        typedef std::forward_iterator_tag iterator_category;
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
