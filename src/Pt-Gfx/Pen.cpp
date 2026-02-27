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
: _penData(new PenData(ColorF(0, 0, 0), 0, Solid, RoundCap, RoundJoin))
{}


Pen::Pen(const ColorF& color)
: _penData(new PenData(color, 1, Solid, RoundCap, RoundJoin))
{}


Pen::Pen(const ColorF& color, std::size_t size, 
         Style style, CapStyle cap, JoinStyle join)
: _penData(new PenData(color, size, style, cap, join))
{}


Pen::Pen(const ColorF& color, std::size_t size, 
         const std::vector<Pt::uint8_t>& dashPattern, 
         CapStyle cap, JoinStyle join)
: _penData(new PenData(color, size, DashPattern, dashPattern, cap, join))
{}


bool Pen::isNull() const
{
    return size() == 0;
}


void Pen::setColor(const ColorF& color)
{
    // COW
    if(_penData.refs() > 1) 
    {
        SmartPtr<PenData> penData( new PenData(*_penData) );
        _penData = penData;
    }

    _penData->setColor(color);
}


const ColorF& Pen::color() const
{
    return _penData->color();
}


void Pen::setSize(std::size_t size)
{
    // COW
    if(_penData.refs() > 1) 
    {
        SmartPtr<PenData> penData( new PenData(*_penData) );
        _penData = penData;
    }

    _penData->setSize(size);
}


std::size_t Pen::size() const
{
    return _penData->size();
}


void Pen::setStyle(Style style)
{
    // COW
    if(_penData.refs() > 1) {
        SmartPtr<PenData> penData( new PenData(*_penData) );
        _penData = penData;
    }

    _penData->setStyle(style);
}


Pen::Style Pen::style() const
{
    return _penData->style();
}


void Pen::setDashPattern(const std::vector<Pt::uint8_t>& dashPattern)
{
    // COW
    if(_penData.refs() > 1) 
    {
        SmartPtr<PenData> penData( new PenData(*_penData) );
        _penData = penData;
    }

    _penData->setDashPattern(dashPattern);
}


const std::vector<Pt::uint8_t>& Pen::dashPattern() const
{
    return _penData->dashPattern();
}


void Pen::setCapStyle(CapStyle cap)
{
    // COW
    if(_penData.refs() > 1) 
    {
        SmartPtr<PenData> penData( new PenData(*_penData) );
        _penData = penData;
    }

    _penData->setCapStyle(cap);
}


Pen::CapStyle Pen::capStyle() const
{
    return _penData->capStyle();
}


void Pen::setJoinStyle(JoinStyle join)
{
    // COW
    if(_penData.refs() > 1) 
    {
        SmartPtr<PenData> penData( new PenData( *_penData) );
        _penData = penData;
    }

    _penData->setJoinStyle(join);
}


Pen::JoinStyle Pen::joinStyle() const
{
    return _penData->joinStyle();
}

} // namespace

} // namespace
