 /* 
  Copyright (C) 2015 Marc Boris Duerner 
  Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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

#include "MainWindowImpl.h"
#include "ApplicationImpl.h"
#include "ScreenImpl.h"

#include <Pt/Forms/Application.h>

namespace Pt {

namespace Forms {

MainWindowImpl::MainWindowImpl(Window* window)
: ChildWindowImpl(window)
{
}


MainWindowImpl::~MainWindowImpl()
{
}


void MainWindowImpl::activate()
{
    Application::instance().mainScreen().impl()->windowManager().activate(*_apiWindow);
}


void MainWindowImpl::show(bool b)
{
    Application::instance().mainScreen().impl()->windowManager().showWindow(*_apiWindow, b);
}


void MainWindowImpl::enable(bool b)
{
    Application::instance().mainScreen().impl()->windowManager().enableWindow(*_apiWindow, b);
}


void MainWindowImpl::resize(const Gfx::SizeF& size)
{
    Application::instance().mainScreen().impl()->windowManager().resizeWindow(*_apiWindow, size);
}


void MainWindowImpl::move(const Gfx::PointF& pos)
{
    Application::instance().mainScreen().impl()->windowManager().moveWindow(*_apiWindow, pos);
}


void MainWindowImpl::update(const Gfx::RectF& updateRect)
{
    double borderWidth = _apiWindow->windowManager().borderWidth();
    double titleHeight = _apiWindow->windowManager().titleHeight();

    Gfx::PointF pos = _apiWindow->position() + updateRect.topLeft();
    pos.addX(borderWidth);
    pos.addY(borderWidth + titleHeight);

    Gfx::RectF screenRect( pos, updateRect.size() );
    Application::instance().mainScreen().impl()->update(screenRect);
}


void MainWindowImpl::onUpdate(Window& child, const Gfx::RectF& childRect)
{
    double borderWidth = _apiWindow->windowManager().borderWidth();
    double titleHeight = _apiWindow->windowManager().titleHeight();

    Gfx::PointF pos = child.position() + childRect.topLeft();
    pos.addX(borderWidth);
    pos.addY(borderWidth + titleHeight);

    Gfx::RectF updateRect( pos, childRect.size() );
    update(updateRect);
}

} // namespace

} // namespace
