/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2013 Marc Boris Dürner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 * USA
 */

#include "ApplicationImpl.h"
#include "MainWindowImpl.h"
#include "WidgetView.h"
#include "CoWindow.h"

#include <Pt/Hmi/Application.h>

namespace Pt {

namespace Hmi {

MainWindowImpl::MainWindowImpl(Window::Type type)
: _windowStyle(0)
, _owner(0)
, _topMost(false)
{
	_pointerEvent.buttons().resize(3);
    
	/* ToDO: Window position tracking timer. Use this:

		NSWindowDidBecomeMainNotification 
		NSWindowDidResignMainNotification 
		NSWindowDidMoveNotification 
		NSWindowDidResizeNotification

		in create().
	*/

	_timer.setActive(Application::instance().loop());
    //TODO: remove timer for window position traking, 
    //      use position changed system event
	_timer.timeout() += Pt::slot(*this, &WindowImpl::onPosition);

	_window = nil;
	_view = [[WidgetView alloc] init: this ];

	Gfx::PointF at(20, 20);
	Gfx::SizeF size(400, 200);

	_windowStyle = NSTitledWindowMask | 
                   NSClosableWindowMask | 
                   NSMiniaturizableWindowMask | 
                   NSResizableWindowMask;
    
    _window = [[CoWindow alloc] initWithContentRect:NSMakeRect(at.x(), 
                                                               at.y(), 
                                                               size.width(), 
                                                               size.height()) 
                                                    styleMask:_windowStyle 
                                                    backing:NSBackingStoreBuffered 
                                                    defer:NO];
    
	[_window setReleasedWhenClosed: NO];
	[_window setAcceptsMouseMovedEvents:YES];
	[_window setInitialFirstResponder: _view];
	[_window setContentView: _view];    
    
	[_window makeKeyAndOrderFront:_window];
	[_view setHidden:NO];
    _level = [_window level];
	_timer.start(100);
}


MainWindowImpl::~MainWindowImpl()
{
    if( _window == nil )
        return;
    
    _timer.stop();
    [_window close];
    [_window release];
    [_view release];
	_view = nil;
    _window = nil;
}


void MainWindowImpl::setWindow(Window& w)
{
    _owner = &w;
}


Gfx::PointF MainWindowImpl::toScreen(const Gfx::PointF& pos) const
{
    NSPoint p = NSMakePoint(pos.x(), pos.y());
    p = [ _window convertPointToScreen: p ];
}


Gfx::PointF MainWindowImpl::fromScreen(const Gfx::PointF& pos) const
{
    NSPoint p = NSMakePoint(pos.x(), pos.y());
    p = [ _window convertPointFromScreen: p ];
}


void MainWindowImpl::show(bool v)
{
    if(v)
    {
        [ _window makeKeyAndOrderFront:_window ];
        [ NSApp activateIgnoringOtherApps:YES ];
    }
    else
    {
        [ _window orderOut:_window ];
    }
}


void MainWindowImpl::close()
{
    [ _window close ];
}


void MainWindowImpl::paint(const Gfx::RectF& rect)
{
    [_view setNeedsDisplay:YES];
}


void MainWindowImpl::activate()
{
    [[NSApp mainWindow] makeKeyAndOrderFront:_window];
}


void MainWindowImpl::enable(bool e)
{
    // TODO
}


void MainWindowImpl::setTopMost(bool e)
{
    // TODO
}


void MainWindowImpl::move(const Gfx::PointF& p)
{
	NSRect windowRect = [_window frame];
    
	int screenHeight = [[NSScreen mainScreen] frame].size.height;

    windowRect.origin.x = p.x();
    windowRect.origin.y = screenHeight - (p.y() + windowRect.size.height );

    [_window setFrame:windowRect display:YES animate:NO];
}


void MainWindowImpl::resize(const Gfx::SizeF& size)
{
    //TODO: this is the client size
	NSRect windowRect = [_window frame];
        
    windowRect.size.width = size.width();
    windowRect.size.height = size.height();
    [_window setFrame:windowRect display:YES animate:NO];
}


void MainWindowImpl::setIcon(const Gfx::Image& icon)
{
}


void MainWindowImpl::setTitle(const std::string& text)
{
	_title = text;

    //Title
    if( _showtitle )
	{ 
		NSString* title = [NSString stringWithCString:_title.c_str() encoding:[NSString defaultCStringEncoding]];		    
        [_window setTitle: title];
	}
    else
	{
		NSString* title = [NSString stringWithCString:"" encoding:[NSString defaultCStringEncoding]];		    
        [_window setTitle: title];        
	}

}


void MainWindowImpl::setMinimumSize(const Gfx::SizeF& s)
{
}


void MainWindowImpl::setMaximumSize(const Gfx::SizeF& s)
{
}


void MainWindowImpl::setState(Window::State p)
{
    switch(p)
    {
        case Pt::Hmi::WindowState::Normal:
            if([_window isMiniaturized])
                [_window deminiaturize:_window];
        break;
            
        case Pt::Hmi::WindowState::Maximazed:
            [_window setFrame: [[NSScreen mainScreen] frame] display:YES];
        break;
            
        case Pt::Hmi::WindowState::Minimized:
            [_window miniaturize: _window];
         break;
    }
}


void MainWindowImpl::grabPointer()
{
   // TODO
}


//void MainWindowImpl::setBorder(WindowBorder::Type p)
// {	
// 	_windowStyle &= (~NSTitledWindowMask & ~NSClosableWindowMask & ~NSResizableWindowMask & ~NSBorderlessWindowMask);

// 	switch( p)
// 	{
// 		case Pt::Hmi::WindowBorder::Sizeable:
// 		case Pt::Hmi::WindowBorder::DialogSizeable:
// 		case Pt::Hmi::WindowBorder::ToolSizeable:
// 		{//Sizeable			 
// 			_windowStyle |= NSTitledWindowMask| NSClosableWindowMask| NSResizableWindowMask;
// 		}
// 		break;
                
// 		case Pt::Hmi::WindowBorder::Dialog:
// 		case Pt::Hmi::WindowBorder::Tool:
// 		{//Fixed size
// 			_windowStyle |= NSTitledWindowMask| NSClosableWindowMask;
// 		}
// 		break;
            
// 		case Pt::Hmi::WindowBorder::NoBorder:
// 		{
// 			_windowStyle |= NSBorderlessWindowMask;
// 		}

// 		default:
// 		break;
// 	}
    
// 	[_window setStyleMask: _windowStyle];
// }


void MainWindowImpl::onLostFocus()
{
    //TODO: call onLostFocus if the window lost focus
	// if( _topMost )
	// 	bringToFront();
}


void MainWindowImpl::onLMouseUp(double x, double y)
{
    // Pt::Gfx::PointF pos = convertMousePosition(x,y);
    
    // _pointerEvent.buttons()[0].setState(DeviceButton::Released);
    // _pointerEvent.setX(pos.x());
	// _pointerEvent.setY(pos.y());
    // _windowEvent.send(_pointerEvent);
}


void MainWindowImpl::onLMouseDown(double x, double y)
{
    // Pt::Gfx::PointF pos = convertMousePosition(x,y);
    
    // _pointerEvent.buttons()[0].setState(DeviceButton::Pressed);
    // _pointerEvent.setX(pos.x());
	// _pointerEvent.setY(pos.y());
	// _windowEvent.send(_pointerEvent);
}


void MainWindowImpl::onMouseMove(double x, double y)
{
    // Pt::Gfx::PointF pos = convertMousePosition(x,y);
    // _pointerEvent.setX(pos.x());
    // _pointerEvent.setY(pos.y());
    // _windowEvent.send(_pointerEvent);
}      


Pt::Gfx::PointF MainWindowImpl::convertMousePosition(double x, double y)
{
    int screenHeight = [[NSScreen mainScreen] frame].size.height;
    NSRect windowRect = [_window frame];
    double gx = x;;
    double gy = windowRect.size.height - y -18;// TODO: determinat the correct client rect
    return Pt::Gfx::PointF(gx,gy);
}


void MainWindowImpl::onKeyDown(int keyCode)
{
    Key::Modifiers modifiers;
    Key key(modifiers, keyCode);

    _keyEvent.setPress(key, keyCode);
    _keyEvent.setId( _owner->vid() );
    
	Application::instance().loop().commitEvent(_keyEvent);
}


void MainWindowImpl::onKeyUp(int key)
{
    Key::Modifiers modifiers;
    Key key(modifiers, keyCode);

    _keyEvent.setRelease(key, keyCode);
    _keyEvent.setId( _owner->vid() );
    
	Application::instance().loop().commitEvent(_keyEvent);
}


void MainWindowImpl::onSpezialKeyEvent(unsigned int mask)
{
    // _keyEvent.setUnicode(0);
    // _keyEvent.setAlt((mask & NSAlternateKeyMask) == NSAlternateKeyMask);
    // _keyEvent.setShift(((mask & NSShiftKeyMask) == NSShiftKeyMask) | ((mask & NSAlphaShiftKeyMask) == NSAlphaShiftKeyMask));
    // _keyEvent.setCtrl(((mask & NSControlKeyMask) == NSControlKeyMask) | ((mask & NSCommandKeyMask) == NSCommandKeyMask));
    
    // Application::instance().loop().commitEvent(_keyEvent);
}
    
    
void MainWindowImpl::onPosition()
{
    // int screenHeight = [[NSScreen mainScreen] frame].size.height;
    
    // //Window
    // NSRect windowRect = [_window frame];
    // Pt::Gfx::PointF pos(windowRect.origin.x,  (screenHeight - windowRect.origin.y - windowRect.size.height));
    
    // if( pos.x()  == _positionEvent.position().x() && pos.y()  == _positionEvent.position().y() )
    //     return;
    
	// _positionEvent.setPosition(pos);
	
    // _windowEvent.send( _positionEvent );
}
    
    
void MainWindowImpl::onSize()
{   
//     int screenHeight = [[NSScreen mainScreen] frame].size.height;
    
// 	if([_window isMiniaturized])
//     {
// 		_resizeEvent.setState(WindowState::Minimized);
//     }
//     else
//     {
//         NSRect maxRect = [[NSScreen mainScreen] frame];
//         NSRect currentRect = [_window frame];
        
//         if( maxRect.size.width == currentRect.size.width && maxRect.size.height == currentRect.size.height &&
//            maxRect.origin.x == currentRect.origin.x &&  maxRect.origin.y == currentRect.origin.y)
//         {		
// 			_resizeEvent.setState(WindowState::Maximazed);
//         }
//         else
//         {
// 			_resizeEvent.setState(WindowState::Normal);
//         }
//     }

//    // TODO: determinate the correct client rect
//     NSRect windowRect = [_window frame];
//     _resizeEvent.setSize( Pt::Gfx::SizeF(windowRect.size.width,windowRect.size.height) );

// 	_windowEvent.send( _resizeEvent );
}


void MainWindowImpl::onClosing()
{
	// CloseEvent closeEvent;
	// _windowEvent.send( closeEvent );
}
    
}

}
