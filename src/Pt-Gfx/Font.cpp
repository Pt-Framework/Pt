/***************************************************************************
 *   Copyright (C) 2006-2007 Tobias Müller                                 *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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

Font::Font(const std::string& name, size_t size, FontStyle fontStyle, ssize_t angle, Direction direction)
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


size_t Font::size() const
{
    return _size;
}


Font::FontStyle Font::fontStyle() const
{
    return _fontStyle;
}


ssize_t Font::angle() const
{
    return _angle;
}


Font::Direction Font::direction() const
{
    return _direction;
}


} // namespace Gfx

} // namespace Pt
