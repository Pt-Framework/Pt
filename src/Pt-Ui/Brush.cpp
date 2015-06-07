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
#include <Pt/Ui/Brush.h>
#include <algorithm>

namespace Pt{
namespace Ui{

Brush::Brush(const Color& color)
: _brushData( new BrushData(SolidFill, color, 0) )
{
}


Brush::Brush(const Image* texture)
: _brushData( new BrushData(TextureFill, Color(0, 0, 0), texture) )
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


const Image& Brush::texture() const
{
    return _brushData->texture();
}


BrushData::BrushData(Brush::FillStyle fillStyle, const Color& color, const Image* texture)
: _fillStyle(fillStyle)
, _color(color)
, _texture(0)
{
  _texture = (texture != 0) ?   new Image(*texture) : new Image(64, 1);
}


BrushData::~BrushData()
{
    delete _texture;
}


Brush::FillStyle BrushData::fillStyle() const
{
    return _fillStyle;
}


const Color& BrushData::color() const
{
    return _color;
}


const Image& BrushData::texture() const
{
    return *_texture;
}

bool operator==(const Brush& a, const Brush& b)
{
	return a._brushData->fillStyle() == b._brushData->fillStyle() &&
	       a._brushData->color() == b._brushData->color();
//           && a._brushData->texture() == b._brushData->texture();    //real image comparison needed ???
}

bool operator<(const Brush& a, const Brush& b)
{
	return a._brushData->fillStyle() < b._brushData->fillStyle();
}


void operator >>=( const SerializationInfo& si, Brush& brush )
{
    Pt::String s;
    si.getString(s);
    brush = Brush( Color::fromHtml(s) );
}


void operator <<=( SerializationInfo& si, const Brush& brush )
{
    si.setString( brush.color().toHtml() );
    si.setTypeName("Brush");
}


} } // namespace
