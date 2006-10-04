/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
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
#include "Pt/Gfx/Brush.h"


namespace Pt {

namespace Gfx {


Brush::Brush(Brush::FillStyle fillStyle, const ARgbColor& color, const ARgbImage* texture)
: _brushData(new BrushData(fillStyle, color, texture))
{
}


Brush::FillStyle Brush::fillStyle() const
{
	return _brushData->fillStyle();
}


const ARgbColor& Brush::color() const
{
	return _brushData->color();
}


const ARgbImage& Brush::texture() const
{
	return _brushData->texture();
}


BrushData::BrushData(Brush::FillStyle fillStyle, const ARgbColor& color, const ARgbImage* texture)
: _fillStyle(fillStyle)
, _color(color)
, _texture(0)
{
	if (texture != 0) {
		_texture = new ARgbImage(*texture);
	}
}


BrushData::~BrushData()
{
	delete _texture;
}


Brush::FillStyle BrushData::fillStyle() const
{
	return _fillStyle;
}


const ARgbColor& BrushData::color() const
{
	return _color;
}


const ARgbImage& BrushData::texture() const
{
	return *_texture;
}



} // namespace gfx

} // namespace ptv
