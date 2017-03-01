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

#include <Pt/Gfx/Algorithm.h>
#include <Pt/Gfx/Math.h>

#include <Pt/Gfx/ArcMode.h>
#include <Pt/Gfx/AntiAliasingMode.h>
#include <Pt/Gfx/Painter.h>

#include "ClipShape.h"

// ======================================================================================
// ===== Configurations and Macros ======================================================
// ======================================================================================

// Fixed-Point 16.16 Settings
#define FIXED_POINT_SHIFT_FACTOR     16         // Shift factor
#define FIXED_POINT_FRACT_BITMASK    0x0000FFFF // Bit mask for the fractional value; must be (2 ^ FIXED_POINT_SHIFT_FACTOR - 1)
#define FIXED_POINT_CONSTANT_ONE     65536      // The value 1.0  in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR    )
#define FIXED_POINT_CONSTANT_HALF    32768      // The value 0.5  in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR / 2)
#define FIXED_POINT_CONSTANT_QUARTER 16384      // The value 0.25 in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR / 4)

// Fixed-Point 16.16 Helper Macros
#define FIXED_POINT_IPART(V)        ( (V) & ~FIXED_POINT_FRACT_BITMASK )
#define FIXED_POINT_FPART(V)        ( (V) &  FIXED_POINT_FRACT_BITMASK )
#define FIXED_POINT_RFPART(V)       ( FIXED_POINT_FRACT_BITMASK - FIXED_POINT_FPART(V) )
#define FIXED_POINT_ROUND(V)        ( FIXED_POINT_IPART( (V) + FIXED_POINT_CONSTANT_HALF ) )
#define FIXED_POINT_FPART_TO_A8(V)  ( FIXED_POINT_FPART (V) >> 8 )
#define FIXED_POINT_RFPART_TO_A8(V) ( FIXED_POINT_RFPART(V) >> 8 )
#define FIXED_POINT_MUL_TO_A8(A, B) ( ( ( (Pt::uint32_t)(A) * (Pt::uint32_t)(B) + FIXED_POINT_FRACT_BITMASK ) >> FIXED_POINT_SHIFT_FACTOR ) )
#define FIXED_POINT_FROM_INT(V)     ( (V) << FIXED_POINT_SHIFT_FACTOR )
#define FIXED_POINT_TO_INT(V)       ( (V) >> FIXED_POINT_SHIFT_FACTOR )

// Coordinate limit
#define MAXIMUM_COORD Painter::MaximumCoordinate
#define MAXIMUM_POINT Painter::MaximumPointCoordinate

// Scale for the pattern buffer
#define PATTERN_BUFFER_SCALE_FACTOR 8

// Just for easy and faster debugging ;)
#include <stdio.h>
#define lprintf(...) fprintf (stderr, __VA_ARGS__)


namespace Pt {
namespace Gfx {


class DrawText2;
class Image;

class Rasterizer2
{
    public:
        // Mask for excluding pixels when drawing line; each element corresponds to
        // the coordinate of one of the pixel(s) of the start and end points
        typedef Point DrawLineMask[4];

        static const DrawLineMask NullLineMask;

        // Weighting filter for Xiaolin Wu's anti-aliasing algorithm
        static const Pt::uint8_t XWAA_WFILTER[256];

    public:
        Rasterizer2( Image& image );

        ~Rasterizer2();

        void setAntiAliasingMode(AntiAliasingMode mode)
        {
            _aaMode = mode;
            updatePenPattern();
        }

