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
{
    setTitle("Popup");
}


Popup::~Popup()
{
}


void Popup::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);

    if( ev.visible() )
    {
        //std::clog << "POPUP SHOW: " << vid() << std::endl;
        //grabPointer();
        Application::instance().eventReceived() += Pt::slot(*this, &Popup::onGlobalMouseEvent);
        Application::instance().eventReceived() += Pt::slot(*this, &Popup::onGlobalTouchEvent);
        Application::instance().eventReceived() += Pt::slot(*this, &Popup::onGlobalActivateEvent);
        Application::instance().eventReceived() += Pt::slot(*this, &Popup::onGlobalMoveEvent);
    }
    else
    {
        //std::clog << "POPUP HIDE: " << vid() << std::endl;
        //releasePointer();
        Application::instance().eventReceived() -= Pt::slot(*this, &Popup::onGlobalMouseEvent);
        Application::instance().eventReceived() -= Pt::slot(*this, &Popup::onGlobalTouchEvent);
        Application::instance().eventReceived() -= Pt::slot(*this, &Popup::onGlobalActivateEvent);
        Application::instance().eventReceived() -= Pt::slot(*this, &Popup::onGlobalMoveEvent);
    }
}


void Popup::onGlobalMouseEvent(const MouseEvent& ev)
{
    //std::clog << "POPUP GLOBAL MOUSE: " << ev.visual()->vid() << std::endl;

    if( ev.isPress() )
    {
        Gfx::PointF pos = fromScreen( ev.position() );
        Gfx::RectF rect( size() );

        if( ! rect.contains(pos) )
        {
            show(false);
        }
    }
}


void Popup::onGlobalTouchEvent(const TouchEvent& ev)
{
    if( ev.isPress() )
    {
        Gfx::PointF pos = fromScreen( ev.position() );
        Gfx::RectF rect( size() );

        if( ! rect.contains(pos) )
        {
            show(false);
        }
    }
}


void Popup::onGlobalActivateEvent(const ActivateEvent& ev)
{
    show(false);
}


void Popup::onGlobalMoveEvent(const MoveEvent& ev)
{
    // ignore moving children !!! REMOVE !!! )
    //for( Visual* v = ev.visual(); v != 0; v = v->parent() )
    //{
    //    if(v == this)
    //        return;
    //}

    // react only to moving top level windows
    Screen& screen = Application::instance().screen();
    const std::vector<Window*>& windows = screen.windows();

    std::vector<Window*>::const_iterator it = std::find( windows.begin(),
                                                         windows.end(), 
                                                         ev.visual() );
    if( it != windows.end() )
        show(false);
}


bool Popup::onMouseEvent(const MouseEvent& ev)
{
    return Base::onMouseEvent(ev);
}


bool Popup::onTouchEvent(const TouchEvent& ev)
{
    return Base::onTouchEvent(ev);
}

} // namespace

} // namespace


