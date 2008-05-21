/***************************************************************************
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
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
#ifndef Pt_System_FileInfo_h
#define Pt_System_FileInfo_h

#include <Pt/System/Api.h>
#include <Pt/System/File.h>
#include <Pt/System/Directory.h>

namespace Pt {

namespace System {

/** @brief Provides information about a node in the file-system.
*/
class FileInfo
{
    public:
        FileInfo();

        explicit FileInfo(const char* path);

        ~FileInfo();

        std::string name() const;

        const char* path() const;

        std::string dirName() const;

        std::size_t size() const;

        bool isDirectory() const;

        bool isFile() const;

        void remove();

        void move(const std::string& newname);

    private:
        int _type;
        std::string _path;
};

} // namespace System

} // namespace Pt

#endif
