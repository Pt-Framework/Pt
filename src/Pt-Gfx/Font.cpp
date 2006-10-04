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
#include "Pt/Gfx/Font.h"

using namespace std;


namespace Pt {

namespace Gfx {

Font::Font(string name, size_t size, FontStyle fontStyle, size_t angle, Direction direction)
: _name(name),
  _size(size),
  _fontStyle(fontStyle),
  _angle(angle),
  _direction(direction)
{
}


std::string Font::name() const
{
	return _name;
}


Font::FontStyle Font::fontStyle() const
{
	return _fontStyle;
}


size_t Font::size() const
{
	return _size;
}


size_t Font::angle() const
{
	return _angle;
}


Font::Direction Font::direction() const
{
	return _direction;
}


void Font::setName(std::string name)
{
	_name = name;
}


void Font::setFontStyle(Font::FontStyle fontStyle)
{
	_fontStyle = fontStyle;
}


void Font::setSize(size_t size)
{
	_size = size;
}


void Font::setAngle(size_t angle)
{
	_angle = angle;
}


void Font::setDirection(Font::Direction direction)
{
	_direction = direction;
}


} // namespace gfx

} // namespace ptv
