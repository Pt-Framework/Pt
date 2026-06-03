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
#include "WindowImpl.h"

#include <Pt/Forms/Window.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PaintEvent.h>

namespace Pt {

namespace Forms {

ScreenImpl::ScreenImpl(ApplicationImpl&)
: _parent(0)
, _captureMonitor(0)
{
}


ScreenImpl::~ScreenImpl()
{
    while( ! _windows.empty() )
        _windows.back()->unparent();

    if(_captureMonitor)
    {
        if([NSApp isRunning])
            [NSEvent removeMonitor:_captureMonitor];

        _captureMonitor = 0;
    }

    setParent(0);
}


void ScreenImpl::setParent(Screen* screen)
{
    _parent = screen;

    if(_parent)
    {
        // TODO:
        NSScreen* mainScreen = [NSScreen mainScreen];
        NSRect screenRect = [mainScreen visibleFrame];
        //std::clog << "screen size: " << screenRect.size.width << "x" 
        //                             << screenRect.size.height << std::endl;

        Gfx::SizeF size(screenRect.size.width, screenRect.size.height);
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


WindowManager& ScreenImpl::windowManager()
{
    return *this;
}


Window* ScreenImpl::findWindow(NSWindow* wnd)
{
    const std::vector<Window*>& windows = Application::instance().screen().windows();

    std::vector<Window*>::const_iterator it;
    for(it = windows.begin(); it != windows.end(); ++it)
    {
        Window* window = *it;

        WindowImpl* impl = static_cast<WindowImpl*>( window->frame() );
        if( window->frame() && impl->window() == wnd )
            return window;
    }
    
    return 0;
}


Gfx::PointF ScreenImpl::toFrame(const WindowImpl& frame, 
                                const Gfx::PointF& pos) const
{
    return frame.fromScreen(pos);
}


Gfx::PointF ScreenImpl::fromFrame(const WindowImpl& frame, 
                                  const Gfx::PointF& pos) const
{
    return frame.toScreen(pos);
}


void ScreenImpl::onAutoCenter(WindowFrame& w, const Gfx::SizeF* size)
{
    if( ! size )
    {
        return;
    }

    Pt::Gfx::SizeF windowSize = *size;
    Pt::Gfx::SizeF screenSize = this->size();

    double x = (screenSize.width() - windowSize.width()) / 2.0;
    double y = (screenSize.height() - windowSize.height()) / 2.0;
    
    //std::clog << "auto-center BEGIN: " << w.window().title() << " " << x << "," << y << std::endl;
    //std::clog << "window size: " << windowSize.width() << "x" << windowSize.height() << std::endl;
    //std::clog << "screen size: " << screenSize.width() << "x" << screenSize.height() << std::endl;

    w.WindowFrame::window().move( Pt::Gfx::PointF(x, y) );
}

///////////////////////////////////////////////////////////////////////
// Widget
///////////////////////////////////////////////////////////////////////

Widget* ScreenImpl::onHitTest(const Gfx::PointF& p)
{
    double scaling = scaleFactor();

    CGFloat screenHeight = [[NSScreen mainScreen] frame].size.height;
    CGFloat y = screenHeight - p.y() * scaling;
    
    NSPoint pnt = NSMakePoint(p.x() * scaling, y);

    NSInteger n =  [ NSWindow windowNumberAtPoint: pnt
                              belowWindowWithWindowNumber: 0 ];

    NSApplication* nsapp = [NSApplication sharedApplication];
    NSWindow* nswin = [nsapp windowWithWindowNumber: n];

    Window* win = findWindow(nswin);
     if( ! win )
         return 0;

    WindowImpl* frame = static_cast<WindowImpl*>( win->frame() );
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


void ScreenImpl::onRequestRepaint(const Gfx::RectF& rect)
{
    if(_parent)
        _parent->repaint(rect);
}

///////////////////////////////////////////////////////////////////////
// WindowManager
///////////////////////////////////////////////////////////////////////

WindowFrame* ScreenImpl::onAttach(Window& w)
{
    WindowImpl* frame = new WindowImpl(*this, w);
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

    Base::onInit(frame);
}


void ScreenImpl::onRelease(WindowFrame& frame)
{
    Base::onRelease(frame);
}

//void ScreenImpl::onShow(Window& w, bool visible)
//{
//    WindowImpl* impl = static_cast<WindowImpl*>( w.frame() );
//    impl->show(visible);
//}


//void ScreenImpl::onActivate(Window& w, bool active)
//{
//    if( ! active )
//        return;
//
//    WindowImpl* impl = static_cast<WindowImpl*>( w.frame() );
//    impl->activate();
//}


//void ScreenImpl::onEnableRequest(Window& w, bool enable)
//{
//    WindowImpl* impl = static_cast<WindowImpl*>( w.frame() );
//    impl->enable(enable);
//}


//void ScreenImpl::onMove(Window& w, const Gfx::PointF& pos)
//{
//    Gfx::PointF aligedPos = w.surface().align(pos);
//
//    //
//    // TODO: scale here instead of in WindowImpl
//    //
//
//    WindowImpl* impl = static_cast<WindowImpl*>( w.frame() );
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
//    WindowImpl* impl = static_cast<WindowImpl*>( w.frame() );
//    impl->resize(alignedSize);
//}


//void ScreenImpl::onSetAbove(Window& w, bool above)
//{
//    WindowImpl* impl = static_cast<WindowImpl*>( w.frame() );
//    impl->setAbove(above);
//}


//void ScreenImpl::onSetTitle(Window& w, const std::string& text)
//{
//    WindowImpl* impl = static_cast<WindowImpl*>( w.frame() );
//    impl->setTitle(text);
//}


//void ScreenImpl::onSetIcon(Window& w, const Gfx::Image& icon)
//{
//    WindowImpl* impl = static_cast<WindowImpl*>( w.frame() );
//    impl->setIcon(icon);
//}


//void ScreenImpl::onSetState(Window& w, const WindowState& state)
//{
//    WindowImpl* impl = static_cast<WindowImpl*>( w.frame() );
//    impl->setState(state);
//}


//void ScreenImpl::onSetSizeLimits(Window& w, const Gfx::SizeF& minSize, 
//                                            const Gfx::SizeF& maxSize)
//{
//    WindowImpl* impl = static_cast<WindowImpl*>( w.frame() );
//    impl->setMinimumSize(minSize);
//    impl->setMaximumSize(maxSize);
//}


//void ScreenImpl::onClosing(Window& w)
//{
//    //WindowImpl* impl = static_cast<WindowImpl*>( w.frame() );
//    //NSWindow* nswin = impl->window();
//
//    //[nswin performClose:nil];
//    //[nswin close];
//
//    CloseEvent ev(w);
//    w.processEvent(ev);
//}


void ScreenImpl::setCapture(Widget* capture)
{
    if(_captureMonitor)
    {
        //std::clog << "RELEASE CAPTURE NSWINDOW: " << _captureMonitor << std::endl;

        if([NSApp isRunning])
            [NSEvent removeMonitor:_captureMonitor];

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
                                       WindowImpl* impl = static_cast<WindowImpl*>( window->frame() );
                                       [impl->view() mouseDown:event];
                                   }
                               }];
}

///////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////

void ScreenImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{
    double scaling = ev.scaleFactor();

    RescaleEvent rev(*this, scaling);
    Base::onProcessRescaleEvent(rev);

    std::vector<Window*>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {
        Window* window = *wit;
        WindowFrame* frame = window->frame();
        
        RescaleEvent ev(*frame, scaling);
        frame->processEvent(ev);
    }
}


void ScreenImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);

