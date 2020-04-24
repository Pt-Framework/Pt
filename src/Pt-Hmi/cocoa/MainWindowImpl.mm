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
#include "KeyMap.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/WindowStateEvent.h>

namespace Pt {

namespace Hmi {

MainWindowImpl::MainWindowImpl(Window::Type type)
: _window(nil)
, _view(nil)
, _windowStyle(0)
, _keyFlags(0)
, _keyEvent(0)
, _mouseEvent(0)
, _level(0)
{   
    MainWindowView* view = [[MainWindowView alloc] initWithImpl: this];
    _view = view;

    Gfx::PointF at(0, 0);
    Gfx::SizeF size(100, 50);

    switch(type)
    {
        case Window::Popup:
            _windowStyle = NSWindowStyleMaskTitled | 
                           NSWindowStyleMaskFullSizeContentView;
            break;

        default:
        case Window::Normal:
            _windowStyle = NSWindowStyleMaskTitled |
                           NSWindowStyleMaskClosable |
                           NSWindowStyleMaskMiniaturizable |
                           NSWindowStyleMaskResizable;
            break;
    }

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

    if(type == Window::Popup)
    {
        [_window setTitlebarAppearsTransparent: YES];
        [_window setTitleVisibility: NSWindowTitleHidden];
        [_window setOpaque:NO];
    }

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


double MainWindowImpl::scaleFactor() const
{
    return [_window backingScaleFactor];
}


void MainWindowImpl::setType(Window::Type type)
{
    // TODO: NSWindowStyleMaskBorderless for Popups
    
    switch(type)
    {
        case Window::Popup:
            _windowStyle = NSWindowStyleMaskTitled |
                           NSWindowStyleMaskFullSizeContentView;
            break;

        default:
        case Window::Normal:
            _windowStyle = NSWindowStyleMaskTitled |
                           NSWindowStyleMaskClosable |
                           NSWindowStyleMaskMiniaturizable |
                           NSWindowStyleMaskResizable;
            break;
    }

    [_window setStyleMask:_windowStyle];
    
    if(type == Window::Popup)
    {
        [_window setTitlebarAppearsTransparent: YES];
        [_window setTitleVisibility: NSWindowTitleHidden];
        [_window setOpaque:NO];
    }
    else
    {
        [_window setTitlebarAppearsTransparent: NO];
        [_window setTitleVisibility: NSWindowTitleVisible];
        [_window setOpaque:YES];
    }
}


Gfx::PointF MainWindowImpl::toScreen(const Gfx::PointF& pos) const
{
    //std::clog << "TO SCREEN POS: " << pos.y() << std::endl;

    CGFloat viewHeight = [_view frame].size.height;
    double y = viewHeight - pos.y();
    //std::clog << "TO SCREEN VH: " << viewHeight << " -> " << y << std::endl;

    NSPoint p = NSMakePoint(pos.x(), y);
    p = [ _window convertPointToScreen: p ];

    CGFloat screenHeight = [[NSScreen mainScreen] frame].size.height;
    y = screenHeight - p.y;
    //std::clog << "TO SCREEN SH: " << screenHeight << " -> " << y << std::endl;

    return Gfx::PointF(p.x, y);
}


Gfx::PointF MainWindowImpl::fromScreen(const Gfx::PointF& pos) const
{   
    //std::clog << "FROM SCREEN POS: " << pos.y() << std::endl;
    
    CGFloat screenHeight = [[NSScreen mainScreen] frame].size.height;
    double y = screenHeight - pos.y();
    //std::clog << "FROM SCREEN SH: " << screenHeight << " -> " << y << std::endl;

    NSPoint p = NSMakePoint(pos.x(), y);
    p = [ _window convertPointFromScreen: p ];

    CGFloat viewHeight = [_view frame].size.height;
    y = viewHeight - p.y;
    //std::clog << "FROM SCREEN VH: " << viewHeight << " -> " << y << std::endl;
    
    return Gfx::PointF(p.x, y);
}


void MainWindowImpl::show(bool visible)
{
    //std::clog << "SHOW: " << visible << std::endl;

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
    [_window performClose:nil];
}


void MainWindowImpl::paint(const Gfx::RectF& rect)
{
    NSRect frameRect = [_window frame];
    NSRect contentRect = [_window contentRectForFrameRect:frameRect];
    CGFloat contentHeight = contentRect.size.height;

    CGFloat x = rect.x();
    CGFloat y = contentHeight - (rect.y() + rect.height());
    CGFloat w = rect.width();
    CGFloat h = rect.height();
    
    //std::clog << "PAINT: " << x << "," << y <<
    //                   " " << w << "x" << h << std::endl;

    NSRect invalidRect = NSMakeRect(x, y, w, h);
    [_view setNeedsDisplayInRect:invalidRect ];
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
    //std::clog << "MOVE: " << p.x() << "," 
    //                      << p.y() << std::endl;

    double scaling = scaleFactor();

    CGFloat screenHeight = [[NSScreen mainScreen] frame].size.height;
    CGFloat windowHeight = [_window frame].size.height;

    CGFloat y = screenHeight - p.y() / scaling - windowHeight;
    NSPoint origin = NSMakePoint(p.x() / scaling, y);

    [_window setFrameOrigin:origin];
}


void MainWindowImpl::resize(const Gfx::SizeF& size)
{
    //std::clog << "RESIZE: " << size.width() << "," 
    //                        << size.height() << std::endl;

    double scaling = scaleFactor();

    NSRect frameRect = [_window frame];
    NSRect contentRect = [_window contentRectForFrameRect:frameRect];

    contentRect.origin.y += contentRect.size.height - size.height() / scaling;
    
    contentRect.size.width = size.width() / scaling;
    contentRect.size.height = size.height() / scaling;

    frameRect = [_window frameRectForContentRect:contentRect];
    [_window setFrame:frameRect display:NO animate:NO];
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
    NSSize minSize = NSMakeSize( s.width(), s.height() );
    [_window setMinSize:minSize];
}


void MainWindowImpl::setMaximumSize(const Gfx::SizeF& s)
{
    NSSize maxSize = NSMakeSize( s.width(), s.height() );
    [_window setMaxSize:maxSize];
}


void MainWindowImpl::setState(Window::State s)
{
    switch(s)
    {
        case Window::Normal:
            if( [_window isMiniaturized] )
                [_window deminiaturize: nil];
            
            if( [_window isZoomed] )
                [_window zoom: nil];
            
            break;

        case Window::Maximized:
            if( ! [_window isZoomed] )
                [_window zoom: nil];
            break;

        case Window::Minimized:
            if( ! [_window isMiniaturized] )
                [_window miniaturize: nil];
            break;
    }
}


void MainWindowImpl::grabPointer()
{
  // pointer is always tracked, even if its outside the window
}


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

    //std::clog << "ON PAINT: " << rect.size.width << "x" 
    //                          << rect.size.height << std::endl;

    Pt::Hmi::PixmapSurfaceImpl* pixmap = window->surface().pixmapImpl();
    CGContextRef pixmapContext = pixmap->context();
    
    CGImageRef image = CGBitmapContextCreateImage(pixmapContext);
    CGFloat imageHeight = CGImageGetHeight(image);

    CGFloat subImageX = rect.origin.x * scaleFactor();
    CGFloat subImageY = rect.origin.y * scaleFactor();
    CGFloat subImageWidth = rect.size.width * scaleFactor();
    CGFloat subImageHeight = rect.size.height * scaleFactor();
    
    CGRect subRect = CGRectMake(subImageX,
                                imageHeight - subImageY - subImageHeight,
                                subImageWidth, 
                                subImageHeight);
    
    CGImageRef subImage = CGImageCreateWithImageInRect(image, subRect);

    NSGraphicsContext* graphicsContext = [NSGraphicsContext currentContext];
    CGContextRef windowContext = [graphicsContext CGContext];

    CGContextDrawImage(windowContext, rect, subImage);
    CGImageRelease(image);
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
    //std::clog << "ON MOVE: " << x << "," << origin.y << std::endl;

    Pt::Gfx::PointF pos(x, y);
    pos = window->toLogical(pos);

    MoveEvent ev(vid, pos);
    Application::instance().impl()->commitEvent( ev );
}


void MainWindowImpl::onResize(const NSSize& viewSize)
{   
    //std::clog << "RESIZE: " << viewSize.width << "x" 
    //                        << viewSize.height << std::endl;

    Window* window = findWindow(_window);
    if( ! window )
        return;

    Pt::uint64_t vid =  window->vid();

    Window::State wstate = Window::Normal;

    if( [_window isZoomed] )
    {		
        wstate = Window::Maximized;
    }
    else if( [_window isMiniaturized] )
    {
        wstate = Window::Minimized;
    }

    if(window->state() != wstate)
    {
        WindowStateEvent wse(vid, wstate);
        Application::instance().impl()->commitEvent(wse);
    }

    Gfx::SizeF to(viewSize.width, 
                  viewSize.height);

    ResizeEvent rev(vid, to);
    Application::instance().impl()->commitEvent(rev);
           
    Gfx::RectF updateRect(Gfx::PointF(0, 0), to);
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


void MainWindowImpl::onKeyDown(unsigned vkey, Pt::Char ch)
{
    //std::clog << "KEY DOWN: " << vkey << std::endl;

    Window* window = findWindow(_window);
    if( ! window )
        return;

    Pt::uint64_t vid =  window->vid();
    
    Pt::uint32_t keyCode = Key::NoKey;
    if(vkey < keyMapSize)
    {
        keyCode = keyMap[vkey];
        
        if(keyCode == Key::NoKey)
            keyCode = toupper(ch).value();
    }

    Key key(_keyModifiers, keyCode);
    _keyEvent.setPress(key, ch);
    _keyEvent.setId(vid);

    Application::instance().loop().commitEvent(_keyEvent);
}


void MainWindowImpl::onKeyUp(unsigned vkey, Pt::Char ch)
{
    //std::clog << "KEY UP: " << vkey << std::endl;

    Window* window = findWindow(_window);
    if( ! window )
        return;

    Pt::uint64_t vid =  window->vid();
    
    Pt::uint32_t keyCode = Key::NoKey;
    if(vkey < keyMapSize)
    {
        keyCode = keyMap[vkey];
        
        if(keyCode == Key::NoKey)
            keyCode = toupper(ch).value();
    }

    Key key(_keyModifiers, keyCode);
    _keyEvent.setRelease(key, ch);
    _keyEvent.setId(vid);

    Application::instance().loop().commitEvent(_keyEvent);
}


void MainWindowImpl::onKeyModifier(unsigned int mask)
{
    //std::clog << "KEY MODIFIER: " << mask << std::endl;

    bool wasShift = (_keyFlags & NSEventModifierFlagShift) == NSEventModifierFlagShift;
    bool wasControl = (_keyFlags & NSEventModifierFlagControl) == NSEventModifierFlagControl;
    bool wasAlt = (_keyFlags & NSEventModifierFlagOption) == NSEventModifierFlagOption;
    bool wasMeta = (_keyFlags & NSEventModifierFlagCommand) == NSEventModifierFlagCommand;
    
    _keyFlags = mask;
    
    bool shift = (_keyFlags & NSEventModifierFlagShift) == NSEventModifierFlagShift;
    bool control = (_keyFlags & NSEventModifierFlagControl) == NSEventModifierFlagControl;
    bool alt = (_keyFlags & NSEventModifierFlagOption) == NSEventModifierFlagOption;
    bool meta = (_keyFlags & NSEventModifierFlagCommand) == NSEventModifierFlagCommand;

    _keyModifiers.clear();
    if(shift)
        _keyModifiers.add(Key::Shift);
    if(control)
        _keyModifiers.add(Key::Control);
    if(alt)
        _keyModifiers.add(Key::Alt);
    if(meta)
        _keyModifiers.add(Key::Meta);

    //
    // send key event for modifier keys
    //
    Window* window = findWindow(_window);
    if( ! window )
        return;

    Pt::uint64_t vid =  window->vid();

    Pt::uint32_t keyCode = Key::NoKey;
    if(wasShift != shift)
        keyCode = Key::ShiftKey;
    if(wasControl != control)
        keyCode = Key::ControlKey;
    if(wasAlt != alt)
        keyCode = Key::AltKey;
    if(wasMeta != meta)
        keyCode = Key::MetaKey;

    if(keyCode == Key::NoKey)
        return;

    bool wasPressed = ( ! wasShift   && shift)   || 
                      ( ! wasControl && control) ||
                      ( ! wasAlt     && alt)     || 
                      ( ! wasMeta    && meta);

    Key key(_keyModifiers, keyCode);
    _keyEvent.setId(vid);

    if(wasPressed)
        _keyEvent.setRelease( key, Pt::Char() );
    else
        _keyEvent.setPress( key, Pt::Char() );

    Application::instance().loop().commitEvent(_keyEvent);
}


void MainWindowImpl::onLMouseDown(double x, double y)
{
    //std::clog << "MOUSE PRESS: " << x << ", " << y << std::endl;

    Window* window = findWindow(_window);
    if( ! window )
        return;

    Pt::uint64_t vid =  window->vid();

    CGFloat height = [_window contentRectForFrameRect:[_window frame]].size.height;
    y = height - y;

    double scaling = Application::instance().scaleFactor();

    Pt::Gfx::PointF pos(x / scaling, 
                        y / scaling);

    _mouseEvent.setPress(MouseEvent::Left);
    _mouseEvent.setPosition(pos);
    _mouseEvent.setId(vid);

    Application::instance().processMouseEvent(_mouseEvent);
}


void MainWindowImpl::onLMouseUp(double x, double y)
{
    //std::clog << "MOUSE RELEASE: " << x << ", " << y << std::endl;

    Window* window = findWindow(_window);
    if( ! window )
        return;

    Pt::uint64_t vid =  window->vid();
    
    CGFloat height = [_window contentRectForFrameRect:[_window frame]].size.height;
    y = height - y;

    double scaling = Application::instance().scaleFactor();

    Pt::Gfx::PointF pos(x / scaling, 
                        y / scaling);

    _mouseEvent.setRelease(MouseEvent::Left);
    _mouseEvent.setPosition(pos);
    _mouseEvent.setId(vid);

    Application::instance().processMouseEvent(_mouseEvent);
}


void MainWindowImpl::onMouseMove(double x, double y)
{
    //std::clog << "MOUSE MOVE: " << x << ", " << y << std::endl;

    Window* window = findWindow(_window);
    if( ! window )
        return;

    Pt::uint64_t vid =  window->vid();
    
    CGFloat height = [_window contentRectForFrameRect:[_window frame]].size.height;
    y = height - y;

    double scaling = Application::instance().scaleFactor();

    Pt::Gfx::PointF pos(x / scaling, 
                        y / scaling);

    _mouseEvent.setMove();
    _mouseEvent.setPosition(pos);
    _mouseEvent.setId(vid);

    Application::instance().processMouseEvent(_mouseEvent);
}

} // namespace

} // namespace
