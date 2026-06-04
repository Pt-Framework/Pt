/* Copyright (C) 2015-2026 Marc Boris Duerner 
  
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

#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include "WindowImpl.h"

#include <Pt/Forms/Window.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PaintEvent.h>

#include <algorithm>

namespace Pt {
namespace Forms {

ScreenImpl::ScreenImpl(ApplicationImpl& app)
: _app(app)
, _parent(0)
, _screenScaling( app.outputScale() > 1 ? static_cast<double>(app.outputScale())
                                         : app.screenScaling() )
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

    if( _parent )
    {
        // Use a large virtual screen size; windows are individually positioned
        Gfx::SizeF size(3840, 2160);
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


Widget* ScreenImpl::onHitTest(const Gfx::PointF& p)
{
    std::vector<Window*>::reverse_iterator it;
    for(it = _windows.rbegin(); it != _windows.rend(); ++it)
    {
        Window* win = *it;
        WindowImpl* frame = static_cast<WindowImpl*>( win->frame() );

        Gfx::PointF pos = toFrame(*frame, p);

        Widget* hit = win->hitTest(pos);
        if( hit )
            return hit;
    }

    return 0;
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
    if( _parent )
        _parent->repaint(rect);
}


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
    RescaleEvent ev(frame, scaleFactor());
    frame.processEvent(ev);

    Base::onInit(frame);
}


void ScreenImpl::onRelease(WindowFrame& frame)
{
    Base::onRelease(frame);
}


void ScreenImpl::setCapture(Widget* /*capture*/)
{
    // Wayland does not support pointer grab directly.
    // Pointer confinement protocol could be used, but is not essential.
}


void ScreenImpl::onAutoCenter(WindowFrame& /*w*/, const Gfx::SizeF* /*size*/)
{
    // On Wayland the compositor decides window placement.
    // We cannot set absolute window positions.
}


void ScreenImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{
    // The total canvas scale is the product of the application (user) scale
    // and the system DPI scale.  WindowFrame uses this to size its physical
    // pixel buffer and to set the canvas transform, so that drawImage and all
    // other drawing primitives are rendered at the correct physical resolution.
    double scaling = ev.scaleFactor() * _screenScaling;

    RescaleEvent rev(*this, scaling);
    Base::onProcessRescaleEvent(rev);

    std::vector<Window*>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {
        Window* window = *wit;
        WindowFrame* frame = window->frame();

        RescaleEvent fev(*frame, scaling);
        frame->processEvent(fev);
    }
}


void ScreenImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);

    Gfx::SizeF size(3840, 2160);
    size /= scaleFactor();

    if( _parent )
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

        if( winRect.size().width() < 0.1 || winRect.height() < 0.1 )
            continue;

        frame->paint(winRect);
    }
}


void ScreenImpl::onPaintEvent(const PaintEvent& ev)
{
    Base::onPaintEvent(ev);

    const Gfx::RectF& rect = ev.rect();
    onPaint(rect);
}


void ScreenImpl::onPaint(const Gfx::RectF& /*rect*/)
{
}


void ScreenImpl::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    for(std::size_t i = 0; i < _windows.size(); ++i)
    {
        Window* w = _windows[i];
        WindowImpl* frame = static_cast<WindowImpl*>( w->frame() );
        frame->onEnable(*w, ev.enabled());
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
    return Base::onMouseEvent(ev);
}


void ScreenImpl::onProcessTouchEvent(const TouchEvent& ev)
{
    ev.widget()->processEvent(ev);
}


bool ScreenImpl::onTouchEvent(const TouchEvent& ev)
{
    return Base::onTouchEvent(ev);
}


void ScreenImpl::onProcessScrollEvent(const ScrollEvent& ev)
{
    ev.widget()->processEvent(ev);
}


bool ScreenImpl::onScrollEvent(const ScrollEvent& ev)
{
    return Base::onScrollEvent(ev);
}


void ScreenImpl::onProcessKeyEvent(const KeyEvent& ev)
{
    Widget* widget = ev.widget();
    if( widget )
        ev.widget()->processEvent(ev);
}


bool ScreenImpl::onKeyEvent(const KeyEvent& ev)
{
    return Base::onKeyEvent(ev);
}

} // namespace Forms
} // namespace Pt
