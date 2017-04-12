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
#define FIXED_POINT_CONSTANT_ONE     65536      // The value 1.0       in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR           )
#define FIXED_POINT_CONSTANT_HALF    32768      // The value 0.5       in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR / 2       )
#define FIXED_POINT_CONSTANT_QUARTER 16384      // The value 0.25      in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR / 4       )
#define FIXED_POINT_CONSTANT_ISQRT2  46341      // The value 1/sqrt(2) in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR / sqrt(2) )
#define FIXED_POINT_CONSTANT_SQRT2   92682      // The value sqrt(2)   in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR * sqrt(2) )

// Fixed-Point 16.16 Helper Macros
#define FIXED_POINT_IPART(V)        ( (V) & ~FIXED_POINT_FRACT_BITMASK )
#define FIXED_POINT_FPART(V)        ( (V) & FIXED_POINT_FRACT_BITMASK )
#define FIXED_POINT_RFPART(V)       ( FIXED_POINT_FRACT_BITMASK - FIXED_POINT_FPART(V) )
#define FIXED_POINT_ROUND(V)        ( FIXED_POINT_IPART( (V) + FIXED_POINT_CONSTANT_HALF ) )
#define FIXED_POINT_FLOOR(V)        ( (V) & ~FIXED_POINT_FRACT_BITMASK )
#define FIXED_POINT_CEIL(V)         ( ((V) | FIXED_POINT_FRACT_BITMASK) + 1)
#define FIXED_POINT_FPART_TO_A8(V)  ( FIXED_POINT_FPART (V) >> 8 )
#define FIXED_POINT_RFPART_TO_A8(V) ( FIXED_POINT_RFPART(V) >> 8 )
#define FIXED_POINT_MUL_TO_A8(A, B) ( ( ( (Pt::uint32_t)(A) * (Pt::uint32_t)(B) + FIXED_POINT_FRACT_BITMASK ) >> FIXED_POINT_SHIFT_FACTOR ) )
#define FIXED_POINT_FROM_FLT(V)     ( (V) * ( (float) FIXED_POINT_CONSTANT_ONE ) )
#define FIXED_POINT_FROM_INT(V)     ( (V) << FIXED_POINT_SHIFT_FACTOR )
#define FIXED_POINT_TO_INT(V)       ( (V) >> FIXED_POINT_SHIFT_FACTOR )

// Coordinate limit
#define MAXIMUM_COORD Painter::MaximumCoordinate
#define MAXIMUM_POINT Painter::MaximumPointCoordinate

#define MAXIMUM_COORD_F (float) Painter::MaximumCoordinate

// Scaling factor and starting value for the pattern buffer
#define PATTERN_BUFFER_NUM_OF_CELLS  64
#define PATTERN_BUFFER_SCALE_FACTOR  4
#define PATTERN_BUFFER_COUNTER_START 0

#define PATTERN_BUFFER_COUNTER_MAX1P FIXED_POINT_FROM_INT(PATTERN_BUFFER_NUM_OF_CELLS * PATTERN_BUFFER_SCALE_FACTOR)
#define PATTERN_BUFFER_COUNTER_MAXMP PATTERN_BUFFER_NUM_OF_CELLS

