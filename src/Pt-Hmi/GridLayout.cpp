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
#include <cmath>

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


void GridLayout::addItem(Widget& w)
{
    add(w);
}


void GridLayout::removeItem(Widget& w)
{
    remove(w);
}


Gfx::SizeF GridLayout::onMeasure(Layouter& layouter, const SizePolicy& policy)
{
    Gfx::SizeF contentSize;

    switch(_orientation)
    {
        default:
        case Vertical:
            contentSize = onMeasureVertical(layouter, policy);
            break;

        case Horizontal:
            contentSize = onMeasureHorizontal(layouter, policy);
            break;
    }

    return contentSize;
}


Gfx::SizeF GridLayout::onMeasureVertical(Layouter& layouter, const SizePolicy& policy)
{
    double itemsWidth = policy.size().width() - padding().leftRight();
    double itemsHeight = policy.size().height() - padding().topBottom();

    Gfx::SizeF itemSize;

    std::vector<Widget*>::const_iterator it;
    std::vector<Widget*>::const_iterator end = widgets().end();

    for(it = widgets().begin(); it != end; ++it)
    {
        Widget* item = *it;

        if( ! item->isVisible() )
            continue;

        SizePolicy itemPolicy(SizePolicy::Preferred, SizePolicy::Preferred);
        itemPolicy.setWidth( itemsWidth - item->margin().leftRight() );
        itemPolicy.setHeight( itemsHeight - item->margin().topBottom() );

        Gfx::SizeF prefSize = layouter.measure(*item, itemPolicy);
        
        double itemWidth = prefSize.width() + item->margin().leftRight();
        double itemHeight = prefSize.height() + item->margin().topBottom();

        double width = std::max<double>( itemWidth, itemSize.width() );
        double height = std::max<double>( itemHeight, itemSize.height() );
        itemSize.set(width, height);
    }

    std::size_t cols = _span;

    if(_span == 0 && itemSize.width() > 0)
    {
        std::size_t maxCols = static_cast<std::size_t>( itemsWidth / itemSize.width() );

        if(policy.horizontal() == SizePolicy::Fixed)
        {
            cols = maxCols;
        }
        else if(policy.horizontal() == SizePolicy::Maximum)
        {
            cols = std::min(cols, maxCols);
        }
        else
        {
            cols = static_cast<std::size_t>(
                      std::sqrt( static_cast<double>(widgets().size()) ) + 0.5 );
        }
    }

    if(cols == 0)
        return Gfx::SizeF(0, 0);

    std::size_t rows = widgets().size() / cols;
    if(widgets().size() % cols > 0)
        ++rows;

    if( (policy.horizontal() == SizePolicy::Preferred ||
         policy.horizontal() == SizePolicy::Any)  &&
        (policy.vertical() == SizePolicy::Fixed ||
         policy.vertical() == SizePolicy::Maximum) )
    {
        std::size_t maxRows = static_cast<std::size_t>( itemsHeight / itemSize.height() );
        if(rows > maxRows)
        {
            rows = maxRows;
            cols = widgets().size() / rows;
            if(widgets().size() % rows > 0)
                ++cols;
        }
    }

    return Gfx::SizeF(cols * itemSize.width(),
                      rows * itemSize.height());
}


