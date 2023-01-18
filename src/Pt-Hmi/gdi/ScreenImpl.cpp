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
#include "MainWindowImpl.h"

#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PaintEvent.h>

#include <Windows.h>

namespace Pt {

namespace Hmi {

ScreenImpl::ScreenImpl(ApplicationImpl&)
: _parent(0)
, _nextResponder(0)
//, _capture(0)
, _screenScaling(1.0)
//, _scaling(1.0)
//, _enabled(true)
//, _enabledState(true)
{
    HWND desktop = GetDesktopWindow();
    //HDC screenDC = GetDC(desktop);

    //int dpix = GetDeviceCaps(screenDC, LOGPIXELSX);
    //std::clog << "SCALING DPI: " << dpix << std::endl;
    
    //_scaling = _screenScaling;
    //_screenScaling = dpix / 96.0;
    //std::clog << "SCALING: " << _screenScaling << std::endl;

    //ReleaseDC(desktop, screenDC);
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

        ResizeEvent rev(*_parent, size);
        _parent->processEvent(rev);
    }
}


void ScreenImpl::setNextResponder(Responder* r)
{
    _nextResponder = r;
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


//double ScreenImpl::scaleFactor() const
//{ 
//    return _scaling; 
//}

///////////////////////////////////////////////////////////////////////
// Responder
///////////////////////////////////////////////////////////////////////

Responder* ScreenImpl::onNextResponder()
{
    return _nextResponder;
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
    POINT pnt;
    pnt.x = p.x() * scaleFactor();
    pnt.y = p.y() * scaleFactor();

    HWND hwnd = WindowFromPoint(pnt);
    Window* win = Application::instance().impl()->findWindow(hwnd);
    if( ! win )
        return 0;

    Gfx::PointF pos = toWindow(*win, p);
    return win->hitTest(pos);
}


Gfx::PointF ScreenImpl::onToParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->toParent(pos);
}


Gfx::PointF ScreenImpl::onFromParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->fromParent(pos);
}


void ScreenImpl::onEvent(const Event& ev)
{
    Base::onEvent(ev);
}


void ScreenImpl::onRepaintRequest(const Gfx::RectF& rect)
{
    if(_parent)
        _parent->repaint(rect);
}

///////////////////////////////////////////////////////////////////////
// WindowManager
///////////////////////////////////////////////////////////////////////

WindowImpl* ScreenImpl::onCreateWindow(const WindowType& type)
{
    return new MainWindowImpl(type);
}


void ScreenImpl::onAttach(Window& w)
{
    _windows.push_back(&w);

    w.setNextResponder(this);
}


void ScreenImpl::onDetach(Window& w)
{
    w.setNextResponder(0);

    std::vector<Window*>::iterator it;
    it = std::remove(_windows.begin(), _windows.end(), &w);
    _windows.erase(it, _windows.end());
}


void ScreenImpl::onInit(Window& w)
{
    RescaleEvent ev( w, scaleFactor() );
    w.processEvent(ev);
    //Application::instance().loop().commitEvent(ev);
}


void ScreenImpl::onRelease(Window& w)
{
}


Gfx::PointF ScreenImpl::onFromWindow(const Window& w, 
                                     const Gfx::PointF& pos) const
{
    const MainWindowImpl* impl = static_cast<const MainWindowImpl*>( w.impl() );

    Gfx::PointF physicalPos = w.surface().toPhysical(pos);
    Gfx::PointF parentPos = impl->toScreen(physicalPos);
    Gfx::PointF logicalPos = w.surface().toLogical(parentPos);
    return logicalPos;
}


Gfx::PointF ScreenImpl::onToWindow(const Window& w, 
                                   const Gfx::PointF& pos) const
{
    const MainWindowImpl* impl = static_cast<const MainWindowImpl*>( w.impl() );

    Gfx::PointF physicalPos = w.surface().toPhysical(pos);
    Gfx::PointF windowPos = impl->fromScreen(physicalPos);
    Gfx::PointF logicalPos = w.surface().toLogical(windowPos);
    return logicalPos;
}


