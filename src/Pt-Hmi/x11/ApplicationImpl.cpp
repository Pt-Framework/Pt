/* Copyright (C) 2013 Marc Boris Duerner
 * Copyright (C) 2013 Aloysius Indrayanto
 * Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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
#include "ApplicationImpl.h"
#ifndef _AIX
#include <X11/Xft/Xft.h>
#endif
#include "Pt/Hmi/Application.h"
#include "Pt/SourceInfo.h"
#include <stdexcept>
#include <vector>

namespace Pt {
namespace Hmi {

ApplicationImpl::ApplicationImpl()
: _display( XOpenDisplay(NULL) )
, _xfd(_display)
, _dpi(1/92.0)
{
    // Open a X11 display connection
    if( ! _display)
        throw std::runtime_error("Could not open X11 display." + PT_SOURCEINFO);

    XSync(_display, false);

    // Set X11 to sync mode. Slow, for debugging only.
    //XSynchronize(_display, true);

    // Do we really need this?
    //XftInit(0);  

    _xfd.setActive(*this);
    _xfd.begin();

    _xfd.flush();
}

double ApplicationImpl::toUnit(int value)
{
	return value;
}

Pt::Gfx::PointF ApplicationImpl::toUnit(const Pt::Gfx::Point& value)
{
	return Pt::Gfx::PointF(value.x(),value.y()); 
}

Pt::Gfx::SizeF ApplicationImpl::toUnit(const Pt::Gfx::Size& value)
{
	return Pt::Gfx::SizeF(value.width(),value.height()); 
}

int ApplicationImpl::fromUnit(double value)
{
	return (int) value;
}

Pt::Gfx::Point ApplicationImpl::fromUnit(const Pt::Gfx::PointF& value)
{
	return Pt::Gfx::Point((int) value.x(), (int) value.y());
}

Pt::Gfx::Size ApplicationImpl::fromUnit(const Pt::Gfx::SizeF& value)
{
	return Pt::Gfx::Size((int) value.width(),(int)value.height()); 
}


double ApplicationImpl::unitSizeInch() const
{
	return _dpi;
}

double ApplicationImpl::unitSizeMm() const
{
	return _dpi;
}


void ApplicationImpl::setResolution(double dpi)
{
	_dpi = dpi;
}


double ApplicationImpl::resolutionDPI() const
{
	return _dpi;
}
		
void ApplicationImpl::showConsole(bool show)
{
}

void ApplicationImpl::nextEvent()
{
	MainLoop::waitNext();
}

ApplicationImpl::~ApplicationImpl()
{
    XSync(_display, true);
    XCloseDisplay(_display);
    _display = NULL;
}

}}

