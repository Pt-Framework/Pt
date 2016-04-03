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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

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
}


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
}


void ChildWindowImpl::setMaximumSize(const Gfx::SizeF& s)
{
}


void ChildWindowImpl::setState(WindowState::Type p)
{
}


void ChildWindowImpl::setBorder(WindowBorder::Type p)
{
}


void ChildWindowImpl::setIcon(const Gfx::Image& p)
{
}


void ChildWindowImpl::setDecoration( WindowDecoration::Flags d )	
{
}


void ChildWindowImpl::setTitle( const std::string& t )
{
}


void ChildWindowImpl::paint(const Gfx::RectF& rect)
{
}

} // namespace

} // namespace
