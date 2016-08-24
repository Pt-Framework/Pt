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
: _hScrollBar(ScrollBar::Horizontal)
, _vScrollBar(ScrollBar::Vertical)
, _maxWidth(0)
, _maxHeight(0)
{
    _hScrollBar.resize( Gfx::SizeF(100,16) );
    _vScrollBar.resize( Gfx::SizeF(16,100) );

    _hScrollBar.changed() += Pt::slot(*this, &ScrollView::onHScroll);
    _vScrollBar.changed() += Pt::slot(*this, &ScrollView::onVScroll);

    add(_hScrollBar);
    add(_vScrollBar);
    add(_layout);
}


ScrollView::~ScrollView()
{
}


void ScrollView::addLayout( Widget& widget)
{
    _layout.add(widget);
    
    for(size_t i = 0; i < _layout.widgets().size(); ++i)
    {
        Widget* w = _layout.widgets().at(i);
        
        _maxWidth = std::max( _maxWidth, 
                              w->position().x() + w->size().width() );
        
        _maxHeight = std::max( _maxHeight, 
                               w->position().y() + w->size().height() );         
    }

    int hrange = static_cast<int>(_maxWidth);
    int vrange = static_cast<int>(_maxHeight);

    _hScrollBar.setRange(0, hrange);
    _vScrollBar.setRange(0, vrange);    
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
    _layout.resize( Gfx::SizeF( ev.size().width() - _hScrollBar.size().height(),  
                                ev.size().height() - _vScrollBar.size().width()) );

    _hScrollBar.show(_layout.size().width() <= _maxWidth);
    _hScrollBar.move(Gfx::PointF( 0, 
                                  ev.size().height() - _hScrollBar.size().height()) );
    _hScrollBar.resize( Gfx::SizeF( ev.size().width() - _hScrollBar.size().height(), 
                                    _hScrollBar.size().height()) );
    
    double hrange = _maxWidth -_layout.size().width();
    _hScrollBar.setRange(0, static_cast<int>(hrange));

    _vScrollBar.show(_layout.size().height() <= _maxHeight);  
    _vScrollBar.move( Gfx::PointF( ev.size().width() - _vScrollBar.size().width(), 
                                   0) );
    _vScrollBar.resize( Gfx::SizeF(_vScrollBar.size().width(), 
                                   ev.size().height() - _vScrollBar.size().width()));

    double vrange = _maxHeight -_layout.size().height();
    _vScrollBar.setRange( 0, static_cast<int>(vrange) );    
}

} // namespace

} // namespace
