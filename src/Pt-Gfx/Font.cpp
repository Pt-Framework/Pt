/***************************************************************************
 *   Copyright (C) 2006-2007 Tobias Mller                                 *
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
#include "Pt/StringStream.h"
#include "Pt/SerializationInfo.h"


namespace Pt {

namespace Gfx {

Font::Font()
: _name("")
,  _size(12)
,  _fontStyle(NormalStyle)
,  _angle(0)
,  _direction(LeftToRightDirection)
{
}

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


void operator >>=(const SerializationInfo& si, Gfx::Font& f)
{
    Pt::String fontName;
    size_t      fontSize;
    ssize_t     fontStyle;
    ssize_t     fontAngle;
    ssize_t     fontDirection;

    Pt::StringStream ss( si.toString() );
    getline( ss, fontName, Pt::Char('-') );

    ss >> fontSize;
    ss.get();

    ss >> fontStyle;
    ss.get();

    ss >> fontAngle;
    ss.get();

    ss >> fontDirection;

    if( ss.fail() )
        throw ConversionError( PT_ERROR_MSG("Font") );

    f = Gfx::Font( fontName.narrow(), fontSize, (Gfx::Font::FontStyle)fontStyle,
                   fontAngle, (Gfx::Font::Direction)fontDirection );
}


void operator <<=(SerializationInfo& si, const Gfx::Font& f)
{
    Pt::StringStream ss;
    ss << Pt::String::widen( f.name() ) << Pt::Char('-')
       << f.size() << Pt::Char('-')
       << f.fontStyle() << Pt::Char('-')
       << f.angle() << Pt::Char('-')
       << f.direction();

    si.setValue( ss.str() );
    si.setTypeName("Brush");
}

} // namespace Gfx

} // namespace Pt
