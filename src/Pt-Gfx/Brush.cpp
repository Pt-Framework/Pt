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


void Brush::setSolidColor(const Color& color)
{
    // COW

    SmartPtr<BrushData> brushData(new BrushData);

    *brushData = *_brushData;
    brushData->setSolidColor(color);

    _brushData = brushData;
}


Brush::Brush(const Color& color)
: _brushData( new BrushData(color) )
{
}


Brush::Brush(const Image& texture)
: _brushData( new BrushData(texture) )
{
}


Brush::Brush(const Color& from, const Color& to, GradientDirection g, float angleDeg, float scale)
: _brushData( new BrushData(from, to, g, angleDeg, scale) )
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


void Brush::setGradient(const Color& from, const Color& to, GradientDirection g, float angleDeg, float scale)
{
    // COW

    SmartPtr<BrushData> brushData(new BrushData);

    *brushData = *_brushData;
    brushData->setGradient(from, to, g, angleDeg, scale);

    _brushData = brushData;
}


const Color& Brush::gradientColor() const
{
    return _brushData->gradientColor();
}


void Brush::setTexture(const Image& texture)
{
    // COW

    SmartPtr<BrushData> brushData(new BrushData);

    *brushData = *_brushData;
    brushData->setTexture(texture);

    _brushData = brushData;
}


const Image& Brush::texture() const
{
    return _brushData->texture();
}


float Brush::angle() const
{
    return _brushData->angle();
}


float Brush::scale() const
{
    return _brushData->scale();
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


BrushData::BrushData(const Color& from, const Color& to, Brush::GradientDirection g, float angleDeg, float scale)
: _color(from)
, _gradientColor(to)
, _angle(angleDeg)
, _scale(scale)
, _isNull(false)
{
    switch(g) {
        case Brush::Horizontal  : _fillStyle = Brush::HorizontalGradient;  break;
        case Brush::Vertical    : _fillStyle = Brush::VerticalGradient;    break;
        case Brush::Linear      : _fillStyle = Brush::LinearGradient;      break;
        case Brush::Rectangular : _fillStyle = Brush::RectangularGradient; break;
        case Brush::Radial      : _fillStyle = Brush::RadialGradient;      break;
        case Brush::Conical     : _fillStyle = Brush::ConicalGradient;     break;
        default                 : _fillStyle = Brush::Solid;               break;
    }
}


BrushData::~BrushData()
{
}


Brush::FillStyle BrushData::fillStyle() const
{
    return _fillStyle;
}


void BrushData::setSolidColor(const Color& color)
{
    _fillStyle = Brush::Solid;
    _color     = color;
    _isNull    = false;
}


const Color& BrushData::color() const
{
    return _color;
}


void BrushData::setGradient(const Color& from, const Color& to, Brush::GradientDirection g, float angleDeg, float scale)
{
}


const Color& BrushData::gradientColor() const
{
    return _gradientColor;
}


void BrushData::setTexture(const Image& texture)
{
}


const Image& BrushData::texture() const
{
    return _texture;
}


float BrushData::angle() const
{
    return _angle;
}


float BrushData::scale() const
{
    return _scale;
}


bool BrushData::isNull() const
{
    return _isNull;
}

} // namespace

} // namespace
