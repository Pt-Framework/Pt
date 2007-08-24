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

#ifndef Pt_System_File_h
#define Pt_System_File_h

#include <Pt/Types.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/Api.h>
#include <Pt/System/Directory.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/FileSystemNode.h>


namespace Pt {

namespace System {

/**
 * @brief Provides common operations on files.
 */
class PT_SYSTEM_API File : public FileSystemNode {

    friend bool operator==(const File& a, const File& b);

    private:
        class FileImpl* _impl;

    public:
        File(const std::string& path = "");

        File(const Directory& baseDir, const std::string& fileName);

        File(const File& file);

        ~File();

        File& operator=(const File& file);

        /**
         * @brief Creates a file in the file system which is described by this
         * File object's abstract path name.
         *
         * If this File object points to a file which already exists, nothing is
         * done but $false$ is returned. If the file does not yet exist, it is
         * created. If the file could not be created, for example because there
         * were not enough access privileges, $false$ is returned. If the file
         * was successfully created, $true$ is returned.
         *
         * @return $true$ if the file could successfully be created; $false$ otherwise.
         */
        bool create();

        virtual const std::string& path() const;

        virtual std::size_t size() const;

        void resize(std::size_t newSize);

        bool exists() const;

        virtual FileSystemNodeType type() const
        {
            return FILE;
        }

        virtual void remove();

        //void copy(const std::string& to) const;

        virtual void move(const std::string& newPath);

        /**
         * @brief Returns the directory in which this file resides.
         *
         * If no directory is specified when the File object is created, so the file is
         * seen as relative to the current directory, an empty string is returned. If
         * the file is contained in the root directory of the file system, for linux
         * a slash ("/") is returned.
         *
         * A returned directory always ends with a trailing path separator character.
         * (A backslash in Windows and a slash in Linux, for example.)
         *
         * @return The directory in which this file resides.
         */
        virtual std::string parentPath() const;

        /**
         * @brief Returns the file name of the file this object points to.
         *
         * The complete file name, including the file name extension is returned.
         * To determine the file name without the file name extension the method
         * baseName() can be used.
         *
         * @return The file name of this File object.
         */
        virtual std::string name() const;

        /**
         * @brief Returns the base file name (without its file name extension) of the file
         * this object points to.
         *
         * Only the base file name, excluding the file name extension is returned. If
         * the file name has no extension, the file name is returned as is. To determine
         * the file name including its extension the method name() can be used.
         *
         * @return The file name excluding the file name extension) of this File object.
         */
        std::string baseName() const;

        /**
         * @brief Returns the file name extensions of the file this object points to.
         *
         * Only the file name extension is returned. If the file has no extension an
         * empty string is returned.
         *
         * @brief The file name extension of this file.
         */
        std::string extension() const;
};

// TODO This operator should check the files on file-system layer by checking if
// they share the same node or something like this.
inline bool operator==(const File& a, const File& b)
{
    return a.path() == b.path();
}

inline bool operator!=(const File& a, const File& b)
{
    return !(a == b);
}


} // namespace System

} // namespace Pt

#endif
