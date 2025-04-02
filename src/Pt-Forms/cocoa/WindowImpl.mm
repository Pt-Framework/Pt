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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include "WindowImpl.h"
#include "ScreenImpl.h"
#include "PixmapImpl.h"
#include "ApplicationImpl.h"
#include "WindowView.h"

#include "KeyMap.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Screen.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/WindowStateEvent.h>
#include <Pt/Hmi/PaintEvent.h>

namespace Pt {

namespace Hmi {

WindowImpl::WindowImpl(ScreenImpl& wm,  Window& w)
: WindowFrame(wm, w)
, _wm(wm)
, _client(w)
, _window(nil)
, _view(nil)
, _windowStyle(0)
, _level(0)
, _keyFlags(0)
{
    WindowView* view = [[WindowView alloc] initWithImpl: this];
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


WindowImpl::~WindowImpl()
{
    if( _window == nil )
        return;

    [_window close];

    [_window release];
    _window = nil;

    [_view release];
    _view = nil;
}


void WindowImpl::onInit(Window& w)
{
    Gfx::PaintSurface* surface = pixmap().surface();
    Gfx::PointF surfacePos(0, 0);
    w.setSurface(surface, surfacePos);

    w.setNextResponder(this);

    double scaling = scaleFactor();
    
    RescaleEvent ev(w, scaling);
    w.processEvent(ev);
}


void WindowImpl::onRelease(Window& w)
{
    w.setNextResponder(0);
    w.setSurface( 0, Gfx::PointF() );
}


void WindowImpl::setType(WindowType type)
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


Gfx::PointF WindowImpl::toScreen(const Gfx::PointF& pos) const
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


Gfx::PointF WindowImpl::fromScreen(const Gfx::PointF& pos) const
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


Gfx::PointF WindowImpl::onToWindow(const Window& w, 
                                       const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF WindowImpl::onFromWindow(const Window& w, 
                                         const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF WindowImpl::onToParent(const Gfx::PointF& pos) const
{
    return _wm.fromFrame(*this, pos); 
}
     
        
Gfx::PointF WindowImpl::onFromParent(const Gfx::PointF& pos) const
{ 
    return _wm.toFrame(*this, pos); 
}


void WindowImpl::paint(const Gfx::RectF& rect)
{
    Gfx::RectF r( rect.topLeft(), rect.size() );

    //std::clog << "PAINT RECT: " << rect.x() << "," << rect.y() <<
    //                   " " << rect.width() << "x" << rect.height() << std::endl;

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


void WindowImpl::onRepaint(Window& w, const Gfx::RectF& rect)
{
    Gfx::PointF screenPos = toScreen( rect.topLeft() );
    Gfx::RectF screenRect( screenPos, rect.size() );
    
    _wm.repaint(screenRect);
}


void WindowImpl::onProcessPaintEvent(const PaintEvent& ev)
{
    Base::onProcessPaintEvent(ev);

    PaintEvent rev( _client, ev.rect() );
    _client.processEvent(rev);
}


void WindowImpl::onPaintEvent(const PaintEvent& ev)
{
    Base::onPaintEvent(ev);
}


void WindowImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{
    double scaling = ev.scaleFactor();
    //scaling *= [_window backingScaleFactor];

    RescaleEvent rev(*this, scaling);
    Base::onProcessRescaleEvent(rev);

    RescaleEvent wev(_client, scaling);
    _client.processEvent(wev);
}


void WindowImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}


//void WindowImpl::show(bool visible)
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


void WindowImpl::onShow(Window& w, bool visible)
{
    //std::clog << "SHOW: " << visible << std::endl;

    if(visible)
    {
        [_view setHidden:YES];
        [_view setHidden:NO];
        [_window orderFrontRegardless];
        //[_window makeKeyAndOrderFront:nil];
    }
    else
    {
        [_window orderOut:_window];
        [_view setHidden:NO];
        [_view setHidden:YES];
    }
}


void WindowImpl::onProcessShowEvent(const ShowEvent& ev)
{
    Base::onProcessShowEvent(ev);

    ShowEvent rev( _client, ev.visible() );
    _client.processEvent(rev);
}


void WindowImpl::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);
}


//void WindowImpl::activate()
//{
//    //std::clog << "ACTIVATE: " << std::endl;
//
//    [_window makeWindow];
//    [_window makeKeyWindow];
//}


void WindowImpl::onActivate(Window& w, bool active)
{
    if( ! active )
        return;

    //std::clog << "ACTIVATE: " << std::endl;

    [_window makeMainWindow];
    [_window makeKeyWindow];
}


void WindowImpl::onProcessActivateEvent(const ActivateEvent& ev)
{
    Base::onProcessActivateEvent(ev);

    ActivateEvent aev( _client, ev.isActive() );
    _client.processEvent(aev);
}


void WindowImpl::onActivateEvent(const ActivateEvent& ev)
{
    Base::onActivateEvent(ev);
}


void WindowImpl::onEnable(Window& w, bool enable)
{
    // TODO
}


void WindowImpl::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    EnableEvent eev( _client, ev.enabled() );
    _client.processEvent(eev);
}


void WindowImpl::onEnableEvent(const EnableEvent& ev)
{    
    Base::onEnableEvent(ev);
}


//void WindowImpl::move(const Gfx::PointF& p)
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
//void WindowImpl::resize(const Gfx::SizeF& size)
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


void WindowImpl::onMove(Window& w, const Gfx::PointF& pos)
{
    //std::clog << "MOVE: " << p.x() << "," 
    //                      << p.y() << std::endl;

    double scaling = scaleFactor();

    CGFloat screenHeight = [[NSScreen mainScreen] frame].size.height;
    CGFloat windowHeight = [_window frame].size.height;

    CGFloat y = screenHeight - pos.y() / scaling - windowHeight;
    NSPoint origin = NSMakePoint(pos.x() / scaling, y);

    [_window setFrameOrigin:origin];
}


void WindowImpl::onProcessMoveEvent(const MoveEvent& ev)
{
    Base::onProcessMoveEvent(ev);

    MoveEvent mev( _client, ev.position() );
    _client.processEvent(mev);
}


void WindowImpl::onResize(Window& w, const Gfx::SizeF& size)
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


void WindowImpl::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);

