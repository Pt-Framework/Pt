/* Copyright (C) 2013 Marc Boris Dürner
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
#include <Pt/Forms/Application.h>
#include <Pt/System/IOError.h>

using namespace Windows::Gfx::Core;

namespace Pt {

namespace Forms {

void FrameworkView::Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView )
{

}

void FrameworkView::Uninitialize()
{
}

void FrameworkView::Run()
{
	//Obtain hat pointer to the window
	_dispatcher = CoreWindow::GetForCurrentThread()->Dispatcher;
	
	//Loop until application closes
	while( _impl->waitNext())
		;
}

void FrameworkView::SetWindow( Windows::Gfx::Core::CoreWindow^ window )
{
	_window = window;
}

void FrameworkView::Load( Platform::String^ entryPoint ) 
{

}


ApplicationImpl::ApplicationImpl()
: Pt::System::EventLoop()
, _isClosed(false)
, _dpi(96.0)
, _frameworkView( ref new FrameworkView())
{
	_frameworkView->setApplicationImpl((long long) this);

	getScreeResolution(_screenWidth, _screenHeight);

	_width  = _screenWidth * unitSizeInch()*_dpi;
	_height = _screenHeight * unitSizeInch()*_dpi;
	
	_factorX = _width / _screenWidth;
	_factorY = _height / _screenHeight;
	_offsetX = 0;
	_offsetY = 0;

	FreeConsole();
}

void ApplicationImpl::showConsole(bool show)
{
	if(show)
		AllocConsole();
	else
		FreeConsole();
}


ApplicationImpl::~ApplicationImpl()
{
}

void ApplicationImpl::setResolution(double dpi)
{
	_dpi = dpi;
}

double ApplicationImpl::resolutionDPI() const
{
	return _dpi;
}

int ApplicationImpl::fromUnit(double unit)
{
	return (int) (unit *unitSizeInch()* _dpi);
}

double ApplicationImpl::toUnit(int unit)
{
	return unitSizeInch()/_dpi * unit;
}


void ApplicationImpl::onAttachTimer(System::Timer& timer)
{ 
    _timerQueue.addTimer(timer); 
}


void ApplicationImpl::onDetachTimer(System::Timer& timer )
{ 
    _timerQueue.removeTimer(timer); 
}

void ApplicationImpl::onAttachSelectable(System::Selectable& s)
{ 
	_selector.attach(s); 
}

void ApplicationImpl::onDetachSelectable(System::Selectable& s)
{ 
	_selector.detach(s);
}


void ApplicationImpl::onCancel(System::Selectable& s)
{
    Pt::System::MutexLock lock(_mutex);

    std::vector<System::Selectable*>::iterator it = _avail.begin();
    while(it != _avail.end())
    {
        if(*it == &s)
            it = _avail.erase(it);
        else
            ++it;
    }
}

void ApplicationImpl::onReady(System::Selectable& s)
{
    Pt::System::MutexLock lock(_mutex);
    _avail.push_back(&s);
}


void ApplicationImpl::onExit()
{
    _eventQueue.exit();
    wake();
}


void ApplicationImpl::onCommitEvent(const Pt::Event& ev)
{ 
    _eventQueue.pushEvent(ev); 
    wake();
}


void ApplicationImpl::onQueueEvent(const Pt::Event& ev)
{ 
    _eventQueue.pushEvent(ev); 
}


void ApplicationImpl::onProcessEvents()
{ 
    _eventQueue.processEvents( this->eventReceived() );
}


void ApplicationImpl::onWake()
{
	if (_frameworkView->dispatcher() == nullptr)
		return;

	_frameworkView->dispatcher()->RunAsync( Windows::Gfx::Core::CoreDispatcherPriority::Normal, ref new Windows::Gfx::Core::DispatchedHandler([](){}) );

}

void ApplicationImpl::Closed(CoreWindow^ Sender, CoreWindowEventArgs^ Args)
{
	_isClosed = true;
}


void ApplicationImpl::onRun()
{
    auto appSource = ref new AppSource((long long)this); 

    Windows::ApplicationModel::Core::CoreApplication::Run(appSource);
}


bool ApplicationImpl::waitNext()
{
    size_t timeout = _timerQueue.processTimers();

    // check all selectables that did not require waiting
    while( true )
    {
        Pt::System::MutexLock lock(_mutex);

        if( _avail.empty() )
            break;

        timeout = 0;
        System::Selectable* s = _avail.back();
        _avail.pop_back();
        lock.unlock();

        s->run();
    }

	// TODO: use timeout
	_frameworkView->dispatcher()->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		
	// returns false when Application::exit was called
	return _eventQueue.processEvents( this->eventReceived() );
}


Pt::Gfx::PointF ApplicationImpl::toUnit(const Pt::Gfx::Point& value)
{
	const double x = value.x() * _factorX  + _offsetX;
	const double y = value.y() * _factorY  + _offsetY;

	return Pt::Gfx::PointF(std::ceil(x),std::ceil(y));
}

Pt::Gfx::SizeF ApplicationImpl::toUnit(const Pt::Gfx::Size& value)
{
	const double width = value.width() * _factorX  + _offsetX;
	const double height = value.height() * _factorY  + _offsetY;

	return Pt::Gfx::SizeF(std::ceil(width),std::ceil(height));
}

Pt::Gfx::Point ApplicationImpl::fromUnit(const Pt::Gfx::PointF& value)
{
	double factorX = _screenWidth / _width;
	double factorY = _screenHeight / _height;
	int x = (int) ( value.x() * factorX); 
	int y = (int) ( value.y() * factorY);
	
	return Pt::Gfx::Point(x,y);
}

Pt::Gfx::Size ApplicationImpl::fromUnit(const Pt::Gfx::SizeF& value)
{
	double factorX = _screenWidth / _width;
	double factorY = _screenHeight / _height;
	int width = (int) ( value.width() * factorX); 
	int height = (int) ( value.height() * factorY);
	
	return Pt::Gfx::Size(width,height);
}

double ApplicationImpl::unitSizeInch() const
{
	return 1.0/72.0;
}

double ApplicationImpl::unitSizeMm() const
{
	return 25.4 * unitSizeInch();
}

void ApplicationImpl::nextEvent()
{
	waitNext();
}

void ApplicationImpl::getScreeResolution(int& horizontal, int& vertical)
{
/*   const HWND hDesktop = GetDesktopWindow();
   RECT desktop;   
   GetWindowRect(hDesktop, &desktop);
   horizontal = desktop.right;
   vertical = desktop.bottom;*/
}

}}
