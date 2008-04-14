/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
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

#include "ProcessImpl.h"
#include "../win32/ProcessImplBase.cpp"


namespace Pt {

namespace System {

ProcessImpl::ProcessImpl(const string& command)
: ProcessImplBase(command)
{}

ProcessImpl::ProcessImpl(const ProcessInfo& procInfo)
: ProcessImplBase( procInfo)
{}

std::string ProcessImpl::getEnvVar(const string& name)
{
    HKEY hk;

    long ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                _T("Software\\ptv\\environment"),
                                0,
                                KEY_QUERY_VALUE,
                                &hk );

    if(ret != ERROR_SUCCESS)
    {
        throw std::runtime_error("Could not open Registry" + PT_SOURCEINFO);
    }

    DWORD type = REG_SZ;
    DWORD byteLength = MAX_PATH * sizeof(TCHAR);
    TCHAR data[MAX_PATH] = {0};
    std::basic_string<TCHAR> wname = win32::fromMultiByte(name);

    ret = RegQueryValueEx(hk, wname.c_str(), NULL, &type, (LPBYTE)data, &byteLength);

    RegCloseKey(hk);

    if(ret != ERROR_SUCCESS)
    {
        throw std::runtime_error("Could not query Registry" + PT_SOURCEINFO);
    }

    if( byteLength==0 || data[0] == 0 )
        return "";

    return win32::toMultiByte( (LPCTSTR)data );
}


void ProcessImpl::setEnvVar(const string& name, const string& value)
{
    HKEY hk;
    DWORD ret = 0;
    ret = RegCreateKeyEx( HKEY_LOCAL_MACHINE,
                            _T("Software\\ptv\\environment"),
                            0,
                            _T(""),
                            0,
                            0,
                            NULL,
                            &hk,
                            &ret );
    if(ret != ERROR_SUCCESS)
    {
        throw std::runtime_error("Could not create Registry key" + PT_SOURCEINFO);
    }

    std::basic_string<TCHAR> wname = win32::fromMultiByte(name);
    std::basic_string<TCHAR> wvalue = win32::fromMultiByte(value);

    LPBYTE data = (LPBYTE)wvalue.c_str();
    DWORD size = (wvalue.size()+1) * sizeof(TCHAR); // size includes \0 char !!!

    LONG lret = RegSetValueEx(hk, wname.c_str(), 0, REG_SZ, data, size);
    RegCloseKey(hk);

    if(lret != ERROR_SUCCESS)
        throw std::runtime_error("Could not set Registry value" + PT_SOURCEINFO);
}

} // namespace Pt

} //namespace System
