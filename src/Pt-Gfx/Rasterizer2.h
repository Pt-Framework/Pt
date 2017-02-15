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


// Uncomment this to use Duff's device
// NOTE: Enabling this optimization does not seem to reduce or increase performance, EXCEPT when using -O0
#define USE_DUFFS_DEVICE

// Uncomment this to use putPixels() for drawing solid colors
// NOTE: enabling this one seems to only improve performance for SourceOver
#define USE_PUTPIXELS_FOR_SOLID_COLOR

class DrawText;
class Image;

class Rasterizer2
{
    public:
        Rasterizer2( Image& image );

        ~Rasterizer2();

        void setAntiAliasingLevel(Pt::uint8_t level)
        {
            _aaLevel = level;
        }

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
        void strokeRect(const Point& tl, const Point& br);
        void strokePolygon(const Point* points, size_t pointCount);

        void fillRect(const Point& tl, const Point& br);
        void fillPolygon(const Point* points, size_t pointCount);
        void fillPolygonSeparate(const Point* points, size_t pointCount);

    private:
        void rasterOnePixelLine(const Point& a, const Point& b);
        void rasterOnePixelHLineSegment(Pt::int32_t x1, Pt::int32_t x2, Pt::int32_t y, const Color& color, bool skipLastPoint);
        void rasterOnePixelVLineSegment(Pt::int32_t x, Pt::int32_t y1, Pt::int32_t y2, const Color& color, bool skipLastPoint);
        void rasterOnePixelGLineSegmentNOAA(Pt::int32_t fx1, Pt::int32_t fy1, Pt::int32_t fx2, Pt::int32_t fy2, const Color& color, bool skipLastPoint);
        void rasterOnePixelGLineSegmentXWAA(Pt::int32_t fx1, Pt::int32_t fy1, Pt::int32_t fx2, Pt::int32_t fy2, const Color& color, bool skipLastPoint);

        void rasterOnePixelRectOutline(const Point& tl, const Point& br);
        void rasterRectArea(const Point& tl, const Point& br);

        void rasterPolygonOutline(const Point* points, size_t pointCount, const Color& color);
        void rasterPolygonAreaJaggies(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY);
        void rasterPolygonAreaFSAA2x2(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY);
        void rasterPolygonAreaSSAA4x4(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY);

        void updateGradientBrush(Pt::int32_t width, Pt::int32_t height);

        void updateClip();
        void genClippedPolygonPoints(std::vector<Point>& dst, const Point* src, const size_t pointCount) const;
        void getPolygonRectMinMax(const Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY);

    private:
        void rasterScanline(
            Pt::int32_t  iterL, Pt::int32_t iterR, Pt::int32_t pixelY,
            Pt::int32_t  minX,  Pt::int32_t minY,
            const Color& color
        );

        template<Pt::uint8_t SUPERSAMPLE_SIZE, Pt::uint8_t MIN_ALPHA, Pt::uint8_t MUL_ALPHA>
        void rasterScanline(
            Pt::int32_t  iterL, Pt::int32_t iterR, Pt::int32_t pixelY,
            Pt::int32_t  minX,  Pt::int32_t minY,  Pt::int32_t sizeX,
            const Color& color, const std::vector<Pt::uint8_t>& alphas
        );

    private:
        Pt::uint8_t     _aaLevel;

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
        bool            _isTexture;

        Rect            _clip;
        Rect            _currentClip;
};

// ======================================================================================
// ===== Templated Private Member Functions =============================================
// ======================================================================================

