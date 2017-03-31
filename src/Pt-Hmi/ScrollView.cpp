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
: _hasScrollBars(true)
, _widget(0)
, _scrollBarX(ScrollBar::Horizontal)
, _scrollBarY(ScrollBar::Vertical)
{
    SizePolicy barPolicyX(SizePolicy::Preferred, SizePolicy::Fixed);
    barPolicyX.setHeight(32);
    _scrollBarX.setSizePolicy(barPolicyX);

    SizePolicy barPolicyY(SizePolicy::Fixed, SizePolicy::Preferred);
    barPolicyY.setWidth(32);
    _scrollBarY.setSizePolicy(barPolicyY);

    _scrollBarX.changed() += Pt::slot(*this, &ScrollView::onScrollBarX);
    _scrollBarY.changed() += Pt::slot(*this, &ScrollView::onScrollBarY);

    _scrollLayout.scrolledX() += Pt::slot(*this, &ScrollView::onScrolledX);
    _scrollLayout.scrolledY() += Pt::slot(*this, &ScrollView::onScrolledY);

    add(_scrollLayout);
    add(_scrollBarX);
    add(_scrollBarY);
}


ScrollView::~ScrollView()
{
}


void ScrollView::setScrollBars(bool hasScrollBars)
{
    _hasScrollBars = hasScrollBars;

    relayout();
}


void ScrollView::setContent(Widget& widget)
{
    if(_widget)
        _scrollLayout.removeItem(*_widget);

    _scrollLayout.addItem(widget);
    _widget = &widget;
}


void ScrollView::setContentMode(SizePolicy::Mode horizontal, 
                                SizePolicy::Mode vertical)
{
    _scrollLayout.setContentMode(horizontal, vertical);
}


int ScrollView::maximumX() const
{
    return _scrollLayout.maximumX();
}


int ScrollView::maximumY() const
{
    return _scrollLayout.maximumY();
}


void ScrollView::onScrollBarX(int pos)
{
    _scrollLayout.scrollX(pos);
}


void ScrollView::onScrollBarY(int pos)
{
    _scrollLayout.scrollY(pos);
}


void ScrollView::onScrolledX(int n)
{
    _scrollBarX.setPosition(n);
}


void ScrollView::onScrolledY(int n)
{
    _scrollBarY.setPosition(n);
}


Gfx::Size ScrollView::onMeasure(const SizePolicy& policy)
{   
    Pt::ssize_t width = policy.size().width();
    Pt::ssize_t height = policy.size().height();
    
    SizePolicy contentPolicy(SizePolicy::Fixed, SizePolicy::Fixed);
    contentPolicy.setSize(width, height);

    _scrollLayout.measure(contentPolicy);

    // TODO: extend the scroll range for the width/height of the visible
    //       scrollbars instead of shrinking the scroll layout

    if( _hasScrollBars && width < _scrollLayout.maximumX() )
        height -= _scrollBarX.size().height();

    if( _hasScrollBars && height < _scrollLayout.maximumY() )
        width -= _scrollBarY.size().width();

    if( _scrollBarX.isVisible() )
    {
        SizePolicy barPolicy(SizePolicy::Fixed, SizePolicy::Preferred);
        barPolicy.setSize(width, height);

        _scrollBarX.measure(barPolicy);
    }

    if( _scrollBarY.isVisible() )
    {
        SizePolicy barPolicy(SizePolicy::Preferred, SizePolicy::Fixed);
        barPolicy.setSize(width, height);

        _scrollBarY.measure(barPolicy);
    }

    return policy.size();
}


void ScrollView::onLayout(const Gfx::Rect& rect)
{
    _scrollLayout.layout( Gfx::Point(0, 0), rect.size() );

    Pt::ssize_t width = rect.size().width();
    Pt::ssize_t height = rect.size().height();

    if(_hasScrollBars)
    {
        _scrollBarX.show( width < _scrollLayout.maximumX() );
        _scrollBarY.show( height < _scrollLayout.maximumY() ); 
    }
    else
    {
        _scrollBarX.show(false);
        _scrollBarY.show(false); 
    }

    if( _scrollBarX.isVisible() )
        height -= _scrollBarX.preferredSize().height();

    if( _scrollBarY.isVisible() )
        width -= _scrollBarY.preferredSize().width();

    // TODO: shrink _scrollLayout

    if( _scrollBarX.isVisible() )
    {
        _scrollBarX.layout( Gfx::Point(0, height),
                            Gfx::Size(width, _scrollBarX.preferredSize().height()) );
    }

    if( _scrollBarY.isVisible() )
    {
        _scrollBarY.layout( Gfx::Point(width, 0),
                            Gfx::Size(_scrollBarY.preferredSize().width(), height) );
    }

    Pt::ssize_t hrange = _scrollLayout.maximumX() - _scrollLayout.size().width();
    if(hrange >= 0)
        updateScrollBar(_scrollBarX, hrange);

    Pt::ssize_t vrange = _scrollLayout.maximumY() -_scrollLayout.size().height();
    if(vrange >= 0)
        updateScrollBar(_scrollBarY, vrange);
}



void ScrollView::updateScrollBar(ScrollBar& sb, Pt::ssize_t maxRange)
{
    int oldPos = sb.position();
    int oldMax = sb.maximumPosition();

    // TODO: setRange might cause a scroll !!!
    sb.setRange( 0, static_cast<int>(maxRange) ); 
    
    if(sb.maximumPosition() > 0)
    {      
        double relPos = Pt::ssize_t(oldPos) / (double)oldMax;
        double newPos = maxRange * relPos + 0.5;

        sb.setPosition( static_cast<int>(newPos) );
    }
}

} // namespace

} // namespace
