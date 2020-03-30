/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2017 Marc Boris Duerner
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
#include "Fixed.h"
#include "Polygonizer.h"
#include "FreeType.h"
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Gfx/Path.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>

// Coordinate limit
#define MAXIMUM_COORD   32767
#define MAXIMUM_COORD_F 32767.0f

// Scaling factor and starting value for the pattern buffer of narrow lines
#define PATTERN_BUFFER_1P_SCALE_FACTOR  4
#define PATTERN_BUFFER_1P_COUNTER_START FIXED_POINT_FROM_INT( (PATTERN_BUFFER_1P_SCALE_FACTOR) - 1 )


namespace Pt {

namespace Gfx {

class DrawText2;
class Image;
class Transform;

class Rasterizer2
{
    public:
        // Weighting filter for Xiaolin Wu's anti-aliasing algorithm
        static const Pt::uint8_t XWAA_WFILTER[256];

        // Mask for excluding pixels when drawing line; each element corresponds to
        // the coordinate of one of the pixel(s) of the start and end points
        typedef Point DrawLineMask[4];

        static const DrawLineMask NullLineMask;

        static const Pt::int32_t MaxCoordinate = MAXIMUM_COORD;

        static Point maxPoint()
        { return Point(MaxCoordinate, MaxCoordinate); }

        static const Pt::int32_t MaxCoordinateF = MAXIMUM_COORD_F;

        static PointF maxPointF()
        { return PointF(MaxCoordinateF, MaxCoordinateF); }

    public:
        Rasterizer2(Image& image);

        ~Rasterizer2();

        bool isAntiAliasing() const;

        void setAntiAliasing(bool on);

        void setImage(Image& image);

        const ImageFormat& format() const;

        void setPen(const Pen& pen);

        const Pen& pen() const
        { return _pen; }

        void setBrush(const Brush& brush);

        const Brush& brush() const
        { return _brush; }

        void setFont(const Font& font);

        const Font& font() const
        { return _font; }

        void setClip(const Rect& clip);

        void resetClip();

        const Rect& clip() const
        { return _clip; }

        void setCompositionMode(const CompositionMode& mode)
        { _compositionMode = mode; }

        const CompositionMode& compositionMode() const
        { return _compositionMode; }

        void drawImage(const Point& to, const Image& image);

        void drawImage(const Point& to, const Image& image, const Rect& imageRect);

        void drawText(const Point& to, const Pt::String& text, const Transform& t);

        FontMetrics fontMetrics( const String& text ) const;

        static FontMetrics fontMetrics( const Font& font, const Pt::String& text );

        void drawLine(const PointF& from, const PointF& to);

        void drawPolyline(const PointF* ps, size_t n);

        void drawRect(const RectF& rect);

        void drawRoundedRect(const RectF& rect, float radius);

        void drawEllipse(const PointF& topLeft, const SizeF& size);

        void drawArc(const PointF& topLeft, const SizeF& size,
                     float degBegin, float degEnd, const ArcMode& arcMode);

        void drawPath(const Path& path, float smoothness);

        void fillPolygon(const PointF* ps, const size_t pointCount);

        void fillPolygon_NR(const PointF* ps, const size_t pointCount);

        void fillPolygons(const std::vector<Polygon>& polygons);

        void fillRect(const RectF& rect);

        void fillRoundedRect(const RectF& rect, float radius);

        void fillEllipse(const PointF& topLeft, const SizeF& size);

        void fillPie(const PointF& topLeft, const SizeF& size,
                     float degBegin, float degEnd);

        void fillChord(const PointF& topLeft, const SizeF& size,
                        float degBegin, float degEnd);

        void fillPath(const Path& path, float smoothness);

    protected:
        void drawNarrowLine(const Point& a, const Point& b, DrawLineMask* maskInOut);

        void drawNarrowPolyline(const PointF* points, size_t pointCount);

        void drawWidePolyline(const PointF* points, const size_t pointCount);

        void drawNarrowPath(const PointF* pointsF, size_t pointCount);

