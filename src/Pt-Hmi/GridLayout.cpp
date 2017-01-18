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

GridLayout::GridLayout(GrowStyle g)
: _growStyle(g)
, _rows(0)
, _columns(0)
{
}


GridLayout::~GridLayout()
{
}


void GridLayout::setGrowStyle(GrowStyle g)
{
    _growStyle = g;
}


std::size_t GridLayout::rows() const
{
    return _rows;
}


void GridLayout::setRows(std::size_t n)
{
    _rows = n;
}


std::size_t GridLayout::columns() const
{
    return _columns;
}


void GridLayout::setColumns(std::size_t n)
{
    _columns = n;
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

        double width = std::max( widget->size().width(), itemSize.width() );
        double height = std::max( widget->size().height(), itemSize.height() );
        itemSize.set(width, height);
    }

    //
    // layout widgets
    //

    double itemX = 0;
    double itemY = 0;

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
        
        if( itemX > size().width() )
        {
            itemX = 0;
            itemY += itemSize.height();
        }
    }
}

} // namespace

} // namespace