    // TODO:
    NSScreen* mainScreen = [NSScreen mainScreen];
    NSRect screenRect = [mainScreen visibleFrame];
    //std::clog << "screen size: " << screenRect.size.width << "x" << screenRect.size.height << std::endl;

    Gfx::SizeF size(screenRect.size.width, screenRect.size.height);
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
        WindowImpl* frame = static_cast<WindowImpl*>( window->frame() );

        Gfx::PointF winPos = toFrame( *frame, screenRect.topLeft() );
        Gfx::RectF winRect( winPos, screenRect.size() );

        winRect = winRect.toIntersected( Gfx::RectF( window->size() ) );

        if(winRect.size().width() < 0.1 || winRect.height() < 0.1)
            continue;

        // send (native) paint event to window
        winRect = Gfx::RectF( winRect.topLeft() /* * window->scaleFactor()*/, 
                              winRect.size() /* * window->scaleFactor()*/);

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
        WindowImpl* frame = static_cast<WindowImpl*>( w->frame() );

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
    ev.widget()->processEvent(ev);
}


bool ScreenImpl::onMouseEvent(const MouseEvent& ev)
{ 
    // TODO: possibly pass on to application
    return Base::onMouseEvent(ev);
}


void ScreenImpl::onProcessTouchEvent(const TouchEvent& ev)
{
    ev.widget()->processEvent(ev);
}


bool ScreenImpl::onTouchEvent(const TouchEvent& ev)
{ 
    // TODO: possibly pass on to application
    return Base::onTouchEvent(ev);
}


void ScreenImpl::onProcessScrollEvent(const ScrollEvent& ev)
{
    ev.widget()->processEvent(ev);
}


bool ScreenImpl::onScrollEvent(const ScrollEvent& ev)
{ 
    // TODO: possibly pass on to application
    return Base::onScrollEvent(ev);
}


void ScreenImpl::onProcessKeyEvent(const KeyEvent& ev)
{
    Widget* widget = ev.widget();
    if(widget)
        widget->processEvent(ev);

    // TODO: dispatch to active window
}


bool ScreenImpl::onKeyEvent(const KeyEvent& ev)
{ 
    // TODO: possibly pass on to application
    return Base::onKeyEvent(ev); 
}

} // namespace

} // namespace
