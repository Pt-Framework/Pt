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
, _mainScreen()
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
  if(_cursorName  == cursor->name() )
    return;

	_impl->setCursor(cursor);

  _cursorName = cursor->name();
}

}}

