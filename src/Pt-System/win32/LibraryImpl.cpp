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
#include "LibraryImpl.h"
#include "win32.h"
#include "Pt/System/IOError.h"

namespace Pt {

namespace System {

void LibraryImpl::open(const std::string& path)
{
    if(_handle != 0)
        return;

#ifdef __cplusplus_winrt
    std::wstring wpath;
    win32::fromMultiByte(path, wpath);
    _handle = ::LoadPackagedLibrary( wpath.c_str(), 0 );
#else
    std::basic_string<TCHAR> tpath;
    win32::fromMultiByte(path, tpath);
    _handle = ::LoadLibrary( tpath.c_str() );
#endif

    if(_handle == 0)
    {
        throw AccessFailed(path);
    }
}


void LibraryImpl::close()
{
    if(_handle != 0)
        ::FreeLibrary(_handle);
}


void* LibraryImpl::resolve(const char* symbol) const
{
    if(_handle == 0)
        return 0;

    std::basic_string<TCHAR> tsymbol;
    win32::fromMultiByte(symbol, tsymbol);
    return (void*) ( ::GetProcAddress( _handle, tsymbol.c_str() ) );
}

} // namespace System

} // namespace Pt