    ResizeEvent rev( _client, ev.size() );
    _client.processEvent(rev);
}


void WindowImpl::onSetAbove(Window& w, bool above)
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


void WindowImpl::onSetTitle(Window& w, const std::string& text)
{
    NSString* title = [NSString stringWithCString:text.c_str() 
                                encoding:[NSString defaultCStringEncoding]];
    [_window setTitle: title];
}


void WindowImpl::onSetIcon(Window& w, const Gfx::Image& icon)
{
}


//void WindowImpl::setState(const WindowState& s)
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


void WindowImpl::onSetState(Window& w, const WindowState& s)
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


void WindowImpl::onProcessWindowStateEvent(const WindowStateEvent& ev)
{
    Base::onProcessWindowStateEvent(ev);

    WindowStateEvent wse( _client, ev.state() );
    Application::instance().processEvent(wse);
}


void WindowImpl::onWindowStateEvent(const WindowStateEvent& ev)
{
}


void WindowImpl::onSetSizeLimits(Window& w, const Gfx::SizeF& minSizeF, 
                                            const Gfx::SizeF& maxSizeF)
{
    NSSize minSize = NSMakeSize( minSizeF.width(), minSizeF.height() );
    [_window setMinSize:minSize];

    NSSize maxSize = NSMakeSize( maxSizeF.width(), maxSizeF.height() );
    [_window setMaxSize:maxSize];
}


//void WindowImpl::grabPointer()
//{
//    // pointer is always tracked, even if its outside the window
//    [_window setAcceptsMouseMovedEvents:YES];
//}
//
//
//void WindowImpl::releasePointer()
//{
//    // pointer is always tracked, even if its outside the window
//    [_window setAcceptsMouseMovedEvents:NO];
//}


