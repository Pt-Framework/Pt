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

#include <Pt/Forms/Application.h>
#include <Pt/Forms/Screen.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/WindowManager.h>
#include <Pt/Forms/WindowStateEvent.h>
#include <Pt/Forms/PaintEvent.h>

@interface PtWindow : NSWindow
{
    Pt::Forms::WindowImpl* _windowImpl;
    NSWindowStyleMask      _style;
}

- (PtWindow*) initWithImpl: (Pt::Forms::WindowImpl*) window 
                     frame: (NSRect) frame
                 styleMask: (NSWindowStyleMask) style;

- (BOOL) canBecomeKeyWindow;

- (BOOL) canBecomeMainWindow;

@end


@implementation PtWindow

- (PtWindow*) initWithImpl: (Pt::Forms::WindowImpl*) window
                     frame: (NSRect) frame
                 styleMask: (NSWindowStyleMask) style 

{
    _windowImpl = window;
    _style = style;

    self = [super initWithContentRect: frame 
                            styleMask: _style 
                              backing: NSBackingStoreBuffered 
                                defer: NO];
    return self;
}

- (BOOL) canBecomeKeyWindow
{
    if(_windowImpl->type() == Pt::Forms::WindowType::Popup)
    {
       return NO;
    }

    return YES;
}

- (BOOL) canBecomeMainWindow
{
    if(_windowImpl->type() == Pt::Forms::WindowType::Popup)
    {
       return NO;
    }

    return YES;
}

@end


