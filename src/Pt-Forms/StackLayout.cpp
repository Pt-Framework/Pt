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

#include <Pt/Forms/StackLayout.h>

namespace Pt {

namespace Forms {

StackLayout::StackLayout()
: _current(NoIndex)
{
}


StackLayout::~StackLayout()
{
}


void StackLayout::addItem(Control& control)
{
    _controls.push_back(&control);

    control.show(false);
    add(control);
}


void StackLayout::removeItem(Control& control)
{
    remove(control);
}


void StackLayout::onRemoveControl(Control& control)
{
    Control::onRemoveControl(control);

    std::vector<Control*>::iterator it = std::find(_controls.begin(), 
                                                  _controls.end(), &control);
    if( it == _controls.end() )
        return;

    std::size_t n = std::distance(_controls.begin(), it);
    if(_current == n)
    {
        _current = NoIndex;
    }

    _controls.erase(it);
    _controlRemoved.send(n);
}


bool StackLayout::empty() const
{
    return _controls.empty();
}


std::size_t StackLayout::size() const
{
    return _controls.size();
}


Control* StackLayout::controlAt(std::size_t n) const
{
    if( n >= _controls.size() )
        return 0;

    return _controls.at(n);
}


std::size_t StackLayout::indexOf(Control& control) const
{
    std::vector<Control*>::const_iterator it;
    it = std::find(_controls.begin(), _controls.end(), &control);

    return it != _controls.end() ? std::distance(_controls.begin(), it)
                                : NoIndex;
}


std::size_t StackLayout::current() const
{
    return _current;
}


void StackLayout::setCurrent(std::size_t n)
{
    if( n >= _controls.size() )
        return;

    if(_current != NoIndex)
        _controls.at(_current)->show(false);

    _current = n;
    _controls.at(_current)->show(true);

    _currentChanged.send(n);
}


Gfx::SizeF StackLayout::onMeasure(const SizePolicy& policy)
{
    Base::onMeasure(policy);

    Gfx::SizeF s;

    std::vector<Control*>::iterator it;
    for(it = _controls.begin(); it != _controls.end(); ++it)
    {
        Control* item = *it;
        item->measure(policy);
        Gfx::SizeF preferredSize = item->preferredSize();

        double width = std::max( preferredSize.width(), s.width() );
        double height = std::max( preferredSize.height(), s.height() );
        
        s.set(width, height);
    }

    return s;
}


void StackLayout::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    Control* control = controlAt(_current);

    if(control)
    {
        Gfx::PointF pos(padding().left() + control->margin().left(), 
                        padding().top()  + control->margin().top());
        
        double hspace = padding().leftRight() + control->margin().leftRight();
        double vspace = padding().topBottom() + control->margin().topBottom();

        Gfx::SizeF size;
        size.setWidth( rect.width() - hspace );
        size.setHeight( rect.height() - vspace );

        control->move(pos);
        control->resize(size);
    }
}

} // namespace

} // namespace
