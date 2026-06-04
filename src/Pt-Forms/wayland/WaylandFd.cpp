/* Copyright (C) 2015-2026 Marc Boris Duerner 
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#include "WaylandFd.h"
#include <Pt/System/IOError.h>
#include <stdexcept>
#include <unistd.h>

namespace Pt {
namespace Forms {

WaylandFd::WaylandFd(struct wl_display* display)
: _display(display)
, _ioh(*this)
, _loop(0)
{
    if( ! _display )
        throw Pt::System::AccessFailed("invalid Wayland display");

    _ioh.fd = wl_display_get_fd(_display);
    if( _ioh.fd < 0 )
        throw Pt::System::AccessFailed("invalid Wayland display fd");
}


WaylandFd::~WaylandFd()
{
}


void WaylandFd::begin()
{
    if( ! _loop )
        throw std::logic_error("I/O device not active");

    Pt::System::Selector& selector = _loop->selector();
    selector.beginRead(&_ioh);
}


void WaylandFd::close()
{
    _ioh.fd = -1;
}


void WaylandFd::processEvents()
{
    wl_display_dispatch_pending(_display);
    wl_display_flush(_display);
}


bool WaylandFd::onRun()
{
    Pt::System::Selector& selector = _loop->selector();
    bool isAvail = selector.isReadable(&_ioh);
    if( ! isAvail )
        return isAvail;

    selector.endRead(&_ioh);
    selector.beginRead(&_ioh);

    wl_display_dispatch(_display);
    _eventReady.send(0);

    return isAvail;
}


void WaylandFd::onCancel()
{
}


void WaylandFd::onAttach(System::EventLoop& loop)
{
    _loop = &loop;
}


void WaylandFd::onDetach(System::EventLoop& loop)
{
    _loop = 0;
}

} // namespace Forms
} // namespace Pt
