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

#include <Pt/Hmi/StackLayout.h>

namespace Pt {

namespace Hmi {

StackLayout::StackLayout()
: _current(NoIndex)
{
}


StackLayout::~StackLayout()
{
}


void StackLayout::addItem(Widget& w)
{
    _widgets.push_back(&w);

    w.show(false);
    add(w);
}


void StackLayout::removeItem(Widget& w)
{
    remove(w);
}


void StackLayout::onRemoveWidget(Widget& w)
{
    Widget::onRemoveWidget(w);

    std::vector<Widget*>::iterator it = std::find(_widgets.begin(), 
                                                  _widgets.end(), &w);
    if( it == _widgets.end() )
        return;

    std::size_t n = std::distance(_widgets.begin(), it);
    if(_current == n)
    {
        _current = NoIndex;
    }

    _widgets.erase(it);
    _widgetRemoved.send(n);
}


bool StackLayout::empty() const
{
    return _widgets.empty();
}


std::size_t StackLayout::size() const
{
    return _widgets.size();
}


Widget* StackLayout::widgetAt(std::size_t n) const
{
    if( n >= _widgets.size() )
        return 0;

    return _widgets.at(n);
}


std::size_t StackLayout::indexOf(Widget& w) const
{
    std::vector<Widget*>::const_iterator it;
    it = std::find(_widgets.begin(), _widgets.end(), &w);

    return it != _widgets.end() ? std::distance(_widgets.begin(), it)
                                : NoIndex;
}


std::size_t StackLayout::current() const
{
    return _current;
}


void StackLayout::setCurrent(std::size_t n)
{
    if( n >= _widgets.size() )
        return;

    if(_current != NoIndex)
        _widgets.at(_current)->show(false);

    _current = n;
    _widgets.at(_current)->show(true);

    _currentChanged.send(n);
}


Gfx::SizeF StackLayout::onMeasure(Layouter& layouter, const SizePolicy& policy)
{
    Base::onMeasure(layouter, policy);

    Gfx::SizeF s;

    std::vector<Widget*>::iterator it;
    for(it = _widgets.begin(); it != _widgets.end(); ++it)
    {
        Widget* item = *it;
        layouter.measure(*item, policy);
        Gfx::SizeF preferredSize = item->preferredSize();

        double width = std::max( preferredSize.width(), s.width() );
        double height = std::max( preferredSize.height(), s.height() );
        
        s.set(width, height);
    }

    return s;
}


void StackLayout::onLayout(Layouter& layouter, const Gfx::RectF& rect)
{
    Base::onLayout(layouter, rect);

    Widget* widget = widgetAt(_current);

    if(widget)
    {
        Gfx::PointF pos(padding().left() + widget->margin().left(), 
                        padding().top()  + widget->margin().top());
        
        double hspace = padding().leftRight() + widget->margin().leftRight();
        double vspace = padding().topBottom() + widget->margin().topBottom();

        Gfx::SizeF size;
        size.setWidth( rect.width() - hspace );
        size.setHeight( rect.height() - vspace );

        layouter.layout( *widget, pos, size );
    }
}

} // namespace

} // namespace
