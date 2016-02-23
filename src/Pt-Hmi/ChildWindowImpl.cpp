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
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Pen.h>

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
    update(rect);
}


void ChildWindowImpl::setVisible(bool visible)
{
    if(visible) 
        activate();

    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    update(rect);
}


void ChildWindowImpl::update(const Gfx::RectF& rect)
{
    Gfx::PointF pos = _apiWindow->position() + rect.topLeft();
    Gfx::RectF parentRect(pos, rect.size());

    if( _apiWindow->parent() )	
        _apiWindow->parent()->update(parentRect);	
}


void ChildWindowImpl::activate()
{
    if( _apiWindow->parent() )
    {
        _apiWindow->parent()->activate();
        _apiWindow->parent()->windowManager().activate( *_apiWindow );	
    }
}


void ChildWindowImpl::setPosition(const Gfx::PointF& p)
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    update(rect);
}

void ChildWindowImpl::setSize(const Gfx::SizeF& size)
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    update(rect);
}

void ChildWindowImpl::setState(WindowState::Type p)
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    update(rect);
}

void ChildWindowImpl::setBorder(WindowBorder::Type p)
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    update(rect);
}

void ChildWindowImpl::setIcon(const Gfx::Image& p)
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    update(rect);
}

void ChildWindowImpl::setEnabled(bool e)
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    update(rect);
}

void ChildWindowImpl::setMinimumSize(const Gfx::SizeF& s)
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    update(rect);
}

void ChildWindowImpl::setMaximumSize(const Gfx::SizeF& s)
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    update(rect);
}


void ChildWindowImpl::setDecoration( WindowDecoration::Flags d )	
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    update(rect);
}

void ChildWindowImpl::setTitle( const std::string& t )
{
    Gfx::RectF rect( Gfx::PointF(0,0), _apiWindow->size() );
    update(rect);
}

}

}
