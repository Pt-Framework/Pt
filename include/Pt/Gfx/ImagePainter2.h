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

#ifndef PT_GFX_IMAGEPAINTER_2_H
#define PT_GFX_IMAGEPAINTER_2_H

#include <Pt/Math.h>

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/AntiAliasingMode.h>
#include <Pt/Gfx/ArcMode.h>
#include <Pt/Gfx/Painter.h>

#include <Pt/System/Path.h>

namespace Pt {
namespace Gfx {


class Rasterizer2;

class PT_GFX_API ImagePainter2 : public Painter
{
    public:
        ImagePainter2( Image& image );

        virtual ~ImagePainter2();

        void setAntiAliasingMode(AntiAliasingMode mode = AntiAliasingMode::Fastest);

        void setImage(Image& image);

        virtual const ImageFormat& format() const;

        virtual void setCompositionMode(const CompositionMode& mode);

        virtual const CompositionMode& compositionMode() const;

        virtual void setClip( const RectF& clip );

        virtual const Gfx::RectF& clip() const;

        virtual void setPen(const Pen& pen);

        virtual const Pen& pen() const;

        virtual void setBrush(const Brush& brush);

        virtual const Brush& brush() const;

        virtual void setFont(const Font& font);

        virtual const Font& font() const;

        virtual FontMetrics fontMetrics(const Pt::String& text) const;

        virtual void drawImage(const PointF& to, const Image& image);

        virtual void drawImage(const PointF& to, const Image& image, const RectF& imageRect);

        virtual void drawText(const PointF& to, const Pt::String& text);

        virtual void drawLine(const PointF& from, const PointF& to);

        virtual void drawRect(const RectF& rect);

        virtual void drawPolyline(const PointF* points, const size_t pointCount);

        virtual void drawEllipse(const PointF& topLeft, const SizeF& size);

        // NOTE: The begin and end angle must move in counter-clockwise direction or something wrong will be drawn!
        virtual void drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, const ArcMode& arcMode);

        virtual void fillRect(const RectF& rect);

        virtual void fillPolygon(const PointF* points, const size_t pointCount);

        virtual void fillEllipse(const PointF& topLeft, const SizeF& size);

        // NOTE: The begin and end angle must move in counter-clockwise direction or something wrong will be drawn!
        virtual void fillArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, const ArcMode& arcMode);

    public:
        static void setFontDir(const System::Path& path);
        static void setDefaultFont(const std::string& name);
        static std::string defaultFont();
        static std::vector<std::string> fontNames();
        static FontMetrics fontMetrics(const Font& font, const Pt::String& text);

    protected:
        // Each key specify the Y coordinate of a scanline;
        // while its element specify the "from" and "to" X coordinates
        struct ScanlineElement {
            Pt::int32_t from;
            Pt::int32_t to;

            ScanlineElement(Pt::int32_t from_ = -1, Pt::int32_t to_ = -1)
            : from(from_), to(to_)
            {}

            bool isNull() const
            { return from == -1 && to == -1; }
        };

        typedef std::vector<ScanlineElement> Scanlines;

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

        // Xiaolin Wu's anti-aliased line data structure (currently it is only used for drawing filled arc)
        struct XWLineData {
            // Point data
            struct XWPointAA {
                Pt::int32_t x;
                Pt::uint8_t a1, a2;

                XWPointAA(Pt::int32_t x_, Pt::uint8_t a1_, Pt::uint8_t a2_)
                : x(x_), a1(a1_), a2(a2_)
                {}
            };

            typedef std::multimap<Pt::int32_t, XWPointAA> XWPoints; // The key is the Y coordinate

            struct XWPoint {
                Pt::int32_t x;
                Pt::uint8_t a1, a2;

                XWPoint(Pt::int32_t x_ = -1, Pt::uint8_t a1_ = 0, Pt::uint8_t a2_ = 0)
                : x(x_), a1(a1_), a2(a2_)
                {}

                bool isNull() const
                { return x == -1 && a1 == 0 && a2 == 0; }
            };

            typedef std::vector< std::vector<XWPoint> > XWPointsV; // The vector index is the Y coordinate


            XWPoints points; // The line's points
            XWPointsV pointsv;
            bool     steep;  // If "true"  then the a2 belongs to (x + 1, y)
                             // If "false" then the a2 belongs to (x, y + 1)

            bool faceL;  // The direction that the line is facing to
            bool faceR;  // ---
            bool faceT;  // ---
            bool faceB;  // ---

            // The line's coordinates
            Pt::int32_t x1, y1, x2, y2;

            bool insideYRange(Pt::int32_t y) const
            {
                if(y1 <= y2) return (y >= y1) && (y <= y2);
                return (y >= y2) && (y <= y1);
            }
        };

    protected:
        // Inline helper functions
        static inline float fastInvSqrt(float x);
        static inline float fastSqrt(float x);
        static inline float fastSin(float x);
        static inline float fastCos(float x);

        static inline float fastAtan2(float y, float x);

