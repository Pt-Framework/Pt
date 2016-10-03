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
, _hScrollBar(ScrollBar::Horizontal)
, _vScrollBar(ScrollBar::Vertical)
{
    _hScrollBar.resize( Gfx::SizeF(100,32) );
    _vScrollBar.resize( Gfx::SizeF(32,100) );

    _hScrollBar.changed() += Pt::slot(*this, &ScrollView::onHScroll);
    _vScrollBar.changed() += Pt::slot(*this, &ScrollView::onVScroll);

    _layout.scrollChanged() += Pt::slot(*this, &ScrollView::onScrollChanged);

    add(_layout);
    add(_hScrollBar);
    add(_vScrollBar);
}


ScrollView::~ScrollView()
{
}


void ScrollView::showScrollBars( bool h, bool v)
{
  _hScrollBar.show(h);
  _vScrollBar.show(v);
}

void ScrollView::setWidget(Widget& widget)
{
    if(_widget)
        _layout.remove(*_widget);

    _layout.add(widget);
    _widget = &widget;

    _hScrollBar.setRange(0, _layout.hRange() );
    _vScrollBar.setRange(0, _layout.vRange() );
}


void ScrollView::onScrollChanged(ScrollLayout& layout, int w , int h)
{
  _hScrollBar.setPosition(w);
  _vScrollBar.setPosition(h);
}


void ScrollView::onHScroll(ScrollBar& bar, int pos)
{
    _layout.scrollX(pos);
}


void ScrollView::onVScroll(ScrollBar& bar, int pos)
{
    _layout.scrollY(pos);
}


void ScrollView::onResizeEvent(const ResizeEvent& ev)
{
    Widget::onResizeEvent(ev);

    _layout.move( Gfx::PointF(0,0) );

    Pt::Gfx::SizeF hSize = _hScrollBar.isVisible() ? _hScrollBar.size() : Pt::Gfx::SizeF(0,0);
    Pt::Gfx::SizeF vSize = _vScrollBar.isVisible() ? _vScrollBar.size() : Pt::Gfx::SizeF(0,0);

    _layout.resize( Gfx::SizeF( ev.size().width() - hSize.height(),  
                                ev.size().height() - vSize.width()) );

    _hScrollBar.show(_layout.size().width() <= _layout.hRange());

    _hScrollBar.move(Gfx::PointF( 0, 
                                  ev.size().height() - hSize.height()) );
    _hScrollBar.resize( Gfx::SizeF( ev.size().width() - hSize.height(), 
                                    hSize.height()) );
    

    _vScrollBar.show(_layout.size().height() <= _layout.vRange());  

    _vScrollBar.move( Gfx::PointF( ev.size().width() - vSize.width(), 
                                   0) );
    _vScrollBar.resize( Gfx::SizeF(_vScrollBar.size().width(), 
                                   ev.size().height() - vSize.width()));

    double hrange = _layout.hRange() -_layout.size().width();
    updateScrollBar(_hScrollBar, hrange);

    double vrange = _layout.vRange() -_layout.size().height();
    updateScrollBar(_vScrollBar, vrange);
}


void ScrollView::updateScrollBar(ScrollBar& scroll, double maxRange)
{
    int oldPos = scroll.position();
    int oldMax = scroll.maximumPosition();

    scroll.setRange( 0, static_cast<int>(maxRange) );    

    if(scroll.maximumPosition() > 0)
    {      
      double relPos = double(oldPos) / oldMax;
      double newPos = maxRange * relPos + 0.5;
      scroll.setPosition( static_cast<int>(newPos) );
    }
}

} // namespace

} // namespace
