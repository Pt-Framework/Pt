/* Copyright (C) 2013 Marc Boris Duerner 
   Copyright (C) 2013 Laurentiu-Gheorghe Crisan
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include "ChildWindowImpl.h"
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Size.h>

namespace Pt {

namespace Hmi {

ChildWindowImpl::ChildWindowImpl(Window* api)
: WindowImpl(api)
{
}


ChildWindowImpl::~ChildWindowImpl()
{
}


void ChildWindowImpl::close()
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    repaint(rect);
}


void ChildWindowImpl::repaint(const Gfx::RectF& updateRect)
{
    Application::instance().repaint( *_apiWindow, updateRect);
}

/*
void ChildWindowImpl::onUpdate(Window& child, const Gfx::RectF& childRect)
{
    if( ! _apiWindow->parent() )
        return;

    double borderWidth = _apiWindow->windowManager().borderWidth();
    double titleHeight = _apiWindow->windowManager().titleHeight();

    Gfx::PointF pos = child.position() + childRect.topLeft();
    pos.addX(borderWidth);
    pos.addY(borderWidth + titleHeight);

    // update rect in _apiWindow client rect coordinates
    Gfx::RectF updateRect( pos, childRect.size() );
    _apiWindow->parent()->onUpdate(*_apiWindow, updateRect);
}
*/

void ChildWindowImpl::activate()
{
    Application::instance().activate( *_apiWindow );
}


void ChildWindowImpl::show(bool visible)
{

    Application::instance().show( *_apiWindow, visible );
}


void ChildWindowImpl::resize(const Gfx::SizeF& size)
{    
    Application::instance().resize( *_apiWindow, size );
}


void ChildWindowImpl::move(const Gfx::PointF& to)
{
    Application::instance().move( *_apiWindow, to );
}


void ChildWindowImpl::enable(bool e)
{
    Application::instance().enable( *_apiWindow, e );
}


void ChildWindowImpl::setMinimumSize(const Gfx::SizeF& s)
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    repaint(rect);
}


void ChildWindowImpl::setMaximumSize(const Gfx::SizeF& s)
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    repaint(rect);
}


void ChildWindowImpl::setState(WindowState::Type p)
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    repaint(rect);
}


void ChildWindowImpl::setBorder(WindowBorder::Type p)
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    repaint(rect);
}


void ChildWindowImpl::setIcon(const Gfx::Image& p)
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    repaint(rect);
}


void ChildWindowImpl::setDecoration( WindowDecoration::Flags d )	
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    repaint(rect);
}


void ChildWindowImpl::setTitle( const std::string& t )
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    repaint(rect);
}

}

}
