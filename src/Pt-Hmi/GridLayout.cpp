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

void GridLayoutVertical(Widget& parent, const Gfx::SizeF& itemSize, unsigned cols)
{
    std::vector<Widget*>::const_iterator it;
    std::vector<Widget*>::const_iterator end = parent.widgets().end();

    double itemX = 0;
    double itemY = 0;
    unsigned col = 0;

    for(it = parent.widgets().begin(); it != end; ++it)
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
            itemX = 0;
            itemY += itemSize.height();
        }
    }
}


void GridLayoutHorizontal(Widget& parent, const Gfx::SizeF& itemSize, unsigned rows)
{
    std::vector<Widget*>::const_iterator it;
    std::vector<Widget*>::const_iterator end = parent.widgets().end();

    double itemX = 0;
    double itemY = 0;
    unsigned row = 0;
    
    for(it = parent.widgets().begin(); it != end; ++it)
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
            itemY = 0;
            itemX += itemSize.width();
        }
    }
}


GridLayout::GridLayout(GrowStyle g, std::size_t span)
: _growStyle(g)
, _span(span)
{
}


GridLayout::~GridLayout()
{
}


void GridLayout::setStyle(GrowStyle g, std::size_t span)
{
    _growStyle = g;
    _span = span;
}


void GridLayout::onLayout()
{
    std::vector<Widget*>::const_iterator it;
    std::vector<Widget*>::const_iterator end = this->widgets().end();

    //
    // determine the cell size from maximum item width and height
    //

    Gfx::SizeF itemSize(0, 0);

    for(it = widgets().begin(); it != end; ++it)
    {
        Widget* widget = *it;

        if( ! widget->isVisible() )
            continue;

        double width = std::max( widget->size().width() + widget->margin().leftRight(), 
                                 itemSize.width() );
        double height = std::max( widget->size().height() + widget->margin().topBottom(), 
                                  itemSize.height() );
        itemSize.set(width, height);
    }

    //
    // layout widgets
    //

    switch(_growStyle)
    {
        default:
        case Vertical:
        {
            std::size_t cols = _span;
            if(_span == 0 )
              cols = static_cast<std::size_t>( size().width() / itemSize.width() );
            
            GridLayoutVertical(*this, itemSize, cols);
            break;
        }

        case Horizontal:
        {
            std::size_t rows = _span;
            if(_span == 0 )
                rows = static_cast<std::size_t>( size().height() / itemSize.height() );
            
            GridLayoutHorizontal(*this, itemSize, rows);
            break;
        }
    }
}

} // namespace

} // namespace
