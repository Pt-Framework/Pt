/* Copyright (C) 2015 Marc Boris Duerner 
  
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

#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include "MainWindowImpl.h"

#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PaintEvent.h>

namespace Pt {

namespace Hmi {

ScreenImpl::ScreenImpl(ApplicationImpl&)
: _parent(0)
, _captureMonitor(0)
, _screenScaling(1.0)
{
}


ScreenImpl::~ScreenImpl()
{
    while( ! _windows.empty() )
        _windows.back()->unparent();

    setParent(0);
}


void ScreenImpl::setParent(Screen* screen)
{
    _parent = screen;

    if(_parent)
    {
        // TODO:
        //NSScreen* mainScreen = [NSScreen mainScreen];
        //NSRect screenRect = [mainScreen visibleFrame];

        //Gfx::SizeF size(screenRect.width, screenRect.height);
        Gfx::SizeF size(640, 480);

        size /= scaleFactor();

        _parent->onResize(*this, size);
        _parent->onShow(*this, true);
    }

    onSetParent(_parent);
}


void ScreenImpl::addWindow(Window& w)
{
    w.setParent(*this); 
}


void ScreenImpl::removeWindow(Window& w)
{ 
    w.unparent(); 
}


const std::vector<Window*>& ScreenImpl::windows() const
{
    return _windows;
}


Window* ScreenImpl::findWindow(NSWindow* wnd)
{
    const std::vector<Window*>& windows = Application::instance().screen().windows();

    std::vector<Window*>::const_iterator it;
    for(it = windows.begin(); it != windows.end(); ++it)
    {
        Window* window = *it;

        MainWindowImpl* impl = static_cast<MainWindowImpl*>( window->frame() );
        if( window->frame() && impl->window() == wnd )
            return window;
    }
    
    return 0;
}


Gfx::PointF ScreenImpl::toFrame(const MainWindowImpl& frame, 
                                const Gfx::PointF& pos) const
{
    return frame.fromScreen(pos);
}


Gfx::PointF ScreenImpl::fromFrame(const MainWindowImpl& frame, 
                                  const Gfx::PointF& pos) const
{
    return frame.toScreen(pos);
}

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

Visual* ScreenImpl::onGetParent() const
{
    return _parent;
}


Visual* ScreenImpl::onHitTest(const Gfx::PointF& p)
{
    double scaling = scaleFactor();

    CGFloat screenHeight = [[NSScreen mainScreen] frame].size.height;
    CGFloat y = screenHeight - p.y() / scaling;
    
    NSPoint pnt = NSMakePoint(p.x() / scaling, y);

    NSInteger n =  [ NSWindow windowNumberAtPoint: pnt
                              belowWindowWithWindowNumber: 0 ];

    NSApplication* nsapp = [NSApplication sharedApplication];
    NSWindow* nswin = [nsapp windowWithWindowNumber: n];

    Window* win = findWindow(nswin);
     if( ! win )
         return 0;

    MainWindowImpl* frame = static_cast<MainWindowImpl*>( win->frame() );
    Gfx::PointF pos = toFrame(*frame, p);
    
    return win->hitTest(pos);
}


Gfx::PointF ScreenImpl::onToParent(const Gfx::PointF& pos) const
{
    return pos + position();
}


Gfx::PointF ScreenImpl::onFromParent(const Gfx::PointF& pos) const
{
    return pos - position();
}


void ScreenImpl::onProcessEvent(const Event& ev)
{
    Base::onProcessEvent(ev);
}


void ScreenImpl::onRepaintRequest(const Gfx::RectF& rect)
{
    if(_parent)
        _parent->repaint(rect);
}

///////////////////////////////////////////////////////////////////////
// WindowManager
///////////////////////////////////////////////////////////////////////

WindowFrame* ScreenImpl::onAttach(Window& w)
{
    MainWindowImpl* frame = new MainWindowImpl(*this, w);
    frame->setNextResponder(this);

    _windows.push_back(&w);

    return frame;
}


void ScreenImpl::onDetach(WindowFrame& frame)
{
    frame.setNextResponder(0);

    Window& w = frame.window();

    std::vector<Window*>::iterator it;
    it = std::remove(_windows.begin(), _windows.end(), &w);
    _windows.erase(it, _windows.end());
}


void ScreenImpl::onInit(WindowFrame& frame)
{
    RescaleEvent ev( frame, scaleFactor() );
    frame.processEvent(ev);
}


void ScreenImpl::onRelease(WindowFrame& frame)
{
}


//void ScreenImpl::onShow(Window& w, bool visible)
//{
//    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.frame() );
//    impl->show(visible);
//}


//void ScreenImpl::onActivate(Window& w, bool active)
//{
//    if( ! active )
//        return;
//
//    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.frame() );
//    impl->activate();
//}


//void ScreenImpl::onEnableRequest(Window& w, bool enable)
//{
//    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.frame() );
//    impl->enable(enable);
//}


//void ScreenImpl::onMove(Window& w, const Gfx::PointF& pos)
//{
//    Gfx::PointF aligedPos = w.surface().align(pos);
//
//    //
//    // TODO: scale here instead of in MainWindowImpl
//    //
//
//    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.frame() );
//    impl->move(aligedPos);
//}
//
//
//void ScreenImpl::onResize(Window& w, const Gfx::SizeF& s)
//{
//    //
//    // align to physical pixel grid
//    //
//    Gfx::SizeF alignedSize = w.surface().align(s);
//
//    //
//    // maximum width and height
//    //
//    if( alignedSize.width() > w.maximumSize().width() )
//        alignedSize.setWidth( w.maximumSize().width() );
//
//    if( alignedSize.height() > w.maximumSize().height() )
//        alignedSize.setHeight( w.maximumSize().height() );
//
//    if( alignedSize.width() < w.minimumSize().width() )
//        alignedSize.setWidth( w.minimumSize().width() );
//
//    if( alignedSize.height() < w.minimumSize().height() )
//        alignedSize.setHeight( w.minimumSize().height() );
//
//    //w.frame()->scaleFactor(); ???
//
//    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.frame() );
//    impl->resize(alignedSize);
//}


//void ScreenImpl::onSetAbove(Window& w, bool above)
//{
//    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.frame() );
//    impl->setAbove(above);
//}


//void ScreenImpl::onSetTitle(Window& w, const std::string& text)
//{
//    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.frame() );
//    impl->setTitle(text);
//}


//void ScreenImpl::onSetIcon(Window& w, const Gfx::Image& icon)
//{
//    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.frame() );
//    impl->setIcon(icon);
//}


//void ScreenImpl::onSetState(Window& w, const WindowState& state)
//{
//    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.frame() );
//    impl->setState(state);
//}


//void ScreenImpl::onSetSizeLimits(Window& w, const Gfx::SizeF& minSize, 
//                                            const Gfx::SizeF& maxSize)
//{
//    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.frame() );
//    impl->setMinimumSize(minSize);
//    impl->setMaximumSize(maxSize);
//}


//void ScreenImpl::onClosing(Window& w)
//{
//    //MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.frame() );
//    //NSWindow* nswin = impl->window();
//
//    //[nswin performClose:nil];
//    //[nswin close];
//
//    CloseEvent ev(w);
//    w.processEvent(ev);
//}


void ScreenImpl::setCapture(Visual* capture)
{
    if(_captureMonitor)
    {
        //std::clog << "RELEASE CAPTURE NSWINDOW: " << _captureMonitor << std::endl;
        [NSEvent removeMonitor: (id)_captureMonitor];
        _captureMonitor = 0;
    }

    if( ! capture )
        return;

    Window* window = 0;

    std::vector<Window*>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {      
        if( capture == *wit || capture->isDescendantOf(**wit) )
        {
            window = *wit;
            break;
        }
    }

    if( ! window )
        return;

    //std::clog << "SET CAPTURE NSWINDOW: " << window->title() << std::endl;

    NSEventMask mask = NSEventMaskLeftMouseDown | NSEventMaskRightMouseDown |
                       NSEventMaskOtherMouseDown;

    // NOTE: local monitors will only capture events on the window frame

    _captureMonitor = [NSEvent addGlobalMonitorForEventsMatchingMask: mask
                               handler:^ void (NSEvent* event) 
                               {
                                   NSEventType eventType = [event type];
                                   
                                   if(eventType == NSEventTypeLeftMouseDown ||
                                      eventType == NSEventTypeRightMouseDown ||
                                      eventType == NSEventTypeOtherMouseDown)
                                   {
                                       MainWindowImpl* impl = static_cast<MainWindowImpl*>( window->frame() );
                                       [impl->view() mouseDown:event];
                                   }
                               }];
}

///////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////

void ScreenImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{
    double scaling = ev.scaleFactor() * _screenScaling;

    RescaleEvent rev(*this, scaling);
    Base::onProcessRescaleEvent(rev);

    std::vector<Window*>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {
        Window* window = *wit;
        WindowFrame* frame = window->impl();
        
        RescaleEvent ev(*frame, scaling);
        frame->processEvent(ev);
    }
}


void ScreenImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);

    // TODO:
    //NSScreen* mainScreen = [NSScreen mainScreen];
    //NSRect screenRect = [mainScreen visibleFrame];

    //Gfx::SizeF size(screenRect.width, screenRect.height);
    Gfx::SizeF size(640, 480);

    size /= scaleFactor();

    _parent->onResize(*this, size);
}


//void ScreenImpl::onRepaint(Window& w, const Gfx::RectF& rect)
//{
//    Gfx::PointF screenPos = onFromWindow( w, rect.topLeft() );
//    Gfx::RectF screenRect( screenPos, rect.size() );
//
//    repaint(screenRect);
//}


void ScreenImpl::onProcessPaintEvent(const PaintEvent& ev)
{
    const Gfx::RectF& screenRect = ev.rect();

    Base::onProcessPaintEvent(ev);

    //
    // paint child windows
    //
    std::vector<Window*>::iterator it;
    for(it = _windows.begin(); it != _windows.end(); ++it)
    {
        Window* window = *it;
        MainWindowImpl* frame = static_cast<MainWindowImpl*>( window->impl() );

        Gfx::PointF winPos = toFrame( *frame, screenRect.topLeft() );
        Gfx::RectF winRect( winPos, screenRect.size() );

        winRect = winRect.intersect( Gfx::RectF( window->size() ) );

        // send (native) paint event to window
        winRect = Gfx::RectF( winRect.topLeft() * _scaling, 
                              winRect.size() * _scaling);

        frame->paint(winRect);
    }
}


void ScreenImpl::onPaintEvent(const PaintEvent& ev)
{    
    Base::onPaintEvent(ev);

    const Gfx::RectF& rect = ev.rect();
    onPaint(rect);
}


void ScreenImpl::onPaint(const Gfx::RectF& rect)
{
}


void ScreenImpl::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    for( size_t i = 0; i < _windows.size(); ++i)
    {
        Window* w = _windows[i];
        MainWindowImpl* frame = static_cast<MainWindowImpl*>( w->impl() );

        frame->onEnable(*w, ev.enabled() );
    }
}


void ScreenImpl::onEnableEvent(const EnableEvent& ev)
{    
    Base::onEnableEvent(ev);
}


void ScreenImpl::onEnable(bool e)
{
    Base::onEnable(e);
}


void ScreenImpl::onProcessMouseEvent(const MouseEvent& ev)
{
    ev.visual()->processEvent(ev);
}


bool ScreenImpl::onMouseEvent(const MouseEvent& ev)
{ 
    // TODO: possibly pass on to application
    return Base::onMouseEvent(ev);
}


void ScreenImpl::onProcessTouchEvent(const TouchEvent& ev)
{
    ev.visual()->processEvent(ev);
}


bool ScreenImpl::onTouchEvent(const TouchEvent& ev)
{ 
    // TODO: possibly pass on to application
    return Base::onTouchEvent(ev);
}


void ScreenImpl::onProcessScrollEvent(const ScrollEvent& ev)
{
    ev.visual()->processEvent(ev);
}


bool ScreenImpl::onScrollEvent(const ScrollEvent& ev)
{ 
    // TODO: possibly pass on to application
    return Base::onScrollEvent(ev);
}


void ScreenImpl::onProcessKeyEvent(const KeyEvent& ev)
{
    Visual* visual = ev.visual();
    if(visual)
        ev.visual()->processEvent(ev);

    // TODO: dispatch to active window
}


bool ScreenImpl::onKeyEvent(const KeyEvent& ev)
{ 
    // TODO: possibly pass on to application
    return Base::onKeyEvent(ev); 
}

} // namespace

} // namespace
