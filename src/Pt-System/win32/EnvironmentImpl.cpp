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
#include "win32.h"
#include "EnvironmentImpl.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/Process.h"

#include <windows.h>
#ifndef _WIN32_WCE
    #include <psapi.h>
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
    #ifdef _WIN32_WCE
    
        throw std::runtime_error("GetCurrentDirectory not supported." + PT_SOURCEINFO);
        
    #else
    
        char path[MAX_PATH+2];
        DWORD len = ::GetCurrentDirectory(MAX_PATH+2, path);
        return std::string(path, len);
        
    #endif
}

const std::string EnvironmentImpl::tempDirectory()
{
    std::string tmpDir = Process::getEnvVar("TEMP");
    if (tmpDir.length() == 0)
    {
        tmpDir = Process::getEnvVar("TMP");
    }
    
    return tmpDir;
}

unsigned long EnvironmentImpl::getTotalMemory()
{

    MEMORYSTATUS memoryStatus;
    memoryStatus.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&memoryStatus);

    return (unsigned long)(memoryStatus.dwTotalPhys / 1024);
}

unsigned long EnvironmentImpl::getFreeMemory()
{
    MEMORYSTATUS memoryStatus;
    memoryStatus.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&memoryStatus);

    return (unsigned long)(memoryStatus.dwAvailPhys / 1024);
}

unsigned long EnvironmentImpl::getProcessMemoryUsage()
{
#ifndef _WIN32_WCE




    //SYSTEM_INFO sysInfo;
    //GetSystemInfo(&sysInfo);
    //printf("dwPageSize: %d\n", sysInfo.dwPageSize);




    PROCESS_MEMORY_COUNTERS pmc;

    if(GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
    {
        return (unsigned long)(pmc.PagefileUsage / 1024);
    }

    else
    {
        return 0;
    }
#else
    return 0;
#endif
}


} // namespace Pt
} // namespace System
