/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan
  
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

#include <Pt/Hmi/ScrollView.h>
#include <algorithm>

namespace Pt {

namespace Hmi {

ScrollView::ScrollView()
: _widget(0)
, _scrollBarX(ScrollBar::Horizontal)
, _scrollBarY(ScrollBar::Vertical)
{
    _scrollBarX.resize( Gfx::SizeF(100, 32) );
    _scrollBarY.resize( Gfx::SizeF(32, 100) );

    _scrollBarX.changed() += Pt::slot(*this, &ScrollView::onScrollBarX);
    _scrollBarY.changed() += Pt::slot(*this, &ScrollView::onScrollBarY);

    _layout.scrolledX() += Pt::slot(*this, &ScrollView::onScrolledX);
    _layout.scrolledY() += Pt::slot(*this, &ScrollView::onScrolledY);

    add(_layout);
    add(_scrollBarX);
    add(_scrollBarY);
}


ScrollView::~ScrollView()
{
}


void ScrollView::enableScrollBars(bool h, bool v)
{
    _scrollBarX.show(h);
    _scrollBarY.show(v);
}


void ScrollView::setWidget(Widget& widget)
{
    if(_widget)
        _layout.remove(*_widget);

    _layout.add(widget);
    _widget = &widget;

    _scrollBarX.setRange( 0, _layout.maximumX() );
    _scrollBarY.setRange( 0, _layout.maximumY() );
}


void ScrollView::onScrollBarX(int pos)
{
    _layout.scrollX(pos);
}


void ScrollView::onScrollBarY(int pos)
{
    _layout.scrollY(pos);
}


void ScrollView::onScrolledX(int n)
{
    _scrollBarX.setPosition(n);
}


void ScrollView::onScrolledY(int n)
{
    _scrollBarY.setPosition(n);
}


void ScrollView::onResizeEvent(const ResizeEvent& ev)
{
    Widget::onResizeEvent(ev);

    double width = ev.size().width();

    if( _scrollBarY.isVisible() )
        width -= _scrollBarY.size().width();

    double height = ev.size().height();

    if( _scrollBarX.isVisible() )
        height -= _scrollBarX.size().height();

    _layout.move( Gfx::PointF(0,0) );
    _layout.resize( Gfx::SizeF( width, height) );

    _scrollBarX.show( width <= _layout.maximumX() );

    if( _scrollBarX.isVisible() )
    {
        _scrollBarX.move( Gfx::PointF(0, height) );
        _scrollBarX.resize( Gfx::SizeF(width, _scrollBarX.size().height()) );
    }

    _scrollBarY.show( height <= _layout.maximumY() );  
    
    if( _scrollBarY.isVisible() )
    {
        _scrollBarY.move( Gfx::PointF(width, 0) );
        _scrollBarY.resize( Gfx::SizeF(_scrollBarY.size().width(), height));
    }

    double hrange = _layout.maximumX() -_layout.size().width();
    updateScrollBar(_scrollBarX, hrange);

    double vrange = _layout.maximumY() -_layout.size().height();
    updateScrollBar(_scrollBarY, vrange);
}


void ScrollView::updateScrollBar(ScrollBar& sb, double maxRange)
{
    int oldPos = sb.position();
    int oldMax = sb.maximumPosition();

    sb.setRange( 0, static_cast<int>(maxRange) );    

    if(sb.maximumPosition() > 0)
    {      
      double relPos = double(oldPos) / oldMax;
      double newPos = maxRange * relPos + 0.5;
      sb.scroll( static_cast<int>(newPos) );
    }
}

} // namespace

} // namespace