void ScreenImpl::onShow(Window& w, bool visible)
{
    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->show(visible);
}


void ScreenImpl::onActivate(Window& w, bool active)
{
    if( ! active )
        return;

    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->activate();
}


void ScreenImpl::onEnableRequest(Window& w, bool enable)
{
    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->enable(enable);
}


void ScreenImpl::onMove(Window& w, const Gfx::PointF& pos)
{
    Gfx::PointF aligedPos = w.surface().align(pos);

    const Gfx::PointF point = w.surface().toPhysical(aligedPos);

    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->move(point);
}


void ScreenImpl::onResize(Window& w, const Gfx::SizeF& s)
{
    //
    // align to physical pixel grid
    //
    Gfx::SizeF alignedSize = w.surface().align(s);

    //
    // maximum width and height
    //
    if( alignedSize.width() > w.maximumSize().width() )
        alignedSize.setWidth( w.maximumSize().width() );

    if( alignedSize.height() > w.maximumSize().height() )
        alignedSize.setHeight( w.maximumSize().height() );

    if( alignedSize.width() < w.minimumSize().width() )
        alignedSize.setWidth( w.minimumSize().width() );

    if( alignedSize.height() < w.minimumSize().height() )
        alignedSize.setHeight( w.minimumSize().height() );

    const Gfx::SizeF size = w.surface().toPhysical(alignedSize);

    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->resize(size);
}


void ScreenImpl::onSetAbove(Window& w, bool above)
{
    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->setAbove(above);
}


void ScreenImpl::onSetTitle(Window& w, const std::string& text)
{
    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->setTitle(text);
}


void ScreenImpl::onSetIcon(Window& w, const Gfx::Image& icon)
{
    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->setIcon(icon);
}


void ScreenImpl::onSetState(Window& w, const WindowState& state)
{
    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->setState(state);
}


void ScreenImpl::onSetSizeLimits(Window& w, const Gfx::SizeF& minSize, 
                                            const Gfx::SizeF& maxSize)
{
    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->setMinimumSize(minSize);
    impl->setMaximumSize(maxSize);
}


void ScreenImpl::onClosing(Window& w)
{
    //MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    //if(impl)
    //    PostMessage(impl->hwnd(), WM_CLOSE, 0, 0);

    CloseEvent ev(w);
    w.processEvent(ev);
}


void ScreenImpl::setCapture(Visual* capture)
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
            MainWindowImpl* impl = static_cast<MainWindowImpl*>( window->impl() );
            ::SetCapture( impl->hwnd() );
            //std::clog << "SET CAPTURE HWND: " << impl->hwnd() << std::endl;
            return;
        }
    }
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
        
        RescaleEvent ev(*window, scaling);
        window->processEvent(ev);
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

    ResizeEvent rev(*_parent, size);
    _parent->processEvent(rev);
}


void ScreenImpl::onRepaint(Window& w, const Gfx::RectF& rect)
{
    Gfx::PointF screenPos = onFromWindow( w, rect.topLeft() );
    Gfx::RectF screenRect( screenPos, rect.size() );

    repaint(screenRect);
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

        Gfx::PointF winPos = onToWindow( *window, screenRect.topLeft() );
        Gfx::RectF winRect( winPos, screenRect.size() );

        winRect = winRect.intersect( Gfx::RectF( window->size() ) );

        // send (native) paint event to window
        winRect = Gfx::RectF( winRect.topLeft() * window->scaleFactor(), 
                              winRect.size() * window->scaleFactor());

        MainWindowImpl* impl = static_cast<MainWindowImpl*>( window->impl() );
        impl->paint(winRect);
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
        onEnableRequest( *w, ev.enabled() );
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

    // TODO: dispatch to active HWND
}


bool ScreenImpl::onKeyEvent(const KeyEvent& ev)
{ 
    // TODO: possibly pass on to application
    return Base::onKeyEvent(ev); 
}

} // namespace

} // namespace
