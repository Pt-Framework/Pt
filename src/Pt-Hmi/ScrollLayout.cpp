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

//#define PT_SCROLL_LAYOUT_OLD 1

namespace Pt {

namespace Hmi {

ScrollLayout::ScrollLayout()
: _hmode(SizePolicy::Any)
, _vmode(SizePolicy::Any)
, _scrollByX(0)
, _scrollByY(0)
, _enableX(true)
, _enableY(true)
, _maxX(0)
, _maxY(0)
{
    //setAcceptInput(true);
}


ScrollLayout::~ScrollLayout()
{
}


void ScrollLayout::addItem(Widget& w)
{
    add(w);
}


void ScrollLayout::removeItem(Widget& w)
{
    remove(w);
}


void ScrollLayout::enableScrolling(bool scrollX, bool scrollY)
{
    _enableX = scrollX;
    _enableY = scrollY;
}


double ScrollLayout::maximumX() const
{
    return _maxX;
}


double ScrollLayout::maximumY() const
{
    return _maxY;
}


void ScrollLayout::scrollX(double posX)
{
    double xpos = surface().align(posX);

    double maxPosX = _maxX - size().width();

    if( xpos > maxPosX  )
        xpos = maxPosX;

    if(xpos < 0)
        xpos = 0;

    double delta = xpos - _scrollPos.x();

#ifdef PT_SCROLL_LAYOUT_OLD
    for( size_t i = 0; i < widgets().size();  ++i)
    {
        Widget* w =  widgets().at(i);

        Gfx::PointF pos = w->position();
        pos.subX(delta);
        w->move(pos);
    }

    _scrollPos.setX(xpos);
    _scrolledX.send(xpos);
    
    update();
#else
    _scrollByX += delta;

    _scrollPos.setX(xpos);
    _scrolledX.send(xpos);

    relayout();
#endif
}


void ScrollLayout::scrollY(double posY)
{
    double ypos = surface().align(posY);

    double maxPosY = _maxY - size().height();

    if( ypos > maxPosY  )
        ypos = maxPosY;

    if(ypos < 0)
        ypos = 0;

    double delta = ypos - _scrollPos.y();

#ifdef PT_SCROLL_LAYOUT_OLD
    for( size_t i = 0; i < widgets().size();  ++ i)
    {
        Widget* w =  widgets().at(i);

        Gfx::PointF pos = w->position();
        pos.subY(delta);
        w->move(pos);
    }

    _scrollPos.setY(ypos);
    _scrolledY.send(ypos);

    update();
#else    
    _scrollByY += delta;

    _scrollPos.setY(ypos);
    _scrolledY.send(ypos);

    relayout();
#endif
}


double ScrollLayout::scrollPosX() const
{
    return _scrollPos.x();
}


double ScrollLayout::scrollPosY() const
{
    return _scrollPos.y();
}


Pt::Signal<double>& ScrollLayout::scrolledX()
{
    return _scrolledX;
}


Pt::Signal<double>& ScrollLayout::scrolledY()
{
    return _scrolledY;
}


void ScrollLayout::setContentMode(SizePolicy::Mode horizontal,
                                  SizePolicy::Mode vertical)
{
    _hmode = horizontal;
    _vmode = vertical;

    relayout();
}


Gfx::SizeF ScrollLayout::onMeasure(const SizePolicy& policy)
{
    std::vector<Widget*>::const_iterator it;
    for(it = widgets().begin() ; it != widgets().end(); ++it)
    {
        Widget* item = *it;

        SizePolicy itemPolicy(_hmode, _vmode);
        itemPolicy.setSize( policy.size() );
        item->measure(itemPolicy);
   }

    double maxWidth = 0;
    double maxHeight = 0;

    for(size_t i = 0; i < widgets().size(); ++i)
    {
        Widget* w = widgets().at(i);

        const Gfx::PointF& wpos = w->position();
        const Gfx::SizeF& wsize = w->preferredSize();

        maxWidth = std::max( maxWidth, wpos.x() +
                                       wsize.width() +
                                       _scrollPos.x() - _scrollByX);

        maxHeight = std::max( maxHeight, wpos.y() +
                                         wsize.height() +
                                         _scrollPos.y() - _scrollByY);
    }

    _maxX = maxWidth;
    _maxY = maxHeight;

    return policy.size();
}


void ScrollLayout::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

#ifdef PT_SCROLL_LAYOUT_OLD
    std::vector<Widget*>::const_iterator it;
    for(it = widgets().begin() ; it != widgets().end(); ++it)
    {
        Widget* w = *it;
        w->layout( w->position(), w->preferredSize() );
    }
#else
    std::vector<Widget*>::const_iterator it;
    for(it = widgets().begin() ; it != widgets().end(); ++it)
    {
        Widget* w = *it;


        Gfx::PointF pos = w->position();
        pos.subY(_scrollByX);
        pos.subY(_scrollByY);

        w->move(pos);
        w->resize( w->preferredSize() );
    }

    _scrollByX = 0;
    _scrollByY = 0;
#endif
}


bool ScrollLayout::onMouseEvent(const MouseEvent& ev)
{
    return Base::onMouseEvent(ev);
}


bool ScrollLayout::onTouchEvent(const TouchEvent& ev)
{
    return Base::onTouchEvent(ev);
}


bool ScrollLayout::onScrollEvent(const ScrollEvent& ev)
{
    // TODO: call base class if new event propagation is implemented
    //Base::onScrollEvent(ev);

    if(ev.wheel() == ScrollEvent::Horizontal)
    {
        if(_enableX)
        {
            double deltaX = _scrollPos.x() - ev.delta();
            scrollX(deltaX);
        }
    }

    if(ev.wheel() == ScrollEvent::Vertical)
    {
        if(_enableY)
        {
            double deltaY = _scrollPos.y() - ev.delta();
            scrollY(deltaY);
        }
    }

    return true;
}

} // namespace

} // namespace