void WindowImpl::onViewPaint(const NSRect& rect)
{
    //std::clog << "ON PAINT: " << rect.size.width << "x" 
    //                          << rect.size.height << std::endl;

    NSRect frameRect = [_window frame];
    NSRect contentRect = [_window contentRectForFrameRect:frameRect];
    CGFloat contentHeight = contentRect.size.height;

    double x = rect.origin.x;
    double y = contentHeight - (rect.origin.y + rect.size.height);
    Pt::Gfx::PointF pos(x, y);

    double width = rect.size.width;
    double height = rect.size.height;
    Gfx::SizeF size(width, height);

    double scaling = scaleFactor();
    pos = pos / scaling;
    size = size / scaling;

    Gfx::RectF paintRect(pos, size);
    PaintEvent pev(*this, paintRect);
    processEvent(pev);

    NSGraphicsContext* graphicsContext = [NSGraphicsContext currentContext];
    CGContextRef windowContext = [graphicsContext CGContext];

    CGContextRef pixmapContext = pixmap().impl()->context();
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


void WindowImpl::onViewActivate(bool isActive)
{
    //std::clog << "activate: " << _client.title() << " " << isActive << std::endl;

    ActivateEvent ev(*this, isActive);
    Application::instance().commitEvent(ev);
}


void WindowImpl::onViewShow(bool v)
{
    //std::clog << "show: " << _client.title() << " " << v << std::endl;

    ShowEvent sev(*this, v);
    Application::instance().commitEvent(sev);
}


void WindowImpl::onViewMove(const NSPoint& viewPos)
{
    CGFloat screenHeight = [[NSScreen mainScreen] frame].size.height;
    CGFloat windowHeight = [_window frame].size.height;

    double x = viewPos.x;
    double y = screenHeight - viewPos.y - windowHeight;

    Pt::Gfx::PointF pos(x, y);

    double scaling = Application::instance().scaleFactor();
    pos = pos / scaling;

    //std::clog << "move: " << _client.title() << " " << pos.x() << ", " << pos.y() << std::endl;

    MoveEvent ev(*this, pos);
    Application::instance().processEvent(ev);
}


void WindowImpl::onViewResize(const NSSize& viewSize)
{   
    //std::clog << "resize: " << _client.title() << " " << viewSize.width << "x" << viewSize.height << std::endl;

    Window::State wstate = WindowState::Normal;

    if( [_window isZoomed] )
    {		
        wstate = WindowState::Maximized;
    }
    else if( [_window isMiniaturized] )
    {
        wstate = WindowState::Minimized;
    }

    if(_client.state() != wstate)
    {
        WindowStateEvent wse( *this, wstate );
        Application::instance().commitEvent(wse);
    }

    Gfx::SizeF to(viewSize.width, 
                  viewSize.height);

    double scaling = Application::instance().scaleFactor();
    to = to / scaling;

    ResizeEvent rev(*this, to);
    Application::instance().processEvent(rev);

    Gfx::RectF updateRect(Gfx::PointF(0, 0), to);
    _client.repaint(updateRect);

    // cocoa performs a paint/display right after a window resize, so we
    // need to process the window update now to avoid flicker
    // 
    // OR: override NSWwindow::setFrame to not perform a paint/display
    Application::instance().impl()->processEvents();
}


void WindowImpl::onViewDidRescale()
{
    CGFloat scale = [ _window backingScaleFactor ];
    std::clog << "BACKING SCALE FACTOR: " << scale << std::endl;
}


void WindowImpl::onViewClosing()
{
    CloseEvent ev(*this);
    Pt::Hmi::Application::instance().commitEvent(ev);
}


void WindowImpl::onClose(Window& w)
{
    //[_window performClose:nil];
    //[_window close];

    WindowFrame* frame = this;

    CloseEvent ev(*frame);
    w.processEvent(ev);
}


void WindowImpl::onProcessCloseEvent(const CloseEvent& ev)
{
    onCloseEvent(ev);

    CloseEvent cev(_client);
    _client.processEvent(cev);
}


void WindowImpl::onCloseEvent(const CloseEvent& ev)
{
}


void WindowImpl::onViewKeyDown(unsigned vkey, Pt::Char ch)
{
    //std::clog << "KEY DOWN: " << vkey << std::endl;


    
    Pt::uint32_t keyCode = Key::NoKey;
    if(vkey < keyMapSize)
    {
        keyCode = keyMap[vkey];
        
        if(keyCode == Key::NoKey)
            keyCode = toupper(ch).value();
    }

    Key key(_keyModifiers, keyCode);
    _keyEvent.setPress(key, ch);
    _keyEvent.setVisual(&_client);

    Application::instance().processEvent(_keyEvent);
}


void WindowImpl::onViewKeyUp(unsigned vkey, Pt::Char ch)
{
    //std::clog << "KEY UP: " << vkey << std::endl;

    Pt::uint32_t keyCode = Key::NoKey;
    if(vkey < keyMapSize)
    {
        keyCode = keyMap[vkey];
        
        if(keyCode == Key::NoKey)
            keyCode = toupper(ch).value();
    }

    Key key(_keyModifiers, keyCode);
    _keyEvent.setRelease(key, ch);
    _keyEvent.setVisual(&_client);

    Application::instance().processEvent(_keyEvent);
}


void WindowImpl::onViewKeyModifier(unsigned int mask)
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
    _keyEvent.setVisual(&_client);

    if(wasPressed)
        _keyEvent.setRelease( key, Pt::Char() );
    else
        _keyEvent.setPress( key, Pt::Char() );

    Application::instance().processEvent(_keyEvent);
}


void WindowImpl::onViewLMouseDown(double x, double y)
{
    //std::clog << "MOUSE PRESS: " << x << ", " << y << std::endl;

    CGFloat height = [_window contentRectForFrameRect:[_window frame]].size.height;
    y = height - y;

    Pt::Gfx::PointF pos(x, y);
    pos /= _client.scaleFactor();

    _mouseEvent.setPress(MouseEvent::Left);
    _mouseEvent.setPosition( _client.toGlobal(pos) );
    _mouseEvent.setVisual(&_client);

    Application::instance().processEvent(_mouseEvent);
}


void WindowImpl::onViewLMouseUp(double x, double y)
{
    //std::clog << "MOUSE RELEASE: " << x << ", " << y << std::endl;

    CGFloat height = [_window contentRectForFrameRect:[_window frame]].size.height;
    y = height - y;

    Pt::Gfx::PointF pos(x, y);
    pos /= _client.scaleFactor();

    _mouseEvent.setRelease(MouseEvent::Left);
    _mouseEvent.setPosition( _client.toGlobal(pos) );
    _mouseEvent.setVisual(&_client);

    Application::instance().processEvent(_mouseEvent);
}


void WindowImpl::onViewMouseMove(double x, double y)
{
    //std::clog << "MOUSE MOVE: " << x << ", " << y << std::endl;

    CGFloat height = [_window contentRectForFrameRect:[_window frame]].size.height;
    y = height - y;

    Pt::Gfx::PointF pos(x, y);
    pos /= _client.scaleFactor();

    _mouseEvent.setMove();
    _mouseEvent.setPosition( _client.toGlobal(pos) );
    _mouseEvent.setVisual(&_client);

    Application::instance().processEvent(_mouseEvent);
}

} // namespace

} // namespace