    private:
        //
        // brushes
        //

        void updateGradientBrush(Pt::int32_t width, Pt::int32_t height);

        void updateGradientBrush_gen1DHorVerGradient(Pt::int32_t width, Pt::int32_t height);

        void updateGradientBrush_gen2DLinearGradient(Pt::int32_t width, Pt::int32_t height);

        void updateGradientBrush_gen2DRadialGradient(Pt::int32_t width, Pt::int32_t height);

        //
        // pen patterns
        //

        void updatePenPattern();

        Pt::uint8_t patternBuffer1PAlpha(Pt::int32_t idx) const;

        Pt::uint8_t patternBuffer1PAlphaPolar(Pt::int32_t x, Pt::int32_t y, float scale) const;

        Pt::uint8_t patternBuffer1PAlphaPolar(Pt::int32_t x, Pt::int32_t y, float scale, float xyRat) const;

        void patternBuffer1PAlpha(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t idx, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const;

        void patternBuffer1PAlphaPolar(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t x, Pt::int32_t y, float scale, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const;

        void patternBuffer1PAlphaPolar(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t x, Pt::int32_t y, float scale, float xyRat, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const;

        //
        // clipping
        //

        void updateClip();

        //
        // wide lines
        //

        void rasterWideLine(const PointF* ps, std::size_t n);

        void rasterWidePolyline(const std::vector<Polygon>& polygons);

        //
        // narrow lines
        //

        void rasterNarrowSolidLine(Pt::int32_t x1, Pt::int32_t y1,
                                   Pt::int32_t x2, Pt::int32_t y2,
                                   const Color& color, DrawLineMask* maskInOut);

        void rasterNarrowSolidLine_F(float x1, float y1,
                                     float x2, float y2,
                                     const Color& color, DrawLineMask* maskInOut);

        void rasterNarrowPatternedLine(Pt::int32_t x1, Pt::int32_t y1,
                                       Pt::int32_t x2, Pt::int32_t y2,
                                       const Color& color, Pt::int32_t& fpiCtrInOut,
                                       DrawLineMask* maskInOut);

        void rasterNarrowPatternedLine_F(float x1, float y1,
                                         float x2, float y2,
                                         const Color& color, Pt::int32_t& fpiCtrInOut,
                                         DrawLineMask* maskInOut);

        //
        // solid narrow lines
        //

        void rasterNarrowSolidHLineSegment(Pt::int32_t x1, Pt::int32_t x2,
                                           Pt::int32_t y, const Color& color,
                                           DrawLineMask* maskInOut);

        void rasterNarrowSolidVLineSegment(Pt::int32_t x, Pt::int32_t y1,
                                           Pt::int32_t y2, const Color& color,
                                           DrawLineMask* maskInOut);

        void rasterNarrowSolidXLineSegment(Pt::int32_t x1, Pt::int32_t y1,
                                           Pt::int32_t x2, Pt::int32_t y2,
                                           const Color& color, DrawLineMask* maskInOut);

        void rasterNarrowSolidGLineSegmentNoAA(Pt::int32_t x1, Pt::int32_t y1,
                                               Pt::int32_t x2, Pt::int32_t y2,
                                               const Color& color,
                                               DrawLineMask* maskInOut);

        void rasterNarrowSolidGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1,
                                               Pt::int32_t x2, Pt::int32_t y2,
                                               const Color& color,
                                               DrawLineMask* maskInOut);

        void rasterNarrowSolidGLineSegmentXWAA_F(float x1, float y1,
                                                 float x2, float y2,
                                                 const Color& color,
                                                 DrawLineMask* maskInOut);

        //
        // solid patterned lines
        //

        void rasterNarrowPatternedXLineSegment(Pt::int32_t x1, Pt::int32_t y1,
                                               Pt::int32_t x2, Pt::int32_t y2,
                                               const Color& color,
                                               Pt::int32_t fpiCtrInc,
                                               Pt::int32_t& fpiCtrInOut,
                                               DrawLineMask* maskInOut);

        void rasterNarrowPatternedGLineSegmentNoAA(Pt::int32_t x1, Pt::int32_t y1,
                                                   Pt::int32_t x2, Pt::int32_t y2,
                                                   const Color& color,
                                                   Pt::int32_t fpiCtrInc,
                                                   Pt::int32_t& fpiCtrInOut,
                                                   DrawLineMask* maskInOut);

        void rasterNarrowPatternedGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1,
                                                   Pt::int32_t x2, Pt::int32_t y2,
                                                   const Color& color,
                                                   Pt::int32_t fpiCtrInc,
                                                   Pt::int32_t& fpiCtrInOut,
                                                   DrawLineMask* maskInOut);

        void rasterNarrowPatternedGLineSegmentXWAA_F(float x1, float y1,
                                                     float x2, float y2,
                                                     const Color& color,
                                                     Pt::int32_t fpiCtrInc,
                                                     Pt::int32_t& fpiCtrInOut,
                                                     DrawLineMask* maskInOut);

        //
        // polygons
        //

        void rasterPolygonNoAA(const PointF* points, std::size_t pointCount,
                               const Color& color,
                               Pt::int32_t minX, Pt::int32_t minY,
                               Pt::int32_t maxX, Pt::int32_t maxY);

        void rasterPolygonsNoAA(const std::vector<Polygon>& polygons,
                                const Color& color,
                                Pt::int32_t minX, Pt::int32_t minY,
                                Pt::int32_t maxX, Pt::int32_t maxY);

        void rasterPolygonXWAA(const PointF* ps, std::size_t pointCount,
                               const Color& color,
                               Pt::int32_t minX_, Pt::int32_t minY_,
                               Pt::int32_t maxX_, Pt::int32_t maxY_);

        void rasterPolygonsXWAA(const std::vector<Polygon>& polygons,
                                const Color& color,
                                Pt::int32_t minX, Pt::int32_t minY,
                                Pt::int32_t maxX, Pt::int32_t maxY);

#if 0
        // REVIEW: Seems nothing actually uses these functions anymore?

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
#endif

        template <typename T>
        struct ScanlineElement
        {
            T from;
            T to;

            inline ScanlineElement(T from_ = -1, T to_ = -1)
            : from(from_), to(to_)
            {}

            inline bool isNull() const
            { return from == -1 && to == -1; }
        };


        typedef ScanlineElement<Pt::int16_t> ScanlineElement16;

        typedef ScanlineElement<Pt::int32_t> ScanlineElement32;

        typedef std::vector<ScanlineElement16> S16V;

        typedef std::vector<ScanlineElement32> S32V;

        typedef std::vector<ScanlineElement16>::const_iterator S16V_CI;

        typedef std::vector<ScanlineElement32>::const_iterator S32V_CI;

        // Polygon scanlines (used for drawing filled polygons with XWAA)
        //     * The vector index specify the Y coordinate of the scanline
        //     * The vector element specify a set of "from" and "to" X coordinates
        typedef std::vector< std::vector<ScanlineElement16> > PolygonScanlines;

        static inline S16V::const_iterator S16V_begin(const PolygonScanlines& ps, const Pt::int32_t i)
        {
            if(i < 0 || i >= (Pt::int32_t) ps.size()) return ps.back().end();
            return ps[i].begin();
        }

        static inline S16V::const_iterator S16V_end(const PolygonScanlines& ps, const Pt::int32_t i)
        {
            if(i < 0 || i >= (Pt::int32_t) ps.size()) return ps.back().end();
            return ps[i].end();
        }

        /*
        // OLD POLYGON XWAA
        // REVIEW: Seems nothing actually uses this function anymore?
        void rasterPolygonBorderXWAA_F(float x1, float y1,
                                       float x2, float y2,
                                       const Color& color,
                                       Pt::int32_t minX, Pt::int32_t minY,
                                       const PolygonScanlines& exclusionZone,
                                       DrawLineMask& maskInOut);
        */

        void rasterPolygonBorderXWAA_F2(float x1, float y1,
                                       float x2, float y2,
                                       const Color& color,
                                       Pt::int32_t minX, Pt::int32_t minY,
                                       const PolygonScanlines& exclusionZone,
                                       DrawLineMask& maskInOut);

        //
        // rects
        //

        void rasterNarrowRect(const Point& tl, const Point& br);

        void rasterNarrowRoundedRect(const RectF& rect, float radius);

        void rasterRectArea(const Point& tl, const Point& br);

        //
        // ellipses
        //

        void fillEllipse(const Point& topLeft, const Size& size);

        void rasterEllipseAreaNoAA(const Point& topLeft, const Size& size);

        //
        // bezier curves
        //

        void rasterNarrowQuadraticBezier(Pt::int32_t x1, Pt::int32_t y1,
                                        Pt::int32_t x2, Pt::int32_t y2,
                                        Pt::int32_t x3, Pt::int32_t y3,
                                        const Color& color,
                                        Pt::int32_t* fpiCtrInOut,
                                        DrawLineMask* maskInOut);

        //
        // arcs
        //

        struct FilledArcInfo
        {
            bool        antiAlias;    // A flag that indicate if the arc will be anti-aliased

            float       degBegin;     // Begin angle
            float       degEnd;       // End angle

            Pt::int32_t minX, minY;   // Top-left coordinate of the arc
            float       ctrX, ctrY;   // Center coordinate of the arc
            float       radX, radY;   // Radius of the arc
            float       radX2, radY2; // Squared radius of the arc
            float       xyRat;        // Ratio of the X and Y radius

            bool        wEven;        // A flag that indicates if the width is even
            bool        hEven;        // A flag that indicates if the height is even

            Pt::int32_t x1, y1;       // Coordinate of the begin point
            Pt::int32_t x2, y2;       // Coordinate of the end point

            Pt::int32_t quartersX;    // The number of quarter points in the X direction
            Pt::int32_t quartersY;    // The number of quarter points in the Y direction
        };

        // Xiaolin Wu's anti-aliased line data structure
        struct ArcXWLineData
        {
            // --- Point data sub-structure ---
            struct XWPoint {
                Pt::int32_t x;
                Pt::uint8_t a1, a2;

                inline XWPoint(Pt::int32_t x_ = -1, Pt::uint8_t a1_ = 0, Pt::uint8_t a2_ = 0)
                : x(x_), a1(a1_), a2(a2_)
                {}

                inline bool isNull() const
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

            inline bool insideYRange(Pt::int32_t y) const
            { return (y >= minY) && (y <= maxY); }
        };

        // Ellipse & arc scanlines (used for drawing filled ellipse and arcs)
        //    * The vector index specify the Y coordinate of the scanline
        //    * The vector element specify the "from" and "to" X coordinates
        typedef std::vector<ScanlineElement32> EAScanlines;
        
        void rasterNarrowArc(const Point& topLeft, const Size& size,
                             float degBegin, float degEnd, const ArcMode& arcMode);

        void rasterArcArea(const Point& topLeft, const Size& size,
                           float degBegin, float degEnd, const ArcMode& arcMode);

        void rasterArcAreaChord(FilledArcInfo& fai);

        void rasterArcAreaPie(FilledArcInfo& fai);

        static void arcUtil_detXWLineDirection(ArcXWLineData& xwLineData);

        static bool arcUtil_pointIsInsideDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBegin, float degEnd, float xyRatio);

        static Pt::uint8_t arcUtil_pointIsInsideDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, Pt::uint8_t alpha, float degBegin, float degEnd, float xyRatio);

