/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan

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

#ifndef PT_GFX_RASTERIZER_2_H
#define PT_GFX_RASTERIZER_2_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/String.h>
#include <Pt/System/Path.h>

namespace Pt {
namespace Gfx {


class DrawText;
class Image;

class Rasterizer2
{
    public:
        Rasterizer2( Image& image );

        ~Rasterizer2();

        void setImage(Image& image);

        const ImageFormat& format() const;

        void setPen( const Pen& pen );

        const Pen& pen() const
        {
            return _pen;
        }

        void setBrush( const Brush& brush );

        const Brush& brush() const
        {
            return _brush;
        }

        void setFont( const Font& font );

        const Font& font() const
        {
            return _font;
        }

        FontMetrics fontMetrics( const String& text ) const;

        static FontMetrics fontMetrics( const Font& font, const Pt::String& text );

        void setClip( const Rect& clip );

        const Rect& clip() const
        {
            return _clip;
        }

        void setCompositionMode(const CompositionMode& mode)
        {
            _compositionMode = mode;
        }

        const CompositionMode& compositionMode() const
        {
            return _compositionMode;
        }

        void image(const Point& to, const Image& image);
        void image(const Point& toIn, const Image& image, const Rect& imageRect);

        void strokeText(const Point& to, const Pt::String& text);
        void strokeOutline(const Point* points, size_t pointCount);

        void fillPolygon(const Point* points, const size_t pointCount);

    private:
        void updateClip();

        void prepWorkBuffer(Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t sizeX, Pt::int32_t sizeY);
        void blitWorkBufferToImage(Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t sizeX, Pt::int32_t sizeY, const Color& color);

        void rasterOnePixelLine(const Point& a, const Point& b);
        void rasterOnePixelLineSegment(Pt::int32_t fx1, Pt::int32_t fy1, Pt::int32_t fx2, Pt::int32_t fy2, Pt::int32_t steps);

        void rasterPolygonOutline(const Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY);

        void rasterSolidTriangles(const Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY);
        void rasterOneSolidTriangle(const Point& v1, const Point& v2, const Point& v3);
        void rasterOneSolidTriangleBottomFlat(const Point& v1, const Point& v2, const Point& v3);
        void rasterOneSolidTriangleTopFlat(const Point& v1, const Point& v2, const Point& v3);
        void rasterFillTriangles(Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY);

        void genClippedPolygonPoints(std::vector<Point>& dst, const Point* src, const size_t pointCount) const;

    private:
        Image*          _image;
        DrawText*       _text;
        Font            _font;
        CompositionMode _compositionMode;

        Pen             _pen;
        Image           _penBuffer;
        ConstPixel      _penPixel;

        Brush           _brush;
        Image           _brushBuffer;
        ConstPixel      _brushPixel;
        const Image*    _brushImage;
        bool            _isGradient;

        Rect            _clip;
        Rect            _currentClip;

        std::vector<Pt::uint16_t> _alphas; // Work buffer
};


} // namespace
} // namespace

#endif