        const AntiAliasingMode& antiAliasingMode() const
        {
            return _aaMode;
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

        // Mask layout for store4Pixels/fill4Pixels function variants that take mask as the last argument:
        //     Mask element        : #0         #1         #2         #3
        //     Affected coordinate : (x1, y1)   (x1, y2)   (x2, y1)   (x2, y2)
        // In this case, "true" means the pixel will be drawn and "false" means it will not be drawn

        void stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2);
        void stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const bool mask[4]);
        void stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::uint8_t alpha);
        void stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::uint8_t alpha, const bool mask[4]);

        inline void fillPixel(Pt::int32_t x, Pt::int32_t y, Pt::int32_t minX, Pt::int32_t minY, Pt::uint8_t alpha);

        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY);
        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, const bool mask[4]);
        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, Pt::uint8_t alpha);
        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, Pt::uint8_t alpha, const bool mask[4]);

        void fillOneScanlineNoAA(Pt::int32_t from, Pt::int32_t to, Pt::int32_t pixelY, Pt::int32_t minX, Pt::int32_t minY);

        void strokeText(const Point& to, const Pt::String& text);
        void strokeOnePixelLine(const Point& a, const Point& b, DrawLineMask* maskInOut);
        void strokeOnePixelRect(const Point& tl, const Point& br);
        void strokeOnePixelPolygon(const Point* points, size_t pointCount, bool autoClose);
        void strokeOnePixelPolybezier(const Point* points, size_t pointCount);
        void strokeOnePixelEllipseArc(const Point& topLeft, const Size& size, float degBegin, float degEnd, const ArcMode& arcMode);

        void fillRect(const Point& tl, const Point& br);
        void fillPolygon(const Point* points, size_t pointCount);
        void fillPolygonSeparate(const Point* points, size_t pointCount);
        void fillEllipse(const Point& topLeft, const Size& size);
        void fillArc(const Point& topLeft, const Size& size, float degBegin, float degEnd, const ArcMode& arcMode);

    public:
        inline Pt::uint8_t patternBufferAlpha(Pt::int32_t idx);
        inline Pt::uint8_t patternBufferAlphaPolar(Pt::int32_t x, Pt::int32_t y, float scale);
        inline Pt::uint8_t patternBufferAlphaPolar(Pt::int32_t x, Pt::int32_t y, float scale, float xyRat);

        inline void patternBufferAlpha(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t idx, Pt::uint8_t alpha0, Pt::uint8_t alpha1);
        inline void patternBufferAlphaPolar(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t x, Pt::int32_t y, float scale, Pt::uint8_t alpha0, Pt::uint8_t alpha1);
        inline void patternBufferAlphaPolar(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t x, Pt::int32_t y, float scale, float xyRat, Pt::uint8_t alpha0, Pt::uint8_t alpha1);

    private:
        // Polygon scanlines (used for drawing filled polygons with XWAA)
        struct PolygonScanline16 {
            Pt::int16_t from, to;

            PolygonScanline16(Pt::int16_t from_, Pt::int16_t to_)
            : from(from_), to(to_)
            {}
        };

        typedef std::vector< std::vector<PolygonScanline16> > PolygonScanline16s; // The vector index is the Y coordinate

        // Arc scanlines (used for drawing filled ellipse and arcs)
        // Each key specify the Y coordinate of a scanline; while its element specify the "from" and "to" X coordinates
        struct EAScanlineElement {
            Pt::int32_t from;
            Pt::int32_t to;

            EAScanlineElement(Pt::int32_t from_ = -1, Pt::int32_t to_ = -1)
            : from(from_), to(to_)
            {}

            bool isNull() const
            { return from == -1 && to == -1; }
        };

        typedef std::vector<EAScanlineElement> EAScanlines;

        // Filled-arc information structure
        struct FilledArcInfo {
            bool        antiAlias;    // A flag that indicate if the arc will be anti-aliased

            float       degBegin;     // Begin angle
            float       degEnd;       // End angle

            Pt::int32_t minX, minY;   // Top-left coordinate of the arc
            Pt::int32_t ctrX, ctrY;   // Center coordinate of the arc
            Pt::int32_t radX, radY;   // Radius of the arc
            Pt::int32_t radX2, radY2; // Squared radius of the arc
            float       xyRat;        // Ratio of the X and Y radius

            Pt::int32_t x1, y1;       // Coordinate of the begin point
            Pt::int32_t x2, y2;       // Coordinate of the end point

            Pt::int32_t quartersX;    // The number of quarter points in the X direction
            Pt::int32_t quartersY;    // The number of quarter points in the Y direction
        };

        // Xiaolin Wu's anti-aliased line data structure (used for drawing filled arcs)
        struct ArcXWLineData {
            // Point data
            struct XWPoint {
                Pt::int32_t x;
                Pt::uint8_t a1, a2;

                XWPoint(Pt::int32_t x_ = -1, Pt::uint8_t a1_ = 0, Pt::uint8_t a2_ = 0)
                : x(x_), a1(a1_), a2(a2_)
                {}

                bool isNull() const
                { return x == -1 && a1 == 0 && a2 == 0; }
            };

            typedef std::vector< std::vector<XWPoint> > XWPoints; // The vector index is the Y coordinate


            XWPoints points; // The line's points
            bool     steep;  // If "true"  then the a2 belongs to (x + 1, y)
                             // If "false" then the a2 belongs to (x, y + 1)

            bool faceL;  // The direction that the line is facing to
            bool faceR;  // ---
            bool faceT;  // ---
            bool faceB;  // ---

            // The line's coordinates
            Pt::int32_t x1, y1, x2, y2;
            Pt::int32_t minY, maxY;

            bool insideYRange(Pt::int32_t y) const
            { return (y >= minY) && (y <= maxY); }
        };

    private:
        void rasterOnePixelSolidLine(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidHLineSegment(Pt::int32_t x1, Pt::int32_t x2, Pt::int32_t y, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidVLineSegment(Pt::int32_t x, Pt::int32_t y1, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidXLineSegment(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidGLineSegmentNoAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        void rasterFillOnePixelSolidGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, const PolygonScanline16s& exclusionZone, DrawLineMask& maskInOut);

        void rasterOnePixelPatternedLine(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);
        void rasterOnePixelPatternedXLineSegment(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, Pt::int32_t fpiCtrInc, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);
        void rasterOnePixelPatternedGLineSegmentNoAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, Pt::int32_t fpiCtrInc, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);
        void rasterOnePixelPatternedGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, Pt::int32_t fpiCtrInc, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);

        void rasterRectArea(const Point& tl, const Point& br);

        void rasterOnePixelPolygonOutline(const Point* points, size_t pointCount, const Color& color, bool autoClose);
        void rasterPolygonAreaNoAA(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY);
        void rasterPolygonAreaFSAA2x2(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY);
        void rasterPolygonAreaXWAA(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY);

        void rasterOnePixelSolidBezierCurve(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t x3, Pt::int32_t y3, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelPatternedBezierCurve(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t x3, Pt::int32_t y3, const Color& color, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);

        void rasterEllipseAreaNoAA(const Point& topLeft, const Size& size);
        void rasterArcAreaChord(FilledArcInfo& fai);
        void rasterArcAreaPie(FilledArcInfo& fai);

    private:
        // Common helper functions
        void updatePenPattern();
        void updateGradientBrush(Pt::int32_t width, Pt::int32_t height);
        void updateClip();

        template<typename T>
        static inline void bubbleSortAscending(T& basket, Pt::int32_t size);

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

        // Polygon-related helper functions
        template<Pt::uint8_t SUPERSAMPLE_SIZE>
        void rasterPolygonAreaFSAAGen(
            const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount,
            const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY
        );

        void genClippedPolygonPoints(std::vector<Point>& dst, const Point* src, const size_t pointCount) const;
        void getPolygonRectMinMax(const Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY);

        // Arc-related helper functions
        static inline bool arcUtil_pointIsInsideDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBegin, float degEnd, float xyRatio);
        static inline void arcUtil_detXWLineDirection(ArcXWLineData& xwLineData);

        static void arcUtil_findExactBegEndPointsCoordinate(FilledArcInfo& fai);
        static void arcUtil_runXWLineAlgorithm(ArcXWLineData& xwLine, const FilledArcInfo& fai, Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2);
        static void arcUtil_genScanlinesForChord(EAScanlines& scanlines, const FilledArcInfo& fai, const ArcXWLineData& xwLine);
        static void arcUtil_cropAndStoreScanlineForChord(EAScanlines& scanlines, const FilledArcInfo& fai, const ArcXWLineData& xwLine, Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t y);
        static void arcUtil_genScanlinesForPie(EAScanlines& scanlines1, EAScanlines& scanlines2, const FilledArcInfo& fai, const ArcXWLineData& xwLine1, const ArcXWLineData& xwLine2);
        static void arcUtil_cropAndStoreScanlineForPie(EAScanlines& scanlines1, EAScanlines& scanlines2, const FilledArcInfo& fai, const ArcXWLineData& xwLine1, const ArcXWLineData& xwLine2, Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t y);

        void arcUtil_rasterCircumferencePixels(FilledArcInfo& fai);
        void arcUtil_rasterClosingXWLine(const FilledArcInfo& fai, const ArcXWLineData& xwLine, Point maskInOut[4]);

    private:
        AntiAliasingMode _aaMode;

        Image*           _image;
        DrawText2*       _text;
        Font             _font;
        CompositionMode  _compositionMode;

        Pen              _pen;
        Image            _penBuffer;
        ConstPixel       _penPixel;
        Pt::uint8_t      _patternBuffer[256];
        Pt::int32_t      _fpatternMaxCtr; // In fixed-point

        Brush            _brush;
        Image            _brushBuffer;
        ConstPixel       _brushPixel;
        const Image*     _brushImage;
        bool             _isGradient;
        bool             _isTexture;

        Rect             _clip;
        Rect             _currentClip;
};


