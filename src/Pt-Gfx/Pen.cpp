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
#include "Pt/Gfx/Pen.h"


namespace Pt {

namespace Gfx {

Pen::Pen(size_t size, const ARgbColor& color, LineStyle lineStyle, CapStyle capStyle, JoinStyle joinStyle)
: _size(size)
, _color(color)
, _lineStyle(lineStyle)
, _capStyle(capStyle)
, _joinStyle(joinStyle)
{

}


size_t Pen::size() const
{
	return _size;
}


const ARgbColor& Pen::color() const
{
	return _color;
}


Pen::LineStyle Pen::lineStyle() const
{
	return _lineStyle;
}


Pen::CapStyle Pen::capStyle() const
{
	return _capStyle;
}


Pen::JoinStyle Pen::joinStyle() const
{
	return _joinStyle;
}


void Pen::setSize(size_t size)
{
	_size = size;
}


void Pen::setColor(const ARgbColor& color)
{
	_color = color;
}


void Pen::setLineStyle(LineStyle lineStyle)
{
	_lineStyle = lineStyle;
}


void Pen::setCapStyle(CapStyle capStyle)
{
	_capStyle = capStyle;
}


void Pen::setJoinStyle(JoinStyle joinStyle)
{
	_joinStyle = joinStyle;
}


} // namespace Gfx

} // namespace Pt
