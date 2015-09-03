/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 
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
#include "ApplicationImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Hmi/PointerEvent.h>
#include <Pt/Hmi/KeyEvent.h>

namespace Pt {
namespace Hmi {

Application::Application(int argc, char** argv)
: System::Application(0, argc, argv)
, _impl( new ApplicationImpl() ) 
, _mainScreen(0)
{ 	
	this->init(*_impl);
	_mainScreen = new Screen();
}


Application::~Application()
{
	delete _mainScreen;
	delete _impl;
}


Application& Application::instance()
{
    return static_cast<Application&>( System::Application::instance() );
}

	
Ui::PointF Application::toUnit(const Ui::Point& value)
{
	return _impl->toUnit(value);
}

Ui::SizeF Application::toUnit(const Ui::Size& value)
{
	return _impl->toUnit(value);
}

Ui::Point Application::fromUnit(const Ui::PointF& value)
{
	return _impl->fromUnit(value);
}

Ui::Size Application::fromUnit(const Ui::SizeF& value)
{
	return _impl->fromUnit(value);
}

Ui::Rect Application::fromUnit(const Ui::RectF& value)
{
	return _impl->fromUnit(value);
}

double Application::unitSizeInch() const
{
	return _impl->unitSizeInch();
}

int Application::fromUnit(double val) 
{
	return _impl->fromUnit(val);
}

double Application::toUnit(int val) 
{
	return _impl->toUnit(val);
}

double Application::unitSizeMm() const
{
	return _impl->unitSizeMm();
}

void Application::setResolution(double dpi)
{
	_impl->setResolution(dpi);
}

double Application::resolutionDPI() const
{
	return _impl->resolutionDPI();
}

ApplicationImpl* Application::impl()
{
	return _impl;
}

void Application::nextEvent()
{
	_impl->nextEvent();
}

void Application::setCursor(const Cursor* cursor)
{
	_impl->setCursor(cursor);

  _cursorName = cursor->name();
}

}}

