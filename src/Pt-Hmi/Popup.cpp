/* Copyright (C) 2017 Marc Boris Duerner
 
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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/Hmi/Popup.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Screen.h>
#include <algorithm>

namespace Pt {

namespace Hmi {

Popup::Popup()
: Window(0, WindowType::Popup)
, _anchor(0)
{
    setTitle("Popup");
}


Popup::~Popup()
{
}


Visual* Popup::anchor()
{
    return _anchor;
}


void Popup::setAnchor(Visual* anchor)
{
    if(_anchor)
        removePeer(*_anchor);

    if(anchor)
    {
        addPeer(*anchor);
        _anchor = anchor;
    }
}


void Popup::onAttachPeer(Visual& peer)
{
    Visual::onAttachPeer(peer);
}


void Popup::onDetachPeer(Visual& peer)
{
    if(_anchor == & peer)
        _anchor = 0;

    Visual::onDetachPeer(peer);
}


void Popup::onRelease()
{
    Application::instance().onShowPopup(*this, false);
}


void Popup::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);
    Application::instance().onShowPopup( *this, ev.visible() );
}

} // namespace

} // namespace


