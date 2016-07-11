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


double Screen::width() const
{
    return _impl->width();
}
        

double Screen::height() const
{
    return _impl->height();
}


Gfx::SizeF Screen::size() const
{
    return Gfx::SizeF( width(), height() );
}


Gfx::PointF Screen::toUnit(const Gfx::Point& value)
{
    return _impl->toUnit(value);
}


Gfx::SizeF Screen::toUnit(const Gfx::Size& value)
{
    return _impl->toUnit(value);
}


Gfx::Point Screen::fromUnit(const Gfx::PointF& value)
{
    return _impl->fromUnit(value);
}


Gfx::Size Screen::fromUnit(const Gfx::SizeF& value)
{
    return _impl->fromUnit(value);
}


Gfx::Rect Screen::fromUnit(const Gfx::RectF& value)
{
    return _impl->fromUnit(value);
}


double Screen::unitSizeInch() const
{
    return _impl->unitSizeInch();
}


double Screen::unitSizeMm() const
{
    return _impl->unitSizeMm();
}


double Screen::resolutionDPI() const
{
    return _impl->resolutionDPI();
}


ScreenImpl* Screen::impl()
{
    return _impl;
}


Gfx::PointF Screen::onToParent(const Window& w, const Gfx::PointF& pos) const
{
    return _impl->toParent(w, pos);
}


Gfx::PointF Screen::onFromParent(const Window& w, const Gfx::PointF& pos) const
{
    return _impl->fromParent(w, pos);
}


void Screen::onResize(Window& w, const Gfx::SizeF& s)
{
    w.impl()->resize(s);
    _impl->onResize(w, s);
}


void Screen::onMove(Window& w, const Gfx::PointF& p)
{
    std::clog << "Screen::onMove " << w.title() << " to " 
              << p.x() << ' ' << p.y() << std::endl;
    
    w.impl()->move(p);
    _impl->onMove(w, p);
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


void Screen::onActivate(Window& w)
{
    w.impl()->activate();
    _impl->onActivate(w);
}


void Screen::onEnable(Window& w, bool enable)
{
    w.impl()->enable(enable);
    _impl->onEnable(w, enable);      
}


void Screen::onUpdate(const Gfx::RectF& updateRect)
{
    _updateRect.unify(updateRect);
    ++_updates;

    UpdateEvent uev(vid(), _updateRect);
    Application::instance().loop().commitEvent(uev);
}


void Screen::onUpdate(Window& w, const Gfx::RectF& updateRect)
{
    Gfx::PointF pos = w.toScreen( updateRect.topLeft() );
    Gfx::RectF rect( pos, updateRect.size() );
    update(rect);
}


void Screen::onEvent(const Event& ev)
{
    if(ev.typeInfo() == typeid(UpdateEvent) )
    {
        const UpdateEvent& uev = static_cast<const UpdateEvent&>(ev);
        onUpdateEvent(uev);
    }

    if(ev.typeInfo() == typeid(PaintEvent) )
    {
        const PaintEvent& pev = static_cast<const PaintEvent&>(ev);
        onPaintEvent(pev);
    }
}


void Screen::onUpdateEvent(const UpdateEvent& ev)
{
    --_updates;
    
    // skip all updates except the last one
    if(_updates > 0)
      return ;
    
    const Gfx::RectF& screenRect = ev.rect();

    std::vector<Window*>::iterator it;
    for(it = _windows.begin(); it != _windows.end(); ++it)
    {
        Gfx::PointF pos = (*it)->fromScreen( screenRect.topLeft() );
        Gfx::RectF rect( pos, screenRect.size() );
        (*it)->onPaint(rect);
    }

    _updateRect.clear();

    PaintEvent pev(vid(), screenRect);
    Application::instance().loop().commitEvent(pev);
}


void Screen::onPaintEvent(const PaintEvent& ev)
{
    _impl->paint( ev.rect() );
}

} // namespace

} // namespace
