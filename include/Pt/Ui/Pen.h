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
#ifndef PT_UI_PEN_H
#define PT_UI_PEN_H

#include <Pt/UI/Api.h>
#include <Pt/Ui/Image.h>
#include <Pt/Ui/Color.h>
#include <Pt/SmartPtr.h>

namespace Pt {
namespace Ui {

class PenData;

/**
* @brief A pen which contains of attributes (size, color) for the drawing of outlines.
*
* Pen objects are used as container of drawing attributes for Painter objects. A size
* and a color can be specified per pen. The size and color are used to draw outlined
* shapes by the Painter. Outlined shapes for example are lines, outlined rectangles
* or ellipses and text.
*
* Example: When setting the pen color to green and the pen size to 5 and setting this
* Pen object as pen for a Painter, painting a line would result in a 5-pixel-sized
* green line.
*
* The Pen object is immutable. Thus a new object has to be created when a pen with
* other attributes is needed.
*/
class PT_UI_API Pen
{
    public:
        enum PenStyle{ SolidStyle = 0, DashStyle =1, DoubleDash = 2};
        enum CapStyle{ FlatCap = 0, RoundCap = 1, TriangularCap = 2, ProjectingCap = 3, ButtCap = 4, NotLastCap = 5 };
        enum JoinStyle{ RoundJoin = 0, BevelJoin = 1, MiterJoin = 2, TriangularJoin = 3};

        /**
        * @brief Creates a new Pen object.
        *
        * The default pen size is 1. The default pen color is black, the default style is solid
        * and the default cap and join style are round.
        */
        Pen();

        /**
        * @brief Creates a new Pen object with the specified size
        *
        * The default pen color is black. The default style is solid. The default cap and join style are round.
        */
        explicit Pen( size_t size );

        /**
        * @brief Creates a new Pen object with the specified style
        *
        * The default pen size is 1. The default pen color is black. The default cap and join style are round.
        */
        explicit Pen( PenStyle style );

        /**
        * @brief Creates a new Pen object with the specified color
        *
        * The default pen size is 1. The default style is solid. The default cap and join style are round.
        */
        explicit Pen( const Color& color );

        /**
        * @brief Creates a new Pen object using the specified size, color and style.
        *
        * The pen size, color and style are optional. The default pen size is 1. The
        * default pen color is black and the default style is solid.
        *
        * @param width The width of the pen. This parameter is optional. The default is 1.
        * @param color The color of the pen. This parameter is optional. The default is black.
        * @param style The style of the pen. This parameter is optional. The default is SolidStyle.
        * @param cap The cap style. This parameter is optional. The default is flat style.
        * @param join The join style. This parameter is optional. The default is round style.
        */
        Pen( size_t width, const Color& color, PenStyle style = SolidStyle, CapStyle cap = RoundCap, JoinStyle join = BevelJoin );

        /**
        * @brief Returns the size of the pen as specified when created.
        *
        * @return The size of the pen.
        */
        size_t size() const;

        /**
        * @brief Returns a reference to the color of the pen as specified when created.
        *
        * @return The color of the pen.
        */
        const Color& color() const;

        /**
        * @brief Returns the pen style.
        *
        * @return The pen style.
        */
        PenStyle style() const;

        /**
        * @brief Returns the cap style
        *
        * @return The cap style.
        */
        CapStyle capStyle() const;

        /**
        * @brief Returns the join style
        *
        * @return The join style.
        */
        JoinStyle joinStyle() const;

        /**
        * @brief Returns a reference to the pen color buffer
        *
        * @return The color buffer of the pen.
        */
        const Image& buffer() const;


  private:
      SmartPtr<PenData> _penData;
};


class PT_UI_API PenData
{
  public:
      PenData( size_t size, const Color& color, Pen::PenStyle style, Pen::CapStyle cap, Pen::JoinStyle join )
      : _size( size )
      , _style( style )
      , _buffer( 64, 1)
      , _capStyle( cap )
      , _joinStyle( join )
			, _color(color)
      { 
				_buffer.setColor( color);
			}

      ~PenData()
      { }

      const Color& color() const
      { return _color; }

      const Image& buffer() const
      { return _buffer; }

      size_t size() const
      { return _size; }

      Pen::PenStyle style() const
      { return _style; }

      Pen::CapStyle capStyle() const
      { return _capStyle;}

      Pen::JoinStyle joinStyle() const
      { return _joinStyle; }

  private:
      size_t          _size;
      Pen::PenStyle   _style;
      Image       _buffer;
      Pen::CapStyle   _capStyle;
      Pen::JoinStyle  _joinStyle;
			Color						_color;
};

} } // namespace 

#endif
