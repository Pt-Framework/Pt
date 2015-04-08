#include "ApplicationImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/KeyEvent.h>

namespace Pt {
namespace Hmi {

Application::Application(int argc, char** argv)
: System::Application(0, argc, argv)
, _impl( new ApplicationImpl() ) 
{ 	
	this->init(*_impl);
}


Application::~Application()
{
	delete _impl;
}


Application& Application::instance()
{
    return static_cast<Application&>( System::Application::instance() );
}

	
Pt::Gfx::PointF Application::toUnit(const Pt::Gfx::Point& value)
{
	return _impl->toUnit(value);
}

Pt::Gfx::SizeF Application::toUnit(const Pt::Gfx::Size& value)
{
	return _impl->toUnit(value);
}

void Application::showConsole(bool show)
{
	_impl->showConsole(show);
}

Pt::Gfx::Point Application::fromUnit(const Pt::Gfx::PointF& value)
{
	return _impl->fromUnit(value);
}

Pt::Gfx::Size Application::fromUnit(const Pt::Gfx::SizeF& value)
{
	return _impl->fromUnit(value);
}

Pt::Gfx::Rect Application::fromUnit(const Pt::Gfx::RectF& value)
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

Pt::Signal<const Pt::Event&>& Application::systemEvent()
{
	return _impl->systemEvent();
}

void Application::nextEvent()
{
	_impl->nextEvent();
}

void Application::setCursor(const Cursor& cursor)
{
	_impl->setCursor(cursor);
}

}}

