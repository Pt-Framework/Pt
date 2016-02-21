/* Copyright (C) 2015 Marc Boris Duerner 
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include <Pt/Hmi/Screen.h>
#include "ScreenImpl.h"
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{

Screen::Screen(ApplicationImpl& app)
: _impl( new ScreenImpl(app) )
, _pointerWindow(0)
{
}


Screen::~Screen()
{
	delete _impl;
}


double Screen::width() const
{
	return _impl->width();
}
		

double Screen::height() const
{
	return _impl->height();
}


Gfx::PointF Screen::toUnit(const Gfx::Point& value)
{
	return _impl->toUnit(value);
}


Gfx::SizeF Screen::toUnit(const Gfx::Size& value)
{
	return _impl->toUnit(value);
}


Gfx::Point Screen::fromUnit(const Gfx::PointF& value)
{
	return _impl->fromUnit(value);
}


Gfx::Size Screen::fromUnit(const Gfx::SizeF& value)
{
	return _impl->fromUnit(value);
}


Gfx::Rect Screen::fromUnit(const Gfx::RectF& value)
{
	return _impl->fromUnit(value);
}


double Screen::unitSizeInch() const
{
	return _impl->unitSizeInch();
}


int Screen::fromUnit(double val) 
{
	return _impl->fromUnit(val);
}


double Screen::toUnit(int val) 
{
	return _impl->toUnit(val);
}


double Screen::unitSizeMm() const
{
	return _impl->unitSizeMm();
}


void Screen::setResolution(double dpi)
{
	_impl->setResolution(dpi);
}


double Screen::resolutionDPI() const
{
	return _impl->resolutionDPI();
}


void Screen::setCursor( const Cursor* cursor )
{
  _impl->setCursor( cursor );
}


void Screen::registerWindow(Window& w)
{
	_windows.push_back(&w);
    _impl->registerWindow(w);
}


void Screen::unregisterWindow(Window& w)
{
    _impl->unregisterWindow(w);

	std::vector<Window*>::iterator it = std::find(_windows.begin(), _windows.end(), &w);
	if( it != _windows.end() )
		_windows.erase(it);
}


Widget* Screen::findWidget(const std::string& name)
{
	std::vector<Window*>::iterator it;
	for(it = _windows.begin(); it != _windows.end(); ++it)
	{
		Widget* w = (*it)->findWidget(name);
		if(w)
			return w;
	}
	
    return 0;
}


Window* Screen::findWindow(const std::string& name)
{
	std::vector<Window*>::iterator it;
	for(it = _windows.begin(); it != _windows.end(); ++it)
	{
        Window* w = (*it)->findWindow(name);
		if(w)
			return w;
	}
	
    return 0;
}


}}
