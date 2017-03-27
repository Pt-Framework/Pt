/* Copyright (C) 2017 Marc Boris Duerner 
  
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

#include <Pt/Hmi/GridLayout.h>
#include <algorithm>

namespace Pt {

namespace Hmi {

GridLayout::GridLayout(Orientation o, std::size_t span)
: _orientation(o)
, _span(span)
{
}


GridLayout::~GridLayout()
{
}


void GridLayout::setOrientation(Orientation o, std::size_t span)
{
    _orientation = o;
    _span = span;
}


Gfx::SizeF GridLayout::onMeasure(const SizePolicy& policy)
{
    Gfx::SizeF contentSize;

    switch(_orientation)
    {
        default:
        case Vertical:
            contentSize = onMeasureVertical(policy);
            break;

        case Horizontal:
            contentSize = onMeasureHorizontal(policy);
            break;
    }

    return contentSize;
}


Gfx::SizeF GridLayout::onMeasureVertical(const SizePolicy& policy)
{
    double itemsWidth = policy.size().width() - padding().leftRight(); 
    double itemsHeight = policy.size().height() - padding().topBottom(); 

    // TODO: handle Any case for width

    Gfx::SizeF itemSize;

    std::vector<Widget*>::const_iterator it;
    std::vector<Widget*>::const_iterator end = widgets().end();

    for(it = widgets().begin(); it != end; ++it)
    {
        Widget* item = *it;

        if( ! item->isVisible() )
            continue;

        SizePolicy itemPolicy(SizePolicy::Preferred, SizePolicy::Any);
        itemPolicy.setWidth( itemsWidth - item->margin().leftRight() );
        itemPolicy.setHeight( itemsHeight - item->margin().topBottom() );

        item->measure(itemPolicy);
        Gfx::SizeF prefSize = item->preferredSize();

        double itemWidth = prefSize.width() + item->margin().leftRight();
        double itemHeight = prefSize.height() + item->margin().topBottom();
        
        double width = std::max( itemWidth, itemSize.width() );
        double height = std::max( itemHeight, itemSize.height() );
        itemSize.set(width, height);
    }

    std::size_t cols = _span;

    if(_span == 0 && itemSize.width() > 0)
        cols = static_cast<std::size_t>( itemsWidth / itemSize.width() );

    if(cols == 0)
        cols = 1;

    std::size_t rows = widgets().size() / cols;
    if(widgets().size() % cols > 0)
        ++rows;

    return Gfx::SizeF(cols * itemSize.width(), 
                      rows * itemSize.height());
}


Gfx::SizeF GridLayout::onMeasureHorizontal(const SizePolicy& policy)
{
    double itemsWidth = policy.size().width() - padding().leftRight(); 
    double itemsHeight = policy.size().height() - padding().topBottom(); 

    // TODO: handle Any case for height

    Gfx::SizeF itemSize;

    std::vector<Widget*>::const_iterator it;
    std::vector<Widget*>::const_iterator end = widgets().end();

    for(it = widgets().begin(); it != end; ++it)
    {
        Widget* item = *it;

        if( ! item->isVisible() )
            continue;

        SizePolicy itemPolicy(SizePolicy::Any, SizePolicy::Preferred);
        itemPolicy.setWidth( itemsWidth - item->margin().leftRight() );
        itemPolicy.setHeight( itemsHeight - item->margin().topBottom() );

        item->measure(itemPolicy);
        Gfx::SizeF prefSize = item->preferredSize();

        double itemWidth = prefSize.width() + item->margin().leftRight();
        double itemHeight = prefSize.height() + item->margin().topBottom();
        
        double width = std::max( itemWidth, itemSize.width() );
        double height = std::max( itemHeight, itemSize.height() );
        itemSize.set(width, height);
    }

    std::size_t rows = _span;

    if(_span == 0 && itemSize.height() > 0)
        rows = static_cast<std::size_t>( itemsHeight / itemSize.height() );

    if(rows == 0)
        rows = 1;

    std::size_t cols = widgets().size() / rows;
    if(widgets().size() % rows > 0)
        ++cols;

    return Gfx::SizeF(cols * itemSize.width(), 
                      rows * itemSize.height());
}


void GridLayout::onLayout(const Gfx::RectF& rect)
{
    //
    // determine the cell size from maximum item width and height
    //

    Gfx::SizeF itemSize(0, 0);

    std::vector<Widget*>::const_iterator it;
    std::vector<Widget*>::const_iterator end = this->widgets().end();

    for(it = widgets().begin(); it != end; ++it)
    {
        Widget* item = *it;

        if( ! item->isVisible() )
            continue;

        double itemWidth = item->preferredSize().width() + item->margin().leftRight();
        double itemHeight = item->preferredSize().height() + item->margin().topBottom();
        
        double width = std::max( itemWidth, itemSize.width() );
        double height = std::max( itemHeight, itemSize.height() );
        itemSize.set(width, height);
    }

    //
    // layout widgets
    //

    switch(_orientation)
    {
        default:
        case Vertical:
            layoutVertical(itemSize);
            break;

        case Horizontal:
            layoutHorizontal(itemSize);
            break;
    }
}


void GridLayout::layoutVertical(const Gfx::SizeF& itemSize)
{
    std::size_t cols = _span;

    if(_span == 0 && itemSize.width() > 0)
    {
        double itemsWidth = size().width() - padding().leftRight();
        cols = static_cast<std::size_t>( itemsWidth / itemSize.width() );
        cols = std::min(cols, widgets().size());
    }

    if(cols == 0)
          cols = 1;

    std::vector<Widget*>::const_iterator it;
    std::vector<Widget*>::const_iterator end = widgets().end();

    double width = size().width() - padding().leftRight();
    double itemsWidth = (itemSize.width() * cols);
    double itemsPadding = (width - itemsWidth) / 2;
    double startX = padding().left() + itemsPadding;

    double itemX = startX;
    double itemY = padding().top();
    unsigned col = 0;

    for(it = widgets().begin(); it != end; ++it)
    {
        Widget* widget = *it;
        
        if( ! widget->isVisible() )
            continue;

        // x/y position within a cell
        double x = ( itemSize.width() - widget->size().width() ) / 2;
        double y = ( itemSize.height() - widget->size().height() ) / 2;

        Gfx::PointF pos(itemX + x, itemY + y);
        widget->move(pos);

        itemX += itemSize.width();
        
        if(++col == cols)
        {
            col = 0;
            itemX = startX;
            itemY += itemSize.height();
        }
    }
}


void GridLayout::layoutHorizontal(const Gfx::SizeF& itemSize)
{
    std::size_t rows = _span;

    if(_span == 0 && itemSize.height() > 0)
    {
        double itemsHeight = size().height() - padding().topBottom();
        rows = static_cast<std::size_t>( itemsHeight / itemSize.height() );
        rows = std::min(rows, widgets().size());
    }

    if(rows == 0)
          rows = 1;

    std::vector<Widget*>::const_iterator it;
    std::vector<Widget*>::const_iterator end = widgets().end();

    double height = size().height() - padding().topBottom();
    double itemsHeight = (itemSize.height() * rows);
    double itemsPadding = (height - itemsHeight) / 2;
    double startY = padding().top() + itemsPadding;

    double itemX = padding().left();
    double itemY = startY;
    unsigned row = 0;
    
    for(it = widgets().begin(); it != end; ++it)
    {
        Widget* widget = *it;
        
        if( ! widget->isVisible() )
            continue;

        // x/y position within a cell
        double x = ( itemSize.width() - widget->size().width() ) / 2;
        double y = ( itemSize.height() - widget->size().height() ) / 2;

        Gfx::PointF pos(itemX + x, itemY + y);
        widget->move(pos);

        itemY += itemSize.height();
        
        if(++row == rows)
        {
            row = 0;
            itemY = startY;
            itemX += itemSize.width();
        }
    }
}

} // namespace

} // namespace
