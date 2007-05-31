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

#include "Pt/System/Environment.h"
#include "Pt/System/Directory.h"
#include "EnvironmentImpl.h"

namespace Pt {
namespace System {


Environment::Environment()
: _impl(0)
{
    _impl = new EnvironmentImpl();
}

Environment::~Environment()
{
    delete _impl;
}

const std::string& Environment::sharedLibraryExtension()
{
    return EnvironmentImpl::sharedLibraryExtension();
}

const std::string& Environment::sharedLibraryPrefix()
{
    return EnvironmentImpl::sharedLibraryPrefix();
}

char Environment::pathSeparator()
{
    return EnvironmentImpl::pathSeparator();
}

Directory Environment::systemDirectory()
{
    std::string path = EnvironmentImpl::systemDirectory();
    return Directory( path.c_str() );
}

Directory Environment::currentDirectory()
{
    std::string path = EnvironmentImpl::currentDirectory();
    return Directory( path.c_str() );
}

Directory Environment::tempDirectory()
{
    std::string path = EnvironmentImpl::tempDirectory();
    return Directory( path.c_str() );
}

unsigned long Environment::getTotalMemory()
{
    return EnvironmentImpl::getTotalMemory();
}

unsigned long Environment::getFreeMemory()
{
    return EnvironmentImpl::getFreeMemory();
}

unsigned long Environment::getProcessMemoryUsage()
{
    return EnvironmentImpl::getProcessMemoryUsage();
}


} // namespace Pt
} // namespace System