// ======================================================================================
// ===== Inlined and/or Templated Public Member Functions ===============================
// ======================================================================================

Pt::uint8_t Rasterizer2::patternBufferAlpha(Pt::int32_t idx)
{ return _patternBuffer[ idx % FIXED_POINT_TO_INT(_fpatternMaxCtr) ]; }

Pt::uint8_t Rasterizer2::patternBufferAlphaPolar(Pt::int32_t x, Pt::int32_t y, float scale)
{ return patternBufferAlpha(Gfx::Math::convertCartesianToPolarCoordinate(x, y) * scale); }

Pt::uint8_t Rasterizer2::patternBufferAlphaPolar(Pt::int32_t x, Pt::int32_t y, float scale, float xyRat)
{
    const float angle = Gfx::Math::convertCartesianToPolarCoordinate(x, y);

    if(xyRat >= 1.0 && angle >= 45) scale /= xyRat;
    if(xyRat <  1.0 && angle <  45) scale *= xyRat;

    return patternBufferAlpha(angle * scale);
}

void Rasterizer2::patternBufferAlpha(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t idx, Pt::uint8_t alpha0, Pt::uint8_t alpha1)
{
    a0 = (Pt::uint32_t) _patternBuffer[ idx % FIXED_POINT_TO_INT(_fpatternMaxCtr) ] * alpha0 / 255;
    a1 = (Pt::uint32_t) _patternBuffer[ idx % FIXED_POINT_TO_INT(_fpatternMaxCtr) ] * alpha1 / 255;
}

