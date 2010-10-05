/*
 * Copyright (C) 2006 Marc Boris D�rner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Pt/Gui/SimpleGridLayout.h"
#include "Pt/Gui/Widget.h"
#include "Pt/Gfx/Point.h"
#include "Pt/Gfx/Size.h"

#include <list>

using namespace std;


namespace Pt {

namespace Gui {


SimpleGridLayoutData::SimpleGridLayoutData(size_t x, size_t y, const Margin& margin)
: LayoutData(margin)
, _x(x)
, _y(y)
{
}


//SimpleGridLayoutData* SimpleGridLayoutData::clone() const
//{
//    return new SimpleGridLayoutData(*this);
//}


void SimpleGridLayoutData::setX(size_t x)
{
    _x = x;
}


void SimpleGridLayoutData::setY(size_t y)
{
    _y = y;
}


size_t SimpleGridLayoutData::x() const
{
    return _x;
}


size_t SimpleGridLayoutData::y() const
{
    return _y;
}




SimpleGridLayout::SimpleGridLayout(
    Widget& widget,
    size_t  columnCount,
    size_t  rowCount,
    ssize_t horizontalGap,
    ssize_t verticalGap
)
: Layout(widget)
, _columnCount(columnCount)
, _rowCount(rowCount)
, _horizontalGap(horizontalGap)
, _verticalGap(verticalGap)
{
}



void SimpleGridLayout::setLayoutData(Widget& widget, const SimpleGridLayoutData& layoutData)
{
    _widget2LayoutData.insert(std::make_pair(&widget, layoutData));
    connect(widget.destroyed, *this, &SimpleGridLayout::remove);
}


void SimpleGridLayout::remove(Widget& widget)
{
    _widget2LayoutData.erase(const_cast<Widget*>(&widget));
}


void SimpleGridLayout::update()
{
    // TODO This is still a little bit buggy as all cells are the same size and depending on the
    // size of the parent container the width of all cells + the width of all gaps will not sum up
    // to the width of the container. A border is visible.

    const std::list<Widget*>& children = this->widget().childWidgets();

    size_t cellWidth  = (this->widget().size().width()  - (_horizontalGap * (_columnCount - 1))) / _columnCount;
    size_t cellHeight = (this->widget().size().height() - (_verticalGap   * (_rowCount    - 1))) / _rowCount;


    list<Widget*>::const_iterator childrenIter;
    for (childrenIter = children.begin(); childrenIter != children.end(); childrenIter++) {

        Widget* w = *childrenIter;
        map<Widget*, SimpleGridLayoutData>::const_iterator findIter = _widget2LayoutData.find(w);
        if (findIter == _widget2LayoutData.end()) {
            // We have no Layout Data for this widget. Just don't layout it.
            continue;
        }

        const SimpleGridLayoutData& data = findIter->second;

        size_t x = data.x() * cellWidth  + data.x() * _horizontalGap + data.margin().left();
        size_t y = data.y() * cellHeight + data.y() * _verticalGap   + data.margin().top();

        size_t leftRightMargin = data.margin().left() + data.margin().right();
        size_t topBottomMargin = data.margin().top()  + data.margin().bottom();

        w->move(x, y);
        w->resize(cellWidth - leftRightMargin, cellHeight - topBottomMargin);
    }
}


Gfx::Size SimpleGridLayout::minimumSize()
{
    const std::list<Widget*>& children = this->widget().childWidgets();

    ssize_t maxWidth = 0;
    ssize_t maxHeight = 0;

    // Find widest and heighest widget.
    list<Widget*>::const_iterator childrenIter = children.begin();

    while (childrenIter != children.end()) {
        Widget* w = *childrenIter;

        maxWidth  = max<size_t>(maxWidth,  w->minimumSize().width());
        maxHeight = max<size_t>(maxHeight, w->minimumSize().height());

        childrenIter++;
    }

    return Gfx::Size(_columnCount * maxWidth  + (_columnCount - 1) * _horizontalGap,
                     _rowCount    * maxHeight + (_rowCount    - 1) * _verticalGap);
}


Gfx::Size SimpleGridLayout::preferredSize()
{
    const std::list<Widget*>& children = this->widget().childWidgets();

    ssize_t maxWidth = 0;
    ssize_t maxHeight = 0;

    // Find widest and heighest widget.
    list<Widget*>::const_iterator childrenIter = children.begin();

    while (childrenIter != children.end()) {
        Widget* w = *childrenIter;

        maxWidth  = max<size_t>(maxWidth,  w->preferredSize().width());
        maxHeight = max<size_t>(maxHeight, w->preferredSize().height());

        childrenIter++;
    }

    return Gfx::Size(_columnCount * maxWidth  + (_columnCount - 1) * _horizontalGap,
                     _rowCount    * maxHeight + (_rowCount    - 1) * _verticalGap);
}


SimpleGridLayout& SimpleGridLayout::create(
    Widget& widget,
    size_t  columnCount,
    size_t  rowCount,
    ssize_t horizontalGap,
    ssize_t verticalGap)
{
    SimpleGridLayout* layout = new SimpleGridLayout(widget, columnCount, rowCount, horizontalGap, verticalGap);
    return *layout;
}


} // namespace Gui

} // namespace Pt
