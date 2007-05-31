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

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>
#include "Pt/SourceInfo.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/Process.h"
#include "EnvironmentImpl.h"
#include "Pt/System/File.h"

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

const std::string EnvironmentImpl::tempDirectory()
{
    std::string tmpDir = Process::getEnvVar("TEMP");
    if (tmpDir.length() == 0)
    {
        tmpDir = Process::getEnvVar("TMP");
    }
    if (tmpDir.length() == 0)
    {
        tmpDir = ( File("/tmp").exists() ? "/tmp" : "" );
    }

    return tmpDir;
}

unsigned long EnvironmentImpl::getTotalMemory()
{
    std::ifstream file("/proc/meminfo");
    std::string key;
    unsigned long value = 0;

    if(!file)
    {
        throw std::runtime_error("Cannot open \"/proc/meminfo\" for reading" + PT_SOURCEINFO);
    }

    while(file.good())
    {
        file >> key;

        if(!key.compare("MemTotal:"))
        {
            file >> value;
            break;
        }
    }

    return value;
}

unsigned long EnvironmentImpl::getFreeMemory()
{
    std::ifstream file("/proc/meminfo");
    std::string key;
    unsigned long value = 0;

    if(!file)
    {
        throw std::runtime_error("Cannot open \"/proc/meminfo\" for reading" + PT_SOURCEINFO);
    }

    while(file.good())
    {
        file >> key;

        if(!key.compare("MemFree:"))
        {
            file >> value;
            break;
        }
    }

    return value;
}

unsigned long EnvironmentImpl::getProcessMemoryUsage()
{
    std::string key;
    unsigned long value = 0;
    unsigned long memoryUsage = 0;
    std::stringstream fileName;
    fileName << "/proc/" << getpid() << "/status";
    std::ifstream file(fileName.str().c_str());

    if(!file)
    {
        throw std::runtime_error("Cannot open \"" + fileName.str() + "\" for reading" + PT_SOURCEINFO);
    }

    while(file.good())
    {
        file >> key;

        if(!key.compare("VmSize:"))
        {
            file >> value;
            memoryUsage = value;
            break;
        }
    }

    return memoryUsage;
}

} // namespace Pt
} // namespace System
