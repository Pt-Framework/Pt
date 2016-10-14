/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2010 Aloysius Indrayanto

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#ifndef PT_GFX_FONT_H
#define PT_GFX_FONT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Types.h>
#include <Pt/String.h>

namespace Pt {

namespace Gfx {

class PT_GFX_API Font
{
    public:
        enum Style 
        {
            Normal = 0, 
            Bold, 
            Italic, 
            BoldItalic
        };

        enum Direction {
            LeftToRight = 0, 
            RightToLeft
        };

    public:
        //! @brief Construct a font.
        explicit Font( const std::string& name      = std::string(),
                       size_t             size      = 12,
                       Style              style     = Normal,
                       ssize_t            angle     = 0,
                       Direction          direction = LeftToRight);

        //! @brief Returns the name of the font
        const std::string& name() const;

        //! @brief Returns the size of the font
        size_t size() const;

        //! @brief Returns the style of the font
        Style style() const;

        //! @brief Returns the angle of the font
        ssize_t angle() const;

        //! @brief Returns the text-flow direction of the font
        Direction direction() const;      

        friend bool operator==(const Font& a, const Font& b);

        friend bool operator!=(const Font& a, const Font& b);

        friend bool operator<(const Font& a, const Font& b);

    private:
        std::string _name;
        size_t      _size;
        Style       _style;
        ssize_t     _angle;
        Direction   _direction;
};


inline bool operator==(const Font& a, const Font& b)
{
    return a._name      == b._name      && 
           a._style     == b._style && 
           a._size      == b._size      && 
           a._angle     == b._angle     &&
           a._direction == b._direction;
}


inline bool operator!=(const Font& a, const Font& b)
{
    return a._name      != b._name      || 
           a._style     != b._style || 
           a._size      != b._size      || 
           a._angle     != b._angle     ||
           a._direction != b._direction;
}


inline bool operator<(const Font& a, const Font& b)
{
    if(a._name < b._name)
        return true;

    if(a._style < b._style)
        return true;

    if(a._size < b._size)
        return true;

    return false;
}

} //namespace

} //namespace

#endif
