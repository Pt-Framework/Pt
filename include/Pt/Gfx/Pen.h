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

/** @brief Attributs for the drawing of outlines.
   
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
        enum Style { Solid      = 0, 
                     Dash       = 1, 
                     DoubleDash = 2 };
        
        /** @brief Pen cap style.
        */
        enum CapStyle { FlatCap = 0, 
                        RoundCap = 1, 
                        TriangularCap = 2, 
                        ProjectingCap = 3, 
                        ButtCap = 4, 
                        NotLastCap = 5 };
        
        /** @brief Pen join style.
        */
        enum JoinStyle { RoundJoin = 0, 
                         BevelJoin = 1, 
                         MiterJoin = 2, 
                         TriangularJoin = 3 };

        /** @brief Constructs a null pen.
        
            The default pen is null.
        */
        Pen();

        /** @brief Constructs a Pen with the specified color.
            
            The pen size is 1, the style is solid and the cap and join
            styles are round.
        */
        explicit Pen(const Color& color);

        /** @brief Constructs a Pen with the specified size, color and styles.
        */
        Pen(const Color& color, std::size_t width,  
            Style style = Solid, CapStyle cap = FlatCap, 
            JoinStyle join = BevelJoin);

        /** @brief Returns the size of the pen.
        */
        std::size_t size() const;

        /** @brief Returns the color of the pen.
        */
        const Color& color() const;

        /** @brief Returns the pen style.
        */
        Style style() const;

        /** @brief Returns the cap style.
        */
        CapStyle capStyle() const;

        /** @brief Returns the join style.
        */
        JoinStyle joinStyle() const;

        bool isNull() const;

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
      , _style(style )      
      , _capStyle(cap)
      , _joinStyle(join)
      { }

      const Color& color() const
      { return _color; }

      std::size_t size() const
      { return _size; }

      Pen::Style style() const
      { return _style; }

      Pen::CapStyle capStyle() const
      { return _capStyle;}

      Pen::JoinStyle joinStyle() const
      { return _joinStyle; }

  private:
      Color          _color;
      std::size_t    _size;
      Pen::Style     _style;   
      Pen::CapStyle  _capStyle;
      Pen::JoinStyle _joinStyle;
};

} // namespace 

} // namespace 

#endif
