/*
 * Copyright (C) 2006-2007 by Tobias Mller
 * Copyright (C) 2006-2007 by Marc Boris Drner
 * Copyright (C) 2006-2007 PTV AG
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef PT_GFX_FONT_H
#define PT_GFX_FONT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Types.h>
#include <Pt/String.h>
#include <Pt/SourceInfo.h>


namespace Pt {

class SerializationInfo;

namespace Gfx {

    class PT_GFX_API Font
    {
        friend bool operator==(const Font& a, const Font& b);
        friend bool operator<(const Font& a, const Font& b);

        public:
            enum FontStyle {
                NormalStyle = 0, BoldStyle, ItalicStyle, BoldItalicStyle
            };

            enum Direction {
                LeftToRightDirection = 0, RightToLeftDirection
            };

        public:
            //! @brief Construct a font object using the given informations
            Font(
                const std::string& name      = "",
                size_t             size      = 12,
                FontStyle          fontStyle = NormalStyle,
                ssize_t            angle     = 0,
                Direction          direction = LeftToRightDirection

            );

            //! @brief Return the name of the font
            std::string name() const;

            //! @brief Return the size of the font
            size_t size() const;

            //! @brief Return the style of the font
            FontStyle fontStyle() const;

            //! @brief Return the angle of the font
            ssize_t angle() const;

            //! @brief Return the text-flow direction of the font
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


    PT_GFX_API void operator >>=( const SerializationInfo& si, Gfx::Font& x );

    PT_GFX_API void operator <<=( SerializationInfo& si, const Gfx::Font& x );

} // namespace Gfx

} // namespace Pt

#endif
