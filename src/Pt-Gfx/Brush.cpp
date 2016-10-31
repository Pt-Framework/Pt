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
#include <Pt/Gfx/Brush.h>
#include <algorithm>

namespace Pt {

namespace Gfx {

Brush::Brush()
: _brushData( new BrushData() )
{
}


Brush::Brush(const Color& color)
: _brushData( new BrushData(color) )
{
}


Brush::Brush(const Image& texture)
: _brushData( new BrushData(texture) )
{
}


Brush::Brush(const Color& from, const Color& to, GradientDirection g)
: _brushData( new BrushData(from, to, g) )
{
}


Brush::FillStyle Brush::fillStyle() const
{
    return _brushData->fillStyle();
}


const Color& Brush::color() const
{
    return _brushData->color();
}


const Color& Brush::gradientColor() const
{
    return _brushData->gradientColor();
}


const Image& Brush::texture() const
{
    return _brushData->texture();
}


bool Brush::isNull() const
{
    return _brushData->isNull();
}


BrushData::BrushData()
: _fillStyle(Brush::Solid)
, _color(0, 0, 0)
, _isNull(true)
{
}


BrushData::BrushData(const Color& color)
: _fillStyle(Brush::Solid)
, _color(color)
, _texture()
, _isNull(false)
{
}


BrushData::BrushData(const Image& texture)
: _fillStyle(Brush::Texture)
, _texture(texture)
, _isNull(false)
{
}


BrushData::BrushData(const Color& from, const Color& to, Brush::GradientDirection g)
: _fillStyle( g == Brush::Horizontal ? Brush::HorizontalGradient
                                     : Brush::VerticalGradient)
, _color(from)
, _gradientColor(to)
, _isNull(false)
{
}


BrushData::~BrushData()
{
}


Brush::FillStyle BrushData::fillStyle() const
{
    return _fillStyle;
}


const Color& BrushData::color() const
{
    return _color;
}


const Color& BrushData::gradientColor() const
{
    return _gradientColor;
}


const Image& BrushData::texture() const
{
    return _texture;
}


bool BrushData::isNull() const
{
    return _isNull;
}

} // namespace

} // namespace
