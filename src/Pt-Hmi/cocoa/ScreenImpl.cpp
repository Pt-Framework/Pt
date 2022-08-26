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
, _nextResponder(0)
, _screenScaling(1.0)
, _scaling(1.0)
, _enabled(true)
, _enabledState(true)
{
    _screenScaling = 1.0;
    _scaling = _screenScaling;

    // TODO:
    //NSScreen* mainScreen = [NSScreen mainScreen];
    //NSRect screenRect = [mainScreen visibleFrame];
    //_size = Gfx::SizeF(screenRect.width, screenRect.height);
    _size = Gfx::SizeF(640, 480);

    _eventReceived += Pt::slot(*this, &ScreenImpl::onProcessMouseEvent);
    _eventReceived += Pt::slot(*this, &ScreenImpl::onProcessTouchEvent);
    _eventReceived += Pt::slot(*this, &ScreenImpl::onProcessScrollEvent);
    _eventReceived += Pt::slot(*this, &ScreenImpl::onProcessKeyEvent);

    _eventReceived += Pt::slot(*this, &ScreenImpl::onProcessRescaleEvent);
    _eventReceived += Pt::slot(*this, &ScreenImpl::onProcessPaintEvent);
    _eventReceived += Pt::slot(*this, &ScreenImpl::onProcessEnableEvent);
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


const Gfx::SizeF& ScreenImpl::size() const
{
    return _size;
}


double ScreenImpl::scaleFactor() const
{
    return _scaling;
}

///////////////////////////////////////////////////////////////////////
// Responder
///////////////////////////////////////////////////////////////////////

Responder* ScreenImpl::onNextResponder()
{
    return _nextResponder;
}


Gfx::PointF ScreenImpl::onToScreen(const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF ScreenImpl::onFromScreen(const Gfx::PointF& pos) const
{
    return pos;
}

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

void ScreenImpl::onEvent(const Event& ev)
{
    _eventReceived.send(ev);
}


void ScreenImpl::onSetCapture(bool capture)
{
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
    RescaleEvent ev(w, _scaling);
    //w.processEvent(ev);
    Application::instance().loop().commitEvent(ev);
}


void ScreenImpl::onRelease(Window& w)
{
}


Gfx::PointF ScreenImpl::onFromWindow(const Window& w, 
                                     const Gfx::PointF& pos) const
{
    const MainWindowImpl* impl = static_cast<const MainWindowImpl*>( w.impl() );

    //Gfx::PointF physicalPos = w.surface().toPhysical(pos);
    //Gfx::PointF parentPos = impl->toScreen(physicalPos);
    //Gfx::PointF logicalPos = w.surface().toLogical(parentPos);
    //return logicalPos;

    Gfx::PointF parentPos = impl->toScreen(pos);
    return parentPos;
}


Gfx::PointF ScreenImpl::onToWindow(const Window& w, 
                                   const Gfx::PointF& pos) const
{
    const MainWindowImpl* impl = static_cast<const MainWindowImpl*>( w.impl() );

    //Gfx::PointF physicalPos = w.surface().toPhysical(pos);
    //Gfx::PointF windowPos = impl->fromScreen(physicalPos);
    //Gfx::PointF logicalPos = w.surface().toLogical(windowPos);
    //return logicalPos;


    Gfx::PointF windowPos = impl->fromScreen(pos);
    return windowPos;
}


Gfx::PointF ScreenImpl::onToScreen(const Window& w, 
                                   const Gfx::PointF& pos) const
{
    Gfx::PointF p = onFromWindow(w, pos);
    return toScreen(p);
}


Gfx::PointF ScreenImpl::onFromScreen(const Window& w, 
                                     const Gfx::PointF& pos) const
{
    Gfx::PointF p = fromScreen(pos);
    return onToWindow(w, p);
}


void ScreenImpl::onRepaint(Window& w, const Gfx::RectF& rect)
{
    Gfx::PointF screenPos = onFromWindow( w, rect.topLeft() );
    Gfx::RectF screenRect( screenPos, rect.size() );

    repaint(screenRect);
}


void ScreenImpl::onShow(Window& w, bool visible)
{
    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->show(visible);
}


void ScreenImpl::onActivate(Window& w, bool active)
{
    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->activate();
}


void ScreenImpl::onEnable(Window& w, bool enable)
{
    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->enable(enable);
}


void ScreenImpl::onMove(Window& w, const Gfx::PointF& pos)
{
    Gfx::PointF aligedPos = w.surface().align(pos);

    //w.impl()->scaleFactor(); ???

    //
    // TODO: scale here instead of in MainWindowImpl
    //

    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->move(aligedPos);
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

    //w.impl()->scaleFactor(); ???

    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->resize(alignedSize);
}


void ScreenImpl::onFrameChanged(Window& w)
{
}


void ScreenImpl::onStateChanged(Window& w)
{
}


void ScreenImpl::onClosing(Window& w)
{
    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    impl->close();
}


void ScreenImpl::onEnter(Window& w, Visual& v)
{
    Application::instance().screen().setPointer(&v);
}


void ScreenImpl::onSetCapture(Window& w, bool capture)
{
}

///////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////

void ScreenImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{
    onRescaleEvent(ev);
}


void ScreenImpl::onRescaleEvent(const RescaleEvent& ev)
{
    _scaling = ev.scaleFactor() * _screenScaling;

    std::vector<Window*>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {
        Window* window = *wit;
        
        RescaleEvent ev(*window, _scaling);
        window->processEvent(ev);
    }
}


void ScreenImpl::onProcessPaintEvent(const PaintEvent& ev)
{
    const Gfx::RectF& screenRect = ev.rect();

    //
    // paint screen
    //
    onPaintEvent(ev);

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
        winRect = Gfx::RectF( winRect.topLeft() * _scaling, 
                              winRect.size() * _scaling);

        MainWindowImpl* impl = static_cast<MainWindowImpl*>( window->impl() );
        impl->paint(winRect);
    }
}


void ScreenImpl::onPaintEvent(const PaintEvent& ev)
{    
    const Gfx::RectF& rect = ev.rect();
    onPaint(rect);
}


void ScreenImpl::onPaint(const Gfx::RectF& rect)
{
}


bool ScreenImpl::isEnabled() const
{
    return _enabledState;
}


void ScreenImpl::onProcessEnableEvent(const EnableEvent& ev)
{
    bool wasEnabled = isEnabled();

    _enabledState = ev.enabled();

    if( wasEnabled != isEnabled() )
    {
        onEnable( ev.enabled() );
    }

    for( size_t i = 0; i < _windows.size(); ++i)
    {
        Window* w = _windows[i];
        onEnable( *w, ev.enabled() );
    }
}


void ScreenImpl::onEnable(bool e)
{
}


void ScreenImpl::onProcessMouseEvent(const MouseEvent& ev)
{
    ev.visual()->processEvent(ev);
}


bool ScreenImpl::onMouseEvent(const MouseEvent& ev)
{ 
    // TODO: possibly pass on to application
    return false; 
}


void ScreenImpl::onProcessTouchEvent(const TouchEvent& ev)
{
    ev.visual()->processEvent(ev);
}


bool ScreenImpl::onTouchEvent(const TouchEvent& ev)
{ 
    // TODO: possibly pass on to application
    return false; 
}


void ScreenImpl::onProcessScrollEvent(const ScrollEvent& ev)
{
    ev.visual()->processEvent(ev);
}


bool ScreenImpl::onScrollEvent(const ScrollEvent& ev)
{ 
    // TODO: possibly pass on to application
    return false; 
}


void ScreenImpl::onProcessKeyEvent(const KeyEvent& ev)
{
    ev.visual()->processEvent(ev);
}


bool ScreenImpl::onKeyEvent(const KeyEvent& ev)
{ 
    // TODO: possibly pass on to application
    return false; 
}

} // namespace

} // namespace
