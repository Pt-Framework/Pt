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

#include "ArcMode.h"
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Gfx/Path.h>
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
#define FIXED_POINT_FROM_FLT(V)     ( Pt::lround( ( (V) * ( (float) FIXED_POINT_CONSTANT_ONE ) ) ) )
#define FIXED_POINT_FROM_INT(V)     ( (V) << FIXED_POINT_SHIFT_FACTOR )
#define FIXED_POINT_TO_INT(V)       ( (V) >> FIXED_POINT_SHIFT_FACTOR )

// Coordinate limit
#define MAXIMUM_COORD Painter::MaximumCoordinate
#define MAXIMUM_POINT Painter::MaximumPointCoordinate

#define MAXIMUM_COORD_F (float) Painter::MaximumCoordinate

// Scaling factor and starting value for the pattern buffer
#define PATTERN_BUFFER_NUM_OF_CELLS  64
#define PATTERN_BUFFER_SCALE_FACTOR  4
#define PATTERN_BUFFER_COUNTER_START FIXED_POINT_FROM_INT(3)

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
class Transform;

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

        void setAntiAliasingMode(bool on)
        {
            _aaMode = on;
            updatePenPattern();
        }

        bool antiAliasingMode() const
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

        void strokeText(const Point& to, const Pt::String& text, const Transform& t);
        void strokeOnePixelLine(const Point& a, const Point& b, DrawLineMask* maskInOut);
        void strokeOnePixelRect(const Point& tl, const Point& br);
        void strokeOnePixelQuadraticPolybezierOutline(const Point* points, size_t pointCount);
        void strokeOnePixelEllipseArc(const Point& topLeft, const Size& size, float degBegin, float degEnd, const ArcMode& arcMode);

        template <typename PointT>
        inline void strokeOnePixelPolygonOutline(const BasicPoint<PointT>* points, size_t pointCount, bool autoClose);

        template <typename PointT>
        inline void penFillPolygon(const BasicPoint<PointT>* points, size_t pointCount);

        template <typename PointT>
        inline void penFillPolygonSeparate(const BasicPoint<PointT>* points, size_t pointCount);

        template <typename PointT>
        void fillPolygon(const BasicPoint<PointT>* points, size_t pointCount);

        void fillPolygons(const std::vector<Polygon>& polygons);

        void rasterPolygonsNoAA(const std::vector<Polygon>& polygons, 
                                const Color& color, 
                                Pt::int32_t minX, Pt::int32_t minY, 
                                Pt::int32_t maxX, Pt::int32_t maxY);

        void rasterPolygonsXWAA(const std::vector<Polygon>& polygons, 
                                const Color& color, 
                                Pt::int32_t minX, Pt::int32_t minY, 
                                Pt::int32_t maxX, Pt::int32_t maxY);

        void fillRect(const Point& tl, const Point& br);

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

        // ValueT converter
        template <typename ValueT> struct CnvValueT;

    private:
        inline void rasterOnePixelSolidLine(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        inline void rasterOnePixelSolidLine_F(float x1, float y1, float x2, float y2, const Color& color, DrawLineMask* maskInOut);

        void rasterOnePixelSolidHLineSegment(Pt::int32_t x1, Pt::int32_t x2, Pt::int32_t y, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidVLineSegment(Pt::int32_t x, Pt::int32_t y1, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidXLineSegment(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidGLineSegmentNoAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut);
        void rasterOnePixelSolidGLineSegmentXWAA_F(float x1, float y1, float x2, float y2, const Color& color, DrawLineMask* maskInOut);

        void rasterOnePixelAreaGLineSegmentXWAA_F(float x1, float y1, float x2, float y2, const Color& color, Pt::int32_t minX, Pt::int32_t minY, const PolygonScanlines& exclusionZone, DrawLineMask& maskInOut);

        inline void rasterOnePixelPatternedLine(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);
        inline void rasterOnePixelPatternedLine_F(float x1, float y1, float x2, float y2, const Color& color, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);

        void rasterOnePixelPatternedXLineSegment(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, Pt::int32_t fpiCtrInc, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);
        void rasterOnePixelPatternedGLineSegmentNoAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, Pt::int32_t fpiCtrInc, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);
        void rasterOnePixelPatternedGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, Pt::int32_t fpiCtrInc, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);
        void rasterOnePixelPatternedGLineSegmentXWAA_F(float x1, float y1, float x2, float y2, const Color& color, Pt::int32_t fpiCtrInc, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut);

        void rasterRectArea(const Point& tl, const Point& br);

        void rasterOnePixelPolygonOutline(const Point* points, size_t pointCount, const Color& color);
        void rasterOnePixelPolygonOutline(const PointF* points, size_t pointCount, const Color& color);

        inline void rasterPolygonArea(const Point* points, const size_t* pointCount, 
                                      size_t polyCount, size_t totalPointCount, 
                                      const Color& color, 
                                      Pt::int32_t minX, Pt::int32_t minY, 
                                      Pt::int32_t maxX, Pt::int32_t maxY);
       
        inline void rasterPolygonArea(const PointF* points, const size_t* pointCount, 
                                      size_t polyCount, size_t totalPointCount, 
                                      const Color& color, 
                                      float minX, float minY, 
                                      float maxX, float maxY);

        void rasterPolygonAreaNoAA(const Point* points, const size_t* pointCount, 
                                   size_t polyCount, size_t totalPointCount, 
                                   const Color& color, 
                                   Pt::int32_t minX, Pt::int32_t minY, 
                                   Pt::int32_t maxX, Pt::int32_t maxY);
        
        void rasterPolygonAreaXWAA(const PointF* points, const size_t* pointCount, 
                                   size_t polyCount, size_t totalPointCount, 
                                   const Color& color, 
                                   float minX, float minY, 
                                   float maxX, float maxY);

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

        // --- Rasterization helper functions ---

        // Mask layout for store4Pixels/fill4Pixels function variants that take mask as the last argument:
        //     Mask element        : #0         #1         #2         #3
        //     Affected coordinate : (x1, y1)   (x1, y2)   (x2, y1)   (x2, y2)
        // In this case, "true" means the pixel will be drawn and "false" means it will not be drawn

        void stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2);
        void stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const bool mask[4]);
        void stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::uint8_t alpha);
        void stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::uint8_t alpha, const bool mask[4]);

        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY);
        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, const bool mask[4]);
        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, Pt::uint8_t alpha);
        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, Pt::uint8_t alpha, const bool mask[4]);
        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, const Pt::uint8_t alphaMask[4]);

        inline void fillPixel(Pt::int32_t x, Pt::int32_t y, Pt::int32_t minX, Pt::int32_t minY, Pt::uint8_t alpha);

        void rasterScanline(
            Pt::int32_t  iterL, Pt::int32_t iterR, Pt::int32_t pixelY,
            Pt::int32_t  minX,  Pt::int32_t minY,
            const Color& color
        );

        void rasterScanlineWithClipping(Pt::int32_t from, Pt::int32_t to, Pt::int32_t pixelY, Pt::int32_t minX, Pt::int32_t minY);

        // --- Polygon-related helper functions ---
        template <typename PointT, typename ValueT>
        inline void getPolygonRectMinMax(const BasicPoint<PointT>* points, size_t pointCount, ValueT& minX, ValueT& minY, ValueT& maxX, ValueT& maxY) const;

        template <typename PointT>
        inline void genClippedPolygonPoints(std::vector< BasicPoint<PointT> >& dst, const BasicPoint<PointT>* src, const size_t pointCount, bool forPolygonOutline) const;

        template <typename PointT, typename ValueT>
        inline void separateAndClipPolygons(ValueT& minX, ValueT& maxX, ValueT& minY, ValueT& maxY, std::vector< BasicPoint<PointT> >& clippedPoints, std::vector<size_t>& clippedCounts, const BasicPoint<PointT>* points, size_t pointCount) const;

        // --- Arc-related helper functions ---
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
        bool _aaMode;

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


//
// Include the inline and/or template function implementation
//
#include "Rasterizer2_PrivateStruct.tpp"
#include "Rasterizer2_PrivateFunc.tpp"
#include "Rasterizer2_PublicFunc.tpp"


} // namespace
} // namespace

#endif
