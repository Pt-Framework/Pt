/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "win32.h"
#include "ApplicationImpl.h"
#include <string>

#ifndef _WIN32_WCE
    #include <psapi.h>
#endif

namespace Pt {

namespace System {

ApplicationImpl::ApplicationImpl()
{
}


ApplicationImpl::~ApplicationImpl()
{
}


void ApplicationImpl::init(EventLoop& s)
{
}


bool ApplicationImpl::ignoreSystemSignal(int sig)
{
    return false;
}


bool ApplicationImpl::catchSystemSignal(int sig)
{
    return false;
}


bool ApplicationImpl::raiseSystemSignal(int sig)
{
    return false;
}


unsigned long ApplicationImpl::usedMemory()
{
#ifndef _WIN32_WCE
    PROCESS_MEMORY_COUNTERS pmc;

    if(GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof(pmc)))
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


#ifdef _WIN32_WCE

std::string ApplicationImpl::getEnvVar(const std::string& name)
{
    HKEY hk;

    long ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                _T("Software\\Pt\\environment"),
                                0,
                                KEY_QUERY_VALUE,
                                &hk );

    if(ret != ERROR_SUCCESS)
    {
        throw SystemError( PT_ERROR_MSG("Could not open Registry") );
    }

    DWORD type = REG_SZ;
    DWORD byteLength = MAX_PATH * sizeof(TCHAR);
    TCHAR data[MAX_PATH] = {0};
    std::basic_string<TCHAR> wname;
    win32::fromMultiByte(name, wname);

    ret = RegQueryValueEx(hk, wname.c_str(), NULL, &type, (LPBYTE)data, &byteLength);

    RegCloseKey(hk);

    if(ret != ERROR_SUCCESS)
    {
        throw SystemError( PT_ERROR_MSG("Could not query Registry") );
    }

    if( byteLength == 0 || data[0] == 0 )
        return "";

    return win32::toMultiByte( (LPCTSTR)data );
}


void ApplicationImpl::unsetEnvVar(const std::string& name)
{
    ApplicationImpl::setEnvVar(name, "");
}


void ApplicationImpl::setEnvVar(const std::string& name, const std::string& value)
{
    HKEY hk;
    DWORD ret = 0;
    ret = RegCreateKeyEx( HKEY_LOCAL_MACHINE,
                            _T("Software\\Pt\\environment"),
                            0,
                            _T(""),
                            0,
                            0,
                            NULL,
                            &hk,
                            &ret );
    if(ret != ERROR_SUCCESS)
    {
        throw SystemError( PT_ERROR_MSG("Could not create Registry key") );
    }

    std::basic_string<TCHAR> wname;
    win32::fromMultiByte(name, wname);
    std::basic_string<TCHAR> wvalue;
    win32::fromMultiByte(value, wvalue);

    LPBYTE data = (LPBYTE)wvalue.c_str();
    DWORD size = wvalue.size() * sizeof(TCHAR);

    LONG lret = RegSetValueEx(hk, wname.c_str(), 0, REG_SZ, data, size);
    RegCloseKey(hk);

    if(lret != ERROR_SUCCESS)
        throw SystemError( PT_ERROR_MSG("Could not set Registry value") );
}

#else

void ApplicationImpl::setEnvVar(const std::string& name, const std::string& value)
{
    if( 0 == SetEnvironmentVariable(name.c_str(), value.c_str()) )
    {
        throw SystemError("Set Environment Variable Error!");
    }
}


void ApplicationImpl::unsetEnvVar(const std::string& name)
{
    if( 0 == SetEnvironmentVariable(name.c_str(), NULL) )
    {
        throw SystemError("UnSet Environment Variable Error!");
    }
}


std::string ApplicationImpl::getEnvVar(const std::string& name)
{
    char cp[200];
    std::string ret;
    DWORD cnt;
    cnt = GetEnvironmentVariable(name.c_str(), cp, 200);
    if( 0 == cnt )
    {
        cnt = GetLastError();
        if( ERROR_ENVVAR_NOT_FOUND ==  cnt )
        {
            return ret;
        }
        throw SystemError("Get Environment Variable Error 1!");
    }
    if(cnt<200)
    {
        ret=cp;
    }
    else
    {
        char *cp2 = new char[cnt+1];
        cnt = GetEnvironmentVariable(name.c_str(), cp2, cnt);
        if( 0 == cnt )
        {
            delete[] cp2;
            throw SystemError("Get Environment Variable Error 2!");
        }

        ret = cp2;
        delete [] cp2;
    }

    return ret;
}

#endif

} // namespace System

} // namespace Pt
