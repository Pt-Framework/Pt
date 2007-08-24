/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Tobias Mueller                                *
 *   Copyright (C) 2006-2007 PTV AG                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef PTV_SYSTEM_DIRECTORY_H
#define PTV_SYSTEM_DIRECTORY_H

#include <Pt/System/System.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/FileSystemNode.h>
#include <Pt/System/Api.h>

#include <string>
#include <list>
#include <iostream>


namespace Pt {

namespace System {

    /** Cycling through Directories.
     *
     * You use the iterator as follows:
    \code
    Directory d("/usr");
    Directory::iterator it = d.begin();
    while (it != d.end())
    {
        std::cout << "name : " << (*it).path() << std::endl;
        ++it;
    }
    \endcode
    */
    class PT_SYSTEM_API DirectoryIterator {
        public:
            DirectoryIterator();

            DirectoryIterator(const char* path);

            DirectoryIterator(const DirectoryIterator& it);

            ~DirectoryIterator();

            DirectoryIterator& operator++();

            DirectoryIterator& operator=(const DirectoryIterator& it);

            bool operator==(const DirectoryIterator& it) const;

            bool operator!=(const DirectoryIterator& it) const;

            FileSystemNode& operator*() const;

        private:
            class DirectoryIteratorImpl* _impl;
    };

/** Directory Operations.
 This class contains methods to create, move, delete directories and gives to possibility to iterate over the contents of the directory.

!Iterator Example:

\code
Directory d("/usr");
Directory::iterator it = d.begin();
while (it != d.end())
{
    std::cout << "name : " << (*it).path() << std::endl;
    ++it;
}
\endcode
*/
    class PT_SYSTEM_API Directory : public FileSystemNode {
        public:
            typedef DirectoryIterator Iterator;

            Directory(const std::string& path = "");

            ~Directory();

            bool create() const;

            bool exists() const;

            virtual const std::string& path() const;

            virtual std::size_t size() const;

            //! Returns an iterator to the node in the directory.
            DirectoryIterator begin() const
            {
                return DirectoryIterator( _path.c_str() );
            }

            //! Returns an iterator to the end of the directory.
            DirectoryIterator end() const
            {
                static DirectoryIterator _end;
                return _end;
            }

            virtual void remove();

            virtual void move(const std::string& newPath);

            /**
             * @brief Returns the directory in which this directory resides.
             *
             * If no directory is specified when the Directory object is created, so the
             * directory is seen as relative to the current directory, an empty string is
             * returned. If the directory is contained in the root directory of the file
             * system, for linux a slash ("/") is returned.
             *
             * A returned directory always ends with a trailing path separator character.
             * (A backslash in Windows and a slash in Linux, for example.)
             *
             * @return The directory in which this directory resides.
             */
            virtual std::string parentPath() const;
            
            /**
             * @brief Returns the name of thi directory, excluding the complete path
             * except the last element -- the directory name.
             *
             * @return The directory name of this Directory object.
             */
            virtual std::string name() const;

            virtual FileSystemNodeType type() const
            {
                return DIRECTORY;
            }

        private:
            std::string _path;
    };

}

}

#endif
