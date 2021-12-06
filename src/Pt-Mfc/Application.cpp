/* 
 * Copyright (C) 2014 Marc Boris Duerner
 * Copyright (C) 2014 Laurentiu-Gheorghe Crisan
 * 
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

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#endif

#ifndef _AFX_ALL_WARNINGS
#define _AFX_ALL_WARNINGS
#endif

#include <Pt/Mfc/Application.h>

namespace Pt {

namespace Mfc {

WinApp::WinApp()
: CWinApp()
, Pt::System::Application(0, 0, 0)
{
    init(_loop);
}


WinApp::~WinApp()
{
}


BOOL WinApp::PumpMessage()
{
    if( _loop.pumpMessage() )
        return __super::PumpMessage();

    return TRUE;
}


WinAppEx::WinAppEx(BOOL bResourceSmartUpdate)
: CWinAppEx(bResourceSmartUpdate)
, Pt::System::Application(0, 0, 0)
{
    init(_loop);
}


WinAppEx::~WinAppEx()
{
}


BOOL WinAppEx::PumpMessage()
{
    if( _loop.pumpMessage() )
        return __super::PumpMessage();

    return TRUE;
}

} // namespace

} // namespace
