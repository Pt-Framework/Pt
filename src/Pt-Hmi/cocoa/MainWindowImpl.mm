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
#include "ScreenImpl.h"
#include "MainWindowImpl.h"
#include "MainWindowView.h"
#include "PixmapSurfaceImpl.h"
#include "KeyMap.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Screen.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/WindowStateEvent.h>
#include <Pt/Hmi/PaintEvent.h>

namespace Pt {

namespace Hmi {

MainWindowImpl::MainWindowImpl(ScreenImpl& wm,  Window& w)
: WindowImpl(wm, w)
, _wm(wm)
, _client(w)
, _window(nil)
, _view(nil)
, _windowStyle(0)
, _level(0)
, _keyFlags(0)
{
    MainWindowView* view = [[MainWindowView alloc] initWithImpl: this];
    _view = view;

    Gfx::PointF at(0, 0);
    Gfx::SizeF size(100, 50);

    switch( w.type() )
    {
        case WindowType::Popup:
            _windowStyle = NSWindowStyleMaskBorderless;
            break;

        default:
        case WindowType::Default:
            _windowStyle = NSWindowStyleMaskTitled |
                           NSWindowStyleMaskClosable |
                           NSWindowStyleMaskMiniaturizable |
                           NSWindowStyleMaskResizable;
            break;
    }

    NSRect windowRect = NSMakeRect( at.x(), at.y(), 
                                    size.width(),  size.height() );

    _window = [[NSWindow alloc] initWithContentRect: windowRect 
                                          styleMask: _windowStyle 
                                            backing: NSBackingStoreBuffered 
                                              defer: NO];
    
    [_window setReleasedWhenClosed: NO];
    //[_window setAcceptsMouseMovedEvents:YES];
    [_window setInitialFirstResponder: view];
    [_window setContentView: view];    
    [_window setDelegate: view];

    _level = [_window level];

    Base::onSetParent(&wm);
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

void MainWindowImpl::onInit(Window& w)
{
    Gfx::PaintSurface& surface = this->surface();
    Gfx::PointF surfacePos(0, 0);
    w.setSurface(&surface, surfacePos);

    w.setNextResponder(this);

    double scaling = scaleFactor();
    
    RescaleEvent ev(w, scaling);
    w.processEvent(ev);
}


void MainWindowImpl::onRelease(Window& w)
{
    w.setNextResponder(0);
    w.setSurface( 0, Gfx::PointF() );
}


void MainWindowImpl::setType(WindowType type)
{
    switch(type)
    {
        case WindowType::Popup:
            _windowStyle = NSWindowStyleMaskBorderless;
            break;

        default:
        case WindowType::Default:
            _windowStyle = NSWindowStyleMaskTitled |
                           NSWindowStyleMaskClosable |
                           NSWindowStyleMaskMiniaturizable |
                           NSWindowStyleMaskResizable;
            break;
    }

    [_window setStyleMask:_windowStyle];
    
    // if(type == WindowType::Popup)
    // {
    //     [_window setTitlebarAppearsTransparent: YES];
    //     [_window setTitleVisibility: NSWindowTitleHidden];
    //     [_window setOpaque:NO];
    // }
    // else
    // {
    //     [_window setTitlebarAppearsTransparent: NO];
    //     [_window setTitleVisibility: NSWindowTitleVisible];
    //     [_window setOpaque:YES];
    // }
}


void MainWindowImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{
    double scaling = ev.scaleFactor();
    scaling *= [_window backingScaleFactor];

    RescaleEvent rev(*this, scaling);
    Base::onProcessRescaleEvent(rev);

    RescaleEvent rev2(_client, scaling);
    _client.processEvent(rev2);
}


void MainWindowImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
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


Gfx::PointF MainWindowImpl::onToWindow(const Window& w, 
                                       const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF MainWindowImpl::onFromWindow(const Window& w, 
                                         const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF MainWindowImpl::onToParent(const Gfx::PointF& pos) const
{
    return _wm.fromFrame(*this, pos); 
}
     
        
Gfx::PointF MainWindowImpl::onFromParent(const Gfx::PointF& pos) const
{ 
    return _wm.toFrame(*this, pos); 
}


void MainWindowImpl::paint(const Gfx::RectF& rect)
{
    Gfx::RectF r( rect.topLeft(), rect.size() );

    NSRect frameRect = [_window frame];
    NSRect contentRect = [_window contentRectForFrameRect:frameRect];
    CGFloat contentHeight = contentRect.size.height;

    CGFloat x = r.x();
    CGFloat y = contentHeight - (r.y() + r.height());
    CGFloat w = r.width();
    CGFloat h = r.height();
    
    //std::clog << "PAINT: " << x << "," << y <<
    //                   " " << w << "x" << h << std::endl;

    NSRect invalidRect = NSMakeRect(x, y, w, h);
    [_view setNeedsDisplayInRect:invalidRect ];
}


void MainWindowImpl::onRepaint(Window& w, const Gfx::RectF& rect)
{
    Gfx::PointF screenPos = toScreen( rect.topLeft() );
    
    Gfx::RectF screenRect( screenPos, rect.size() );
    _wm.repaint(screenRect);
}


void MainWindowImpl::onProcessPaintEvent(const PaintEvent& ev)
{
    Base::onProcessPaintEvent(ev);

    PaintEvent rev( _window, ev.rect() );
    _window.processEvent(rev);
}


void MainWindowImpl::onPaintEvent(const PaintEvent& ev)
{
    Base::onPaintEvent(ev);
}


//void MainWindowImpl::show(bool visible)
//{
//    //std::clog << "SHOW: " << visible << std::endl;
//
//    if(visible)
//    {
//        [_view setHidden:NO];
//        [_window orderFrontRegardless];
//        //[_window makeKeyAndOrderFront:nil];
//    }
//    else
//    {
//        [_window orderOut:_window];
//        [_view setHidden:YES];
//    }
//}


void MainWindowImpl::onShow(Window& w, bool visible)
{
    //std::clog << "SHOW: " << visible << std::endl;

    if(visible)
    {
        [_view setHidden:NO];
        [_window orderFrontRegardless];
        //[_window makeKeyAndOrderFront:nil];
    }
    else
    {
        [_window orderOut:_window];
        [_view setHidden:YES];
    }
}


void MainWindowImpl::onProcessShowEvent(const ShowEvent& ev)
{
    Base::onProcessShowEvent(ev);

    ShowEvent rev( _window, ev.visible() );
    _window.processEvent(rev);
}


void MainWindowImpl::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);
}


//void MainWindowImpl::activate()
//{
//    //std::clog << "ACTIVATE: " << std::endl;
//
//    [_window makeMainWindow];
//    [_window makeKeyWindow];
//}


void MainWindowImpl::onActivate(Window& w, bool active)
{
    if( ! active )
        return;

    //std::clog << "ACTIVATE: " << std::endl;

    [_window makeMainWindow];
    [_window makeKeyWindow];
}


void MainWindowImpl::onProcessActivateEvent(const ActivateEvent& ev)
{
    Base::onProcessActivateEvent(ev);

    ActivateEvent aev( _window, ev.isActive() );
    _window.processEvent(aev);
}


void MainWindowImpl::onActivateEvent(const ActivateEvent& ev)
{
    Base::onActivateEvent(ev);
}


void MainWindowImpl::onEnable(Window& w, bool enable)
{
    // TODO
}


void MainWindowImpl::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    EnableEvent eev( _window, ev.enabled() );
    _window.processEvent(eev);
}


void MainWindowImpl::onEnableEvent(const EnableEvent& ev)
{    
    Base::onEnableEvent(ev);
}


//void MainWindowImpl::move(const Gfx::PointF& p)
//{
//    //std::clog << "MOVE: " << p.x() << "," 
//    //                      << p.y() << std::endl;
//
//    double scaling = scaleFactor();
//
//    CGFloat screenHeight = [[NSScreen mainScreen] frame].size.height;
//    CGFloat windowHeight = [_window frame].size.height;
//
//    CGFloat y = screenHeight - p.y() / scaling - windowHeight;
//    NSPoint origin = NSMakePoint(p.x() / scaling, y);
//
//    [_window setFrameOrigin:origin];
//}
//
//
//void MainWindowImpl::resize(const Gfx::SizeF& size)
//{
//    //std::clog << "RESIZE: " << size.width() << "," 
//    //                        << size.height() << std::endl;
//
//    double scaling = scaleFactor();
//
//    NSRect frameRect = [_window frame];
//    NSRect contentRect = [_window contentRectForFrameRect:frameRect];
//
//    contentRect.origin.y += contentRect.size.height - size.height() / scaling;
//    
//    contentRect.size.width = size.width() / scaling;
//    contentRect.size.height = size.height() / scaling;
//
//    frameRect = [_window frameRectForContentRect:contentRect];
//    [_window setFrame:frameRect display:NO animate:NO];
//}


void MainWindowImpl::onMove(Window& w, const Gfx::PointF& to)
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


void MainWindowImpl::onResize(Window& w, const Gfx::SizeF& size)
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


void MainWindowImpl::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);