template<Pt::uint8_t SUPERSAMPLE_SIZE, Pt::uint8_t MIN_ALPHA, Pt::uint8_t MUL_ALPHA>
void Rasterizer2::rasterScanline(
    Pt::int32_t  iterL, Pt::int32_t iterR, Pt::int32_t pixelY,
    Pt::int32_t  minX,  Pt::int32_t minY,  Pt::int32_t sizeX,
    const Color& color, const std::vector<Pt::uint8_t>& alphas
)
{
    // A helper macro to scale the alpha
    #define RSL_SCALE_ALPHA(A) ( Pt::uint16_t(A) * MUL_ALPHA / SUPERSAMPLE_SIZE / SUPERSAMPLE_SIZE )

    // The minimum, middle, and maximum values for alpha
    #define RSL_MIN_ALPHA  MIN_ALPHA
    #define RSL_MID_ALPHA (MIN_ALPHA * SUPERSAMPLE_SIZE    )
    #define RSL_MAX_ALPHA (MIN_ALPHA * SUPERSAMPLE_SIZE * SUPERSAMPLE_SIZE)

    //
    // Draw pixels that belongs to the left-part of the span to the image
    //
    if(iterL < 0) iterL = 0;


    // Skip fully-transparent pixels
#ifdef USE_DUFFS_DEVICE
    if(true) {
        register const Pt::uint8_t* src  = &alphas[0];
        register       Pt::int32_t  cnt  = sizeX - 1;
        register       Pt::int32_t  n    = (cnt + 7) / 8;
        register       Pt::int32_t  k    = iterL;
        switch(cnt % 8) {
                case 0 : do { if(src[k]) {n = 0; break; } ++k;
                case 7 :      if(src[k]) {n = 0; break; } ++k;
                case 6 :      if(src[k]) {n = 0; break; } ++k;
                case 5 :      if(src[k]) {n = 0; break; } ++k;
                case 4 :      if(src[k]) {n = 0; break; } ++k;
                case 3 :      if(src[k]) {n = 0; break; } ++k;
                case 2 :      if(src[k]) {n = 0; break; } ++k;
                case 1 :      if(src[k]) {n = 0; break; } ++k;
                         } while (--n > 0);
        }
        iterL = k;
    }
#else
    for(; iterL < sizeX; ++iterL) {
        if(alphas[iterL]) break;
    }
#endif

     // Texture or gradient
    if(_isTexture || _isGradient) {
        for(; iterL < sizeX; ++iterL) {
            // Break if we have reached the non anti-aliased part of the span
            if(alphas[iterL] >= RSL_MAX_ALPHA) break;
            // Draw the pixel
            const Pt::int32_t iterX = minX + iterL;
            const Pt::int32_t iterY = minY + pixelY;
            const Pt::int32_t tX    = iterL  % _brushImage->width ();
            const Pt::int32_t tY    = pixelY % _brushImage->height();
            ConstPixel srcPixel(_brushImage->view(), tX, tY);
            Pixel      dstPixel(_image->view(), iterX, iterY);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, RSL_SCALE_ALPHA(alphas[iterL]));
        }
    }

    // Solid color
    else {
        for(; iterL < sizeX; ++iterL) {
            // Break if we have reached the non anti-aliased part of the span
            if(alphas[iterL] >= RSL_MAX_ALPHA) break;
            // Draw the pixel
            Pixel pixel(_image->view(), minX + iterL, minY + pixelY);
            _image->format().setPixel(pixel, color, _compositionMode, RSL_SCALE_ALPHA(alphas[iterL]));
        }
    }

    //
    // Draw pixels that belongs to the right-part of the span to the image
    //
    if(iterR >= sizeX) iterR = sizeX - 1;

    // Skip fully-transparent pixels
#ifdef USE_DUFFS_DEVICE
    if(true) {
        register const Pt::uint8_t* src  = &alphas[0];
        register       Pt::int32_t  cnt  = sizeX - 1;
        register       Pt::int32_t  n    = (cnt + 7) / 8;
        register       Pt::int32_t  k    = iterR;
        switch(cnt % 8) {
                case 0 : do { if(src[k]) {n = 0; break; } --k;
                case 7 :      if(src[k]) {n = 0; break; } --k;
                case 6 :      if(src[k]) {n = 0; break; } --k;
                case 5 :      if(src[k]) {n = 0; break; } --k;
                case 4 :      if(src[k]) {n = 0; break; } --k;
                case 3 :      if(src[k]) {n = 0; break; } --k;
                case 2 :      if(src[k]) {n = 0; break; } --k;
                case 1 :      if(src[k]) {n = 0; break; } --k;
                         } while (--n > 0);
        }
        iterR = k;
    }
