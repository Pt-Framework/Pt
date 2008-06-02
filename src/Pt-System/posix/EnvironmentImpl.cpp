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

bool EnvironmentImpl::changeDirectory( const std::string& path)
{
    int retVal = chdir( path.c_str());
    return retVal != -1;
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
        tmpDir = ( Pt::System::FileInfo::exists("/tmp") ? "/tmp" : "" );
    }

    return tmpDir;
}

unsigned long EnvironmentImpl::getTotalMemory()
{
#ifdef __QNX__
      char                   *str = SYSPAGE_ENTRY(strings)->data;
      struct asinfo_entry    *as = SYSPAGE_ENTRY(asinfo);
      uint64_t                value = 0;
      unsigned                num;

      for( num = _syspage_ptr->asinfo.entry_size / sizeof(*as); num > 0; --num)
      {
              if(std::strcmp(&str[as->name], "ram") == 0)
              {
                      value += as->end - as->start + 1;
              }
              ++as;
      }

      return (unsigned long)(value / 1048576);

#else

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
#endif
}

unsigned long EnvironmentImpl::getFreeMemory()
{
#ifdef __QNX__
    struct  stat    pstat;
    stat("/proc", &pstat);
    return (unsigned long)(pstat.st_size / 1048576);
#else
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
#endif
}

unsigned long EnvironmentImpl::getProcessMemoryUsage()
{
#ifdef __QNX__
    int                 num = 0;
    int                 fd = -1;
    procfs_mapinfo      *mapinfos;
    unsigned long       memoryUsage = 0;


    std::stringstream fileName;
    fileName << "/proc/" <<  getpid() << "/as" ;

    if ((fd = open(fileName.str().c_str(), O_RDONLY)) != -1)
    {
        /*
            This code is postcard-ware.
            (C) 2001, Igor Kovalenko (kovalenko@home.com)
            It is free for any use as long as whatever you do is free too,
            source is available and this copyleft notice is included.
            You're encouraged to send me an e-postcard if it was helpful for you.
        */
        devctl( fd, DCMD_PROC_MAPINFO, NULL, 0, &num );
        mapinfos = new procfs_mapinfo[num];

        /*
            segment-specific data
        */

        devctl( fd, DCMD_PROC_PAGEDATA, mapinfos, sizeof(*mapinfos) * num, &num );

        for (int i = 0; i < num; i++ )
        {

            /*
                Find out what this memory segment is.
                Is gets real nasty here. Yuck :(

                Commited:       !MAP_LAZY || PG_HWMAPPED

                    Executables:    MAP_ELF | MAP_FIXED
                    Shared libs:    MAP_ELF @ 0xbxxxxxxx
                            Code:       MAP_SHARED
                            Image FS:       MAP_PHYS
                            Data:       MAP_SYSRAM

                    Stack:          MAP_STACK (always MAP_LAZY, except for procnto)

                    Heap:           MAP_SYSRAM

                    Shared memory:  MAP_SHARED @ (0x4xxxxxxx || 0xc0000000 for procnto)
                        Device memory:      MAP_PHYS
            */

            if ((mapinfos[i].flags & MAP_LAZY) && !(mapinfos[i].flags & PG_HWMAPPED))
            {
                /*
                    Just reserved address space, ignore
                */
                continue;
            }

            if (mapinfos[i].flags & MAP_ELF) /* ELF code or data */
            {
                if ((mapinfos[i].flags & MAP_TYPE) == MAP_SHARED) /* must be code */
                {
                    // Do not take code into account
                }
                else if (mapinfos[i].flags & MAP_SYSRAM && mapinfos[i].vaddr >= 0xb0000000) /* SO data */
                {
                    memoryUsage += mapinfos[i].size;
                }
                else /* Executable data */
                {
                    memoryUsage += mapinfos[i].size;
                }
            }
            else if (mapinfos[i].flags & MAP_STACK) /* Must be commited stack */
            {
                memoryUsage += mapinfos[i].size;
            }
            else if ((mapinfos[i].flags & MAP_SYSRAM)
                    && !(mapinfos[i].flags & MAP_PHYS))
            {
                /*
                    Must be heap or explicit private anon mapping
                */
                memoryUsage += mapinfos[i].size;
            }
            else
            /*
                This must be either memory mapped file or anonymous shared mapping.
                Either way there's not much we can do about it since all such objects are
                reported as /dev/mem with the same rdev and ino values.
                In english, there's no way to count this memory reliably, due to possible
                undetectable sharing.
                So we don't include this in totals. This is why free_memory (from /proc)
                will be more than (total_memory - used_memory).

                All right, just count it 'as is' so far. And bug Sebastien to fix it...
            */
            {
                memoryUsage += mapinfos[i].size;
            }
       }

       delete [] mapinfos;
    }
    return memoryUsage / 1000; //return memory consumption in kB
#else
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
#endif
}

} // namespace Pt
} // namespace System
