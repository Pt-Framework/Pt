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

#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include "WindowImpl.h"

#include <Pt/Forms/Window.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PaintEvent.h>

#include <Windows.h>

namespace Pt {

namespace Forms {

ScreenImpl::ScreenImpl(ApplicationImpl&)
: _parent(0)
, _screenScaling(1.0)
{
    HWND desktop = GetDesktopWindow();
    HDC screenDC = GetDC(desktop);

    int dpix = GetDeviceCaps(screenDC, LOGPIXELSX);
    //std::clog << "SCREEN SCALING DPI: " << dpix << std::endl;
    
    _screenScaling = dpix / 96.0;
    //std::clog << "SCREEN SCALING: " << _screenScaling << std::endl;

    ReleaseDC(desktop, screenDC);
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
        HWND desktop = GetDesktopWindow();

        RECT r;   
        GetWindowRect(desktop, &r);
    
        Gfx::SizeF size(r.right, r.bottom);
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


void ScreenImpl::setCapture(Widget* capture)
{
    if( ! capture )
    {
        //std::clog << "RELEASE CAPTURE HWND" << std::endl;
        ::ReleaseCapture();
        return;
    }

    std::vector<Window*>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {
        Window* window = *wit;
        
        if( capture == window || capture->isDescendantOf(*window) )
        {
            WindowImpl* impl = static_cast<WindowImpl*>( window->frame() );
            ::SetCapture( impl->hwnd() );
            //std::clog << "SET CAPTURE HWND: " << impl->hwnd() << std::endl;
            return;
        }
    }
}

//
// TODO: onAutoCenter in WindowManager
//
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

    w.window().move( Pt::Gfx::PointF(x, y) );
}

///////////////////////////////////////////////////////////////////////
// Widget
///////////////////////////////////////////////////////////////////////

Widget* ScreenImpl::onHitTest(const Gfx::PointF& p)
{
    POINT pnt = {0};
    pnt.x = p.x() * scaleFactor();
    pnt.y = p.y() * scaleFactor();

    HWND hwnd = WindowFromPoint(pnt);
    Window* win = Application::instance().impl()->findWindow(hwnd);
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
    double windowScaling = scaleFactor() / _screenScaling;
    RescaleEvent ev( frame, windowScaling );
    frame.processEvent(ev);

    Base::onInit(frame); // window::onConnect
}


void ScreenImpl::onRelease(WindowFrame& frame)
{
    Base::onRelease(frame);
}

///////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////

void ScreenImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{
    double scaling = ev.scaleFactor();

    RescaleEvent rev(*this, scaling * _screenScaling);
    Base::onProcessRescaleEvent(rev);

    std::vector<Window*>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {
        Window* window = *wit;
        WindowFrame* frame = window->frame();

        RescaleEvent wev(*frame, scaling);
        frame->processEvent(wev);
    }
}


void ScreenImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);

    HWND desktop = GetDesktopWindow();
    RECT r;   
    GetWindowRect(desktop, &r);

    Gfx::SizeF size(r.right, r.bottom);
    size /= ev.scaleFactor();

    _parent->onResize(*this, size);
}


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
        winRect = Gfx::RectF( winRect.topLeft() * window->scaleFactor(), 
                              winRect.size() * window->scaleFactor());
        
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
    Widget* widget = ev.widget();

    if( widget && widget != this && widget != _parent )
    {
        widget->processEvent(ev);
        return;
    }

    POINT screenPos = {0};
    screenPos.x = ev.position().x() * scaleFactor();
    screenPos.y = ev.position().y() * scaleFactor();

    Window* window = 0;
        
    for(HWND h = GetTopWindow(NULL); h != NULL; h = GetWindow(h, GW_HWNDNEXT))
    {
        RECT rect;
        GetClientRect(h, &rect);

        POINT pos = screenPos;
        ScreenToClient(h, &pos);
            
        if( PtInRect(&rect, pos) )
        {
            window = Application::instance().impl()->findWindow(h);
            if(window)
                break;
        }
    }

    if(window)
    {
        window->processEvent(ev);
        return;
    }

    Base::onProcessMouseEvent(ev);
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
    if(widget && widget != this && widget != _parent)
    {
        widget->processEvent(ev);
        return;
    }

    HWND h = GetActiveWindow();

    Window* window = Application::instance().impl()->findWindow(h);
    if(window)
        window->processEvent(ev);
}


bool ScreenImpl::onKeyEvent(const KeyEvent& ev)
{ 
    // TODO: possibly pass on to application
    return Base::onKeyEvent(ev); 
}

} // namespace

} // namespace
