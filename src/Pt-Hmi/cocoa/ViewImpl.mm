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
#include "ViewImpl.h"
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Application.h>
#include "ApplicationImpl.h"
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/WindowController.h>
#include <Pt/Hmi/View.h>
#include "WidgetView.h"
#include "Window.h"

namespace Pt{
namespace Hmi{


ViewImpl::ViewImpl()
: _model(0)
, _visible(false)
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

NSView* ViewImpl::view()
{
	return _view;
}
    
void ViewImpl::create()
{
    _window = nil;
    _view = [[WidgetView alloc] init: this ];

	Gfx::PointF at(20, 20);
	Gfx::SizeF size(400, 200);

	_window = [[Window alloc] initWithContentRect:NSMakeRect(at.x(), at.y(), size.width(), size.height()) styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask backing:NSBackingStoreBuffered defer:NO];
    
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

ViewImpl::~ViewImpl()
{
	destroy();
}

void ViewImpl::destroy()
{
    if(_window == nil)
        return;
    
    _timer.stop();
    [_window close];
    [_window release];
    [_view release];
	_view = nil;
    _window = nil;
    
    //Notife closed
    _controller->ClosedAction.send(_controller);
}    
    
void ViewImpl::writeWindowProperties()
{        
	//Visibility
	if(_model->Visible.get() && !_visible)
	{
		[_window makeKeyAndOrderFront:_window];
		[NSApp activateIgnoringOtherApps:YES];
		_visible = true;
	}
        
	if(!_model->Visible.get() && _visible)
	{
		[_window orderOut:_window];
		_visible = false;
		return;
	}
    
    //Title
    NSString* title = [NSString stringWithCString:_model->Caption.get().c_str() encoding:[NSString defaultCStringEncoding]];
		
    if( _model->ShowTitle.get())
        [_window setTitle: title];
    else
        [_window setTitle: title];
        
    //Border styl
    int windowStyle = 0;
    
    switch( _model->Border.get())
    {
        case Pt::Hmi::WindowBorderType::Sizeable:
        case Pt::Hmi::WindowBorderType::DialogSizeable:
        case Pt::Hmi::WindowBorderType::ToolSizeable:
        {//Sizeable
            windowStyle = NSTitledWindowMask| NSClosableWindowMask| NSResizableWindowMask;
        }
        break;
                
        case Pt::Hmi::WindowBorderType::Dialog:
        case Pt::Hmi::WindowBorderType::Tool:
        {//Fixed size
            windowStyle =  NSTitledWindowMask| NSClosableWindowMask;
        }
        break;
            
        case Pt::Hmi::WindowBorderType::NoBorder:
        {
             windowStyle = NSBorderlessWindowMask;
        }

        default:
        break;
    }
    
    //Windows state
    if( _model->ShowMinimizeButton.get())
    {
        windowStyle |=  NSMiniaturizableWindowMask;
    }

    
    [_window setStyleMask: windowStyle];
}

    
void ViewImpl::onLMouseUp(double x, double y)
{
    Pt::Gfx::PointF pos = convertMousePosition(x,y);
    
    _mouseEvent.buttons()[0].setState(DeviceButton::Released);
    _mouseEvent.setX(pos.x());
	_mouseEvent.setY(pos.y());
    Application::instance().systemEvent().send(_mouseEvent);
}
    
void ViewImpl::onLMouseDown(double x, double y)
{
    Pt::Gfx::PointF pos = convertMousePosition(x,y);
    
    _mouseEvent.buttons()[0].setState(DeviceButton::Pressed);
    _mouseEvent.setX(pos.x());
	_mouseEvent.setY(pos.y());

   Application::instance().systemEvent().send(_mouseEvent);
}
    
void ViewImpl::onMouseMove(double x, double y)
{
    Pt::Gfx::PointF pos = convertMousePosition(x,y);
    _mouseEvent.setX(pos.x());
    _mouseEvent.setY(pos.y());
    Application::instance().systemEvent().send(_mouseEvent);
}
    
    
void ViewImpl::centerWindowTo(NSRect* parentRect)
{
    int screenHeight = [[NSScreen mainScreen] frame].size.height;
    
    int horizontal = parentRect->size.width/2 +parentRect->origin.x;
    int vertical = screenHeight - (parentRect->size.height/2 + parentRect->origin.y);
    
    Pt::Gfx::Size mySize  = _model->fromUnit(_model->Size.get());
        
    int posX = horizontal - (mySize.width()/2);
    int posY = vertical - (mySize.height()/2);
        
    _model->Position.set(_model->toUnit(Pt::Gfx::Point( posX,posY)));
}
    
void ViewImpl::writeWindowSizeAndPos(bool firstShow)
{
    if(firstShow)
    {
        switch(_model->WindowStartPostion.get())
        {
            case WindowStartPositionType::Manual:
                break;
                
            case WindowStartPositionType::CenterParent:
            {
                WindowController* parent = _controller->windowParent();
				
                if( parent == 0)
                {
				    NSRect rect = [[NSScreen mainScreen] frame];
                    centerWindowTo(&rect);
                }
                else
                {
				    
                    ViewImpl* parentImpl = 0;
                    
                    for(size_t i = 0; i < parent->outputDevices().size(); ++i)
                    {
                        View* parentDevice = dynamic_cast<View*>(parent->outputDevices()[i]);
                        
                        if( parentDevice == 0)
                            continue;
                        
                        parentImpl = dynamic_cast<ViewImpl*>(parentDevice->impl());
						
                        if( parentImpl != 0)
                            break;
                    }
                
				     NSRect rect = [parentImpl->window() frame];
				
                    centerWindowTo(&rect);
                }
            }
                break;
                
            case WindowStartPositionType::CenterScreen:
			{
			NSRect rect = [[NSScreen mainScreen] frame];
                centerWindowTo(&rect);
                    }
			
                break;
        }
    }
    
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
    
    int screenHeight = [[NSScreen mainScreen] frame].size.height;
    NSRect windowRect =  [_window frame];
    
    //Position
    windowRect.origin.x = _model->Position.get().x();
    windowRect.origin.y = screenHeight - (_model->Position.get().y() + _model->Size.get().height());
    
    //Size
    windowRect.size.width = _model->Size.get().width();
    windowRect.size.height = _model->Size.get().height();
    [_window setFrame:windowRect display:YES animate:NO];
    
}
    
Pt::Gfx::PointF ViewImpl::convertMousePosition(double x, double y)
{
    NSRect windowRect = [_window frame];
    double gx = x;
    double gy = (windowRect.size.height - y);
    return Pt::Gfx::PointF(gx,gy);
}
    
void ViewImpl::onKeyDown(int key)
{
    _keyEvent.setUnicode(key);
    
    _keyEvent.setState(KeyEvent::KeyDown);
    
	Application::instance().systemEvent().send(_keyEvent);
}

void ViewImpl::onKeyUp(int key)
{
    _keyEvent.setUnicode(key);
    
    _keyEvent.setState(KeyEvent::KeyUp);
    
	Application::instance().systemEvent().send(_keyEvent);
}
    
void ViewImpl::onSpezialKeyEvent(unsigned int mask)
{
    _keyEvent.setUnicode(0);
    _keyEvent.setAlt((mask & NSAlternateKeyMask) == NSAlternateKeyMask);
    _keyEvent.setShift(((mask & NSShiftKeyMask) == NSShiftKeyMask) | ((mask & NSAlphaShiftKeyMask) == NSAlphaShiftKeyMask));
    _keyEvent.setCtrl(((mask & NSControlKeyMask) == NSControlKeyMask) | ((mask & NSCommandKeyMask) == NSCommandKeyMask));
    
   	Application::instance().systemEvent().send(_keyEvent);
}
    
void ViewImpl::checkModal()
{
    if(_model->TopMost.get())
        [_window setLevel: NSFloatingWindowLevel];
    else
        [_window setLevel: NSNormalWindowLevel];
}
    
void ViewImpl::onPosition()
{
    if( _model == 0)
        return;
    
    if([_window isMiniaturized])
    {
        _model->WindowState = WindowStateType::Minimized;
    }
    else
    {
        NSRect maxRect = [[NSScreen mainScreen] frame];
        NSRect currentRect = [_window frame];
        
        if( maxRect.size.width == currentRect.size.width && maxRect.size.height == currentRect.size.height &&
           maxRect.origin.x == currentRect.origin.x &&  maxRect.origin.y == currentRect.origin.y)
        {
            _model->WindowState = WindowStateType::Minimized;
        }
        else
        {
            _model->WindowState = WindowStateType::Normal;
        }
    }

    
    int screenHeight = [[NSScreen mainScreen] frame].size.height;
    
    //Window
    NSRect windowRect = [_window frame];
    Pt::Gfx::PointF pos(windowRect.origin.x, screenHeight - (windowRect.origin.y + windowRect.size.height));
    
    if( pos.x()  == _model->Position.get().x() && pos.y()  == _model->Position.get().y())
        return;
    
    _model->Position = pos;
}
    
void ViewImpl::onPositionAndSize()
{
    if( _model == 0)
        return;
    
    int screenHeight = [[NSScreen mainScreen] frame].size.height;
    
    //Window
    NSRect windowRect = [_window frame];
    _model->Position = Pt::Gfx::PointF(windowRect.origin.x,screenHeight - (windowRect.origin.y + windowRect.size.height));
    _model->Size = Pt::Gfx::SizeF(windowRect.size.width,windowRect.size.height);
}
    
bool ViewImpl::onCanClose()
{
    bool canClose = false;
    
    _controller->ClosingAction.send(_controller, canClose);

    if(canClose)
    {
        destroy();
    }
    return canClose;
}

Pt::Gfx::Painter* ViewImpl::nativePainter()
{
	return _nativePainter;
}
    
void ViewImpl::output(Pt::Hmi::Controller* controller, Pt::Hmi::Model* model)
{
        bool firstShow = (_model == 0);
        _model = dynamic_cast<WindowModel*>(model);
        _controller = dynamic_cast<WindowController*>(controller);
        
        _mouseEvent.setController(_controller);
        _keyEvent.setController(_controller);
    
        //Create/Destroy handling
        if(_model->Closed.get())
        {
            if(_view != nil)
                destroy();
            
            _model= 0;
            return;
        }
        else
        {
            if(_view == nil)
            {
                if(_model->Visible.get())
                    create();
                else
                    return;
            }
        }
        
        //Size and position handling
        writeWindowProperties();
        checkModal();
        writeWindowSizeAndPos(firstShow);
        
        //Redraw
        [_view setNeedsDisplay:YES];
}
    
}}
