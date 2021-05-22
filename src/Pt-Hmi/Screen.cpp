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
  MA  02110-1301  USA
*/

#include "ScreenImpl.h"
#include "MainWindowImpl.h"
#include <Pt/Hmi/Screen.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/Window.h>

namespace Pt {

namespace Hmi {

Screen::Screen(ApplicationImpl& app)
: _impl( new ScreenImpl(app) )
, _updates(0)
{
    _impl->init(*this);

    _size = toLogical( _impl->size() );
}


Screen::~Screen()
{
    delete _impl;
}


void Screen::onInit(Window& w)
{
    _windows.push_back(&w);
    _impl->registerWindow(w);
}


void Screen::onDeinit(Window& w)
{
    _impl->unregisterWindow(w);

    std::vector<Window*>::iterator it;
    it = std::remove(_windows.begin(), _windows.end(), &w);
    _windows.erase(it, _windows.end());
}


Window* Screen::findWindow(const std::string& name)
{
    std::vector<Window*>::iterator it;
    for(it = _windows.begin(); it != _windows.end(); ++it)
    {
        if( (*it)->name() == name )
            return *it;
    }
    
    return 0;
}


Widget* Screen::findWidget(const std::string& name)
{
    std::vector<Window*>::iterator it;
    for(it = _windows.begin(); it != _windows.end(); ++it)
    {
        Window* w = *it;
        Widget* widget = w->findWidget(name);
        if( widget )
            return widget;
    }
    
    return 0;
}


const std::vector<Window*>& Screen::windows() const
{
  return _windows;
}


Visual* Screen::onParent() const
{
    return 0;
}


Gfx::PointF Screen::onToParent(const Gfx::PointF& pos) const
{
    return pos;
}

Gfx::PointF Screen::onFromParent(const Gfx::PointF& pos) const
{
    return pos;
}


const Gfx::PointF& Screen::onPosition() const
{
    return _position;
}


const Gfx::SizeF& Screen::onSize() const
{   
    return _size;

    //return toLogical( _impl->size() );
}


double Screen::onScaleFactor() const
{   
    // TODO: support multiple screens

    double scaling = Application::instance().scaleFactor();
    return _impl->scaleFactor() * scaling;
}


ScreenImpl* Screen::impl()
{
    return _impl;
}


Gfx::PointF Screen::onFromWindow(const Window& w, const Gfx::PointF& pos) const
{
    return _impl->toParent(w, pos);
}


Gfx::PointF Screen::onToWindow(const Window& w, const Gfx::PointF& pos) const
{
    return _impl->fromParent(w, pos);
}


void Screen::onResize(Window& w, const Gfx::SizeF& s)
{
    const Gfx::SizeF size = w.toPhysical(s);

    w.impl()->resize(size);
    _impl->onResize(w, s);
}


void Screen::onMove(Window& w, const Gfx::PointF& p)
{   
    const Gfx::PointF point = w.toPhysical(p);

    w.impl()->move(point);
    _impl->onMove(w, p);
}


void Screen::onFrameChanged(Window& w)
{
    _impl->onFrameChanged(w);
}


void Screen::onStateChanged(Window& w)
{
    _impl->onStateChanged(w);
}


void Screen::onClosing(Window& w)
{
    w.impl()->close();
    _impl->onClosing(w);
}


void Screen::onClose(Window& w)
{
    _impl->onClose(w);
}


void Screen::onShow(Window& w, bool visible)
{
    _impl->onShow(w, visible);
}


void Screen::onActivate(Window& w, bool active)
{
    w.impl()->activate();
    _impl->onActivate(w, active);
}


void Screen::onEnable(Window& w, bool enable)
{
    w.impl()->enable(enable);
    _impl->onEnable(w, enable);
}


void Screen::onEvent(const Event& ev)
{
    if(ev.typeInfo() == typeid(PaintEvent) )
    {
        const PaintEvent& pev = static_cast<const PaintEvent&>(ev);
        paintEvent(pev);
    }
}


//
//void Screen::repaint()
//{
//    Gfx::RectF rect( Gfx::PointF(0, 0), size() );
//    repaint(rect);
//}
//
//
//void Screen::repaint(const Gfx::RectF& updateRect)
//{
//    _updateRect.unify(updateRect);
//    ++_updates;
//
//    PaintEvent uev(vid(), _updateRect);
//    Application::instance().loop().commitEvent(uev);
//}


void Screen::onRelayout()
{
}


void Screen::onRepaint(Window& w, const Gfx::RectF& windowRect)
{
     Pt::Gfx::PointF pos = fromClient( windowRect.topLeft(), w );
     Gfx::RectF rect( pos, windowRect.size() );
   
     onRepaint(rect);
}


void Screen::onRepaint(const Gfx::RectF& rect)
{
    _updateRect.unify(rect);
    ++_updates;

    PaintEvent uev(vid(), _updateRect);
    Application::instance().loop().commitEvent(uev);
}


void Screen::paintEvent(const PaintEvent& ev)
{
    --_updates;

    // skip all updates except the last one
    if(_updates > 0)
      return;
    
    _updateRect.clear();

    const Gfx::RectF& screenRect = ev.rect();
    //onPaintContent(screenRect);

    // onPaint
    onPaintContent(screenRect);

    std::vector<Window*>::iterator it;
    for(it = _windows.begin(); it != _windows.end(); ++it)
    {
        Window* window = *it;

        Gfx::PointF winPos = toClient( screenRect.topLeft(), *window );
        Gfx::RectF winRect( winPos, screenRect.size() );

        winRect = winRect.intersect( Gfx::RectF( window->size() ) );

        //paintContent(*window, winRect);

        // send paint event to window
        window->impl()->paint(winRect);
        //_impl->paint(*window, winRect);
    }

    _impl->paint(screenRect);
}

// onPaint
void Screen::onPaintContent(const Gfx::RectF& screenRect)
{
    // onPaintContent (screen specific)
    onPaintScreen(screenRect);

    //std::clog << std::endl;
    //_clock.start();
    //std::clog << "Screen::onPaintContent " << std::endl;

    //static int nnn = 0;
    //std::clog << "screen update: " 
    //          << _clock.stop().toUSecs() << " usecs. " 
    //          << ++nnn << std::endl;
    //std::clog << "    " << ev.rect().topLeft().x() << ',' << ev.rect().topLeft().y()
    //          << ' ' << ev.rect().width() << 'x' << ev.rect().height() << std::endl;
}

// onPaintContent (screen specific)
void Screen::onPaintScreen(const Gfx::RectF& rect)
{
    // _impl->paintContent(rect);
}

} // namespace

} // namespace