        static inline float convertCartesianToPolarCoordinate(float x, float y);
        static inline bool pointIsInsideArcDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBegin, float degEnd, float xyRatio);

        // Arc-related helper functions
        static inline void arcUtil_detXWLineDirection(XWLineData& xwLineData);

        static void arcUtil_findExactBegEndPointsCoordinate(FilledArcInfo& fai);
        static void arcUtil_runXWLineAlgorithm(XWLineData& xwLine, const FilledArcInfo& fai, Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2);
        static void arcUtil_genScanlinesForChord(Scanlines& scanlines, const FilledArcInfo& fai, const XWLineData& xwLine);
        static void arcUtil_cropAndStoreScanlineForChord(Scanlines& scanlines, const FilledArcInfo& fai, const XWLineData& xwLine, Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t y);
        static void arcUtil_genScanlinesForPie(Scanlines& scanlines1, Scanlines& scanlines2, const FilledArcInfo& fai, const XWLineData& xwLine1, const XWLineData& xwLine2);
        static void arcUtil_cropAndStoreScanlineForPie(Scanlines& scanlines1, Scanlines& scanlines2, const FilledArcInfo& fai, const XWLineData& xwLine1, const XWLineData& xwLine2, Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t y);

        void arcUtil_drawCircumferencePixels(FilledArcInfo& fai);
        void arcUtil_drawXWLine(const FilledArcInfo& fai, const XWLineData& xwLine, const XWLineData* xwLineExclusion);

        // Drawing functions
        virtual void drawOnePixelSolidEllipseArcImpl(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, const ArcMode& arcMode);

        virtual void fillEllipseImplNoAA(const PointF& topLeft, const SizeF& size);

        virtual void fillArcChordImpl(FilledArcInfo& fai);
        virtual void fillArcPieImpl(FilledArcInfo& fai);

    private:
        RectF        _clip;
        Rasterizer2* _rasterizer;
};

// ======================================================================================
// ===== Private Member Structures and Functions ========================================
// ======================================================================================

#if defined(__arm__) || defined(__thumb__) || defined(_M_ARM) || defined(_M_ARMT) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || defined(_ARM) || defined(__arm)

// ### TODO: Check if these functions are actually faster in ARM CPUs !!! ###

inline float ImagePainter2::fastInvSqrt(float x)
{
    // https://en.wikipedia.org/wiki/Fast_inverse_square_root

    const float x2 = x * 0.5f;

    union {
        float       f;
        Pt::int32_t i;
    } u;

    u.f = x;
    u.i = 0x5F3759DF - ( u.i >> 1 );
    u.f = u.f * ( 1.5f - ( x2 * u.f * u.f ) );

    return u.f;
}

inline float ImagePainter2::fastSqrt(float x)
{
    // https://en.wikipedia.org/wiki/Methods_of_computing_square_roots

    union {
        float       f;
        Pt::int32_t i;
    } u;

    u.f = x;
    u.i = (1 << 29) + (u.i >> 1) - (1 << 22) - 0x0004C000;
    u.f = (u.f + x / u.f) * 0.5;

    return u.f;
}

inline float ImagePainter2::fastSin(float x)
{
    if (x > Pt::Pi) x -= Pt::PiDouble;

    const float b =  4 / Pt::Pi;
    const float c = -4 / Pt::PiSqr;
    const float p = 0.225;
    const float y = b * x + c * x * ::fabs(x);

    return p * (y * ::fabs(y) - y) + y;
}

inline float ImagePainter2::fastCos(float x)
{
    x += Pt::PiHalf;
    if(x > Pt::PiDouble) x -= Pt::PiDouble;

    return fastSin(x);
}

#else

inline float ImagePainter2::fastInvSqrt(float x)
{ return 1.0f / ::sqrtf(x); }

inline float ImagePainter2::fastSqrt(float x)
{ return ::sqrtf(x); }

inline float ImagePainter2::fastSin(float x)
{ return ::sin(x); }

inline float ImagePainter2::fastCos(float x)
{ return ::cos(x); }

#endif

inline float ImagePainter2::fastAtan2(float y, float x)
{
    // From https://gist.github.com/volkansalma/2972237
    // Original code by Volkan SALMA, 2012

    if(x == 0.0f) {
        if(y >  0.0f) return Pt::PiHalf;
        if(y == 0.0f) return 0.0f;
        return -Pt::PiHalf;
    }

    const float z = y / x;
          float atan;

    if(fabs(z) < 1.0f) {
        atan = z / (1.0f + 0.28f * z * z);
        if(x < 0.0f) {
            if(y < 0.0f) return atan - Pt::Pi;
            return atan + Pt::Pi;
        }
    }

    else {
        atan = Pt::PiHalf - z / (z * z + 0.28f);
        if(y < 0.0f) return atan - Pt::Pi;
    }

    return atan;
}

inline float ImagePainter2::convertCartesianToPolarCoordinate(float x, float y)
{
    // Quadrant I & II
    if(y >= 0)
        return fastAtan2(y, x) * 180 / Pt::Pi;

    // Quadrant III && IV
    return fastAtan2(y, x) * 180 / Pt::Pi + 360;
}

inline bool ImagePainter2::pointIsInsideArcDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBegin, float degEnd, float xyRatio)
{
    // IMPORTANT NOTES:
    //     * The Y coordinate goes from low to high according to the coordinate system being used:
    //           - cartesian coordinate system: from the horizontal axis (the X axis) to the top;
    //           - computer  coordinate system: from the top of the screen to the bottom of the screen;
    //       This will cause sign inversion for trigonometry-based calculations in the Y coordinate.
    //     * The movement from begin angle to end angle must be in counter-clockwise (CCW), otherwise
    //       something wrong will be drawn.

    const float angle = convertCartesianToPolarCoordinate( (x - ctrX), -(y - ctrY) * xyRatio);

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

inline void ImagePainter2::arcUtil_detXWLineDirection(XWLineData& xwLineData)
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

