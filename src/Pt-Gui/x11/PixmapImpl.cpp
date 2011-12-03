/*
 * Copyright (C) 2006 Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "PixmapImpl.h"
#include "PixmapPainterImpl.h"
#include "ApplicationImpl.h"

#include <Pt/Gui/Application.h>
#include <Pt/Gui/Pixmap.h>

#include <iostream>
using namespace std;


namespace Pt {

namespace Gui {


PixmapImpl::PixmapImpl(size_t width, size_t height)
: _size( std::max(width, size_t(1)), std::max(height, size_t(1)) )
, _painter(0)
{
    //X11Context::instance().lock();

    Display* display = MainLoop::instance().display();
    unsigned int depth = DefaultDepth( display, DefaultScreen(display) );
    _drawable = XCreatePixmap(display, XDefaultRootWindow(display), _size.width(), _size.height(), depth);
    XSync(display, false);

    //X11Context::instance().unlock();
}


PixmapImpl::PixmapImpl(const PixmapImpl& pimpl)
: _size( pimpl.size() )
{
    //X11Context::instance().lock();

    Display* display = MainLoop::instance().display();
    unsigned int screen = DefaultScreen(display);
    unsigned int depth = DefaultDepth( display, screen );

    _drawable = XCreatePixmap(display, XDefaultRootWindow(display), _size.width(), _size.height(), depth);

    // copy contents to the pixmap
    XCopyArea( display,
               pimpl.x11Drawable(), // source
               _drawable, // destination
               DefaultGC(display, screen),
               0, 0,
               _size.width(),
               _size.height(),
               0, 0);

    XSync(display, false);

    //X11Context::instance().unlock();
}


PixmapImpl::~PixmapImpl()
{
    // Destroy the painter (in case we created one).
    delete _painter;

    Display* display = MainLoop::instance().display();
    XFreePixmap(display, _drawable);
}


Painter PixmapImpl::painter()
{
    if (!_painter) _painter = new PixmapPainterImpl(*this);

    return Painter(_painter);
}


} // namespace Gui

} // namespace Pt
