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
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/System/IOError.h>
#include "MainWindowImpl.h"

namespace Pt {

namespace Hmi {

/////////////////////////////////////////////////////////////////////////////
// Selector
/////////////////////////////////////////////////////////////////////////////

Selector::Selector()
{
}

Selector::~Selector()
{
}

DWORD Selector::waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout)
{	
    DWORD result = MsgWaitForMultipleObjects(numHandles, (HANDLE *)handles, false, msecs, QS_ALLEVENTS);

    if(result == WAIT_FAILED)
        throw Pt::System::IOError( PT_ERROR_MSG("WaitForMultipleObjects failed") );

    if( result == WAIT_TIMEOUT)
    {
        isTimeout = true;
        return 0;
    }

    DWORD offset = result - WAIT_OBJECT_0;

    if(offset == numHandles)
    {
	    MSG msg;

	    while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
	    {
			    TranslateMessage(&msg);
			    DispatchMessage(&msg);
	    }	
    }

    return offset;
}

/////////////////////////////////////////////////////////////////////////////
// ApplicationImpl
/////////////////////////////////////////////////////////////////////////////

ApplicationImpl::ApplicationImpl()
: Pt::System::EventLoop()
{
#ifndef _DEBUG  
	FreeConsole();
#endif

	_instanceHandle = (HINSTANCE)GetModuleHandle(NULL);

	registerWindowClasses();
}


ApplicationImpl::~ApplicationImpl()
{
}


void ApplicationImpl::registerWindowClasses()
{
	std::string topLevelWindow = "Pt-Hmi";

	WNDCLASS topWindowClass;

	topWindowClass.style         = CS_HREDRAW | CS_VREDRAW;
	topWindowClass.lpfnWndProc   = (WNDPROC)ApplicationImpl::wndProc;
	topWindowClass.cbClsExtra    = 0;
	topWindowClass.cbWndExtra    = 0;
	topWindowClass.hInstance     = _instanceHandle;
	topWindowClass.hIcon         = NULL;
	topWindowClass.hCursor       = NULL;
	topWindowClass.hbrBackground = NULL;
	topWindowClass.lpszMenuName  = NULL;
	topWindowClass.lpszClassName = topLevelWindow.c_str();

	RegisterClass(&topWindowClass);
}


void ApplicationImpl::unregisterWindowClasses()
{
    UnregisterClass("Pt-Hmi", _instanceHandle);
}


long CALLBACK ApplicationImpl::wndProc(HWND hwnd, unsigned int message, unsigned int wParam, long lParam)
{
	Pt::Hmi::Application& app =  *((Pt::Hmi::Application*)&Pt::Hmi::Application::instance());

    for( size_t i = 0; i <  app.mainScreen().windows().size(); ++i )
    {
        Window* w = app.mainScreen().windows()[i];
     
        MainWindowImpl* impl = static_cast<MainWindowImpl*>( w->impl() );

        if( impl->hwnd() != hwnd )
            continue;

        if( !impl->processEvent( message, wParam, lParam ) )
    		return DefWindowProc(hwnd, message, wParam, lParam);
    }
    
	return DefWindowProc(hwnd, message, wParam, lParam);
}


void ApplicationImpl::nextEvent()
{
	waitNext();
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

void ApplicationImpl::onRun()
{
    while( this->waitNext() )
        ;
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


void ApplicationImpl::onWake()
{ 
    _selector.wake(); 
}


void ApplicationImpl::onAttachTimer(System::Timer& timer)
{ 
    _timerQueue.addTimer(timer); 
}


void ApplicationImpl::onDetachTimer(System::Timer& timer )
{ 
    _timerQueue.removeTimer(timer); 
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

    bool isActive = true;
    if( _selector.waitForWake(timeout) )
        isActive = _eventQueue.processEvents( this->eventReceived() );

    return isActive;
}

}}