        static void arcUtil_findExactBegEndPointsCoordinate(FilledArcInfo& fai);

        static void arcUtil_runXWLineAlgorithm(ArcXWLineData& xwLine, const FilledArcInfo& fai, Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2);

        static void arcUtil_genScanlinesForChord(EAScanlines& scanlines, const FilledArcInfo& fai, const ArcXWLineData& xwLine);

        static void arcUtil_cropAndStoreScanlineForChord(EAScanlines& scanlines, const FilledArcInfo& fai, const ArcXWLineData& xwLine, Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t y);

        static void arcUtil_genScanlinesForPie(EAScanlines& scanlines1, EAScanlines& scanlines2, const FilledArcInfo& fai, const ArcXWLineData& xwLine1, const ArcXWLineData& xwLine2);

        static void arcUtil_cropAndStoreScanlineForPie(EAScanlines& scanlines1, EAScanlines& scanlines2, const FilledArcInfo& fai, const ArcXWLineData& xwLine1, const ArcXWLineData& xwLine2, Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t y);

        void arcUtil_rasterCircumferencePixels(FilledArcInfo& fai);

        void arcUtil_rasterClosingXWLine(const FilledArcInfo& fai, const ArcXWLineData& xwLine, Point maskInOut[4]);

        //
        // pixel operations
        //

