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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#include "ApplicationImpl.h"
#include "MainWindowImpl.h"
#include "MainWindowView.h"
#include "PixmapSurfaceImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Window.h>

namespace Pt {

namespace Hmi {

MainWindowImpl::MainWindowImpl(Window::Type type)
: _window(nil)
, _view(nil)
, _windowStyle(0)
, _keyEvent(0)
, _mouseEvent(0)
, _level(0)
, _topMost(false)
{   
    MainWindowView* view = [[MainWindowView alloc] initWithImpl: this ];
    _view = view;

    Gfx::PointF at(0, 0);
    Gfx::SizeF size(100, 50);

    _windowStyle = NSWindowStyleMaskTitled |
                   NSWindowStyleMaskClosable |
                   NSWindowStyleMaskMiniaturizable |
                   NSWindowStyleMaskResizable;
    
    _window = [[NSWindow alloc] initWithContentRect:NSMakeRect(at.x(), 
                                                               at.y(), 
                                                               size.width(), 
                                                               size.height()) 
                                                    styleMask:_windowStyle 
                                                    backing:NSBackingStoreBuffered 
                                                    defer:NO];
    
    [_window setReleasedWhenClosed: NO];
    [_window setAcceptsMouseMovedEvents:YES];
    [_window setInitialFirstResponder: view];
    [_window setContentView: view];    
    [_window setDelegate: view];

    _level = [_window level];
}


MainWindowImpl::~MainWindowImpl()
{
    if( _window == nil )
        return;

    [_window close];

    [_window release];
    _window = nil;

    [_view release];
    _view = nil;
}


Gfx::PointF MainWindowImpl::toScreen(const Gfx::PointF& pos) const
{
    NSPoint p = NSMakePoint(pos.x(), pos.y());
    p = [ _window convertPointToScreen: p ];
    return Gfx::PointF(p.x, p.y);
}


Gfx::PointF MainWindowImpl::fromScreen(const Gfx::PointF& pos) const
{
    NSPoint p = NSMakePoint(pos.x(), pos.y());
    p = [ _window convertPointFromScreen: p ];
    return Gfx::PointF(p.x, p.y);
}


void MainWindowImpl::show(bool visible)
{
    std::clog << "SHOW: " << visible << std::endl;

    if(visible)
    {
        //[NSApp activateIgnoringOtherApps:YES];
        [_view setHidden:NO];

        [_window orderFront: nil];
        [_window makeKeyWindow];
        [_window makeMainWindow];
    }
    else
    {
        [_window orderOut:_window];
        [_view setHidden:YES];
    }
}


void MainWindowImpl::close()
{
    [_window close];
}


void MainWindowImpl::paint(const Gfx::RectF& rect)
{
    std::clog << "PAINT" << std::endl;
    [_view setNeedsDisplay:YES];
}


void MainWindowImpl::activate()
{
    [_window makeKeyWindow];
    [_window makeMainWindow];
}


void MainWindowImpl::enable(bool e)
{
    // TODO
}


void MainWindowImpl::setTopMost(bool onTop)
{
    if(onTop)
    {
        [_window setLevel: NSMainMenuWindowLevel];
    }
    else
    {
        [_window setLevel: NSNormalWindowLevel];
    }
}


void MainWindowImpl::move(const Gfx::PointF& p)
{
    CGFloat screenHeight = [[NSScreen mainScreen] frame].size.height;
    CGFloat windowHeight = [_window frame].size.height;

    CGFloat y = screenHeight - p.y() - windowHeight;
    NSPoint origin = NSMakePoint(p.x(), y);
    
    [_window setFrameOrigin:origin];
}


void MainWindowImpl::resize(const Gfx::SizeF& size)
{
    //TODO: use
    //- (NSRect)contentRectForFrameRect:(NSRect)windowFrame
    //- (NSRect)frameRectForContentRect:(NSRect)windowContent

    NSRect rect = [_window frame];
    rect.origin.y += rect.size.height - size.height();
    rect.size.width = size.width();
    rect.size.height = size.height();

    [_window setFrame:rect display:NO animate:NO];
}


void MainWindowImpl::setIcon(const Gfx::Image& icon)
{
}


void MainWindowImpl::setTitle(const std::string& text)
{
    _title = text;

    NSString* title = [NSString stringWithCString:_title.c_str() 
                                encoding:[NSString defaultCStringEncoding]];
    [_window setTitle: title];
}


void MainWindowImpl::setMinimumSize(const Gfx::SizeF& s)
{
}


void MainWindowImpl::setMaximumSize(const Gfx::SizeF& s)
{
}


void MainWindowImpl::setState(Window::State s)
{
    switch(s)
    {
        case Window::Normal:
            if([_window isMiniaturized])
                [_window deminiaturize:_window];
            break;

        case Window::Maximized:
            [_window setFrame: [[NSScreen mainScreen] frame] display:YES];
            break;

        case Window::Minimized:
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


Window* MainWindowImpl::findWindow(NSWindow* wnd)
{
    const std::vector<Window*>& windows = Application::instance().screen().windows();

    std::vector<Window*>::const_iterator it;
    for(it = windows.begin(); it != windows.end(); ++it)
    {
        Window* window = *it;

        if( window->impl() && window->impl()->window() == wnd )
            return window;
    }
    
    return 0;
}


void MainWindowImpl::onPaint(const NSRect& rect)
{
    Window* window = findWindow(_window);
    if( ! window )
        return;

    std::clog << "ON PAINT: " << rect.size.width << "x" 
                              << rect.size.height << std::endl;

    std::clog << "drawSurface: " <<  window->surface().pixmapImpl()->context()
              << std::endl;

    Pt::Hmi::PixmapSurfaceImpl* pixmap = window->surface().pixmapImpl();
    CGContextRef pixmapContext = pixmap->context();
    CGImageRef image = CGBitmapContextCreateImage(pixmapContext);

    NSGraphicsContext* graphicsContext = [NSGraphicsContext currentContext];
    CGContextRef currentContext = [graphicsContext CGContext];

    CGContextDrawImage(currentContext, rect, image);
    CGImageRelease(image);
}


void MainWindowImpl::onKeyDown(int keyCode)
{
    Window* window = findWindow(_window);
    if( ! window )
        return;

    Pt::uint64_t vid =  window->vid();

    Key::Modifiers modifiers;
    Key key(modifiers, keyCode);

    _keyEvent.setPress(key, keyCode);
    _keyEvent.setId(vid);

    Application::instance().loop().commitEvent(_keyEvent);
}


void MainWindowImpl::onKeyUp(int keyCode)
{
    Window* window = findWindow(_window);
    if( ! window )
        return;

    Pt::uint64_t vid =  window->vid();

    Key::Modifiers modifiers;
    Key key(modifiers, keyCode);

    _keyEvent.setRelease(key, keyCode);
    _keyEvent.setId(vid);

    Application::instance().loop().commitEvent(_keyEvent);
}


void MainWindowImpl::onKeyModifier(unsigned int mask)
{
    // _keyEvent.setUnicode(0);
    // _keyEvent.setAlt((mask & NSAlternateKeyMask) == NSAlternateKeyMask);
    // _keyEvent.setShift(((mask & NSShiftKeyMask) == NSShiftKeyMask) | ((mask & NSAlphaShiftKeyMask) == NSAlphaShiftKeyMask));
    // _keyEvent.setCtrl(((mask & NSControlKeyMask) == NSControlKeyMask) | ((mask & NSCommandKeyMask) == NSCommandKeyMask));
    
    // Application::instance().loop().commitEvent(_keyEvent);
}


void MainWindowImpl::onLostFocus()
{
    //TODO: call onLostFocus if the window lost focus
    // if( _topMost )
    //     bringToFront();
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
    

void MainWindowImpl::onShow(bool v)
{
    Window* window = findWindow(_window);
    if( ! window )
        return;

    Pt::uint64_t vid =  window->vid();

    ShowEvent sev(vid, v);
    Application::instance().impl()->commitEvent(sev);

    window->invalidate();
}


void MainWindowImpl::onMove()
{
    Window* window = findWindow(_window);
    if( ! window )
        return;

    Pt::uint64_t vid =  window->vid();

    CGFloat screenHeight = [[NSScreen mainScreen] frame].size.height;
    CGFloat windowHeight = [_window frame].size.height;
    NSPoint origin = [_window frame].origin;

    double x = origin.x;
    double y = screenHeight - origin.y - windowHeight;
    std::clog << "MOVE: " << x << "," << y << std::endl;

    Pt::Gfx::PointF pos(x, y);
    pos = Application::instance().screen().toLogical(pos);

    MoveEvent ev(vid, pos);
    Application::instance().impl()->commitEvent( ev );     
}


void MainWindowImpl::onResize(const NSSize& frameSize)
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

    Window* window = findWindow(_window);
    if( ! window )
        return;

    Pt::uint64_t vid =  window->vid();

    Gfx::SizeF to(frameSize.width, frameSize.height);

    ResizeEvent rev(vid, to);
    Application::instance().impl()->commitEvent(rev);
           
    Gfx::RectF updateRect(Gfx::PointF(0,0), to);
    window->update(updateRect);

    // cocoa performs a paint/display right after a window resize, so we
    // need to process the window update now to avoid flicker
    // 
    // OR: override NSWwindow::setFrame to not perform a paint/display
    Application::instance().impl()->processEvents();
}


void MainWindowImpl::onClosing()
{
    Window* window = findWindow(_window);
    if( ! window )
        return;

    Pt::uint64_t vid =  window->vid();

    CloseEvent closeEvent(vid);
    window->processEvent(closeEvent);
}
    
} // namespace

} // namespace
