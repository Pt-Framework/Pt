/***************************************************************************
 *   Copyright (C) 2007 Tobias Mueller                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "Pt/Gui/BorderLayout.h"
#include "Pt/Gui/Widget.h"
#include "Pt/Math/Point.h"
#include "Pt/Math/Size.h"

#include <list>
#include <cmath>

using namespace std;

namespace Pt {
namespace Gui {



BorderLayout::BorderLayout(Widget& widget, size_t spacing)
: Layout(widget)
, _spacing(spacing)
, _north(0)
, _east(0)
, _south(0)
, _west(0)
, _center(0)
{    
}



void BorderLayout::setLayoutData(Widget& widget, Orientation orientation)
{
    if ((orientation == NORTH  && _north  != 0) ||
        (orientation == EAST   && _east   != 0) ||
        (orientation == SOUTH  && _south  != 0) ||
        (orientation == WEST   && _west   != 0) ||
        (orientation == CENTER && _center != 0))
    {
        throw std::invalid_argument("There already is a widget with this orientation in its layout data.");
    }

    switch (orientation)
    {
        case NORTH:  _north  = &widget; break;
        case EAST:   _east   = &widget; break;
        case SOUTH:  _south  = &widget; break;
        case WEST:   _west   = &widget; break;
        case CENTER: _center = &widget; break;
    }

    connect(widget.destroyed, *this, &BorderLayout::remove);
}


void BorderLayout::remove(Widget& widget)
{
    if (_north == &widget) {
        _north = 0;
    } else if (_east == &widget) {
        _east = 0;
    } else if (_south == &widget) {
        _south = 0;
    } else if (_west == &widget) {
        _west = 0;
    } else if (_center == &widget) {
        _center = 0;
    }
}


void BorderLayout::update()
{
    // const std::list<Widget*>& children = this->widget().childWidgets();

    ssize_t top    = this->widget().insets().top();
    ssize_t left   = this->widget().insets().left();
    ssize_t bottom = this->widget().size().height() - this->widget().insets().bottom();
    ssize_t right  = this->widget().size().width() - this->widget().insets().right();

    if (_north != 0) {
        _north->move(left, top);
        _north->resize(right - left, _north->preferredSize().height());
        top += _north->size().height() + _spacing;
    }

    if (_south != 0) {
        _south->move(left, bottom - _south->preferredSize().height());
        _south->resize(size_t(std::max(ssize_t(0), right - left)), _south->preferredSize().height());
        bottom -= _south->size().height() + _spacing;
    }

    if (_east != 0) {
        const Pt::Math::Size& preferredSize = _east->preferredSize();
        _east->move(right -preferredSize .width(), top);
        _east->resize(preferredSize.width(), size_t(std::max(ssize_t(0), bottom - top)));
        right -= _east->size().width() + _spacing;
    }

    if (_west != 0) {
        _west->move(left, top);
        _west->resize(_west->preferredSize().width(), size_t(std::max(ssize_t(0), bottom - top)));
        left += _west->size().width() + _spacing;
    }

    if (_center != 0) {
        _center->move(left, top);
        _center->resize(size_t(std::max(ssize_t(0), right - left)), size_t(std::max(ssize_t(0), bottom - top)));
    }
}


Math::Size BorderLayout::minimumSize()
{
    size_t maxWidth        = 0;
    size_t maxHeight       = 0;
    size_t middleMaxWidth  = 0;
    size_t middleMaxHeight = 0;

    if (_north != 0) {
        maxWidth  = _north->minimumSize().width();
        maxHeight = _north->minimumSize().height() + _spacing;
    }

    if (_south != 0) {
        maxWidth   = std::max(maxWidth, _south->minimumSize().width());
        maxHeight += _south->minimumSize().height() + _spacing;
    }

    if (_west != 0) {
        middleMaxWidth += _west->minimumSize().width() + _spacing;
        middleMaxHeight = _west->minimumSize().height();
    }

    if (_east != 0) {
        middleMaxWidth += _east->minimumSize().width() + _spacing;
        middleMaxHeight = size_t(std::max(ssize_t(0), ssize_t(_east->minimumSize().height())));
    }

    if (_center != 0) {
        middleMaxWidth += _center->minimumSize().width();
        middleMaxHeight = size_t(std::max(ssize_t(0), ssize_t(_center->minimumSize().height())));
    }

    maxWidth   = std::max(maxWidth, middleMaxWidth);
    maxHeight += middleMaxHeight;

    return Math::Size(maxWidth  + this->widget().insets().left() + this->widget().insets().right(),
                      maxHeight + this->widget().insets().top()  + this->widget().insets().bottom());
}


Math::Size BorderLayout::preferredSize()
{
    size_t maxWidth        = 0;
    size_t maxHeight       = 0;
    size_t middleMaxWidth  = 0;
    size_t middleMaxHeight = 0;

    if (_north != 0) {
        maxWidth  = _north->preferredSize().width();
        maxHeight = _north->preferredSize().height() + _spacing;
    }

    if (_south != 0) {
        maxWidth   = std::max(maxWidth, _south->preferredSize().width());
        maxHeight += _south->preferredSize().height() + _spacing;
    }

    if (_west != 0) {
        middleMaxWidth += _west->preferredSize().width() + _spacing;
        middleMaxHeight = _west->preferredSize().height();
    }

    if (_east != 0) {
        middleMaxWidth += _east->preferredSize().width() + _spacing;
        middleMaxHeight = size_t(std::max(ssize_t(middleMaxHeight), ssize_t(_east->preferredSize().height())));
    }

    if (_center != 0) {
        middleMaxWidth += _center->preferredSize().width();
        middleMaxHeight = size_t(std::max(ssize_t(middleMaxHeight), ssize_t(_center->preferredSize().height())));
    }

    maxWidth   = std::max(maxWidth, middleMaxWidth);
    maxHeight += middleMaxHeight;

    return Math::Size(maxWidth  + this->widget().insets().left() + this->widget().insets().right(),
                      maxHeight + this->widget().insets().top()  + this->widget().insets().bottom());
}


BorderLayout& BorderLayout::create(Widget& widget, size_t spacing)
{
    BorderLayout* layout = new BorderLayout(widget, spacing);
    return *layout;
}


} // namespace Gui
} // namespace Pt