        void fillPixel(Pt::int32_t x, Pt::int32_t y,
                       Pt::int32_t minX, Pt::int32_t minY,
                       Pt::uint8_t alpha);

        // Mask layout for store4Pixels/fill4Pixels function variants that take mask as the last argument:
        //     Mask element        : #0         #1         #2         #3
        //     Affected coordinate : (x1, y1)   (x1, y2)   (x2, y1)   (x2, y2)
        // In this case, "true" means the pixel will be drawn and "false" means it will not be drawn

        void stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                           Pt::int32_t x2, Pt::int32_t y2);

        void stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                           Pt::int32_t x2, Pt::int32_t y2,
                           const bool mask[4]);

        void stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                           Pt::int32_t x2, Pt::int32_t y2,
                           Pt::uint8_t alpha);

        void stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                           Pt::int32_t x2, Pt::int32_t y2,
                           Pt::uint8_t alpha, const bool mask[4]);

        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                         Pt::int32_t x2, Pt::int32_t y2,
                         Pt::int32_t minX, Pt::int32_t minY);

        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                         Pt::int32_t x2, Pt::int32_t y2,
                         Pt::int32_t minX, Pt::int32_t minY,
                         const bool mask[4]);

        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                         Pt::int32_t x2, Pt::int32_t y2,
                         Pt::int32_t minX, Pt::int32_t minY,
                         Pt::uint8_t alpha);

        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                         Pt::int32_t x2, Pt::int32_t y2,
                         Pt::int32_t minX, Pt::int32_t minY,
                         Pt::uint8_t alpha, const bool mask[4]);

        void fill4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                         Pt::int32_t x2, Pt::int32_t y2,
                         Pt::int32_t minX, Pt::int32_t minY,
                         const Pt::uint8_t alphaMask[4]);

        void rasterScanline(Pt::int32_t iterL, Pt::int32_t iterR, Pt::int32_t pixelY,
                            Pt::int32_t  minX, Pt::int32_t minY, const Color& color);

        void rasterScanlineClipped(Pt::int32_t from, Pt::int32_t to,
                                   Pt::int32_t pixelY,
                                   Pt::int32_t minX,
                                   Pt::int32_t minY);

    private:
        Image*           _image;
        Polygonizer      _polygonizer;

        CompositionMode  _compositionMode;
        bool             _aaMode;
        bool             _isGradient;
        bool             _isTexture;

        Brush            _brush;
        Image            _brushBuffer;
        ConstPixel       _brushPixel;
        const Image*     _brushImage;

        Pen              _pen;
        Image            _penBuffer;
        ConstPixel       _penPixel;


        // Pattern buffer for narrow lines
        std::vector<Pt::uint8_t> _patternBuffer1PDyn;
        Pt::int32_t              _patternBuffer1PDynCntMax;

        // Font & text related
        Font             _font;
        FTC_FaceID       _faceId;
        std::size_t      _fontSize;
        FTC_ImageTypeRec _imageType;
        Transform        _transform;

        // Clip related
        Rect             _clip;
        Rect             _currentClip;
};


} // namespace

} // namespace

#endif
