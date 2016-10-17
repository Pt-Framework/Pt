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

#include <Pt/Hmi/ScrollLayout.h>

namespace Pt {

namespace Hmi {

ScrollLayout::ScrollLayout()
: _lastScrollPos(0,0)
, _doScroll(false)
, _enableX(true)
, _enableY(true)
, _hrange(0)
, _vrange(0)
{
  setAcceptInput(true);
}


ScrollLayout::~ScrollLayout()
{
}


void ScrollLayout::enableXScroll(bool e)
{
    _enableX = e;
}


void ScrollLayout::enableYScroll(bool e)
{
    _enableY = e;
}


void ScrollLayout::scrollX(double position)
{
    if( position > (_hrange - size().width()) || position < 0  )
        return;

    double delta = position - _lastScrollPos.x();

    for( size_t i = 0; i < widgets().size();  ++i)
    {
        Widget* w =  widgets().at(i);
            
        Gfx::PointF pos = w->position();
        pos.subX(delta);
        w->move(pos);
    }

    _lastScrollPos.setX( position);

    _scrolledX.send( (int) _lastScrollPos.x() );
}


void ScrollLayout::scrollY(double position)
{    
    if( position > (_vrange - size().height()) || position < 0  )
        return;

    double delta = position - _lastScrollPos.y();

    for( size_t i = 0; i < widgets().size();  ++ i)
    {
        Widget* w =  widgets().at(i);
            
        Gfx::PointF pos = w->position();
        pos.subY(delta);
        w->move(pos);
    }

    _lastScrollPos.setY( position);

    _scrolledY.send( (int) _lastScrollPos.y() );
}


void ScrollLayout::reset()
{
    scrollX(0);
    scrollY(0);
  
    updateRange();
}


void ScrollLayout::onMouseEvent(const MouseEvent& ev)
{
   if( ev.isPress() )
   {
      _lastPos = ev.position();
      _doScroll = true;    
    }

   if( ev.isRelease() )
      _doScroll = false;

   if( _doScroll )
   {
      Gfx::PointF delta = ev.position() - _lastPos;

      if(_enableY) 
          scrollY( _lastScrollPos.y() - delta.y());

      if(_enableX) 
          scrollX( _lastScrollPos.x() - delta.x());

      _lastPos = ev.position();
   }
}

void ScrollLayout::onTouchEvent(const TouchEvent& ev)
{    
   if( ev.isPress() )
   {
      _lastPos = ev.position();
      _doScroll = true;    
    }

   if( ev.isRelease() )
      _doScroll = false;

   if( _doScroll )
   {
        Gfx::PointF delta = ev.position() - _lastPos;

        if(_enableY) 
            scrollY( _lastScrollPos.y() - delta.y());

        if(_enableX) 
            scrollX( _lastScrollPos.x() - delta.x());

        _lastPos = ev.position();
   }
}


void ScrollLayout::onAddWidget(Widget& w)
{
    Layout::onAddWidget(w);
    updateRange();
}


void ScrollLayout::onRemoveWidget(Widget& w)
{
    Layout::onRemoveWidget(w);
    updateRange();
}


void ScrollLayout::updateRange()
{
    double maxWidth = 0;
    double maxHeight = 0;
    
    for( size_t i = 0;  i < widgets().size(); ++ i)
    {
      const Widget& w =  *widgets()[i];

      maxWidth = std::max( maxWidth, w.position().x() +  w.size().width() );
      maxHeight= std::max( maxHeight, w.position().y() +  w.size().height() );
    }

    _hrange = static_cast<int>(maxWidth);
    _vrange = static_cast<int>(maxHeight);
}

} // namespace

} // namespace
