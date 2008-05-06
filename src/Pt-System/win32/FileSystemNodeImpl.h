/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
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
#include "win32.h"
#include "Pt/System/Directory.h"
#include "Pt/System/File.h"
#include <cstring>
#include <windows.h>

namespace Pt {

namespace System {

class FileSystemNodeImpl
{
    public:
        static FileSystemNode* create(const char* path)
        {
            std::basic_string<TCHAR> tpath = win32::fromMultiByte(path);
            DWORD attr = GetFileAttributes( tpath.c_str() );

            if(attr == 0xffffffff)
            {
                if( 0 != strstr(path, ".sys") )
                    return new File(path);

                throw SystemError( std::string("Could not get file attributes: ") + path, PT_SOURCEINFO);
            }

            if(attr & FILE_ATTRIBUTE_DIRECTORY) {
                return new Directory(path);
            }
            else {
                return new File(path);
            }

            return 0;
        }

        static FileSystemNode::Type stat(const char* path)
        {
            std::basic_string<TCHAR> tpath = win32::fromMultiByte(path);
            DWORD attr = GetFileAttributes( tpath.c_str() );

            if(attr == 0xffffffff)
            {
                if( 0 != strstr(path, ".sys") )
                    return FileSystemNode::File;

                return FileSystemNode::Invalid;
            }

            if(attr & FILE_ATTRIBUTE_DIRECTORY)
                return FileSystemNode::Directory;

            return FileSystemNode::File;
        }
};

} // namespace System

} // namespace Pt


