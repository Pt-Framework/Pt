/* Copyright (C) 2013 Marc Boris Duerner
 * Copyright (C) 2013 Aloysius Indrayanto
 * Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include "X11Fd.h"
#ifndef _AIX
#include <X11/Xft/Xft.h>
#endif
#include "ApplicationImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/SourceInfo.h>

#include <stdexcept>
#include <vector>

namespace Pt {
namespace Hmi {

X11Fd::X11Fd(Display* display)
: _display(display)
, _ioh(*this)
, _loop(0)
{
    _ioh.fd = XConnectionNumber( _display );
}

X11Fd::~X11Fd()
{
    try
    {
        this->close();
    }
    catch(...)
    {}
}

bool X11Fd::onRun()
{

    if( ! XPending(_display) )
        return false;

    while( XPending(_display) > 0 ) 
	{
        XNextEvent(_display, &_xev);
		Application::instance().impl()->WindowEvent.send(_xev);
    }

    return true;
}

void X11Fd::onAttach(System::EventLoop& loop)
{ 
    _loop = &loop;
}


void X11Fd::onDetach(System::EventLoop& loop)
{ 
    _loop = 0; 
}


}}