// Just for debugging ;)
//#warning "Just for debugging ;)"
//#include <stdio.h>
//#define lprintf(...) fprintf (stderr, __VA_ARGS__)


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
        { return _aaMode; }

        void setImage(Image& image);

        const ImageFormat& format() const;

        void setPen( const Pen& pen );

        const Pen& pen() const
        { return _pen; }

        void setBrush( const Brush& brush );

        const Brush& brush() const
        { return _brush; }

        void setFont( const Font& font );

        const Font& font() const
        { return _font; }

        FontMetrics fontMetrics( const String& text ) const;

        static FontMetrics fontMetrics( const Font& font, const Pt::String& text );

        void setClip( const Rect& clip );

        const Rect& clip() const
        { return _clip; }

        void setCompositionMode(const CompositionMode& mode)
        { _compositionMode = mode; }

        const CompositionMode& compositionMode() const
        { return _compositionMode; }

        void blitImage(const Point& to, const Image& image);
        void blitImage(const Point& to, const Image& image, const Rect& imageRect);

        void strokeText(const Point& to, const Pt::String& text);
        void strokeOnePixelLine(const Point& a, const Point& b, DrawLineMask* maskInOut);
        void strokeOnePixelRect(const Point& tl, const Point& br);
        void strokeOnePixelPolygonOutline(const Point* points, size_t pointCount, bool autoClose);
        void strokeOnePixelPolygonOutline(const PointF* points, size_t pointCount, bool autoClose);
        void strokeOnePixelQuadraticPolybezierOutline(const Point* points, size_t pointCount);
        void strokeOnePixelEllipseArc(const Point& topLeft, const Size& size, float degBegin, float degEnd, const ArcMode& arcMode);

        void penFillPolygon(const Point* points, size_t pointCount);
        void penFillPolygon(const PointF* points, size_t pointCount);
        void penFillPolygonSeparate(const Point* points, size_t pointCount);
        void penFillPolygonSeparate(const PointF* points, size_t pointCount);

        void fillRect(const Point& tl, const Point& br);
        void fillPolygon(const Point* points, size_t pointCount);
        void fillPolygon(const PointF* points, size_t pointCount);
        void fillEllipse(const Point& topLeft, const Size& size);
        void fillArc(const Point& topLeft, const Size& size, float degBegin, float degEnd, const ArcMode& arcMode);

    public:
        inline const Pt::uint8_t* patternBufferMP64() const;

    private:
        // Scanline element
        template <typename T>
        struct ScanlineElement;

        typedef ScanlineElement<Pt::int16_t> ScanlineElement16;
        typedef ScanlineElement<Pt::int32_t> ScanlineElement32;

        // Polygon scanlines (used for drawing filled polygons with XWAA)
        //     * The vector index specify the Y coordinate of the scanline
        //     * The vector element specify a set of "from" and "to" X coordinates
        typedef std::vector< std::vector<ScanlineElement16> > PolygonScanlines;

        // Ellipse & arc scanlines (used for drawing filled ellipse and arcs)
        //    * The vector index specify the Y coordinate of the scanline
        //    * The vector element specify the "from" and "to" X coordinates
        typedef std::vector<ScanlineElement32> EAScanlines;

        // Filled-arc information structure (used for drawing filled arcs)
        struct FilledArcInfo;

        // Xiaolin Wu's anti-aliased line data structure (used for drawing filled arcs)
        struct ArcXWLineData;

    private:
        void rasterOnePixelSolidLine(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidHLineSegment(Pt::int32_t x1, Pt::int32_t x2, Pt::int32_t y, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidVLineSegment(Pt::int32_t x, Pt::int32_t y1, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidXLineSegment(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidGLineSegmentNoAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidGLineSegmentXWAA_F(float x1, float y1, float x2, float y2, const Color& color, DrawLineMask* maskInOut);

        void rasterOnePixelAreaGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, Pt::int32_t minX, Pt::int32_t minY, const PolygonScanlines& exclusionZone, DrawLineMask& maskInOut);
        void rasterOnePixelAreaGLineSegmentXWAA_F(float x1, float y1, float x2, float y2, const Color& color, Pt::int32_t minX, Pt::int32_t minY, const PolygonScanlines& exclusionZone, DrawLineMask& maskInOut);

        void rasterOnePixelPatternedLine(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);
        void rasterOnePixelPatternedXLineSegment(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, Pt::int32_t fpiCtrInc, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);
        void rasterOnePixelPatternedGLineSegmentNoAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, Pt::int32_t fpiCtrInc, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);
        void rasterOnePixelPatternedGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, Pt::int32_t fpiCtrInc, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);
        void rasterOnePixelPatternedGLineSegmentXWAA_F(float x1, float y1, float x2, float y2, const Color& color, Pt::int32_t fpiCtrInc, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);

        inline void rasterOnePixelSolidLine_F(float x1, float y1, float x2, float y2, const Color& color, DrawLineMask* maskInOut);
        inline void rasterOnePixelPatternedLine_F(float x1, float y1, float x2, float y2, const Color& color, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);

        void rasterRectArea(const Point& tl, const Point& br);

        void rasterOnePixelPolygonOutline(const Point* points, size_t pointCount, const Color& color);
        void rasterOnePixelPolygonOutline(const PointF* points, size_t pointCount, const Color& color);

        void rasterPolygonAreaNoAA(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY);
        void rasterPolygonAreaFSAA2x2(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY);
        void rasterPolygonAreaXWAA(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY);
        void rasterPolygonAreaXWAA(const PointF* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, float minX, float minY, float maxX, float maxY);

        void rasterOnePixelQuadraticBezierCurve(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t x3, Pt::int32_t y3, const Color& color, Pt::int32_t* fpiCtrInOut, DrawLineMask* maskInOut);

        void rasterEllipseAreaNoAA(const Point& topLeft, const Size& size);
        void rasterArcAreaChord(FilledArcInfo& fai);
        void rasterArcAreaPie(FilledArcInfo& fai);

    private:
        // --- Generic helper functions ---
        void updateClip();

        void updatePenPattern();

        void updateGradientBrush(Pt::int32_t width, Pt::int32_t height);
        void updateGradientBrush_gen1DHorVerGradient(Pt::int32_t width, Pt::int32_t height);
        void updateGradientBrush_gen2DLinearGradient(Pt::int32_t width, Pt::int32_t height);
        void updateGradientBrush_gen2DRectangularGradient(Pt::int32_t width, Pt::int32_t height);
        void updateGradientBrush_gen2DRadialGradient(Pt::int32_t width, Pt::int32_t height);
        void updateGradientBrush_gen2DConicalGradient(Pt::int32_t width, Pt::int32_t height);

        inline void updateGradientBrush_getStartEndColors(Pt::uint8_t rgbaStart[4], Pt::uint8_t rgbaEnd[4]);
        inline void updateGradientBrush_getCtrRatXY(float& ctrX, float& ctrY, float &xyRat, float& yxRat, Pt::int32_t width, Pt::int32_t height);

        inline Pt::uint8_t patternBuffer1PAlpha(Pt::int32_t idx) const;
        inline Pt::uint8_t patternBuffer1PAlphaPolar(Pt::int32_t x, Pt::int32_t y, float scale) const;
        inline Pt::uint8_t patternBuffer1PAlphaPolar(Pt::int32_t x, Pt::int32_t y, float scale, float xyRat) const;

        inline void patternBuffer1PAlpha(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t idx, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const;
        inline void patternBuffer1PAlphaPolar(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t x, Pt::int32_t y, float scale, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const;
        inline void patternBuffer1PAlphaPolar(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t x, Pt::int32_t y, float scale, float xyRat, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const;

        template<typename T>
        static inline void bubbleSortAscending(T& basket, Pt::int32_t size);

        // --- Rasterization-related helper functions ---

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
        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, const Pt::uint8_t alphaMask[4]);

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

        void rasterScanlineWithClipping(Pt::int32_t from, Pt::int32_t to, Pt::int32_t pixelY, Pt::int32_t minX, Pt::int32_t minY);

        // --- Polygon-related helper functions ---
        void getPolygonRectMinMax(const Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY) const;
        void getPolygonRectMinMax(const PointF* points, size_t pointCount, float& minX, float& minY, float& maxX, float& maxY) const;
        void genClippedPolygonPoints(std::vector<Point>& dst, const Point* src, const size_t pointCount, bool forPolygonOutline) const;
        void genClippedPolygonPoints(std::vector<PointF>& dst, const PointF* src, const size_t pointCount, bool forPolygonOutline) const;
        void separateAndClipPolygons(Pt::int32_t& minX, Pt::int32_t& maxX, Pt::int32_t& minY, Pt::int32_t& maxY, std::vector<Point>& clippedPoints, std::vector<size_t>& clippedCounts, const Point* points, size_t pointCount) const;
        void separateAndClipPolygons(float& minX, float& maxX, float& minY, float& maxY, std::vector<PointF>& clippedPoints, std::vector<size_t>& clippedCounts, const PointF* points, size_t pointCount) const;

        // Arc-related helper functions
        static inline void arcUtil_detXWLineDirection(ArcXWLineData& xwLineData);

        static inline bool arcUtil_pointIsInsideDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBegin, float degEnd, float xyRatio);
        static inline Pt::uint8_t arcUtil_pointIsInsideDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, Pt::uint8_t alpha, float degBegin, float degEnd, float xyRatio);

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
        Pt::uint8_t      _patternBuffer1P[PATTERN_BUFFER_NUM_OF_CELLS * PATTERN_BUFFER_SCALE_FACTOR]; // Pattern buffer for one-pixel line
        Pt::uint8_t      _patternBufferMP[PATTERN_BUFFER_NUM_OF_CELLS];                               // Pattern buffer for thick line

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
// ===== Private Member Structure Definitions ===========================================
// ======================================================================================

// Scanline element
template <typename T>
struct Rasterizer2::ScanlineElement {
    T from;
    T to;

    ScanlineElement(T from_ = -1, T to_ = -1)
    : from(from_), to(to_)
    {}

    bool isNull() const
    { return from == -1 && to == -1; }
};

// Filled-arc information structure (used for drawing filled arcs)
struct Rasterizer2::FilledArcInfo {
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
struct Rasterizer2::ArcXWLineData {
    // --- Point data sub-structure ---
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

    // --- Data ---
    XWPoints points;  // The line's points
    bool     steep;   // If "true"  then the a2 belongs to (x + 1, y)
                      // If "false" then the a2 belongs to (x, y + 1)
    bool     swapDir; // A flag that indicates if the line direction is swapped

    bool faceL; // The direction that the line is facing to
    bool faceR; // ---
    bool faceT; // ---
    bool faceB; // ---

    // The line's coordinates
    Pt::int32_t x1, y1, x2, y2;
    Pt::int32_t minY, maxY;

    bool insideYRange(Pt::int32_t y) const
    { return (y >= minY) && (y <= maxY); }
};


// ======================================================================================
// ===== Inlined Public Member Functions ================================================
// ======================================================================================

const Pt::uint8_t* Rasterizer2::patternBufferMP64() const
{ return _patternBufferMP; }


// ======================================================================================
// ===== Inlined and/or Templated Private Member Functions ==============================
// ======================================================================================

void Rasterizer2::rasterOnePixelSolidLine_F(float x1, float y1, float x2, float y2, const Color& color, DrawLineMask* maskInOut)
{ rasterOnePixelSolidGLineSegmentXWAA_F(x1, y1, x2, y2, color, maskInOut); }

void Rasterizer2::rasterOnePixelPatternedLine_F(float x1, float y1, float x2, float y2, const Color& color, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut)
{
    // Check the size of the line
    const float sizeX = ::fabs(x2 - x1);
    const float sizeY = ::fabs(y2 - y1);
    const float sizeS = sizeX + sizeY;
    const float sizeL = Gfx::Math::fastSqrt(sizeX * sizeX + sizeY * sizeY);

    // Calculate the incremental factor of the pattern indexing counter
    const Pt::int32_t fpiCtrInc = Pt::Gfx::Math::zrint(FIXED_POINT_CONSTANT_ISQRT2 * PATTERN_BUFFER_SCALE_FACTOR * sizeS / sizeL);

    // Rasterize line
    rasterOnePixelPatternedGLineSegmentXWAA_F(x1, y1, x2, y2, color, fpiCtrInc, fpiCtrInOut, maskInOut);
}

void Rasterizer2::updateGradientBrush_getStartEndColors(Pt::uint8_t rgbaStart[4], Pt::uint8_t rgbaEnd[4])
{
    rgbaStart[0] = _brush.color        ().red  () / 257;
    rgbaStart[1] = _brush.color        ().green() / 257;
    rgbaStart[2] = _brush.color        ().blue () / 257;
    rgbaStart[3] = _brush.color        ().alpha() / 257;

    rgbaEnd  [0] = _brush.gradientColor().red  () / 257;
    rgbaEnd  [1] = _brush.gradientColor().green() / 257;
    rgbaEnd  [2] = _brush.gradientColor().blue () / 257;
    rgbaEnd  [3] = _brush.gradientColor().alpha() / 257;
}

void Rasterizer2::updateGradientBrush_getCtrRatXY(float& ctrX, float& ctrY, float &xyRat, float& yxRat, Pt::int32_t width, Pt::int32_t height)
{
    ctrX  = width  * 0.5f + _brush.offsetX();
    ctrY  = height * 0.5f + _brush.offsetY();

    xyRat = (ctrX > ctrY) ? (ctrX / ctrY) : 1.0f;
    yxRat = (ctrY > ctrX) ? (ctrY / ctrX) : 1.0f;
}

Pt::uint8_t Rasterizer2::patternBuffer1PAlpha(Pt::int32_t idx) const
{ return _patternBuffer1P[ idx % FIXED_POINT_TO_INT(PATTERN_BUFFER_COUNTER_MAX1P) ]; }

Pt::uint8_t Rasterizer2::patternBuffer1PAlphaPolar(Pt::int32_t x, Pt::int32_t y, float scale) const
{ return patternBuffer1PAlpha(Gfx::Math::convertCartesianToPolarCoordinate(x, y) * scale); }

Pt::uint8_t Rasterizer2::patternBuffer1PAlphaPolar(Pt::int32_t x, Pt::int32_t y, float scale, float xyRat) const
{
    const float angle = Gfx::Math::convertCartesianToPolarCoordinate(x, y);

    if(xyRat >= 1.0 && angle >= 45) scale /= xyRat;
    if(xyRat <  1.0 && angle <  45) scale *= xyRat;

    return patternBuffer1PAlpha(angle * scale);
}

void Rasterizer2::patternBuffer1PAlpha(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t idx, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const
{
    a0 = (Pt::uint32_t) _patternBuffer1P[ idx % FIXED_POINT_TO_INT(PATTERN_BUFFER_COUNTER_MAX1P) ] * alpha0 / 255;
    a1 = (Pt::uint32_t) _patternBuffer1P[ idx % FIXED_POINT_TO_INT(PATTERN_BUFFER_COUNTER_MAX1P) ] * alpha1 / 255;
}

void Rasterizer2::patternBuffer1PAlphaPolar(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t x, Pt::int32_t y, float scale, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const
{ patternBuffer1PAlpha(a0, a1, Gfx::Math::convertCartesianToPolarCoordinate(x, y) * scale, alpha0, alpha1); }

void Rasterizer2::patternBuffer1PAlphaPolar(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t x, Pt::int32_t y, float scale, float xyRat, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const
{
    const float angle = Gfx::Math::convertCartesianToPolarCoordinate(x, y);

    if(xyRat >= 1.0 && angle >= 45) scale /= xyRat;
    if(xyRat <  1.0 && angle <  45) scale *= xyRat;

    patternBuffer1PAlpha(a0, a1, angle * scale, alpha0, alpha1);
}

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

void Rasterizer2::fillPixel(Pt::int32_t x, Pt::int32_t y, Pt::int32_t minX, Pt::int32_t minY, Pt::uint8_t alpha)
{
    // Check the clipping
    if(!ClipShapeI::insideXRange(x, _currentClip)) return;
    if(!ClipShapeI::insideYRange(y, _currentClip)) return;

    // Draw the pixels using texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        const Pt::int32_t dx = x - minX;
        const Pt::int32_t dy = y - minY;
        const Pt::int32_t tx = _isGradient ? std::min(bw - 1, dx) : (dx % bw);
        const Pt::int32_t ty = _isGradient ? std::min(bh - 1, dy) : (dy % bh);
        ConstPixel srcPixel(_brushImage->view(), tx, ty);
        Pixel      dstPixel(_image->view(), x, y);
        _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
    }

    // Draw the pixels using solid color
    else {
        Pixel pixel(_image->view(), x, y);
        _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
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
    for(; iterL < sizeX; ++iterL) {
        if(alphas[iterL]) break;
    }

    // Texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        for(; iterL < sizeX; ++iterL) {
            // Break if we have reached the non anti-aliased part of the span
            if(alphas[iterL] >= RSL_MAX_ALPHA) break;
            // Draw the pixel
            const Pt::int32_t iterX = minX + iterL;
            const Pt::int32_t iterY = minY + pixelY;
            const Pt::int32_t tx = _isGradient ? std::min(bw - 1, iterL ) : (iterL  % bw);
            const Pt::int32_t ty = _isGradient ? std::min(bh - 1, pixelY) : (pixelY % bh);
            ConstPixel srcPixel(_brushImage->view(), tx, ty);
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
    for(; iterR >= 0; --iterR) {
        if(alphas[iterR]) break;
    }

    // Texture or gradient
    if(_isTexture || _isGradient) {
        for(; iterR >= 0; --iterR) {
            // Break if we have reached the non anti-aliased part of the span
            if(alphas[iterR] >= RSL_MAX_ALPHA) break;
            // Draw the pixel
            const Pt::int32_t iterX = minX + iterR;
            const Pt::int32_t iterY = minY + pixelY;
            const Pt::int32_t bw    = _brushImage->width ();
            const Pt::int32_t bh    = _brushImage->height();
            const Pt::int32_t tx    = _isGradient ? std::min(bw - 1, iterR ) : (iterR  % bw);
            const Pt::int32_t ty    = _isGradient ? std::min(bh - 1, pixelY) : (pixelY % bh);
            ConstPixel srcPixel(_brushImage->view(), tx, ty);
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

    // Draw the span using texture (or gradient texture)
    if(_isTexture) {
        const Pt::int32_t bw        = _brushImage->width();
        const Pt::int32_t bh        = _brushImage->height();
              Pt::int32_t iterX     = iterL;
              Pt::int32_t spanWidth = iterR - iterL + 1;
        while(spanWidth > 0) {
            const Pt::int32_t tx = _isGradient ? std::min(bw - 1, iterX ) : (iterX  % bw);
            const Pt::int32_t ty = _isGradient ? std::min(bh - 1, pixelY) : (pixelY % bh);
            const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tx);
            if(n) {
                ConstPixel srcPixel(_brushImage->view(), tx, ty);
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
            const Pt::int32_t textureY = std::min<Pt::int32_t>(pixelY, _brushImage->height() - 1);
            ConstPixel        srcPixel(_brushImage->view(), 0, textureY);
            Pixel             dstPixel(_image->view(), minX + iterX, minY + pixelY);
            _image->format().setPixels(dstPixel, srcPixel, spanWidth, _compositionMode);
        }
        // Fill the span - horizontal gradient
        else {
            while(spanWidth > 0) {
                const Pt::int32_t tx = std::min<Pt::int32_t>(iterX,     _brushImage->width () - 1);
                const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tx);
                if(n) {
                    ConstPixel srcPixel(_brushImage->view(), tx, 0);
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
        _image->format().setPixels(pixel, color, iterR - iterL + 1, _compositionMode);
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

void Rasterizer2::arcUtil_detXWLineDirection(ArcXWLineData& xwLineData)
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

bool Rasterizer2::arcUtil_pointIsInsideDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBegin, float degEnd, float xyRatio)
{
    // IMPORTANT NOTES:
    //     * The Y coordinate goes from low to high according to the coordinate system being used:
    //           - cartesian coordinate system: from the horizontal axis (the X axis) to the top;
    //           - computer  coordinate system: from the top of the screen to the bottom of the screen;
    //       This will cause sign inversion for trigonometry-based calculations in the Y coordinate.
    //     * The movement from begin angle to end angle must be in counter-clockwise (CCW), otherwise
    //       something wrong will be drawn.

    const float angle = Gfx::Math::convertCartesianToPolarCoordinate( (x - ctrX), -(y - ctrY) * xyRatio );

    // Both begin and end angle are negative
    if(degBegin < 0 && degEnd < 0) {
        return angle >= (degBegin + 360) && angle <= (degEnd + 360);
    }

    // Begin angle is negative but end angle is positive
    if(degBegin < 0 && degEnd >= 0) {
        if(angle >= (degBegin + 360) && angle <= 360   ) return true;
        if(angle >= 0                && angle <= degEnd) return true;
        return false;
    }

    // Both begin and end angle are positive
    return angle >= degBegin && angle <= degEnd;
}

Pt::uint8_t Rasterizer2::arcUtil_pointIsInsideDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, Pt::uint8_t alpha, float degBegin, float degEnd, float xyRatio)
{
    // IMPORTANT NOTES:
    //     * The Y coordinate goes from low to high according to the coordinate system being used:
    //           - cartesian coordinate system: from the horizontal axis (the X axis) to the top;
    //           - computer  coordinate system: from the top of the screen to the bottom of the screen;
    //       This will cause sign inversion for trigonometry-based calculations in the Y coordinate.
    //     * The movement from begin angle to end angle must be in counter-clockwise (CCW), otherwise
    //       something wrong will be drawn.

    const float relX  = x - ctrX;
    const float relY  = y - ctrY;
    const float relM  = std::max( ::fabs(relX), ::fabs(relY) );
    const float angle = Gfx::Math::convertCartesianToPolarCoordinate(relX, -relY * xyRatio);
    const float limit = 100.0f / relM;

    // Both begin and end angle are negative
    if(degBegin < 0 && degEnd < 0) {
        degBegin += 360;
        degEnd   += 360;
        if(angle >= degBegin && angle <= degEnd) {
            const float db = angle  - degBegin;
            const float de = degEnd - angle;
            const float dm = std::min(db, de);
            if(dm > limit) return alpha;
            return (dm > limit) ? alpha : (alpha * dm / limit);
        }
        return 0;
    }

    // Begin angle is negative but end angle is positive
    if(degBegin < 0 && degEnd >= 0) {
        degBegin += 360;
        if(angle >= degBegin && angle <= 360) {
            const float dm = angle - degBegin;
            if(dm > limit) return alpha;
            return (dm > limit) ? alpha : (alpha * dm / limit);
        }
        if(angle >= 0  && angle <= degEnd) {
            const float dm = degEnd - angle;
            if(dm > limit) return alpha;
            return (dm > limit) ? alpha : (alpha * dm / limit);
        }
        return 0;
    }

    // Both begin and end angle are positive
    if(angle >= degBegin && angle <= degEnd) {
        const float db = angle  - degBegin;
        const float de = degEnd - angle;
        const float dm = std::min(db, de);
        return (dm > limit) ? alpha : (alpha * dm / limit);
    }
    return 0;
}


} // namespace
} // namespace

#endif