Gfx::SizeF GridLayout::onMeasureHorizontal(Layouter& layouter, const SizePolicy& policy)
{
    double itemsWidth = policy.size().width() - padding().leftRight();
    double itemsHeight = policy.size().height() - padding().topBottom();

    Gfx::SizeF itemSize;

    std::vector<Widget*>::const_iterator it;
    std::vector<Widget*>::const_iterator end = widgets().end();

    for(it = widgets().begin(); it != end; ++it)
    {
        Widget* item = *it;

        if( ! item->isVisible() )
            continue;

        SizePolicy itemPolicy(SizePolicy::Preferred, SizePolicy::Preferred);
        itemPolicy.setWidth( itemsWidth - item->margin().leftRight() );
        itemPolicy.setHeight( itemsHeight - item->margin().topBottom() );

        Gfx::SizeF prefSize = layouter.measure(*item, itemPolicy);

        double itemWidth = prefSize.width() + item->margin().leftRight();
        double itemHeight = prefSize.height() + item->margin().topBottom();

        double width = std::max<double>( itemWidth, itemSize.width() );
        double height = std::max<double>( itemHeight, itemSize.height() );
        itemSize.set(width, height);
    }

    std::size_t rows = _span;

    if(_span == 0 && itemSize.height() > 0)
    {
        std::size_t maxRows = static_cast<std::size_t>( itemsHeight / itemSize.height() );

        if(policy.vertical() == SizePolicy::Fixed)
        {
            rows = maxRows;
        }
        else if(policy.vertical() == SizePolicy::Maximum)
        {
            rows = std::min(rows, maxRows);
        }
        else
        {
            rows = static_cast<std::size_t>(
                      std::sqrt( static_cast<double>(widgets().size()) ) + 0.5 );
        }
    }

    if(rows == 0)
        return Gfx::SizeF(0, 0);

    std::size_t cols = widgets().size() / rows;
    if(widgets().size() % rows > 0)
        ++cols;

    if( (policy.vertical() == SizePolicy::Preferred ||
         policy.vertical() == SizePolicy::Any)  &&
        (policy.horizontal() == SizePolicy::Fixed ||
         policy.horizontal() == SizePolicy::Maximum) )
    {
        std::size_t maxCols = static_cast<std::size_t>( itemsWidth / itemSize.width() );
        if(cols > maxCols)
        {
            cols = maxCols;
            rows = widgets().size() / cols;
            if(widgets().size() % cols > 0)
                ++rows;
        }
    }

    return Gfx::SizeF(cols * itemSize.width(),
                      rows * itemSize.height());
}


void GridLayout::onLayout(Layouter& layouter, const Gfx::RectF& rect)
{
    Layout::onLayout(layouter, rect);

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

        double width = std::max<double>( itemWidth, itemSize.width() );
        double height = std::max<double>( itemHeight, itemSize.height() );
        itemSize.set(width, height);
    }

    //
    // layout widgets
    //

    switch(_orientation)
    {
        default:
        case Vertical:
            onLayoutVertical(layouter, itemSize, rect);
            break;

        case Horizontal:
            onLayoutHorizontal(layouter, itemSize, rect);
            break;
    }
}


void GridLayout::onLayoutVertical(Layouter& layouter, const Gfx::SizeF& itemSize, const Gfx::RectF& rect)
{
    std::size_t cols = _span;

    if(_span == 0 && itemSize.width() > 0)
    {
        double itemsWidth = rect.width() - padding().leftRight();
        cols = static_cast<std::size_t>( itemsWidth / itemSize.width() );
        cols = std::min(cols, widgets().size());
    }

    if(cols == 0)
          cols = 1;

    std::vector<Widget*>::const_iterator it;
    std::vector<Widget*>::const_iterator end = widgets().end();

    double width = rect.width() - padding().leftRight();
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
        double x = ( itemSize.width() - widget->preferredSize().width() ) / 2;
        double y = ( itemSize.height() - widget->preferredSize().height() ) / 2;

        Gfx::PointF pos(itemX + x, itemY + y);
        layouter.layout( *widget, pos, widget->preferredSize() );

        itemX += itemSize.width();

        if(++col == cols)
        {
            col = 0;
            itemX = startX;
            itemY += itemSize.height();
        }
    }
}


void GridLayout::onLayoutHorizontal(Layouter& layouter, const Gfx::SizeF& itemSize, const Gfx::RectF& rect)
{
    std::size_t rows = _span;

    if(_span == 0 && itemSize.height() > 0)
    {
        double itemsHeight = rect.height() - padding().topBottom();
        rows = static_cast<std::size_t>( itemsHeight / itemSize.height() );
        rows = std::min(rows, widgets().size());
    }

    if(rows == 0)
          rows = 1;

    std::vector<Widget*>::const_iterator it;
    std::vector<Widget*>::const_iterator end = widgets().end();

    double height = rect.height() - padding().topBottom();
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
        double x = ( itemSize.width() - widget->preferredSize().width() ) / 2;
        double y = ( itemSize.height() - widget->preferredSize().height() ) / 2;

        Gfx::PointF pos(itemX + x, itemY + y);
        layouter.layout( *widget, pos, widget->preferredSize() );

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
