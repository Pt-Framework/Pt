/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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

#include "WindowImpl.h"
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{

WindowImpl::WindowImpl(Window* api)
: _apiWindow(api)
{
}


WindowImpl::~WindowImpl()
{
}

void WindowImpl::onResize(Window& child,
                          const Gfx::SizeF& oldSize, 
                          const Gfx::SizeF& size)
{
    double borderWidth = _apiWindow->windowManager().borderWidth();
    double titleHeight = _apiWindow->windowManager().titleHeight();

    Gfx::PointF updatePos(0,0);
    updatePos.subX(borderWidth);
    updatePos.subY(borderWidth + titleHeight);
   
    Gfx::SizeF updateSize;
    updateSize.setWidth( std::max(oldSize.width(), size.width()) );
    updateSize.setHeight( std::max(oldSize.height(), size.height()) );
    updateSize.addWidth(2* borderWidth);
    updateSize.addHeight(2* borderWidth + titleHeight);
    
    Gfx::RectF updateRect(updatePos, updateSize);
    child.update(updateRect);
}


void WindowImpl::onMove(Window& child, 
                        const Gfx::PointF& oldPos, 
                        const Gfx::PointF& pos)
{
    const double borderWidth  = Application::instance().windowBorderWidth();
    const double titleHeight  = Application::instance().windowTitleHeight();

    Gfx::RectF updateRect( oldPos, child.size() );    
    Gfx::RectF movedRect( pos, child.size() );
    updateRect.unify(movedRect);

    Gfx::SizeF updateSize = updateRect.size();
    updateSize.addWidth(2* borderWidth);
    updateSize.addHeight(2* borderWidth + titleHeight);
    updateRect.setSize(updateSize);

    _apiWindow->update(updateRect);
}

}}
