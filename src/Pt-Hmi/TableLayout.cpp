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
#include <cmath>

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


void TableLayout::removeItem(Widget& w)
{
    remove(w);
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


void TableLayout::setColumn(std::size_t col, SizeMode mode, Pt::ssize_t size)
{
    std::size_t cols = col + 1;
    if( cols > _columnSizes.size() )
        _columnSizes.resize(cols);
    
    _columnSizes.at(col) = SizeInfo(mode, size);
}


void TableLayout::setRow(std::size_t row, SizeMode mode, Pt::ssize_t size)
{
    std::size_t rows = row + 1;
    if( rows > _rowSizes.size() )
        _rowSizes.resize(rows);
    
    _rowSizes.at(row) = SizeInfo(mode, size);
}


Gfx::Size TableLayout::onMeasure(const SizePolicy& policy)
{
    Pt::ssize_t itemsWidth = policy.width() - padding().leftRight(); 
    Pt::ssize_t itemsHeight = policy.height() - padding().topBottom();

    Gfx::Size contentSize;

    for(std::size_t row = 0; row < _rows.size(); ++row)
    {
        SizeInfo& rowPolicy = _rowSizes.at(row);
        std::size_t columns = _rows.at(row).size();

        Pt::ssize_t rowWidth = 0;
        Pt::ssize_t rowHeight = 0;
        
        for(std::size_t col = 0; col != columns; ++col)
        {
            SizeInfo& columnPolicy = _columnSizes.at(col);

            Widget* item = _rows.at(row).at(col);
            if( ! item )
                continue;

            if( ! item->isVisible() )
                continue;
            
            SizePolicy itemPolicy(SizePolicy::Preferred, SizePolicy::Preferred);
            itemPolicy.setWidth( itemsWidth - item->margin().leftRight() );
            itemPolicy.setHeight( itemsHeight - item->margin().topBottom() );
            
            if(columnPolicy.mode() == TableLayout::Fixed)
            {
                itemPolicy.setHorizontal(SizePolicy::Fixed);
                itemPolicy.setWidth( columnPolicy.size() );
            }

            if(rowPolicy.mode() == TableLayout::Fixed)
            {
                itemPolicy.setVertical(SizePolicy::Fixed);
                itemPolicy.setHeight( rowPolicy.size() );
            }

            item->measure(itemPolicy);
            Gfx::Size prefSize = item->preferredSize();

            Pt::ssize_t itemWidth = prefSize.width() + item->margin().leftRight();
            Pt::ssize_t itemHeight = prefSize.height() + item->margin().topBottom();
        
            rowWidth += itemWidth;
            rowHeight = std::max(rowHeight, itemHeight);
        }

        contentSize.addWidth( std::max(contentSize.width(), rowWidth) );
        contentSize.addHeight(rowHeight);
    }

    contentSize.addWidth( padding().leftRight() );
    contentSize.addHeight( padding().topBottom() );
    return contentSize;
}


void TableLayout::onLayout(const Gfx::Rect& rect)
{
    //
    // calculate row sizes
    //

    Pt::ssize_t rowAvail = rect.height() - padding().topBottom();
    std::size_t rowFills = 0;

    for(std::size_t row = 0; row < _rows.size(); ++row)
    {
        SizeInfo& rowPolicy = _rowSizes.at(row);
        std::size_t columns = _rows.at(row).size();
        
        for(std::size_t col = 0; col != columns; ++col)
        {
            Widget* item = _rows.at(row).at(col);
            if( ! item )
                continue;
            
            if(rowPolicy.mode() == TableLayout::Preferred)
            {
                Pt::ssize_t height = item->preferredSize().height() +
                                item->margin().topBottom();

                height = std::max(rowPolicy.size(), height);

                rowPolicy.setSize( std::floor(height + 0.5) );
            }
        }

        if(rowPolicy.mode() == TableLayout::Fill)
            ++rowFills;

        rowAvail -= rowPolicy.size();
        rowAvail = rowAvail;
    }

    //
    // calculate column sizes
    //

    Pt::ssize_t columnAvail = rect.width() - padding().leftRight();
    std::size_t columnFills = 0;
    std::size_t columns = _rows.empty() ? 0 : _rows.front().size();

    for(std::size_t col = 0; col != columns; ++col)
    {
        SizeInfo& columnPolicy = _columnSizes.at(col);
        
        for(std::size_t row = 0; row < _rows.size(); ++row)
        {
            Widget* item = _rows.at(row).at(col);
            if( ! item )
                continue;

            if(columnPolicy.mode() == TableLayout::Preferred)
            {
                Pt::ssize_t width = item->preferredSize().width() + 
                               item->margin().leftRight();
                width = std::max(columnPolicy.size(), width);
                
                columnPolicy.setSize(width);
            }
        }

        if(columnPolicy.mode() == TableLayout::Fill)
            ++columnFills;

        columnAvail -= columnPolicy.size();
        columnAvail = columnAvail;
    }
    
    //
    // cell layouting
    //

    Pt::ssize_t rowFillSize = rowFills > 0 ? rowAvail / rowFills : 0;
    Pt::ssize_t columnFillSize = columnFills > 0 ? columnAvail / columnFills : 0;
    Pt::ssize_t y = padding().top();

    for(std::size_t row = 0; row < _rows.size(); ++row)
    {
        SizeInfo& rowPolicy = _rowSizes.at(row);
        Pt::ssize_t rowSize = rowPolicy.mode() == TableLayout::Fill ? rowFillSize
                                                               : rowPolicy.size();

        Pt::ssize_t x = padding().left();
        std::size_t columns = _rows.at(row).size();
        
        for(std::size_t col = 0; col < columns; ++col)
        {
            SizeInfo& columnPolicy = _columnSizes.at(col);
            Pt::ssize_t columnSize = columnPolicy.mode() == TableLayout::Fill ? columnFillSize
                                                                         : columnPolicy.size();

            Widget* item = _rows.at(row).at(col);

            if( item && item->isVisible() )
            {
                Gfx::Point pos( x + item->margin().left(), 
                                 y + item->margin().top() );

                Gfx::Size size( columnSize - item->margin().leftRight(), 
                                 rowSize - item->margin().topBottom() );

                item->layout(pos, size);
            }
            
            x += columnSize;
        }

        y += rowSize;
    }
}

} // namespace

} // namespace
