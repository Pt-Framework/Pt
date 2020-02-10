/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2017-2017 Aloysius Indrayanto

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef PT_GFX_PEN_H
#define PT_GFX_PEN_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Color.h>
#include <Pt/SmartPtr.h>
#include <cstddef>

namespace Pt {

namespace Gfx {

class PenData;

/** @brief Attributes for the drawing of outlines.

    Pen objects are used as container of drawing attributes for Painter
    objects. A size and a color can be specified per pen. The size and
    color are used to draw outlined shapes by the Painter. Outlined shapes
    for example are lines, outlined rectangles or ellipses and text.
*/
class PT_GFX_API Pen
{
    public:
        /** @brief Pen line style.
        */
        enum Style 
        { 
            Solid = 0,
            Dot   = 1,
            Dash  = 2,
            DashPattern = 3
        };

        /** @brief Pen cap style.
        */
        enum CapStyle 
        { 
            FlatCap   = 0,
            SquareCap = 1,
            RoundCap  = 2
        };

        /** @brief Pen join style.
        */
        enum JoinStyle 
        { 
            NoJoin    = 0,
            BevelJoin = 1,
            MiterJoin = 2,
            RoundJoin = 3
        };

        /** @brief Constructs a null pen.

            The default pen is null.
        */
        Pen();

        /** @brief Constructs a Pen with the specified color.

            The pen size is 1, the style is solid and the cap and join
            styles are round.
        */
        Pen(const Color& color);

        /** @brief Constructs a Pen with the specified size, color and styles.
        */
        Pen(const Color& color, std::size_t width, Style style = Solid,
            CapStyle cap = FlatCap, JoinStyle join = BevelJoin);

        /** @brief Constructs a Pen with the specified size, color and custom styles.
        */
        Pen(const Color& color, std::size_t width,
            const std::vector<Pt::uint8_t>& dashPattern,
            CapStyle cap = FlatCap, JoinStyle join = BevelJoin);

        /** @brief Returns true if the pen is null.
        */
        bool isNull() const;

        /** @brief Sets the size of the pen.
        */
        void setSize(std::size_t size);

        /** @brief Returns the size of the pen.
        */
        std::size_t size() const;

        /** @brief Sets the color of the pen.
        */
        void setColor(const Color& color);

        /** @brief Returns the color of the pen.
        */
        const Color& color() const;

        /** @brief Sets the pen style.
        */
        void setStyle(Style style = Solid);

        /** @brief Returns the pen style.
        */
        Style style() const;

        /** @brief Returns true if the pen is solid.
        */
        bool isSolid() const
        { return style() == Solid; }

        /** @brief Sets the pen user dash pattern.
        */
        void setDashPattern(const std::vector<Pt::uint8_t>& dashPattern);

        /** @brief Returns the pen user dash pattern.
        */
        const std::vector<Pt::uint8_t>& dashPattern() const;

        /** @brief Sets the cap style.
        */
        void setCapStyle(CapStyle cap = FlatCap);

        /** @brief Returns the cap style.
        */
        CapStyle capStyle() const;

        /** @brief Sets the join style.
        */
        void setJoinStyle(JoinStyle join = BevelJoin);

        /** @brief Returns the join style.
        */
        JoinStyle joinStyle() const;

    private:
      SmartPtr<PenData> _penData;
};


class PT_GFX_API PenData
{
  public:
      PenData(const Color& color, std::size_t size,
              Pen::Style style, Pen::CapStyle cap, Pen::JoinStyle join)
      : _color(color)
      , _size(size)
      , _style(style)
      , _capStyle(cap)
      , _joinStyle(join)
      {}

      PenData(const Color& color, std::size_t size,
              Pen::Style style, const std::vector<Pt::uint8_t>& dashPattern, Pen::CapStyle cap, Pen::JoinStyle join)
      : _color(color)
      , _size(size)
      , _style(style)
      , _dashPattern(dashPattern)
      , _capStyle(cap)
      , _joinStyle(join)
      {}

      PenData(const Color& color, std::size_t size,
              Pen::Style style, const Pt::uint8_t* dashPatternBeg, const Pt::uint8_t* dashPatternEnd, Pen::CapStyle cap, Pen::JoinStyle join)
      : _color(color)
      , _size(size)
      , _style(style)
      , _dashPattern(dashPatternBeg, dashPatternEnd)
      , _capStyle(cap)
      , _joinStyle(join)
      {}

      void setColor(const Color& color)
      { _color = color; }

      const Color& color() const
      { return _color; }

      void setSize(std::size_t size)
      { _size = size; }

      std::size_t size() const
      { return _size; }

      void setStyle(Pen::Style style)
      {
          _style = style;
          _dashPattern.clear();
      }

      Pen::Style style() const
      { return _style; }

      void setDashPattern(const std::vector<Pt::uint8_t>& dashPattern)
      {
          _style       = Pen::DashPattern;
          _dashPattern = dashPattern;
      }

      const std::vector<Pt::uint8_t>& dashPattern() const
      { return _dashPattern; }

      void setCapStyle(Pen::CapStyle cap)
      { _capStyle = cap;}

      Pen::CapStyle capStyle() const
      { return _capStyle;}

      void setJoinStyle(Pen::JoinStyle join)
      { _joinStyle = join; }

      Pen::JoinStyle joinStyle() const
      { return _joinStyle; }

  private:
      Color                    _color;
      std::size_t              _size;
      Pen::Style               _style;
      std::vector<Pt::uint8_t> _dashPattern;
      Pen::CapStyle            _capStyle;
      Pen::JoinStyle           _joinStyle;
};

} // namespace

} // namespace

#endif
