/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2017-2017 Aloysius Indrayanto

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

#include <Pt/Gfx/Pen.h>

namespace Pt {
namespace Gfx {


Pen::Pen()
: _penData(new PenData(Color(0,0,0), 0, Solid, 0, RoundCap, RoundJoin))
{ }


Pen::Pen(const Color& color)
: _penData(new PenData(color, 1, Solid, 0, RoundCap, RoundJoin))
{ }


Pen::Pen(const Color& color, std::size_t size, Style style, CapStyle cap, JoinStyle join)
: _penData(new PenData(color, size, (style != UserDefined) ? style : Solid, 0, cap, join))
{ }


Pen::Pen(const Color& color, std::size_t size, Pt::uint64_t stylePattern, CapStyle cap, JoinStyle join)
: _penData(new PenData(color, size, UserDefined, stylePattern, cap, join))
{ }


void Pen::setColor(const Color& color)
{
    // COW

    SmartPtr<PenData> penData(new PenData);

    *penData = *_penData;
    penData->setColor(color);

    _penData = penData;
}


const Color& Pen::color() const
{
    return _penData->color();
}


void Pen::setSize(std::size_t size)
{
    // COW

    SmartPtr<PenData> penData(new PenData);

    *penData = *_penData;
    penData->setSize(size);

    _penData = penData;
}


std::size_t Pen::size() const
{
    return _penData->size();
}


void Pen::setStyle(Style style)
{
    // COW

    SmartPtr<PenData> penData(new PenData);

    *penData = *_penData;
    penData->setStyle( (style != UserDefined) ? style : Solid, 0 );

    _penData = penData;
}


void Pen::setStyle(Pt::uint64_t stylePattern)
{
    // COW

    SmartPtr<PenData> penData(new PenData);

    *penData = *_penData;
    penData->setStyle(UserDefined, stylePattern);

    _penData = penData;
}


Pen::Style Pen::style() const
{
    return _penData->style();
}


Pt::uint64_t Pen::styleUserPattern() const
{
    return _penData->styleUserPattern();
}


void Pen::setCapStyle(CapStyle cap)
{
    // COW

    SmartPtr<PenData> penData(new PenData);

    *penData = *_penData;
    penData->setCapStyle(cap);

    _penData = penData;
}


Pen::CapStyle Pen::capStyle() const
{
    return _penData->capStyle();
}


void Pen::setJoinStyle(JoinStyle join)
{
    // COW

    SmartPtr<PenData> penData(new PenData);

    *penData = *_penData;
    penData->setJoinStyle(join);

    _penData = penData;
}


Pen::JoinStyle Pen::joinStyle() const
{
    return _penData->joinStyle();
}


bool Pen::isNull() const
{
    return size() == 0;
}


} // namespace
} // namespace
