/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2006 by PTV AG                                          *
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

#include "EnvironmentImpl.h"
#include "Pt/System/SystemError.h"

#include <unistd.h>

namespace Pt {

namespace System {


EnvironmentImpl::EnvironmentImpl()
{
}

EnvironmentImpl::~EnvironmentImpl()
{
}

const std::string& EnvironmentImpl::sharedLibraryExtension()
{
    static std::string sharedLibraryExtention(".so");

    return sharedLibraryExtention;
}

const std::string& EnvironmentImpl::sharedLibraryPrefix()
{
    static std::string sharedLibraryPrefix("lib");

    return sharedLibraryPrefix;
}

const std::string& EnvironmentImpl::systemDirectory()
{
    static std::string systemDir("/");
    
    return systemDir;
}

const std::string EnvironmentImpl::currentDirectory()
{
    char cwd[PATH_MAX];

    if( !getcwd(cwd, PATH_MAX) )
        throw SystemError("Could not get current working directroy", PT_SOURCEINFO);

    return std::string(cwd);
}

} // namespace Pt
} // namespace System
