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
#include "Pt/System/FileInfo.h"
#include <cstring>
#include <windows.h>

namespace Pt {

namespace System {

class FileInfoImpl
{
    public:
        static FileInfo::Type getType(const char* path)
        {
            std::basic_string<TCHAR> tpath = win32::fromMultiByte(path);
            DWORD attr = GetFileAttributes( tpath.c_str() );

            if(attr == 0xffffffff)
            {
                if( 0 != strstr(path, ".sys") )
                    return FileInfo::File;

                return FileInfo::Invalid;
            }

            if(attr & FILE_ATTRIBUTE_DIRECTORY)
                return FileInfo::Directory;

            return FileInfo::File;
        }
};

} // namespace System

} // namespace Pt


