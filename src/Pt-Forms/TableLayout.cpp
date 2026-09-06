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

#include <Pt/Forms/TableLayout.h>
#include <Pt/Forms/PaintContext.h>
#include <algorithm>
#include <cmath>

namespace Pt {

namespace Forms {

TableLayout::TableLayout()
{
}


TableLayout::~TableLayout()
{
}


void TableLayout::addItem(Control& control, std::size_t row, std::size_t col)
{
    if(control.parent() == this)
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

    add(control);
    _rows.at(row).at(col) = &control;
}


void TableLayout::removeItem(Control& control)
{
    remove(control);
}


void TableLayout::onRemoveControl(Control& control)
{
    std::vector<Row>::iterator rit;
    for(rit = _rows.begin(); rit != _rows.end(); ++rit)
    {
        Row::iterator it;
        for(it = rit->begin(); it != rit->end(); ++it)
        {
            Control* item = (*it);
            if(item == &control)
            {
                *it = 0;
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

    _columnSizes.at(col) = SizeInfo(mode, size);
}


void TableLayout::setRow(std::size_t row, SizeMode mode, double size)
{
    std::size_t rows = row + 1;
    if( rows > _rowSizes.size() )
        _rowSizes.resize(rows);

    _rowSizes.at(row) = SizeInfo(mode, size);
}


Gfx::SizeF TableLayout::onMeasure(const SizePolicy& policy)
{
    double itemsWidth = policy.width() - padding().leftRight();
    double itemsHeight = policy.height() - padding().topBottom();

    Gfx::SizeF contentSize;

    for(std::size_t row = 0; row < _rows.size(); ++row)
    {
        SizeInfo& rowPolicy = _rowSizes.at(row);
        std::size_t columns = _rows.at(row).size();

        double rowWidth = 0;
        double rowHeight = 0;

        for(std::size_t col = 0; col != columns; ++col)
        {
            SizeInfo& columnPolicy = _columnSizes.at(col);

            Control* item = _rows.at(row).at(col);
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
            Gfx::SizeF prefSize = item->preferredSize();

            double itemWidth = prefSize.width() + item->margin().leftRight();
            double itemHeight = prefSize.height() + item->margin().topBottom();

            rowWidth += itemWidth;
            rowHeight = std::max<double>(rowHeight, itemHeight);
        }

        contentSize.addWidth( std::max<double>(contentSize.width(), rowWidth) );
        contentSize.addHeight(rowHeight);
    }

    contentSize.addWidth( padding().leftRight() );
    contentSize.addHeight( padding().topBottom() );
    return contentSize;
}


void TableLayout::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    //
    // calculate row sizes
    //

    double rowAvail = rect.height() - padding().topBottom();
    std::size_t rowFills = 0;

    for(std::size_t row = 0; row < _rows.size(); ++row)
    {
        SizeInfo& rowPolicy = _rowSizes.at(row);
        std::size_t columns = _rows.at(row).size();

        if(rowPolicy.mode() == TableLayout::Preferred)
        {
            rowPolicy.setSize(0);

            for(std::size_t col = 0; col != columns; ++col)
            {
                Control* item = _rows.at(row).at(col);
                if( ! item )
                    continue;

                double height = item->preferredSize().height() +
                                item->margin().topBottom();

                height = std::max<double>(rowPolicy.size(), height);

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

    double columnAvail = rect.width() - padding().leftRight();
    std::size_t columnFills = 0;
    std::size_t columns = _rows.empty() ? 0 : _rows.front().size();

    for(std::size_t col = 0; col != columns; ++col)
    {
        SizeInfo& columnPolicy = _columnSizes.at(col);

        if(columnPolicy.mode() == TableLayout::Preferred)
        {
            columnPolicy.setSize(0);

            for(std::size_t row = 0; row < _rows.size(); ++row)
            {
                Control* item = _rows.at(row).at(col);
                if( ! item )
                    continue;

                const Gfx::Scaling& scaling = item->scaling();
                double preferredWidth = scaling.align( item->preferredSize().width() );
                double width = preferredWidth + item->margin().leftRight();
                width = std::max<double>(columnPolicy.size(), width);

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

    double rowFillSize = rowFills > 0 ? rowAvail / rowFills : 0;
    double columnFillSize = columnFills > 0 ? columnAvail / columnFills : 0;
    double y = padding().top();

    for(std::size_t row = 0; row < _rows.size(); ++row)
    {
        SizeInfo& rowPolicy = _rowSizes.at(row);
        double rowSize = rowPolicy.mode() == TableLayout::Fill ? rowFillSize
                                                               : rowPolicy.size();

        double x = padding().left();
        std::size_t columns = _rows.at(row).size();

        for(std::size_t col = 0; col < columns; ++col)
        {
            SizeInfo& columnPolicy = _columnSizes.at(col);
            double columnSize = columnPolicy.mode() == TableLayout::Fill ? columnFillSize
                                                                         : columnPolicy.size();

            Control* item = _rows.at(row).at(col);

            if( item && item->isVisible() )
            {
                Gfx::PointF pos( x + item->margin().left(),
                                 y + item->margin().top() );

                Gfx::SizeF size( columnSize - item->margin().leftRight(),
                                 rowSize - item->margin().topBottom() );

                item->move(pos);
                item->resize(size);
            }

            x += columnSize;
        }

        y += rowSize;
    }
}


TableLayout2::TableLayout2()
{
}


TableLayout2::~TableLayout2()
{
}


void TableLayout2::addItem(Control& control, std::size_t row, std::size_t col)
{
    if(control.parent() == this)
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

    add(control);
    _rows.at(row).at(col) = &control;
}


void TableLayout2::removeItem(Control& control)
{
    remove(control);
}


void TableLayout2::onRemoveControl(Control& control)
{
    std::vector<Row>::iterator rit;
    for(rit = _rows.begin(); rit != _rows.end(); ++rit)
    {
        Row::iterator it;
        for(it = rit->begin(); it != rit->end(); ++it)
        {
            Control* item = (*it);
            if(item == &control)
            {
                *it = 0;
                break;
            }
        }
    }
}


void TableLayout2::setColumn(std::size_t col, SizeMode mode, double size)
{
    std::size_t cols = col + 1;
    if( cols > _columnSizes.size() )
        _columnSizes.resize(cols);

    _columnSizes.at(col) = SizeInfo(mode, size);
    relayout();
}


void TableLayout2::setRow(std::size_t row, SizeMode mode, double size)
{
    std::size_t rows = row + 1;
    if( rows > _rowSizes.size() )
        _rowSizes.resize(rows);

    _rowSizes.at(row) = SizeInfo(mode, size);
    relayout();
}


Control* TableLayout2::cell(std::size_t row, std::size_t column) const
{
    if(row >= _rows.size())
        return 0;

    if(column >= _rows[row].size())
        return 0;

    return _rows[row][column];
}


TableLayout2::SizeMode TableLayout2::columnMode(std::size_t column) const
{
    if(column >= _columnSizes.size())
        return Preferred;

    return _columnSizes[column].mode();
}


TableLayout2::SizeMode TableLayout2::rowMode(std::size_t row) const
{
    if(row >= _rowSizes.size())
        return Preferred;

    return _rowSizes[row].mode();
}


void TableLayout2::measureItem(Control& item,
                               SizeMode colMode,
                               SizeMode rowMode,
                               double width,
                               double height)
{
    SizePolicy::Mode hMode = SizePolicy::Preferred;
    SizePolicy::Mode vMode = SizePolicy::Preferred;

    if(colMode == Fixed || colMode == Fill)
        hMode = SizePolicy::Fixed;

    if(rowMode == Fixed || rowMode == Fill)
        vMode = SizePolicy::Fixed;

    SizePolicy itemPolicy(hMode, vMode);
    itemPolicy.setWidth( std::max(0.0, width - item.margin().leftRight()) );
    itemPolicy.setHeight( std::max(0.0, height - item.margin().topBottom()) );
    item.measure(itemPolicy);
}


void TableLayout2::computeTracks(std::vector<double>& tracks,
                                 const std::vector<SizeInfo>& infos,
                                 std::size_t count,
                                 double content,
                                 bool horizontal) const
{
    tracks.assign(count, 0.0);

    std::size_t fillCount = 0;
    double used = 0;

    for(std::size_t i = 0; i < count; ++i)
    {
        SizeMode mode = i < infos.size() ? infos[i].mode() : Preferred;

        if(mode == Fill)
        {
            ++fillCount;
            continue;
        }

        if(mode == Fixed)
        {
            tracks[i] = infos[i].size();
        }
        else
        {
            double size = 0;

            if(horizontal)
            {
                for(std::size_t row = 0; row < _rows.size(); ++row)
                {
                    Control* item = cell(row, i);
                    if( ! item || ! item->isVisible() )
                        continue;

                    double w = item->preferredSize().width() +
                               item->margin().leftRight();
                    size = std::max(size, w);
                }
            }
            else
            {
                std::size_t nCols = _rows.empty() ? 0 : _rows.front().size();
                for(std::size_t col = 0; col < nCols; ++col)
                {
                    Control* item = cell(i, col);
                    if( ! item || ! item->isVisible() )
                        continue;

                    double h = item->preferredSize().height() +
                               item->margin().topBottom();
                    size = std::max(size, h);
                }
            }

            tracks[i] = size;
        }

        used += tracks[i];
    }

    const double leftover = std::max(0.0, content - used);
    const double fillSize = fillCount > 0 ? leftover / fillCount : 0.0;

    std::size_t lastFill = count;
    for(std::size_t i = 0; i < count; ++i)
    {
        SizeMode mode = i < infos.size() ? infos[i].mode() : Preferred;
        if(mode == Fill)
        {
            tracks[i] = fillSize;
            lastFill = i;
        }

        tracks[i] = scaling().align(tracks[i]);
    }

    if(fillCount > 0)
    {
        double others = 0;
        for(std::size_t i = 0; i < count; ++i)
        {
            if(i != lastFill)
                others += tracks[i];
        }

        tracks[lastFill] = std::max(0.0, content - others);
    }
}


Gfx::SizeF TableLayout2::onMeasure(const SizePolicy& policy)
{
    const double contentWidth =
        std::max(0.0, policy.width() - padding().leftRight());
    const double contentHeight =
        std::max(0.0, policy.height() - padding().topBottom());

    const std::size_t nRows = _rows.size();
    const std::size_t nCols = nRows == 0 ? 0 : _rows.front().size();

    double fixedWidth = 0;
    for(std::size_t col = 0; col < nCols; ++col)
    {
        if(columnMode(col) == Fixed)
            fixedWidth += _columnSizes[col].size();
    }

    double fixedHeight = 0;
    for(std::size_t row = 0; row < nRows; ++row)
    {
        if(rowMode(row) == Fixed)
            fixedHeight += _rowSizes[row].size();
    }

    const double preferredHintW = std::max(0.0, contentWidth - fixedWidth);
    const double preferredHintH = std::max(0.0, contentHeight - fixedHeight);

    for(std::size_t row = 0; row < nRows; ++row)
    {
        const SizeMode rMode = rowMode(row);
        const SizeMode measureRowMode = rMode == Fill ? Preferred : rMode;
        const double rowHint = rMode == Fixed ? _rowSizes[row].size()
                                              : preferredHintH;

        for(std::size_t col = 0; col < nCols; ++col)
        {
            const SizeMode cMode = columnMode(col);
            if(cMode == Fill)
                continue;

            Control* item = cell(row, col);
            if( ! item || ! item->isVisible() )
                continue;

            const double colHint = cMode == Fixed ? _columnSizes[col].size()
                                                  : preferredHintW;
            measureItem(*item, cMode, measureRowMode, colHint, rowHint);
        }
    }

    std::vector<double> colTracks;
    computeTracks(colTracks, _columnSizes, nCols, contentWidth, true);

    for(std::size_t row = 0; row < nRows; ++row)
    {
        const SizeMode rMode = rowMode(row);
        if(rMode == Fill)
            continue;

        const double rowHint = rMode == Fixed ? _rowSizes[row].size()
                                              : preferredHintH;

        for(std::size_t col = 0; col < nCols; ++col)
        {
            if(columnMode(col) != Fill)
                continue;

            Control* item = cell(row, col);
            if( ! item || ! item->isVisible() )
                continue;

            measureItem(*item, Fill, rMode, colTracks[col], rowHint);
        }
    }

    std::vector<double> rowTracks;
    computeTracks(rowTracks, _rowSizes, nRows, contentHeight, false);

    for(std::size_t row = 0; row < nRows; ++row)
    {
        if(rowMode(row) != Fill)
            continue;

        for(std::size_t col = 0; col < nCols; ++col)
        {
            Control* item = cell(row, col);
            if( ! item || ! item->isVisible() )
                continue;

            measureItem(*item, columnMode(col), Fill,
                        colTracks[col], rowTracks[row]);
        }
    }

    Gfx::SizeF contentSize;
    for(std::size_t col = 0; col < nCols; ++col)
    {
        contentSize.addWidth(colTracks[col]);
    }

    for(std::size_t row = 0; row < nRows; ++row)
    {
        contentSize.addHeight(rowTracks[row]);
    }

    contentSize.addWidth( padding().leftRight() );
    contentSize.addHeight( padding().topBottom() );
    return contentSize;
}


void TableLayout2::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    const double contentWidth =
        std::max(0.0, rect.width() - padding().leftRight());
    const double contentHeight =
        std::max(0.0, rect.height() - padding().topBottom());

    const std::size_t nRows = _rows.size();
    const std::size_t nCols = nRows == 0 ? 0 : _rows.front().size();

    std::vector<double> colTracks;
    std::vector<double> rowTracks;
    computeTracks(colTracks, _columnSizes, nCols, contentWidth, true);
    computeTracks(rowTracks, _rowSizes, nRows, contentHeight, false);

    double y = padding().top();
    for(std::size_t row = 0; row < nRows; ++row)
    {
        double x = padding().left();
        const double rowSize = rowTracks[row];

        for(std::size_t col = 0; col < nCols; ++col)
        {
            const double columnSize = colTracks[col];
            Control* item = cell(row, col);

            if( item && item->isVisible() )
            {
                Gfx::PointF pos( x + item->margin().left(),
                                 y + item->margin().top() );

                Gfx::SizeF size(
                    std::max(0.0, columnSize - item->margin().leftRight()),
                    std::max(0.0, rowSize - item->margin().topBottom()) );

                item->move(pos);
                item->resize(size);
            }

            x += columnSize;
        }

        y += rowSize;
    }
}

} // namespace

} // namespace