void Rasterizer2::patternBufferAlphaPolar(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t x, Pt::int32_t y, float scale, Pt::uint8_t alpha0, Pt::uint8_t alpha1)
{ patternBufferAlpha(a0, a1, Gfx::Math::convertCartesianToPolarCoordinate(x, y) * scale, alpha0, alpha1); }

void Rasterizer2::patternBufferAlphaPolar(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t x, Pt::int32_t y, float scale, float xyRat, Pt::uint8_t alpha0, Pt::uint8_t alpha1)
{
    const float angle = Gfx::Math::convertCartesianToPolarCoordinate(x, y);

    if(xyRat >= 1.0 && angle >= 45) scale /= xyRat;
    if(xyRat <  1.0 && angle <  45) scale *= xyRat;

    patternBufferAlpha(a0, a1, angle * scale, alpha0, alpha1);
}

void Rasterizer2::fillPixel(Pt::int32_t x, Pt::int32_t y, Pt::int32_t minX, Pt::int32_t minY, Pt::uint8_t alpha)
{
    // Check the clipping
    if(!ClipShape::insideXRange(x, _currentClip)) return;
    if(!ClipShape::insideYRange(y, _currentClip)) return;

    // Draw the pixels using texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        ConstPixel srcPixel(_brushImage->view(), (x - minX) % bw, (y - minY) % bh);
        Pixel      dstPixel(_image->view(), x, y);
        _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
    }

    // Draw the pixels using solid color
    else {
        Pixel pixel(_image->view(), x, y);
        _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
    }
}


// ======================================================================================
// ===== Inlined and/or Templated Private Member Functions ==============================
// ======================================================================================

template<typename T>
void Rasterizer2::bubbleSortAscending(T& basket, Pt::int32_t size)
{
    for(Pt::int32_t i = 0; i < size - 1;) {
        if(basket[i] > basket[i + 1]) {
            std::swap(basket[i], basket[i + 1]);
            if(i) --i;
        }
        else {
            ++i;
        }
    }
}

template<Pt::uint8_t SUPERSAMPLE_SIZE, Pt::uint8_t MIN_ALPHA, Pt::uint8_t MUL_ALPHA>
void Rasterizer2::rasterScanline(
    Pt::int32_t  iterL, Pt::int32_t iterR, Pt::int32_t pixelY,
    Pt::int32_t  minX,  Pt::int32_t minY,  Pt::int32_t sizeX,
    const Color& color, const std::vector<Pt::uint8_t>& alphas
)
{
    // A helper macro to scale the alpha
    #define RSL_SCALE_ALPHA(A) ( Pt::uint16_t(A) * MUL_ALPHA / SUPERSAMPLE_SIZE / SUPERSAMPLE_SIZE )

    // The maximum value for alpha
    #define RSL_MAX_ALPHA (MIN_ALPHA * SUPERSAMPLE_SIZE * SUPERSAMPLE_SIZE)

    //
    // Draw pixels that belongs to the left-part of the span to the image
    //
    if(iterL < 0) iterL = 0;

    // Skip fully-transparent pixels
    // Use Duff's device for:
    //     for(; iterL < sizeX; ++iterL) {
    //         if(alphas[iterL]) break;
    //     }
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
    // Use Duff's device for:
    //     for(; iterR >= 0; --iterR) {
    //         if(alphas[iterR]) break;
    //     }
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
        Pixel pixel(_image->view(), minX + iterL, minY + pixelY);
        _image->format().setPixels(pixel, _brush.color(), iterR - iterL + 1, _compositionMode);
        //Pt::int32_t iterX     = minX + iterL;
        //Pt::int32_t spanWidth = iterR - iterL + 1;
        //while(spanWidth > 0) {
        //    const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
        //    if(n) {
        //        Pixel pixel(_image->view(), iterX, minY + pixelY);
        //        _image->format().copy(pixel, _brushPixel, n, _compositionMode);
        //    }
        //    spanWidth -= n;
        //    iterX     += n;
        //}
    }

    // Undefine the macros
    #undef RSL_SCALE_ALPHA
    #undef RSL_MAX_ALPHA
}

// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
template<Pt::uint8_t SUPERSAMPLE_SIZE>
void Rasterizer2::rasterPolygonAreaFSAAGen(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // Internal macros
    #define FSAA_MUL_ALPHA 255
    #define FSAA_MIN_ALPHA 1
    #define FSAA_MAX_ALPHA (FSAA_MIN_ALPHA * SUPERSAMPLE_SIZE * SUPERSAMPLE_SIZE)
    #define FSAA_MID_ALPHA (FSAA_MIN_ALPHA * SUPERSAMPLE_SIZE)

    // Calculate the size of the polygon
    Pt::int32_t sizeX = (maxX - minX + 1);
    Pt::int32_t sizeY = (maxY - minY + 1);

    // Prepare a work buffer
    std::vector<Pt::uint8_t> alphas(sizeX, 0);

    // Scale the polygon to be twice as large and translate its origin to (0, 0)
    std::vector<Pt::int32_t> pointX(totalPointCount, 0);
    std::vector<Pt::int32_t> pointY(totalPointCount, 0);

    for(size_t i = 0; i < totalPointCount; ++i) {
        pointX[i] = (points[i].x() - minX) * SUPERSAMPLE_SIZE;
        pointY[i] = (points[i].y() - minY) * SUPERSAMPLE_SIZE;
    }

    // List of nodes that define the horizontal spans
    // Row (Y) ... Row (Y + SUPERSAMPLE_SIZE - 1)
    std::vector<Pt::int32_t> nodeX[SUPERSAMPLE_SIZE];
    for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
        nodeX[s].resize(totalPointCount * 2);
    }

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = 0; pixelY <= sizeY; ++pixelY) {
        // We examine multiple rows at a time
        Pt::int32_t iterY[SUPERSAMPLE_SIZE];
        iterY[0] = pixelY * SUPERSAMPLE_SIZE;
        for(Pt::int32_t s = 1; s < SUPERSAMPLE_SIZE; ++s) {
            iterY[s] = iterY[0] + s;
        }
        // Base pointers for the polygons
        const Pt::int32_t* curPointBaseX = pointX.data();
        const Pt::int32_t* curPointBaseY = pointY.data();
        // Build a list of nodes using all the polygons
        Pt::int32_t nodes[SUPERSAMPLE_SIZE] = { 0 };
        for(size_t p = 0; p < polyCount; ++p) {
            // Get the current point count
            const size_t curPointCount = pointCount[p];
            // Loop through the points
            Pt::int32_t j = curPointCount - 1;
            for(size_t i = 0; i < curPointCount; ++i) {
                // Get the coordinates
                const Pt::int32_t curXi = *(curPointBaseX + i);
                const Pt::int32_t curYi = *(curPointBaseY + i);
                const Pt::int32_t curXj = *(curPointBaseX + j);
                const Pt::int32_t curYj = *(curPointBaseY + j);
                // Row (Y) ... Row (Y + SUPERSAMPLE_SIZE - 1)
                for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
                    if( ( iterY[s] >= curYi && iterY[s] < curYj ) || ( iterY[s] >= curYj && iterY[s] < curYi ) ) {
                        // Bail out if we have produced too many nodes
                        if((size_t) nodes[s] >= nodeX[s].size()) return;
                        // Calculate the nodes' coordinates
                        const Pt::int32_t deltaYp = iterY[s] - curYi;
                        const Pt::int32_t deltaYj = curYj    - curYi;
                        const Pt::int32_t deltaXj = curXj    - curXi;
                        const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi)
                                                  + ( (FIXED_POINT_FROM_INT(deltaYp) + FIXED_POINT_CONSTANT_QUARTER) /
                                                      deltaYj * deltaXj
                                                    );
                        nodeX[s][nodes[s]++] = FIXED_POINT_TO_INT(interXf);
                    }
                }
                // Update the searching index
                j = i;
            }
            // Increment the base pointers
            curPointBaseX += curPointCount;
            curPointBaseY += curPointCount;
        }
        // Skip if there is no node
        bool gotNodes = false;
        for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
            if(nodes[s]) {
                gotNodes = true;
                break;
            }
        }
        if(!gotNodes) continue;
        // Sort the nodes using bubble sort
        for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
            bubbleSortAscending(nodeX[s], nodes[s]);
        }
        // Reset the alphas
        memset(&alphas[0], 0, alphas.size());
