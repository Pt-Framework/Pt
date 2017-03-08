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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/Hmi/TableLayout.h>
#include <algorithm>

namespace Pt {

namespace Hmi {

TableLayout::TableLayout()
{
}


TableLayout::~TableLayout()
{
}


void TableLayout::addItem(Widget& w, std::size_t row, std::size_t col)
{
    if(w.parent() == this)
        return;

    std::size_t rows = row + 1;
    std::size_t cols = col + 1;

    if( rows > _rowSizes.size() )
        _rowSizes.resize(rows);

    if( cols > _columnSizes.size() )
        _columnSizes.resize(cols);
    
    std::size_t currentCols = _rows.empty() ? 0 : _rows.back().size();
    
    if(cols < currentCols)
        cols = currentCols;

    if( rows > _rows.size() )
        _rows.resize(rows);

    std::vector<Row>::iterator it;
    for(it = _rows.begin(); it != _rows.end(); ++it)
    {
        if( cols > it->size() )
            it->resize(cols);
    }

    add(w);
    _rows.at(row).at(col) = &w;
}


void TableLayout::onRemoveWidget(Widget& w)
{
    std::vector<Row>::iterator rit;
    for(rit = _rows.begin(); rit != _rows.end(); ++rit)
    {
        Row::iterator it;
        for(it = rit->begin(); it != rit->end(); ++it)
        {
            Widget* item = (*it);
            if(item == &w)
            {
                *it = 0;
                remove(w);
                break;
            }
        }
    }
}


void TableLayout::setColumn(std::size_t col, SizeMode mode, double size)
{
    std::size_t cols = col + 1;
    if( cols > _columnSizes.size() )
        _columnSizes.resize(cols);
    
    _columnSizes.at(col) = SizePolicy(mode, size);
}


void TableLayout::setRow(std::size_t row, SizeMode mode, double size)
{
    std::size_t rows = row + 1;
    if( rows > _rowSizes.size() )
        _rowSizes.resize(rows);
    
    _rowSizes.at(row) = SizePolicy(mode, size);
}


void TableLayout::onLayout()
{
    //
    // calculate row sizes
    //

    double rowAvail = size().height() - padding().topBottom();
    std::size_t rowFills = 0;

    for(std::size_t row = 0; row < _rows.size(); ++row)
    {
        SizePolicy& rowPolicy = _rowSizes.at(row);
        std::size_t columns = _rows.at(row).size();
        
        for(std::size_t col = 0; col != columns; ++col)
        {
            Widget* item = _rows.at(row).at(col);
            if( ! item )
                continue;
            
            if(rowPolicy.mode() == Preferred)
            {
                double height = item->preferredSize().height() +
                                item->margin().topBottom();
                height = std::max(rowPolicy.size(), height);
                
                rowPolicy.setSize(height);
            }
        }

        if(rowPolicy.mode() == Fill)
            ++rowFills;

        rowAvail -= rowPolicy.size();
        rowAvail = rowAvail;
    }

    //
    // calculate column sizes
    //

    double columnAvail = size().width() - padding().leftRight();
    std::size_t columnFills = 0;
    std::size_t columns = _rows.empty() ? 0 : _rows.front().size();

    for(std::size_t col = 0; col != columns; ++col)
    {
        SizePolicy& columnPolicy = _columnSizes.at(col);
        
        for(std::size_t row = 0; row < _rows.size(); ++row)
        {
            Widget* item = _rows.at(row).at(col);
            if( ! item )
                continue;

            if(columnPolicy.mode() == Preferred)
            {
                double width = item->preferredSize().width() + 
                               item->margin().leftRight();
                width = std::max(columnPolicy.size(), width);
                
                columnPolicy.setSize(width);
            }
        }

        if(columnPolicy.mode() == Fill)
            ++columnFills;

        columnAvail -= columnPolicy.size();
        columnAvail = columnAvail;
    }
    
    //
    // cell layouting
    //

    double rowFillSize = rowFills > 0 ? rowAvail / rowFills : 0;
    double columnFillSize = columnFills > 0 ? columnAvail / columnFills : 0;
    double y = padding().top();

    for(std::size_t row = 0; row < _rows.size(); ++row)
    {
        SizePolicy& rowPolicy = _rowSizes.at(row);
        double rowSize = rowPolicy.mode() == Fill ? rowFillSize
                                                  : rowPolicy.size();

        double x = padding().left();
        std::size_t columns = _rows.at(row).size();
        
        for(std::size_t col = 0; col < columns; ++col)
        {
            SizePolicy& columnPolicy = _columnSizes.at(col);
            double columnSize = columnPolicy.mode() == Fill ? columnFillSize
                                                            : columnPolicy.size();

            Widget* item = _rows.at(row).at(col);

            if( item && item->isVisible() )
            {
                Gfx::PointF pos( x + item->margin().left(), 
                                 y + item->margin().top() );
                item->move(pos);

                double width = columnSize;
                double height = rowSize;

                Gfx::SizeF size( width - item->margin().leftRight(), 
                                 height - item->margin().topBottom() );
                item->resize(size);
            }
            
            x += columnSize;
        }

        y += rowSize;
    }
}

} // namespace

} // namespace
