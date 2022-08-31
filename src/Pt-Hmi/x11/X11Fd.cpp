/*
  Copyright (C) 2015 Marc Boris Duerner 

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include "X11Fd.h"
#include "ApplicationImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/SourceInfo.h>

#ifndef _AIX
#include <X11/Xft/Xft.h>
#endif

#include <stdexcept>
#include <vector>

namespace Pt {

namespace Hmi {

X11Fd::X11Fd(Display* display)
: _display(display)
, _ioh(*this)
, _loop(0)
{
    _ioh.fd = XConnectionNumber(_display);

    if( _ioh.fd < 0 )
        throw Pt::System::AccessFailed("invalid X11 display");
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


void X11Fd::begin()
{      
    if( ! _loop )
        throw std::logic_error("I/O device not active");

    Pt::System::Selector& selector = _loop->selector();
    selector.beginRead(&_ioh);

    //std::clog << "BEGIN X11FD" << std::endl;
}


void X11Fd::close()
{ 
    if(_ioh.fd != -1)
    {
        ::close(_ioh.fd); 
        _ioh.fd = -1;
    }
}


void X11Fd::flush()
{ 
    // TODO: flush may not be neccessary

    while( XPending(_display) > 0 ) 
    {
        XNextEvent(_display, &_xev);

        // if(_xev.xany.type == ConfigureNotify)
        //     std::clog << "ConfigureNotify" << std::endl;
        // else
        //     std::clog << "EVENT: " <<  _xev.xany.type << std::endl;

        _eventReady.send(_xev);
    }
}


bool X11Fd::onRun()
{
    //std::clog << "X11Fd::onRun BEGIN" << std::endl;

    Pt::System::Selector& selector = _loop->selector();
    bool isAvail = selector.isReadable(&_ioh);
    if( ! isAvail ) 
        return isAvail;

    //selector.endRead(&_ioh);

    while( XPending(_display) > 0 ) 
    {
        XNextEvent(_display, &_xev);
        
        // if(_xev.xany.type == ConfigureNotify)
        //     std::clog << "ConfigureNotify " <<  _xev.xany.type  << std::endl;
        // else
        //     std::clog << "EVENT: " <<  _xev.xany.type << std::endl;

        _eventReady.send(_xev);
    }

    //selector.beginRead(&_ioh);

    return isAvail;
}


void X11Fd::onCancel()
{ 
    throw std::logic_error("not implemented"); 
}


void X11Fd::onAttach(System::EventLoop& loop)
{ 
    _loop = &loop;
}


void X11Fd::onDetach(System::EventLoop& loop)
{ 
    _loop = 0; 
}

} // namespace

} // namespace