#if 1
        // Accumulate the alphas of the samples between the node pairs
        for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
            for(Pt::int32_t i = 0; i < nodes[0]; i += 2) {

                // Calculate the cells and coverage areas
                const Pt::int32_t from      = nodeX[s][i    ];
                const Pt::int32_t to        = nodeX[s][i + 1];
                const Pt::int32_t from_cell = from / SUPERSAMPLE_SIZE;
                const Pt::int32_t to_cell   = to   / SUPERSAMPLE_SIZE;
                const Pt::int32_t from_area = SUPERSAMPLE_SIZE - ( from - from_cell * SUPERSAMPLE_SIZE );
                const Pt::int32_t to_area   = ( to - to_cell * SUPERSAMPLE_SIZE ) + 1;
                // If the span is short, accumulate alphas for the whole span directly
                // in order to avoid some alpha-related artifacts
                if( to - from <= SUPERSAMPLE_SIZE ) {
                    for(Pt::int32_t k = from; k <= to; ++k) {
                        alphas[k / SUPERSAMPLE_SIZE] += FSAA_MIN_ALPHA;
                    }
                    continue;
                }
                // Accumulate alphas for the left side and right side of the span
                alphas[from_cell] += from_area;
                alphas[to_cell  ] += to_area;
                // Assign alphas for the middle side of the span
                for(Pt::int32_t k = (from_cell + 1); k <= (to_cell - 1); ++k) {
                    alphas[k] += FSAA_MID_ALPHA;
                }
            }
        }