#else
    for(; iterR >= 0; --iterR) {
        if(alphas[iterR]) break;
    }
#endif

     // Texture or gradient
    if(_isTexture || _isGradient) {
        for(; iterR >= 0; --iterR) {
            // Break if we have reached the non anti-aliased part of the span
            if(alphas[iterR] >= RSL_MAX_ALPHA) break;
            // Draw the pixel
            const Pt::int32_t iterX = minX + iterR;
            const Pt::int32_t iterY = minY + pixelY;
            const Pt::int32_t tX    = iterR  % _brushImage->width ();
            const Pt::int32_t tY    = pixelY % _brushImage->height();
            ConstPixel srcPixel(_brushImage->view(), tX, tY);
            Pixel      dstPixel(_image->view(), iterX, iterY);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, RSL_SCALE_ALPHA(alphas[iterR]));
        }
    }

    // Solid color
    else {
        for(; iterR >= 0; --iterR) {
            // Break if we have reached the non anti-aliased part of the span
            if(alphas[iterR] >= RSL_MAX_ALPHA) break;
            // Draw the pixel
            Pixel pixel(_image->view(), minX + iterR, minY + pixelY);
            _image->format().setPixel(pixel, color, _compositionMode, RSL_SCALE_ALPHA(alphas[iterR]));
        }
    }

    //
    // Draw pixels that belongs to the middle-part of the span to the image
    //
    if(iterR < iterL) return;


    // Draw the span using texture
    if(_isTexture) {
        Pt::int32_t iterX     = iterL;
        Pt::int32_t spanWidth = iterR - iterL + 1;
        while(spanWidth > 0) {
            const Pt::int32_t tX = iterX  % _brushImage->width ();
            const Pt::int32_t tY = pixelY % _brushImage->height();
            const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tX);
            if(n) {
                ConstPixel srcPixel(_brushImage->view(), tX, tY);
                Pixel      dstPixel(_image->view(), minX + iterX, minY + pixelY);
                _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
            }
            spanWidth -= n;
            iterX     += n;
        }
    }

    // Draw the span using gradient
    else if(_isGradient) {
        Pt::int32_t iterX     = iterL;
        Pt::int32_t spanWidth = iterR - iterL + 1;
        // Fill the span - vertical gradient
        if(_brush.fillStyle() == Pt::Gfx::Brush::VerticalGradient) {
            const Pt::int32_t textureY = pixelY % _brushImage->height();
            ConstPixel        srcPixel(_brushImage->view(), 0, textureY);
            Pixel             dstPixel(_image->view(), minX + iterX, minY + pixelY);
            _image->format().setPixels(dstPixel, srcPixel, spanWidth, _compositionMode);
        }
        // Fill the span - horizontal gradient
        else {
            while(spanWidth > 0) {
                const Pt::int32_t tX = iterX  % _brushImage->width ();
                const Pt::int32_t tY = pixelY % _brushImage->height();
                const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tX);
                if(n) {
                    ConstPixel srcPixel(_brushImage->view(), tX, tY);
                    Pixel      dstPixel(_image->view(), minX + iterX, minY + pixelY);
                    _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
                }
                spanWidth -= n;
                iterX     += n;
            }
        }
    }

    // Draw the span using solid color
    else {
#ifdef USE_PUTPIXELS_FOR_SOLID_COLOR
        Pixel pixel(_image->view(), minX + iterL, minY + pixelY);
        _image->format().setPixels(pixel, _brush.color(), iterR - iterL + 1, _compositionMode);
#else
        Pt::int32_t iterX     = minX + iterL;
        Pt::int32_t spanWidth = iterR - iterL + 1;
        while(spanWidth > 0) {
            const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
            if(n) {
                Pixel pixel(_image->view(), iterX, minY + pixelY);
                _image->format().copy(pixel, _brushPixel, n, _compositionMode);
            }
            spanWidth -= n;
            iterX     += n;
        }
#endif
    }

    // Undefine the macros
    #undef RSL_SCALE_ALPHA
    #undef RSL_MIN_ALPHA
    #undef RSL_MID_ALPHA
    #undef RSL_MAX_ALPHA
}


} // namespace
} // namespace

#endif
