/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2008 Peter Barth                                        *
 *   Copyright (C) 2006-2008 PTV AG                                        *
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

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>
#include "Pt/SourceInfo.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/Process.h"
#include "EnvironmentImpl.h"
#include "Pt/System/File.h"

#include <hal.h>
#include <hal_data.h>
#include "SymbianTools.h"

#ifdef __SYMBIAN32__
    #include <sys/syslimits.h>
#endif

#ifdef __QNX__
    #include <sys/memmsg.h>
    #include <termios.h>
    #include <fcntl.h>
    #include <process.h>
    #include <sys/types.h>
    #include <sys/procfs.h>
    #include <stdio.h>
    #include <sys/syspage.h>
    #include <sys/dcmd_io-net.h>
    #include <string.h>

#endif

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
    static std::string sharedLibraryExtention(".dll");

    return sharedLibraryExtention;
}

const std::string& EnvironmentImpl::sharedLibraryPrefix()
{
    static std::string sharedLibraryPrefix("");

    return sharedLibraryPrefix;
}

const std::string& EnvironmentImpl::systemDirectory()
{
    static std::string systemDir("c:\\");

    return systemDir;
}

const std::string EnvironmentImpl::currentDirectory()
{
    char cwd[PATH_MAX];

    if( !getcwd(cwd, PATH_MAX) )
        throw SystemError("Could not get current working directroy", PT_SOURCEINFO);

    return std::string(cwd);
}

const std::string EnvironmentImpl::tempDirectory()
{
    std::string tmpDir("");

    // on symbian we strip the process path 
    // and return that as temporary directory
    TParse parser;
    
    if (parser.SetNoWild(RProcess().FileName(), 0, 0) == KErrNone) 
    {
        tmpDir = SymbianTools::TPtrC2string(parser.DriveAndPath());
    }
    
    return tmpDir;
}

unsigned long EnvironmentImpl::getTotalMemory()
{
    TInt freeMem;
    HAL::Get(HALData::EMemoryRAM, freeMem);    
    return freeMem;
}

unsigned long EnvironmentImpl::getFreeMemory()
{
    TInt freeMem;
    HAL::Get(HALData::EMemoryRAMFree, freeMem);    
    return freeMem;
}

unsigned long EnvironmentImpl::getProcessMemoryUsage()
{
    // unsupported on symbian
    return 0;
}

} // namespace Pt
} // namespace System