#else
        // Accumulate the alphas of the samples between the node pairs
        // --- Check if all the rows have the same number of nodes ---
        const Pt::int32_t nodes0            = nodes[0];
              bool        hasSameNumOfNodes = true;
        for(Pt::int32_t s = 1; s < SUPERSAMPLE_SIZE; ++s) {
            if(nodes[s] == nodes0) continue;
            hasSameNumOfNodes = false;
            break;
        }
        // --- The number of nodes within all the rows are equal ---
        if(hasSameNumOfNodes) {
            for(Pt::int32_t i = 0; i < nodes0; i += 2) {
                // Get the coordinates
                Pt::int32_t from[SUPERSAMPLE_SIZE];
                Pt::int32_t to  [SUPERSAMPLE_SIZE];
                for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
                    from[s] = nodeX[s][i    ];
                    to  [s] = nodeX[s][i + 1];
                }
                // Sort the coordinates
                bubbleSortAscending(from, SUPERSAMPLE_SIZE);
                bubbleSortAscending(to,   SUPERSAMPLE_SIZE);
                // Calculate the cells
                Pt::int32_t from_cell[SUPERSAMPLE_SIZE];
                Pt::int32_t to_cell  [SUPERSAMPLE_SIZE];
                bool        shortSpan = false;
                for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
                    from_cell[s] = from[s] / SUPERSAMPLE_SIZE;
                    to_cell  [s] = to  [s] / SUPERSAMPLE_SIZE;
                    if(abs(to_cell[s] - from_cell[s]) <= SUPERSAMPLE_SIZE) shortSpan = true;
                }
                // If the span is short, accumulate alphas for the whole span directly
                // in order to avoid some alpha-related artifacts
                if(shortSpan) {
                    for(Pt::int32_t n = 0; n < SUPERSAMPLE_SIZE; ++n) {
                        for(Pt::int32_t k = from[n]; k <= to[n]; ++k) {
                            alphas[k / SUPERSAMPLE_SIZE] += FSAA_MIN_ALPHA;
                        }
                    }
                    continue;
                }
                // Accumulate alphas for the left side of the span
                // --- Each distinct cells ---
                std::set<Pt::int32_t> proc_ds;
                for(Pt::int32_t n = 0; n < SUPERSAMPLE_SIZE; ++n) {
                    // Alpha contribution from this cell
                    alphas[ from_cell[n] ] += SUPERSAMPLE_SIZE - ( from[n] - from_cell[n] * SUPERSAMPLE_SIZE );
                    // Alpha Contribution from cells of the left side of this cell
                    for(Pt::int32_t b = n; b > 0; --b) {
                        // Ensure that each cell is not processed more than once
                        if(proc_ds.find(from_cell[b]) != proc_ds.end()) continue;
                        proc_ds.insert(from_cell[b]);
                        // Accumulate the alphas
                        for(Pt::int32_t k = 0; k < b; ++k) {
                            // Accumulate the alpha if the cell is on the left side of the reference cell
                            if(from_cell[k] < from_cell[b]) alphas[ from_cell[b] ] += SUPERSAMPLE_SIZE;
                        }
                    }
                }
                // --- In-between cells ---
                std::set<Pt::int32_t> proc_is;
                for(Pt::int32_t n = (SUPERSAMPLE_SIZE - 1); n > 0; --n) {
                    // Ensure that each reference cell is not processed more than once
                    if(proc_is.find(from_cell[n]) != proc_is.end()) continue;
                    proc_is.insert(from_cell[n]);
                    // Walk through the cells on the left side of the span up until the reference cell
                    for(Pt::int32_t b = 0; b < n; ++b) {
                        // Skip if this cell is not really on the left side of the reference cell
                        if(from_cell[b] >= from_cell[n]) continue;
                        // Walk through the in-between cells
                        for(Pt::int32_t k = (from_cell[b] + 1); k < from_cell[n]; ++k) {
                            // Accumulate the alpha only if the cell is not one of the reference cell
                            if(proc_ds.find(k) == proc_ds.end()) alphas[k] += SUPERSAMPLE_SIZE;
                        }
                    }
                }
                // Accumulate alphas for the right side of the span
                // --- Each distinct cells ---
                proc_ds.clear();
                for(Pt::int32_t n = 0; n < SUPERSAMPLE_SIZE; ++n) {
                    // Alpha contribution from this cell
                    alphas[ to_cell[n] ] += ( to[n] - to_cell[n] * SUPERSAMPLE_SIZE ) + 1;
                    // Alpha Contribution from cells of the right side of this cell
                    for(Pt::int32_t b = n; b < (SUPERSAMPLE_SIZE - 1); ++b) {
                        // Ensure that each cell is not processed more than once
                        if(proc_ds.find(to_cell[b]) != proc_ds.end()) continue;
                        proc_ds.insert(to_cell[b]);
                        // Accumulate the alphas
                        for(Pt::int32_t k = (b + 1); k < SUPERSAMPLE_SIZE; ++k) {
                            // Accumulate the alpha if the cell is on the right side of the reference cell
                            if(to_cell[k] > to_cell[b]) alphas[ to_cell[b] ] += SUPERSAMPLE_SIZE;
                        }
                    }
                }
                // --- In-between cells ---
                proc_is.clear();
                for(Pt::int32_t n = 0; n < (SUPERSAMPLE_SIZE - 1); ++n) {
                    // Ensure that each reference cell is not processed more than once
                    if(proc_is.find(to_cell[n]) != proc_is.end()) continue;
                    proc_is.insert(to_cell[n]);
                    // Walk through the cells on the right side of the span down until the reference cell
                    for(Pt::int32_t b = (SUPERSAMPLE_SIZE - 1); b > n; --b) {
                        // Skip if this cell is not really on the right side of the reference cell
                        if(to_cell[b] <= to_cell[n]) continue;
                        // Walk through the in-between cells
                        for(Pt::int32_t k = (to_cell[b] - 1); k > to_cell[n]; --k) {
                            // Accumulate the alpha only if the cell is not one of the reference cell
                            if(proc_ds.find(k) == proc_ds.end()) alphas[k] += SUPERSAMPLE_SIZE;
                        }
                    }
                }
                // Assign alphas for the middle side of the span
                const Pt::int32_t msMin = (from_cell[SUPERSAMPLE_SIZE - 1] + 1);
                const Pt::int32_t msMax = (to_cell  [0                   ] - 1);
                const Pt::int32_t msLen = msMax - msMin + 1;
                if(msLen > 0) memset(&alphas[msMin], FSAA_MAX_ALPHA, msLen);
            }
        }
        // Accumulate the alphas of the samples between the node pairs
        // --- The number of nodes within all or some of the rows are not equal ---
        else {
            for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
                //for(Pt::int32_t i = 0; i < nodes[s]; i += 2) {
                //    const Pt::int32_t from = nodeX[s][i    ];
                //    const Pt::int32_t to   = nodeX[s][i + 1];
                //    for(Pt::int32_t k = from; k <= to; ++k) {
                //        alphas[k / SUPERSAMPLE_SIZE] += FSAA_MIN_ALPHA;
                //    }
                //}
                for(Pt::int32_t i = 0; i < nodes0; i += 2) {

                    // Calculate the cells and coverage areas
                    const Pt::int32_t from      = nodeX[s][i    ];
                    const Pt::int32_t to        = nodeX[s][i + 1];
                    const Pt::int32_t from_cell = from / SUPERSAMPLE_SIZE;
                    const Pt::int32_t to_cell   = to   / SUPERSAMPLE_SIZE;
                    const Pt::int32_t from_area = SUPERSAMPLE_SIZE - ( from - from_cell * SUPERSAMPLE_SIZE );
                    const Pt::int32_t to_area   = ( to - to_cell * SUPERSAMPLE_SIZE ) + 1;
                    // If the span is short, accumulate alphas for the whole span directly
                    // in order to avoid some alpha-related artifacts
                    if( to - from <= SUPERSAMPLE_SIZE ) {
                        for(Pt::int32_t k = from; k <= to; ++k) {
                            alphas[k / SUPERSAMPLE_SIZE] += FSAA_MIN_ALPHA;
                        }
                        continue;
                    }
                    // Accumulate alphas for the left side and right side of the span
                    alphas[from_cell] += from_area;
                    alphas[to_cell  ] += to_area;
                    // Assign alphas for the middle side of the span
                    for(Pt::int32_t k = (from_cell + 1); k <= (to_cell - 1); ++k) {
                        alphas[k] += FSAA_MID_ALPHA;
                    }
                }
            }
        }
