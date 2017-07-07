/* Copyright (C) 2006-2017 Marc Boris Duerner
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

#ifndef PT_GFX_BRUSH_H
#define PT_GFX_BRUSH_H

#include <Pt/Gfx/Image.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Gfx {

class BrushData;

class PT_GFX_API Brush
{
    public:
        enum FillStyle
        {
            Solid     = 0,
            Texture   = 1,
            Gradient  = 2,
        };

        enum PositionMode
        {
            Absolute = 0,
            Relative = 1
        };

        enum GradientStyle
        {
            Horizontal  = 0, // only for old painters
            Vertical    = 1, // only for old painters
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

        Brush(const Image& texture, Pt::int32_t offX = 0, Pt::int32_t offY = 0);

        static Brush verticalGradient(const Color& from, const Color& to);

        static Brush horizontalGradient(const Color& from, const Color& to);

        static Brush linearGradient(const Color& from, const Color& to, 
                                    float angle = 0.0f);
        
        /** @brief Constructs a absolute positioned radial gradient.
        */
        static Brush radialGradient(const Color& from, const Color& to, 
                                    const PointF& focus);

        /** @brief Constructs a relative positioned radial gradient.
        */
        static Brush radialGradient(const Color& from, const Color& to, 
                                    float rx = 0.5, float ry = 0.5);

        static Brush conicalGradient(const Color& from, const Color& to, 
                                     float angle = 0.0f, const PointF& center = PointF());

        static Brush rectangularGradient(const Color& from, const Color& to, 
                                         float angle = 0.0f);

        FillStyle fillStyle() const;

        PositionMode positionMode() const;

        void setColor(const Color& color);

        const Color& color() const;

        GradientStyle gradient() const;

        const Color& gradientColor() const;

        float gradientAngle() const;

        const PointF& gradientFocus() const;

        void setTexture(const Image& texture, 
                        Pt::int32_t offX = 0, Pt::int32_t offY = 0);

        const Image& texture() const;

        // TODO: offset for textures is the origin and needs to be
        //       handled diferently in the painters

        Pt::int32_t offsetX() const;

        Pt::int32_t offsetY() const;

        bool isGradient() const;

        bool isTexture() const;

        bool isNull() const;

    private:
        Brush(BrushData* data);

    private:
        SmartPtr<BrushData> _brushData;
};


class BrushData
{
    public:
        BrushData()
        : _isNull(true)
        , _fillStyle(Brush::Solid)
        , _color(0, 0, 0)
        , _gradient(Brush::Horizontal)
        {}

        BrushData(const Color& color)
        : _isNull(false)
        , _fillStyle(Brush::Solid)
        , _color(color)
        , _gradient(Brush::Horizontal)
        {}

        BrushData(const Image& texture, 
                  Pt::int32_t offsetX, Pt::int32_t offsetY);

        // only for old Painter
        BrushData(const Color& from, const Color& to, 
                  Brush::GradientStyle g);

        ~BrushData()
        {}

        Brush::FillStyle fillStyle() const
        { return _fillStyle; }

        Brush::PositionMode positionMode() const
        { return _positionMode; }

        void setColor(const Color& color)
        {
            _fillStyle = Brush::Solid;
            _color     = color;
            _isNull    = false;

            _texture   = Image();
        }

        const Color& color() const
        { return _color; }

        void setLinearGradient(const Color& from, const Color& to, 
                               float angle);

        void setRadialGradient(const Color& from, const Color& to, 
                               const PointF& focus);

        void setRadialGradient(const Color& from, const Color& to, 
                               float rx, float ry);

        void setConicalGradient(const Color& from, const Color& to, 
                                const PointF& center, float angle);

        void setRectangularGradient(const Color& from, const Color& to, 
                                    float angle);

        Brush::GradientStyle gradient() const
        { return _gradient; }

        const Color& gradientColor() const
        { return _gradientColor; }

        const PointF& gradientFocus() const
        { return _gradientFocus; }

        void setTexture(const Image& texture, 
                       Pt::int32_t offsetX, Pt::int32_t offsetY);

        const Image& texture() const
        { return _texture; }

        float gradientAngle() const
        { return _gradientAngle; }

        Pt::int32_t offsetX() const
        { return _ofsX; }

        Pt::int32_t offsetY() const
        { return _ofsY; }

        bool isGradient() const
        { return _fillStyle == Brush::Gradient; }

        bool isTexture() const
        { return _fillStyle == Brush::Texture; }

        bool isNull() const
        { return _isNull; }

    private:
        bool                 _isNull;
        Brush::FillStyle     _fillStyle;
        Brush::PositionMode  _positionMode;
        Color                _color;
        Brush::GradientStyle _gradient;
        Color                _gradientColor;
        float                _gradientAngle;
        PointF               _gradientFocus;
        Pt::int32_t          _ofsX;
        Pt::int32_t          _ofsY;
        Image                _texture;
};

} // namespace

} // namespace

#endif
