/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
 * Copyright (C) 2010 Aloysius Indrayanto
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Image.h>
#include <Pt/StringStream.h>
#include <Pt/Convert.h>
#include <Pt/SourceInfo.h>

namespace Pt {
namespace Gfx {

Pen::Pen()
: _penData(new PenData(1, Color(0,0,0), SolidStyle, RoundCap, RoundJoin))
{ }


Pen::Pen(size_t size)
: _penData(new PenData(size, Color(0,0,0), SolidStyle, RoundCap, RoundJoin))
{ }


Pen::Pen(PenStyle style)
: _penData(new PenData(1, Color(0,0,0), style, RoundCap, RoundJoin))
{ }


Pen::Pen(const Color& color)
: _penData(new PenData(1, color, SolidStyle, RoundCap, RoundJoin))
{ }


Pen::Pen(size_t size, const Color& color, PenStyle style, CapStyle cap, JoinStyle join)
: _penData(new PenData(size, color, style, cap, join))
{ }


size_t Pen::size() const
{
    return _penData->size();
}


const Color& Pen::color() const
{
    return _penData->color();
}


Pen::PenStyle Pen::style() const
{
    return _penData->style();
}


Pen::CapStyle Pen::capStyle() const
{
    return _penData->capStyle();
}


Pen::JoinStyle Pen::joinStyle() const
{
    return _penData->joinStyle();
}


const Image& Pen::buffer() const
{
    return _penData->buffer();
}

} } // namespace