#endif
        //lprintf("%03d: ", pixelY); for(size_t k = 0; k < alphas.size(); ++k) lprintf("%02d ", alphas[k] / FSAA_MIN_ALPHA); lprintf("\n");
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes[0]; i += 2) {
            const Pt::int32_t iterL = nodeX[0][i    ] / SUPERSAMPLE_SIZE - 1;
            const Pt::int32_t iterR = nodeX[0][i + 1] / SUPERSAMPLE_SIZE + 1;
            rasterScanline<SUPERSAMPLE_SIZE, FSAA_MIN_ALPHA, FSAA_MUL_ALPHA>(
                iterL, iterR, pixelY, minX, minY, sizeX, color, alphas
            );
        }
    }

    // Undefine the macros
    #undef FSAA_MUL_ALPHA
    #undef FSAA_MIN_ALPHA
    #undef FSAA_MAX_ALPHA
    #undef FSAA_MID_ALPHA
}

inline bool Rasterizer2::arcUtil_pointIsInsideDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBegin, float degEnd, float xyRatio)
{
    // IMPORTANT NOTES:
    //     * The Y coordinate goes from low to high according to the coordinate system being used:
    //           - cartesian coordinate system: from the horizontal axis (the X axis) to the top;
    //           - computer  coordinate system: from the top of the screen to the bottom of the screen;
    //       This will cause sign inversion for trigonometry-based calculations in the Y coordinate.
    //     * The movement from begin angle to end angle must be in counter-clockwise (CCW), otherwise
    //       something wrong will be drawn.

    const float angle = Gfx::Math::convertCartesianToPolarCoordinate( (x - ctrX), -(y - ctrY) * xyRatio);

    if(degBegin < 0 && degEnd < 0) {
        return angle >= (degBegin + 360) && angle <= (degEnd + 360);
    }

    if(degBegin < 0 && degEnd >= 0) {
        if( angle >= (degBegin + 360) && angle <= 360   ) return true;
        if( angle >= 0                && angle <= degEnd) return true;
        return false;
    }

    return angle >= degBegin && angle <= degEnd;
}

inline void Rasterizer2::arcUtil_detXWLineDirection(ArcXWLineData& xwLineData)
{
    // Calculate the direction vector
    const Pt::int32_t vx = xwLineData.x2 - xwLineData.x1; // Vector from the begin point to the end point
    const Pt::int32_t vy = xwLineData.y2 - xwLineData.y1; // ---
    const Pt::int32_t vz = 0;                             // ---
    const Pt::int32_t rx = 0;                             // Vector from the point of origin (0, 0, 0) that points out of the monitor
    const Pt::int32_t ry = 0;                             // ---
    const Pt::int32_t rz = 1;                             // ---
    const Pt::int32_t cx = vy * rz - vz * ry;             // Cross product of the above two vectors
    const Pt::int32_t cy = vz * rx - vx * rz;             // ---
  //const Pt::int32_t cz = vx * ry - vy * rx;             // ---

    // Determine the direction that the line is facing to
    xwLineData.faceT = cy < 0;
    xwLineData.faceB = cy > 0;
    xwLineData.faceL = cx < 0;
    xwLineData.faceR = cx > 0;
}


} // namespace
} // namespace

#endif
