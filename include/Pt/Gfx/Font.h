/***************************************************************************
 *   Copyright (C) 2006-2007 by Tobias Mller                              *
 *   Copyright (C) 2006-2007 by Marc Boris Drner                          *
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

#ifndef PT_GFX_FONT_H
#define PT_GFX_FONT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Types.h>
#include <Pt/AnyTraits.h>
#include <Pt/SourceInfo.h>

#include <Pt/String.h>

namespace Pt {

namespace Gfx {

    class PT_GFX_API Font
    {
        friend bool operator==(const Font& a, const Font& b);
        friend bool operator<(const Font& a, const Font& b);

        public:
            enum FontStyle {
                NormalStyle, BoldStyle, ItalicStyle, BoldItalicStyle
            };

            enum Direction {
                LeftToRightDirection, RightToLeftDirection
            };

        public:
            Font(
                const std::string& name,
                size_t             size = 12,
                FontStyle          fontStyle = NormalStyle,
                ssize_t            angle     = 0,
                Direction          direction = LeftToRightDirection
                
            );

            const std::string& name() const;

            size_t size() const;

            FontStyle fontStyle() const;

            ssize_t angle() const;

            Direction direction() const;

        private:
            std::string _name;
            size_t      _size;
            FontStyle   _fontStyle;
            ssize_t     _angle;
            Direction   _direction;
    };

    inline bool operator==(const Font& a, const Font& b)
    {
        return
               a._name.compare(b._name) == 0
            && a._fontStyle             == b._fontStyle
            && a._size                  == b._size
            && a._angle                 == b._angle
            && a._direction             == b._direction;
    }

    inline bool operator<(const Font& a, const Font& b)
    { 
        return a._size < b._size;
    }

} // namespace Gfx



/*
template <>
struct AnyTraits<Gfx::Font> {
	static void output(std::ostream& os, const Gfx::Font& value);
	static void input(std::istream& is, Gfx::Font& value);
	static void output(std::basic_ostream<Pt::Char>& os, const Gfx::Font& value);
	static void input(std::basic_istream<Pt::Char>& is, Gfx::Font& value);
};


template <typename CharT>
inline void outputGeneric(std::basic_ostream<CharT>& os, const Gfx::Font& value)
{
    os << '(';
    Pt::AnyTraits<std::string>::output(os, value.name());
    os << ' ' << value.size() << ' ' << value.fontStyle() << ' ' << value.angle() << ' ' << value.direction() << ')';
}

	
inline void Pt::AnyTraits<Gfx::Font>::output(std::ostream& os, const Gfx::Font& value)
{
	outputGeneric(os, value);
}


inline void Pt::AnyTraits<Gfx::Font>::output(std::basic_ostream<Pt::Char>& os, const Gfx::Font& value)
{
	outputGeneric(os, value);
}


template <typename CharT>
inline void inputGeneric(std::basic_istream<CharT>& is, Gfx::Font& value)
{
    CharT ch;
    	
    is >> ch;
    if (ch != '(')
    {
	    throw std::runtime_error("Could not read Font value" + PT_SOURCEINFO);
    }

    std::string fontName;         
    size_t      fontSize;
    ssize_t     fontStyle;
    ssize_t     fontAngle;
    ssize_t     fontDirection;

    AnyTraits<std::string>::input(is, fontName);
    is >> fontSize;
    is >> fontStyle;
    is >> fontAngle;
    is >> fontDirection;

    is >> ch;
    if (ch != ')')
    {
	    throw std::runtime_error("Could not read Font value" + PT_SOURCEINFO);
    }

    value = Gfx::Font(fontName, fontSize, (Gfx::Font::FontStyle)fontStyle, fontAngle, (Gfx::Font::Direction)fontDirection);
}


inline void Pt::AnyTraits<Gfx::Font>::input(std::istream& is, Gfx::Font& value)
{
	inputGeneric(is, value);
}


inline void Pt::AnyTraits<Gfx::Font>::input(std::basic_istream<Pt::Char>& is, Gfx::Font& value)
{
	inputGeneric(is, value);
}
*/

} // namespace Pt

#endif
