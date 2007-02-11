/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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

#include "Pt/Api.h"
#include "Pt/System/Directory.h"
#include "Pt/System/File.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


namespace Pt {

namespace System {

    class PT_API FileSystemImpl {
        public:
            FileSystemImpl()
            {}

            static FileSystemNode* create(const char* path)
            {
                struct stat st;
                if( 0 != ::stat(path, &st) )
                    throw SystemError("Could not stat file", PT_SOURCEINFO);
            
                if( S_ISREG(st.st_mode) )
                {
                    return new File(path);
                }
                else if( S_ISDIR(st.st_mode) ) 
                {
                    return new Directory(path);
                }
                else {
                    return new File(path);
                }
                /*
                else if(S_ISCHR(st.st_mode))
                    type = File::CharDevice;
                else if(S_ISBLK(st.st_mode))
                    type = File::BlockDevice;
                else if(S_ISFIFO(st.st_mode))
                    type = File::Pipe;
                #ifdef S_ISSOCK
                else if(S_ISSOCK(st.st_mode))
                    type = File::Pipe;
                #endif
                #ifdef S_ISLNK
                else if(S_ISLNK(st.st_mode))
                    type = File::Link;
                #endif
                */
            
                return 0;
            }
    };

} // namespace System

} // namespace Pt