    ResizeEvent rev( _client, ev.size() );
    _client.processEvent(rev);
}


void MainWindowImpl::setAbove(bool above)
{
    if(above)
    {
        [_window setLevel: NSFloatingWindowLevel];
    }
    else
    {
        [_window setLevel: NSNormalWindowLevel];
    }
}


void MainWindowImpl::onSetTitle(Window& w, const std::string& text)
{
    NSString* title = [NSString stringWithCString:text.c_str() 
                                encoding:[NSString defaultCStringEncoding]];
    [_window setTitle: title];
}


void MainWindowImpl::onSetIcon(Window& w, const Gfx::Image& icon)
{
}


//void MainWindowImpl::setState(const WindowState& s)
//{
//    switch(s)
//    {
//        case WindowState::Normal:
//            if( [_window isMiniaturized] )
//                [_window deminiaturize: nil];
//            
//            if( [_window isZoomed] )
//                [_window zoom: nil];
//            
//            break;
//
//        case WindowState::Maximized:
//            if( ! [_window isZoomed] )
//                [_window zoom: nil];
//            break;
//
//        case WindowState::Minimized:
//            if( ! [_window isMiniaturized] )
//                [_window miniaturize: nil];
//            break;
//    }
//}


void MainWindowImpl::onSetState(Window& w, const WindowState& s)
{
    switch(s)
    {
        case WindowState::Normal:
            if( [_window isMiniaturized] )
                [_window deminiaturize: nil];
            
            if( [_window isZoomed] )
                [_window zoom: nil];
            
            break;

        case WindowState::Maximized:
            if( ! [_window isZoomed] )
                [_window zoom: nil];
            break;

        case WindowState::Minimized:
            if( ! [_window isMiniaturized] )
                [_window miniaturize: nil];
            break;
    }
}


