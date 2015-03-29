/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2013 Marc Boris Dürner
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
#include <Pt/Hmi/Application.h>
#include "ApplicationImpl.h"
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>

namespace Pt{
namespace Hmi{


WindowImpl::WindowImpl(PaintSurface* surface)
: _surface( surface )
, _showtitle( true )
, _topMost( false )
{
	_mouseEvent.buttons().resize(3);
    
	/* ToDO: Window position tracking timer. Use this:

		NSWindowDidBecomeMainNotification 
		NSWindowDidResignMainNotification 
		NSWindowDidMoveNotification 
		NSWindowDidResizeNotification

		in create().
	*/

	_timer.setActive(Application::instance().loop());
	_timer.timeout() += Pt::slot(*this, &ViewImpl::onPosition);

	create();
}


NSView* WindowImpl::view()
{
	return _view;
}

    
void WindowImpl::create()
{
	_window = nil;
	_view = [[WidgetView alloc] init: this ];

	Gfx::PointF at(20, 20);
	Gfx::SizeF size(400, 200);

	_windowStyle = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
	_window = [[Window alloc] initWithContentRect:NSMakeRect(at.x(), at.y(), size.width(), size.height()) styleMask:_windowStyle backing:NSBackingStoreBuffered defer:NO];
    
	[_window setReleasedWhenClosed: NO];
	[_window setAcceptsMouseMovedEvents:YES];
	[_window setInitialFirstResponder: _view];
	[_window setContentView: _view];    
    
	[_window makeKeyAndOrderFront:_window];
	[_view setHidden:NO];
		_level = [_window level];
	_visible = true;
	_timer.start(100);
}


WindowImpl::~WindowImpl()
{
	destroy();
}


void WindowImpl::destroy()
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


void WindowImpl::show()
{
	[_window setIsVisible:YES]
}
    

void WindowImpl::hide()
{
	[_window setIsVisible:NO]
}

void WindowImpl::render()
{
	[_view setNeedsDisplay:YES];
}

void WindowImpl::setPosition(const Gfx::PointF& p)
{
	NSRect windowRect =  [_window frame];
    
	int screenHeight = [[NSScreen mainScreen] frame].size.height;

    windowRect.origin.x = p.x();
    windowRect.origin.y = screenHeight - (p.y() + windowRect.size.height );

    [_window setFrame:windowRect display:YES animate:NO];
}


void WindowImpl::setSize(const Gfx::SizeF& size)
{
	NSRect windowRect =  [_window frame];
        
    windowRect.size.width = size.width();
    windowRect.size.height = size.height();
    [_window setFrame:windowRect display:YES animate:NO];
}

void WindowImpl::showTitle(bool p)
{
	_showtitle = p;
	setCaption( _title );
}

void WindowImpl::setCaption(const std::string& text)
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

void WindowImpl::showMinimizedButton(bool p)
{   
    if( p )
		windowStyle |=  NSMiniaturizableWindowMask;
	else
		windowStyle &=  ~NSMiniaturizableWindowMask;
    
    [_window setStyleMask: windowStyle];
}
  
void WindowImpl::showMaximizeButton(bool p)
{
	//TODO:
}
  
void WindowImpl::showSysMenu(bool p)
{
	//TODO:
}

void WindowImpl::setForceTopMost(bool force)
{
	_topMost = force;

	if( _topMost )
		bringToFront();
}
  
void WindowImpl::setWindowState(WindowState::Type p)
{
    switch(_model->WindowState.get())
    {
        case Pt::Hmi::WindowStateType::Normal:
            if([_window isMiniaturized])
                [_window deminiaturize:_window];
        break;
            
        case Pt::Hmi::WindowStateType::Maximazed:
            [_window setFrame: [[NSScreen mainScreen] frame] display:YES];
        break;
            
        case Pt::Hmi::WindowStateType::Minimized:
            [_window miniaturize: _window];
         break;
    }
}
  
void WindowImpl::setBorder(WindowBorder::Type p)
{	
	_windowStyle &= (~NSTitledWindowMask & ~NSClosableWindowMask & ~NSResizableWindowMask & ~NSBorderlessWindowMask);

	switch( p)
	{
		case Pt::Hmi::WindowBorderType::Sizeable:
		case Pt::Hmi::WindowBorderType::DialogSizeable:
		case Pt::Hmi::WindowBorderType::ToolSizeable:
		{//Sizeable			 
			_windowStyle |= NSTitledWindowMask| NSClosableWindowMask| NSResizableWindowMask;
		}
		break;
                
		case Pt::Hmi::WindowBorderType::Dialog:
		case Pt::Hmi::WindowBorderType::Tool:
		{//Fixed size
			_windowStyle |= NSTitledWindowMask| NSClosableWindowMask;
		}
		break;
            
		case Pt::Hmi::WindowBorderType::NoBorder:
		{
			_windowStyle |= NSBorderlessWindowMask;
		}

		default:
		break;
	}
    
	[_window setStyleMask: _windowStyle];
}
  
void WindowImpl::showInTaskbar(bool p)
{
	//TODO:
}
  
void WindowImpl::setIcon(const Pt::Gfx::ARgbImage& p)
{
	//TODO:
}

void WindowImpl::setEnable(bool e)
{
	//TODO:
}

void WindowImpl::bringToFront()
{
    [_window setLevel: NSFloatingWindowLevel];
}


void WindowImpl::onLostFocus()
{
	if( _topMost )
		brintToFront();
}
    
void WindowImpl::onLMouseUp(double x, double y)
{
    Pt::Gfx::PointF pos = convertMousePosition(x,y);
    
    _mouseEvent.buttons()[0].setState(DeviceButton::Released);
    _mouseEvent.setX(pos.x());
	_mouseEvent.setY(pos.y());
    _windowEvent.send(_mouseEvent);
}
    
void WindowImpl::onLMouseDown(double x, double y)
{
    Pt::Gfx::PointF pos = convertMousePosition(x,y);
    
    _mouseEvent.buttons()[0].setState(DeviceButton::Pressed);
    _mouseEvent.setX(pos.x());
	_mouseEvent.setY(pos.y());
	_windowEvent.send(_mouseEvent);
}
    
void WindowImpl::onMouseMove(double x, double y)
{
    Pt::Gfx::PointF pos = convertMousePosition(x,y);
    _mouseEvent.setX(pos.x());
    _mouseEvent.setY(pos.y());
    _windowEvent.send(_mouseEvent);
}      

Pt::Gfx::PointF WindowImpl::convertMousePosition(double x, double y)
{
    NSRect windowRect = [_window frame];
    double gx = x;
    double gy = (windowRect.size.height - y);
    return Pt::Gfx::PointF(gx,gy);
}
    
void WindowImpl::onKeyDown(int key)
{
    _keyEvent.setUnicode(key);
    
    _keyEvent.setState(KeyEvent::KeyDown);
    
	_windowEvent.send(_keyEvent);
}

void WindowImpl::onKeyUp(int key)
{
    _keyEvent.setUnicode(key);
    
    _keyEvent.setState(KeyEvent::KeyUp);
    
	_windowEvent.send(_keyEvent);
}
    
void WindowImpl::onSpezialKeyEvent(unsigned int mask)
{
    _keyEvent.setUnicode(0);
    _keyEvent.setAlt((mask & NSAlternateKeyMask) == NSAlternateKeyMask);
    _keyEvent.setShift(((mask & NSShiftKeyMask) == NSShiftKeyMask) | ((mask & NSAlphaShiftKeyMask) == NSAlphaShiftKeyMask));
    _keyEvent.setCtrl(((mask & NSControlKeyMask) == NSControlKeyMask) | ((mask & NSCommandKeyMask) == NSCommandKeyMask));
    
   	_windowEvent.send(_keyEvent);
}
    
    
void WindowImpl::onPosition()
{      
    
    int screenHeight = [[NSScreen mainScreen] frame].size.height;
    
    //Window
    NSRect windowRect = [_window frame];
    Pt::Gfx::PointF pos(windowRect.origin.x, screenHeight - (windowRect.origin.y + windowRect.size.height));
    
    if( pos.x()  == _positionEvent.position().x() && pos.y()  == _positionEvent.position().y() )
        return;
    
	_positionEvent.setPosition(pos);
	
    _windowEvent.send( _positionEvent );
}
    
void WindowImpl::onSize()
{   
    int screenHeight = [[NSScreen mainScreen] frame].size.height;
    
	if([_window isMiniaturized])
    {
		_resizeEvent.setState(WindowStateType::Minimized);
    }
    else
    {
        NSRect maxRect = [[NSScreen mainScreen] frame];
        NSRect currentRect = [_window frame];
        
        if( maxRect.size.width == currentRect.size.width && maxRect.size.height == currentRect.size.height &&
           maxRect.origin.x == currentRect.origin.x &&  maxRect.origin.y == currentRect.origin.y)
        {		
			_resizeEvent.setState(WindowStateType::Maximized);
        }
        else
        {
			_resizeEvent.setState(WindowStateType::Normal);
        }
    }

    //Window
    NSRect windowRect = [_window frame];
    _resizeEvent.setSize( Pt::Gfx::SizeF(windowRect.size.width,windowRect.size.height) );

	_windowEvent.send( _resizeEvent );
}

void WindowImpl::onClosing()
{
	CloseEvent closeEvent;
	_windowEvent.send( closeEvent );
}
    
}}
