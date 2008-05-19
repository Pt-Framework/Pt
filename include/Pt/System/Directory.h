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
#ifndef PT_SYSTEM_DIRECTORY_H
#define PT_SYSTEM_DIRECTORY_H

#include <Pt/System/Api.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/FileSystemNode.h>
#include <Pt/System/File.h>
#include <string>

namespace Pt {

namespace System {

class Directory;

/** @brief A directory could not be found at a given path
*/
class PT_SYSTEM_API DirectoryNotFound : public SystemError
{
    public:
        DirectoryNotFound(const Directory& dir, const SourceInfo& si);

        ~DirectoryNotFound() throw();
};

/** @brief Iterates over entries of a directory.

    You use the iterator as follows:
    \code
    Directory d("/usr");
    Directory::iterator it = d.begin();
    while (it != d.end())
    {
        std::cout << "name : " << it->path() << std::endl;
        ++it;
    }
    \endcode
*/
class PT_SYSTEM_API DirectoryIterator
{
    public:
        DirectoryIterator();

        DirectoryIterator(const char* path);

        DirectoryIterator(const DirectoryIterator& it);

        ~DirectoryIterator();

        DirectoryIterator& operator++();

        DirectoryIterator& operator=(const DirectoryIterator& it);

        bool operator==(const DirectoryIterator& it) const
        { return _impl == it._impl; }

        bool operator!=(const DirectoryIterator& it) const
        { return _impl != it._impl; }

        const char* path() const;

        const char* name() const;

        const char* operator*() const;

    private:
        class DirectoryIteratorImpl* _impl;
};

/** @brief Represents a single directory in the file-system.
    This class contains methods to create, move, delete directories and 
    gives to possibility to iterate over the contents of the directory.

    Iterator Example:
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
class PT_SYSTEM_API Directory : public FileSystemNode 
{
    friend class FileInfo;

    public:
        typedef DirectoryIterator Iterator;

        explicit Directory(const std::string& path);

        ~Directory();

        virtual std::size_t size() const;

        //! @brief Returns an iterator to the node in the directory.
        DirectoryIterator begin() const;

        //! @brief Returns an iterator to the end of the directory.
        DirectoryIterator end() const;

        virtual void remove();

        virtual void move(const std::string& newPath);

        virtual std::string dirName() const;

        //! @brief Returns the name of the directory excluding the path.
        virtual std::string name() const;

        virtual Type type() const;

    public:
        static void create(const char* path);

        static bool exists(const char* path)
        {
            return FileSystemNode::stat(path) == FileSystemNode::Directory;
        }

    protected:
        Directory();

    private:
        class DirectoryImpl* _impl;
};

/** @brief Provides information about a node in the file-system.
*/
class FileInfo
{
    public:
        FileInfo();

        explicit FileInfo(const char* path);

        FileInfo(const FileInfo& fi);

        ~FileInfo();

        FileInfo& operator=(const FileInfo& f);

        const char* name() const;

        const char* path() const;

        const FileSystemNode& node() const;

    private:
        File _file;
        Directory _dir;
        FileSystemNode* _node;
        std::string _path;
};

}

}

#endif
