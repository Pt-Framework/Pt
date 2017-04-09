/* Copyright (C) 2006-2016 Marc Boris Duerner
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

#ifndef PT_GFX_BRUSH_H
#define PT_GFX_BRUSH_H

#include <Pt/SmartPtr.h>

#include <Pt/Gfx/Image.h>


namespace Pt {
namespace Gfx {


class BrushData;


class PT_GFX_API Brush
{
    public:
        enum FillStyle
        {
            Solid               = 0,
            Texture             = 1,
            HorizontalGradient  = 2,
            VerticalGradient    = 3,
            LinearGradient      = 4,
            RectangularGradient = 5,
            RadialGradient      = 6,
            ConicalGradient     = 7
        };

        enum GradientDirection
        {
            Horizontal  = 0,
            Vertical    = 1,
            Linear      = 2,
            Rectangular = 3,
            Radial      = 4,
            Conical     = 5
        };

    public:
        /** @brief Contructs a null brush.
        */
        Brush();

        Brush(const Color& color);

        // TODO: texture offset
        Brush(const Image& texture);

        Brush(const Color& from, const Color& to, GradientDirection g, float rotDeg = 0.0f, float scale = 1.0f, Pt::int32_t ofsX = 0, Pt::int32_t ofsY = 0);

        FillStyle fillStyle() const;

        void setSolidColor(const Color& color);

        const Color& color() const;

        void setGradient(const Color& from, const Color& to, GradientDirection g, float rotDeg = 0.0f, float scale = 1.0f, Pt::int32_t ofsX = 0, Pt::int32_t ofsY = 0);

        void setGradientRotation(float rotDeg = 0.0f);

        void setGradientScale(float scale = 1.0f);

        void setGradientOffset(Pt::int32_t ofsX = 0, Pt::int32_t ofsY = 0);

        const Color& gradientColor() const;

        void setTexture(const Image& texture);

        const Image& texture() const;

        float rotation() const;

        float scale() const;

        Pt::int32_t offsetX() const;

        Pt::int32_t offsetY() const;

        bool isGradient() const;

        bool isNull() const;

    private:
        SmartPtr<BrushData> _brushData;
};


class BrushData
{
    public:
        BrushData();

        BrushData(const Color& color);

        BrushData(const Image& texture);

        BrushData(const Color& from, const Color& to, Brush::GradientDirection g, float rotDeg, float scale, Pt::int32_t ofsX, Pt::int32_t ofsY);

        ~BrushData();

        Brush::FillStyle fillStyle() const;

        void setSolidColor(const Color& color);

        const Color& color() const;

        void setGradient(const Color& from, const Color& to, Brush::GradientDirection g, float rotDeg, float scale, Pt::int32_t ofsX, Pt::int32_t ofsY);

        void setGradientRotation(float rotDeg);

        void setGradientScale(float scale);

        void setGradientOffset(Pt::int32_t ofsX, Pt::int32_t ofsY);

        const Color& gradientColor() const;

        void setTexture(const Image& texture);

        const Image& texture() const;

        float rotation() const;

        float scale() const;

        Pt::int32_t offsetX() const;

        Pt::int32_t offsetY() const;

        bool isGradient() const;

        bool isNull() const;

    private:
        Brush::FillStyle _fillStyle;
        Color            _color;
        Image            _texture;
        Color            _gradientColor;
        float            _rotDeg;
        float            _scale;
        Pt::int32_t      _ofsX;
        Pt::int32_t      _ofsY;
        bool             _isNull;
};


} // namespace
} // namespace

#endif