namespace Pt {

namespace Forms {

WindowImpl::WindowImpl(ScreenImpl& wm,  Window& w)
: WindowFrame(wm, w)
, _wm(wm)
, _client(w)
, _window(nil)
, _view(nil)
, _windowStyle(0)
, _keyFlags(0)
{
    NSRect noGeometry = NSMakeRect(0, 0, 1, 1);

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

    _window = [[PtWindow alloc] initWithImpl: this 
                                frame: noGeometry 
                                styleMask: _windowStyle];

    [_window setColorSpace:[NSColorSpace sRGBColorSpace]];

    if( w.type() == WindowType::Popup )
    {
        //[_window setLevel: NSPopUpMenuWindowLevel];
        [_window setLevel: NSNormalWindowLevel];
    }
    else
    {
        [_window setLevel: NSNormalWindowLevel];
    }

	  NSWindowController * windowController = [[NSWindowController alloc] initWithWindow:_window]; 
	  [windowController autorelease]; 

    [_window setReleasedWhenClosed: NO];
    //[_window setAcceptsMouseMovedEvents:YES];

    NSView* contentView = [_window contentView];
    NSRect contentBounds = [contentView bounds];

    WindowView* view = [[WindowView alloc] initWithImpl: this 
                                           frame: contentBounds];
    _view = view;

    //[_window setContentView: view];
    [contentView addSubview: _view];

    //_view.translatesAutoresizingMaskIntoConstraints = NO;
    //[NSLayoutConstraint activateConstraints:@[
    //    [_view.topAnchor constraintEqualToAnchor:[contentView topAnchor]],
    //    [_view.bottomAnchor constraintEqualToAnchor:[contentView bottomAnchor]],
    //    [_view.leadingAnchor constraintEqualToAnchor:[contentView leadingAnchor]],
    //    [_view.trailingAnchor constraintEqualToAnchor:[contentView trailingAnchor]]
    //]];

    //[_window setInitialFirstResponder: view];

    Base::onSetParent(&wm);
}


WindowImpl::~WindowImpl()
{
    if( _window == nil )
        return;

    if(_view != nil)
        [static_cast<WindowView*>(_view) detachFromWindow];

    [_window close];

    [_window release];
    _window = nil;

    [_view release];
    _view = nil;
}


double WindowImpl::applicationScaleFactor() const
{
    double scale = Application::instance().scaleFactor();
    return scale > 0.0 ? scale : 1.0;
}


double WindowImpl::backingScaleFactor() const
{
    if(_window != nil)
    {
        return [_window backingScaleFactor];
    }

    NSScreen* screen = [NSScreen mainScreen];
    return screen ? [screen backingScaleFactor] : 1.0;
}


double WindowImpl::totalScaleFactor() const
{
    return applicationScaleFactor() * backingScaleFactor();
}


Gfx::PointF WindowImpl::toNative(const Gfx::PointF& pos) const
{
    double scale = applicationScaleFactor();
    return pos * scale;
}


Gfx::SizeF WindowImpl::toNative(const Gfx::SizeF& size) const
{
    double scale = applicationScaleFactor();
    return size * scale;
}


Gfx::RectF WindowImpl::toNative(const Gfx::RectF& rect) const
{
    return Gfx::RectF( toNative(rect.topLeft()),
                       toNative(rect.size()) );
}


Gfx::PointF WindowImpl::fromNative(const Gfx::PointF& pos) const
{
    double scale = applicationScaleFactor();
    return pos / scale;
}


Gfx::SizeF WindowImpl::fromNative(const Gfx::SizeF& size) const
{
    double scale = applicationScaleFactor();
    return size / scale;
}


Gfx::RectF WindowImpl::fromNative(const Gfx::RectF& rect) const
{
    return Gfx::RectF( fromNative(rect.topLeft()),
                       fromNative(rect.size()) );
}


void WindowImpl::onInit(Window& w)
{
    Pixmap& surface = pixmap();
    Gfx::PointF surfacePos(0, 0);
    w.setSurface(&surface, surfacePos);

    w.setNextResponder(this);

    if( screen() )
    {
        double scaling = scaleFactor();
    
        RescaleEvent ev(w, scaling);
        w.processEvent(ev);
    }
}


void WindowImpl::onRelease(Window& w)
{
    w.setNextResponder(0);
    w.setSurface( 0, Gfx::PointF() );
}


void WindowImpl::onConnect(Screen& screen)
{
    Base::onConnect(screen);
}


void WindowImpl::onDisconnect()
{
    Base::onDisconnect();
}


WindowType WindowImpl::type() const
{
    return _client.type();
}


void WindowImpl::setType(WindowType type)
{
    //std::clog << "WindowImpl::setType: " << type << std::endl;

    //
    // TODO: possibly need to remove the view when switching to/from popup
    //

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
}


Gfx::PointF WindowImpl::toScreen(const Gfx::PointF& pos) const
{
    Gfx::PointF appKitPos = toNative(pos);

    CGFloat viewHeight = [_view frame].size.height;
    double y = viewHeight - appKitPos.y();

    NSPoint p = NSMakePoint(appKitPos.x(), y);
    p = [ _window convertPointToScreen: p ];

    CGFloat screenHeight = [[NSScreen mainScreen] frame].size.height;
    Gfx::PointF screenPos(p.x, screenHeight - p.y);

    return fromNative(screenPos);
}


Gfx::PointF WindowImpl::fromScreen(const Gfx::PointF& pos) const
{   
    Gfx::PointF appKitPos = toNative(pos);
    
    CGFloat screenHeight = [[NSScreen mainScreen] frame].size.height;
    double y = screenHeight - appKitPos.y();

    NSPoint p = NSMakePoint(appKitPos.x(), y);
    p = [ _window convertPointFromScreen: p ];

    CGFloat viewHeight = [_view frame].size.height;
    Gfx::PointF viewPos(p.x, viewHeight - p.y);
    
    return fromNative(viewPos);
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
    Gfx::RectF appKitRect = toNative(rect);

    //std::clog << "PAINT RECT: " << rect.x() << "," << rect.y() <<
    //                   " " << rect.width() << "x" << rect.height() << std::endl;

    NSRect frameRect = [_window frame];
    NSRect contentRect = [_window contentRectForFrameRect:frameRect];
    CGFloat contentHeight = contentRect.size.height;

    CGFloat x = appKitRect.x();
    CGFloat y = contentHeight - (appKitRect.y() + appKitRect.height());
    CGFloat w = appKitRect.width();
    CGFloat h = appKitRect.height();
    
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

    //std::clog << "RESCALE EVENT: " << backingScaleFactor() << std::endl;
    //std::clog << "APP SCALING: " << Application::instance().scaleFactor() << std::endl;
    scaling *= backingScaleFactor();

    RescaleEvent rev(*this, scaling);
    Base::onProcessRescaleEvent(rev);

    RescaleEvent wev(_client, scaling);
    _client.processEvent(wev);
}


void WindowImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}


void WindowImpl::onShow(Window& w, bool visible)
{
    //std::clog << "SHOW: " << _client.title() << " " << visible << std::endl;

    if(visible)
    {
        if( w.type() == WindowType::Popup )
        {
            [_window orderFront:nil];
        }
        else
        {
            //[_window makeMainWindow];
            [_window orderFront:nil];
        }
    }
    else
    {
        [_window orderOut:nil];
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


void WindowImpl::onActivate(Window& w, bool active)
{
    if( ! active )
        return;

    if( w.type() == WindowType::Popup )
    {
        //std::clog << "ACTIVATE: key" << std::endl;
        [_window makeKeyWindow];
        return;
    }

    //std::clog << "ACTIVATE: main and key" << std::endl;
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


void WindowImpl::onMove(Window& w, const Gfx::PointF& pos)
{
    //std::clog << "MOVE: " << pos.x() << "," << pos.y() << std::endl;

    Gfx::PointF appKitPos = toNative(pos);

    CGFloat screenHeight = [[NSScreen mainScreen] frame].size.height;
    CGFloat windowHeight = [_window frame].size.height;
    //std::clog << "TODO: WindowImpl.mm 462: get height of parent screen " << screenHeight << std::endl;

    NSPoint origin;
    origin.x = appKitPos.x();
    origin.y = screenHeight - appKitPos.y() - windowHeight;
    //std::clog << "MOVED ORIGIN: " << _frame.origin.x << "x" << _frame.origin.y << std::endl;

    [_window setFrameOrigin:origin];

    //std::clog << " MOVE END " << std::endl;
}


void WindowImpl::onProcessMoveEvent(const MoveEvent& ev)
{
    Base::onProcessMoveEvent(ev);

    MoveEvent mev( _client, ev.position() );
    _client.processEvent(mev);
}


Gfx::SizeF WindowImpl::onResize(Window& w, const Gfx::SizeF& size)
{
    //static int nnn = 0;
    //std::clog << ++nnn << " RESIZE: " << size.width() << ", "  << size.height() << std::endl;

    Gfx::SizeF appKitSize = toNative(size);

    //std::clog << "RESIZE FRAME: " << _frame.origin.x << "," << _frame.origin.y
    //                              << _frame.size.width << "," << _frame.size.height << std::endl;
    NSRect frameRect = [_window frame];
    NSRect content = [_window contentRectForFrameRect:frameRect];

    //std::clog << "CURRENT FRAME: " << frameRect.origin.x << "," << frameRect.origin.y << " "
    //                               << frameRect.size.width << "," << frameRect.size.height << std::endl;
    
    frameRect.origin.y += content.size.height - appKitSize.height();
    frameRect.size.width += appKitSize.width() - content.size.width;
    frameRect.size.height += appKitSize.height() - content.size.height;
    
    //std::clog << "RESIZED FRAME: " << frameRect.origin.x << "," << frameRect.origin.y << " "
    //                               << frameRect.size.width << "," << frameRect.size.height << std::endl;

    [_window setFrame:frameRect display:NO];

    //std::clog << ++nnn << " RESIZE END " << std::endl;

    return size;
}


void WindowImpl::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);

    ResizeEvent rev( _client, ev.size() );
    _client.processEvent(rev);
}


void WindowImpl::onSetAbove(Window& w, bool above)
{
    [_window orderFront:nil];
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


void WindowImpl::onSetState(Window& w, const WindowState& s)
{
    //
    // TODO: Borderless windows are always considered to be zoomed, so
    //       the logic below does not work. Instead we could set the
    //       frame to the visibleFrame of the screen.
    //
    if(_client.type() == WindowType::Popup)
    {
        //std::clog << "TODO: WindowImpl::onSetStatePopup handle popups. " << std::endl;
        return;
    }

    switch(s)
    {
        case WindowState::Normal:
            if( [_window isMiniaturized] )
            {
                [_window deminiaturize: nil];
            }
            
            if( [_window isZoomed] )
            {
                [_window zoom: nil];
            }
            
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
    //std::clog << "onSetSizeLimits: " << minSizeF.width() << "x" << minSizeF.height()
    //                          << " " << maxSizeF.width() << "x" << maxSizeF.height() << std::endl;

    Gfx::SizeF minAppKit = toNative(minSizeF);
    Gfx::SizeF maxAppKit = toNative(maxSizeF);

    NSSize minSize = NSMakeSize( minAppKit.width(), minAppKit.height() );
    [_window setMinSize:minSize];

    NSSize maxSize = NSMakeSize( maxAppKit.width(), maxAppKit.height() );
    [_window setMaxSize:maxSize];
}


void WindowImpl::onAutoCenter(Window& w, const Gfx::SizeF* size) 
{
    _wm.onAutoCenter(*this, size);
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
    NSRect frame = [_window frame];
    NSRect content = [_window contentRectForFrameRect:frame];

    double x = rect.origin.x;
    double y = content.size.height - (rect.origin.y + rect.size.height);
    double width = rect.size.width;
    double height = rect.size.height;

    Gfx::RectF nativeRect( Gfx::PointF(x, y), Gfx::SizeF(width, height) );
    Gfx::RectF paintRect = fromNative(nativeRect);
    PaintEvent pev(*this, paintRect);
    processEvent(pev);

    CGImageRef image = pixmap().impl()->getCGImage();

    NSGraphicsContext* graphicsContext = [NSGraphicsContext currentContext];
    CGContextRef windowContext = [graphicsContext CGContext];

    CGFloat backingScale = [_window backingScaleFactor];
    CGRect sourceRect = CGRectMake(x * backingScale, y * backingScale,
                                   width * backingScale, height * backingScale);

#ifdef PT_FORMS_WARN_UNALIGNED_BLIT
    CGRect destRect = CGContextConvertRectToDeviceSpace(windowContext, rect);
    Detail::warnIfExpensiveBlit("WindowImpl::onViewPaint",
                                sourceRect, destRect);
#endif

    CGImageRef sourceImage = CGImageCreateWithImageInRect(image, sourceRect);

    CGContextSaveGState(windowContext);
    CGContextSetBlendMode(windowContext, kCGBlendModeCopy);
    CGContextDrawImage(windowContext, rect, sourceImage);
    CGContextRestoreGState(windowContext);

    CGImageRelease(sourceImage);
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
    pos = fromNative(pos);

    //std::clog << "onViewMove: " << _client.title() << " " << pos.x() << ", " << pos.y() << std::endl;

    MoveEvent ev(*this, pos);
    Application::instance().processEvent(ev);
}


void WindowImpl::onViewResize(const NSSize& viewSize)
{   
    //std::clog << "onViewResize: " << _client.title() << " " << viewSize.width << "x" << viewSize.height << std::endl;

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
    to = fromNative(to);

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
    CGFloat scale = backingScaleFactor();
    std::clog << "BACKING SCALE FACTOR: " << scale << std::endl;
}


void WindowImpl::onViewClosing()
{
    CloseEvent ev(*this);
    Pt::Forms::Application::instance().commitEvent(ev);
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
    _keyEvent.setWidget(&_client);

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
    _keyEvent.setWidget(&_client);

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
    _keyEvent.setWidget(&_client);

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
    pos = fromNative(pos);

    _mouseEvent.setPress(MouseEvent::Left);
    _mouseEvent.setPosition( _client.toGlobal(pos) );
    _mouseEvent.setWidget(&_client);

    Application::instance().processEvent(_mouseEvent);
}


void WindowImpl::onViewLMouseUp(double x, double y)
{
    //std::clog << "MOUSE RELEASE: " << x << ", " << y << std::endl;

    CGFloat height = [_window contentRectForFrameRect:[_window frame]].size.height;
    y = height - y;

    Pt::Gfx::PointF pos(x, y);
    pos = fromNative(pos);

    _mouseEvent.setRelease(MouseEvent::Left);
    _mouseEvent.setPosition( _client.toGlobal(pos) );
    _mouseEvent.setWidget(&_client);

    Application::instance().processEvent(_mouseEvent);
}


void WindowImpl::onViewMouseMove(double x, double y)
{
    //std::clog << "MOUSE MOVE: " << x << ", " << y << std::endl;

    CGFloat height = [_window contentRectForFrameRect:[_window frame]].size.height;
    y = height - y;

    Pt::Gfx::PointF pos(x, y);
    pos = fromNative(pos);

    _mouseEvent.setMove();
    _mouseEvent.setPosition( _client.toGlobal(pos) );
    _mouseEvent.setWidget(&_client);

    Application::instance().processEvent(_mouseEvent);
}


void WindowImpl::onViewRMouseDown(double x, double y)
{
    //std::clog << "MOUSE PRESS: " << x << ", " << y << std::endl;

    CGFloat height = [_window contentRectForFrameRect:[_window frame]].size.height;
    y = height - y;

    Pt::Gfx::PointF pos(x, y);
    pos = fromNative(pos);

    _mouseEvent.setPress(MouseEvent::Right);
    _mouseEvent.setPosition( _client.toGlobal(pos) );
    _mouseEvent.setWidget(&_client);

    Application::instance().processEvent(_mouseEvent);
}


void WindowImpl::onViewRMouseUp(double x, double y)
{
    //std::clog << "MOUSE RELEASE: " << x << ", " << y << std::endl;

    CGFloat height = [_window contentRectForFrameRect:[_window frame]].size.height;
    y = height - y;

    Pt::Gfx::PointF pos(x, y);
    pos = fromNative(pos);

    _mouseEvent.setRelease(MouseEvent::Right);
    _mouseEvent.setPosition( _client.toGlobal(pos) );
    _mouseEvent.setWidget(&_client);

    Application::instance().processEvent(_mouseEvent);
}

} // namespace

} // namespace
