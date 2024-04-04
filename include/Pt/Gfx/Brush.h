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

#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Image.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Gfx {

class BrushData;
class ImageSurface;

class ColorStop
{
    public:
        ColorStop(float position, const Color& color)
        : _position(position)
        , _color(color)
        {}

        float position() const
        { return _position; }

        const Color& color() const
        { return _color; }

    private:
        float _position;
        Color _color;
};


class ColorStops
{
    public:
        ColorStops()
        {}

        ~ColorStops()
        {}

        bool empty() const
        { return _stops.empty(); }

        std::size_t size() const
        { return _stops.size(); }

        void clear()
        { _stops.clear(); }

        // TODO: Throw exception if the position < 0.0 or position > 1.0
        // TODO: Throw exception if the positions are mixed up
        void add(float position, const Color& color)
        { _stops.push_back( ColorStop(position, color) ); }

        const ColorStop& operator[] (std::size_t n) const
        { return _stops[n]; }

        const ColorStop& front() const
        { return _stops.front(); }

        const ColorStop& back() const
        { return _stops.back(); }

        void calculateInterpolatedColor(Color& res, const float position) const;


    private:
        std::vector<ColorStop> _stops;
};


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
            Radial      = 3
        };

    public:
        /** @brief Contructs a null brush.
        */
        Brush();

        Brush(const Color& color);

        Brush(const Image& texture, Pt::int32_t offX = 0, Pt::int32_t offY = 0);

        static Brush verticalGradient(const Color& from, const Color& to);

        static Brush horizontalGradient(const Color& from, const Color& to);

        static Brush verticalGradient(const ColorStops& colorStops);

        static Brush horizontalGradient(const ColorStops& colorStops);

        /** @brief Constructs an absolute positioned linear gradient.
        */
        static Brush linearGradient(const PointF& begin, const PointF& end,
                                    const ColorStops& colorStops);

        /** @brief Constructs a relative positioned linear gradient.
        */
        static Brush linearGradient(float beginX, float beginY,
                                    float endX, float endY,
                                    const ColorStops& colorStops);

        /** @brief Constructs an absolute positioned radial gradient.
        */
        static Brush radialGradient(const PointF& begin, float beginRadius,
                                    const PointF& end, float endRadius,
                                    const ColorStops& colorStops);

        /** @brief Constructs a relative positioned radial gradient.
        */
        static Brush radialGradient(float beginX, float beginY, float beginRadius,
                                    float endX, float endY, float endRadius,
                                    const ColorStops& colorStops);

        /** @brief Returns the brushes fill style.
        */
        FillStyle fillStyle() const;

        PositionMode positionMode() const;

        void setColor(const Color& color);

        const Color& color() const;

        /** @brief Returns the gradient style.
        */
        GradientStyle gradient() const;

        // remove when linear gradients use color stops
        const Color& gradientColor() const;

        /** @brief Color stops of a gradient.
        */
        const ColorStops& gradientStops() const;

        /** @brief Begin of a linear or radial gradient.
        */
        const PointF& gradientBegin() const;

        /** @brief Radius of a radial gradient begin circle.
        */
        float gradientBeginRadius() const;

        /** @brief End of a linear or radial gradient.
        */
        const PointF& gradientEnd() const;

        /** @brief Radius of a radial gradient end circle.
        */
        float gradientEndRadius() const;

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
        , _ofsX(0)
        , _ofsY(0)
        , _texture(0)
        {}

        BrushData(const Color& color)
        : _isNull(false)
        , _fillStyle(Brush::Solid)
        , _color(color)
        , _gradient(Brush::Horizontal)
        , _ofsX(0)
        , _ofsY(0)
        , _texture(0)
        {}

        BrushData(const Image& texture,
                  Pt::int32_t offsetX, Pt::int32_t offsetY);

        // only for old Painter
        BrushData(const Color& from, const Color& to,
                  Brush::GradientStyle g);

        ~BrushData();

        Brush::FillStyle fillStyle() const
        { return _fillStyle; }

        Brush::PositionMode positionMode() const
        { return _positionMode; }

        void setSolid(const Color& color);

        const Color& color() const
        { return _gradientStops.empty() ? _color : _gradientStops.front().color(); }

        // 1D gradient
        void set1DGradient(Brush::GradientStyle g, const ColorStops& colorStops);

        // absolute positioned linear gradient
        void setLinearGradient(const PointF& begin, const PointF& end,
                               const ColorStops& colorStops);

        // relative positioned linear gradient
        void setLinearGradient(float beginX, float beginY,
                               float endX, float endY,
                               const ColorStops& colorStops);


        // absolute positioned radial gradient
        void setRadialGradient(const PointF& begin, float beginRadius,
                               const PointF& end, float endRadius,
                               const ColorStops& colorStops);

        // relative positioned radial gradient
        void setRadialGradient(float beginX, float beginY, float beginRadius,
                               float endX, float endY, float endRadius,
                               const ColorStops& colorStops);

        Brush::GradientStyle gradient() const
        { return _gradient; }

        // remove when linear gradients use color stops
        const Color& gradientColor() const
        { return _gradientStops.empty() ? _color : _gradientStops.back().color(); }

        const ColorStops& gradientStops() const
        { return _gradientStops; }

        const PointF& gradientBegin() const
        { return _gradientBegin; }

        float gradientBeginRadius() const
        { return _gradientBeginRadius; }

        const PointF& gradientEnd() const
        { return _gradientEnd; }

        float gradientEndRadius() const
        { return _gradientEndRadius; }

        void setTexture(const Image& texture,
                        Pt::int32_t offsetX, Pt::int32_t offsetY);

        const Image& texture() const;

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
        ColorStops           _gradientStops;
        PointF               _gradientBegin;
        float                _gradientBeginRadius;
        PointF               _gradientEnd;
        float                _gradientEndRadius;

        Pt::int32_t          _ofsX;
        Pt::int32_t          _ofsY;
        ImageSurface*        _texture;
};

} // namespace

} // namespace

#endif