void MainWindowImpl::onProcessWindowStateEvent(const WindowStateEvent& ev)
{
    Base::onProcessWindowStateEvent(ev);

    WindowStateEvent wse( _client, ev.state() );
    Application::instance().processEvent(wse);
}


void MainWindowImpl::onWindowStateEvent(const WindowStateEvent& ev)
{
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


//void MainWindowImpl::grabPointer()
//{
//    // pointer is always tracked, even if its outside the window
//    [_window setAcceptsMouseMovedEvents:YES];
//}
//
//
//void MainWindowImpl::releasePointer()
//{
//    // pointer is always tracked, even if its outside the window
//    [_window setAcceptsMouseMovedEvents:NO];
//}


void MainWindowImpl::onPaint(const NSRect& rect)
{
    ScreenImpl* screen = Application::instance().screen().impl();
    Window* window = screen->findWindow(_window);
    if( ! window )
        return;

    //std::clog << "ON PAINT: " << rect.size.width << "x" 
    //                          << rect.size.height << std::endl;

    NSRect frameRect = [_window frame];
    NSRect contentRect = [_window contentRectForFrameRect:frameRect];
    CGFloat contentHeight = contentRect.size.height;

    double x = contentHeight - (rect.origin.x + rect.size.height);
    double y = rect.origin.y;
    Pt::Gfx::PointF pos(x, y);

    double width = rect.size.width;
    double height = rect.size.height;
    Gfx::SizeF size(width, height);

    double scaling = scaleFactor();
    pos = pos / scaling;
    size = size / scaling;

    Gfx::RectF paintRect(pos, size);

    WindowImpl* frame = window.impl();
    PaintEvent pev(*frame, paintRect);
    frame->processEvent(pev);

    NSGraphicsContext* graphicsContext = [NSGraphicsContext currentContext];
    CGContextRef windowContext = [graphicsContext CGContext];

    MainWindowImpl* windowImpl = static_cast<MainWindowImpl*>( window->impl() );
    Pt::Hmi::PixmapSurfaceImpl* pixmap = windowImpl->surface().pixmapImpl();
    CGContextRef pixmapContext = pixmap->context();
    
    CGImageRef image = CGBitmapContextCreateImage(pixmapContext);
    CGFloat imageHeight = CGImageGetHeight(image);

    CGFloat subImageX = rect.origin.x * scaling;
    CGFloat subImageY = rect.origin.y * scaling;
    CGFloat subImageWidth = rect.size.width * scaling;
    CGFloat subImageHeight = rect.size.height * scaling;
    
    CGRect subRect = CGRectMake(subImageX,
                                imageHeight - subImageY - subImageHeight,
                                subImageWidth, 
                                subImageHeight);
    
    CGImageRef subImage = CGImageCreateWithImageInRect(image, subRect);

    CGContextDrawImage(windowContext, rect, subImage);

    CGImageRelease(image);
}


void MainWindowImpl::onActivate(bool isActive)
{
    ScreenImpl* screen = Application::instance().screen().impl();
    Window* window = screen->findWindow(_window);
    if( ! window )
        return;

    ActivateEvent ev(*window->impl(), isActive);
    Application::instance().commitEvent(ev);
}


void MainWindowImpl::onShow(bool v)
{
    ScreenImpl* screen = Application::instance().screen().impl();
    Window* window = screen->findWindow(_window);
    if( ! window )
        return;

    ShowEvent sev(*window->impl(), v);
    Application::instance().commitEvent(sev);
}


void MainWindowImpl::onMove()
{
    ScreenImpl* screen = Application::instance().screen().impl();
    Window* window = screen->findWindow(_window);
    if( ! window )
        return;

    CGFloat screenHeight = [[NSScreen mainScreen] frame].size.height;
    CGFloat windowHeight = [_window frame].size.height;
    NSPoint origin = [_window frame].origin;

    double x = origin.x;
    double y = screenHeight - origin.y - windowHeight;
    //std::clog << "ON MOVE: " << x << "," << origin.y << std::endl;

    Pt::Gfx::PointF pos(x, y);

    double scaling = Application::instance().scaleFactor();
    pos = pos / scaling;

    MoveEvent ev(*window->impl(), pos);
    Application::instance().processEvent(ev);
}


void MainWindowImpl::onResize(const NSSize& viewSize)
{   
    //std::clog << "RESIZE: " << viewSize.width << "x" 
    //                        << viewSize.height << std::endl;

    ScreenImpl* screen = Application::instance().screen().impl();
    Window* window = screen->findWindow(_window);
    if( ! window )
        return;

    Window::State wstate = WindowState::Normal;

    if( [_window isZoomed] )
    {		
        wstate = WindowState::Maximized;
    }
    else if( [_window isMiniaturized] )
    {
        wstate = WindowState::Minimized;
    }

    if(window->state() != wstate)
    {
        WindowStateEvent wse( *window->impl(), wstate );
        Application::instance().commitEvent(wse);
    }

    Gfx::SizeF to(viewSize.width, 
                  viewSize.height);

    double scaling = Application::instance().scaleFactor();
    to = to / scaling;

    ResizeEvent rev(*window->impl(), to);
    Application::instance().processEvent(rev);

    Gfx::RectF updateRect(Gfx::PointF(0, 0), to);
    window->repaint(updateRect);

    // cocoa performs a paint/display right after a window resize, so we
    // need to process the window update now to avoid flicker
    // 
    // OR: override NSWwindow::setFrame to not perform a paint/display
    //Application::instance().impl()->processEvents();
}


void MainWindowImpl::onClosing()
{
    //ScreenImpl* screen = Application::instance().screen().impl();
    //Window* window = screen->findWindow(_window);
    //if( ! window )
    //    return;
    //
    //window->close();

    //CloseEvent closeEvent(*window);
    //window->processEvent(closeEvent);

    WindowImpl* frame = this;

    CloseEvent ev(*frame);
    commitEvent(ev);
}


void MainWindowImpl::onClose(Window& w)
{
    //[_window performClose:nil];
    //[_window close];

    WindowImpl* frame = this;

    CloseEvent ev(*frame);
    w.processEvent(ev);
}


void MainWindowImpl::onProcessCloseEvent(const CloseEvent& ev)
{
    onCloseEvent(ev);

    CloseEvent cev(_client);
    _client.processEvent(cev);
}


void MainWindowImpl::onCloseEvent(const CloseEvent& ev)
{
}


void MainWindowImpl::onKeyDown(unsigned vkey, Pt::Char ch)
{
    //std::clog << "KEY DOWN: " << vkey << std::endl;

    ScreenImpl* screen = Application::instance().screen().impl();
    Window* window = screen->findWindow(_window);
    if( ! window )
        return;
    
    Pt::uint32_t keyCode = Key::NoKey;
    if(vkey < keyMapSize)
    {
        keyCode = keyMap[vkey];
        
        if(keyCode == Key::NoKey)
            keyCode = toupper(ch).value();
    }

    Key key(_keyModifiers, keyCode);
    _keyEvent.setPress(key, ch);
    _keyEvent.setVisual(window);

    Application::instance().processEvent(_keyEvent);
}


void MainWindowImpl::onKeyUp(unsigned vkey, Pt::Char ch)
{
    //std::clog << "KEY UP: " << vkey << std::endl;

    ScreenImpl* screen = Application::instance().screen().impl();
    Window* window = screen->findWindow(_window);
    if( ! window )
        return;
    
    Pt::uint32_t keyCode = Key::NoKey;
    if(vkey < keyMapSize)
    {
        keyCode = keyMap[vkey];
        
        if(keyCode == Key::NoKey)
            keyCode = toupper(ch).value();
    }

    Key key(_keyModifiers, keyCode);
    _keyEvent.setRelease(key, ch);
    _keyEvent.setVisual(window);

    Application::instance().processEvent(_keyEvent);
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
    ScreenImpl* screen = Application::instance().screen().impl();
    Window* window = screen->findWindow(_window);
    if( ! window )
        return;

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
    _keyEvent.setVisual(window);

    if(wasPressed)
        _keyEvent.setRelease( key, Pt::Char() );
    else
        _keyEvent.setPress( key, Pt::Char() );

    Application::instance().processEvent(_keyEvent);
}


void MainWindowImpl::onLMouseDown(double x, double y)
{
    //std::clog << "MOUSE PRESS: " << x << ", " << y << std::endl;

    ScreenImpl* screen = Application::instance().screen().impl();
    Window* window = screen->findWindow(_window);
    if( ! window )
        return;

    CGFloat height = [_window contentRectForFrameRect:[_window frame]].size.height;
    y = height - y;

    double scaling = Application::instance().scaleFactor();

    Pt::Gfx::PointF pos(x / scaling, 
                        y / scaling);

    _mouseEvent.setPress(MouseEvent::Left);
    _mouseEvent.setPosition( window->toGlobal(pos) );
    _mouseEvent.setVisual(window);

    Application::instance().processEvent(_mouseEvent);
}


void MainWindowImpl::onLMouseUp(double x, double y)
{
    //std::clog << "MOUSE RELEASE: " << x << ", " << y << std::endl;

    ScreenImpl* screen = Application::instance().screen().impl();
    Window* window = screen->findWindow(_window);
    if( ! window )
        return;
    
    CGFloat height = [_window contentRectForFrameRect:[_window frame]].size.height;
    y = height - y;

    double scaling = Application::instance().scaleFactor();

    Pt::Gfx::PointF pos(x / scaling, 
                        y / scaling);

    _mouseEvent.setRelease(MouseEvent::Left);
    _mouseEvent.setPosition( window->toGlobal(pos) );
    _mouseEvent.setVisual(window);

    Application::instance().processEvent(_mouseEvent);
}


void MainWindowImpl::onMouseMove(double x, double y)
{
    //std::clog << "MOUSE MOVE: " << x << ", " << y << std::endl;

    ScreenImpl* screen = Application::instance().screen().impl();
    Window* window = screen->findWindow(_window);
    if( ! window )
        return;
    
    CGFloat height = [_window contentRectForFrameRect:[_window frame]].size.height;
    y = height - y;

    double scaling = Application::instance().scaleFactor();

    Pt::Gfx::PointF pos(x / scaling, 
                        y / scaling);

    _mouseEvent.setMove();
    _mouseEvent.setPosition( window->toGlobal(pos) );
    _mouseEvent.setVisual(window);

    Application::instance().processEvent(_mouseEvent);
}

} // namespace

} // namespace
