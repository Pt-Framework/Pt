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

            ScanlineElement(Pt::int32_t from_, Pt::int32_t to_)
            : from(from_), to(to_)
            {}
        };

        typedef std::map<Pt::int32_t, ScanlineElement> Scanlines;

        // Xiaolin Wu's anti-aliased line data
        struct XWLineData;

        // Filled-arc information structure
        struct FilledArcInfo;

    protected:
        // Inline helper functions
        static inline float fastInvSqrt(float x);
        static inline float fastSqrt(float x);
        static inline float fastSin(float x);
        static inline float fastCos(float x);

        static inline float fastAtan2(float y, float x);

        static inline float convertCartesianToPolarCoordinate(float x, float y);
        static inline bool pointIsInsideArcDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBegin, float degEnd);

        // Arc-related helper functions
        static void arcUtil_runXWLineAlgorithm(XWLineData& dst, Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY);

        // Drawing functions
        virtual void drawOnePixelSolidEllipseArcImpl(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, const ArcMode& arcMode);

        virtual void fillEllipseImplNoAA(const PointF& topLeft, const SizeF& size);



        /*
        virtual void fillArcChordImpl(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);
        virtual void fillArcPieImpl(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

        static inline void arcUtilDetermineHoleDirection(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, bool& faceL, bool& faceR, bool& faceT, bool& faceB);
        static inline void arcUtilMarkOutsideScanlinesRL(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, bool faceL, bool faceR, bool faceT, bool faceB, Scanlines& scanlines);

        static void arcUtilCropScanlinesUsingXWu(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, bool faceL, bool faceR, bool faceT, bool faceB, Scanlines& scanlines);
        static void arcUtilCalcScanlines(Pt::int32_t radX, Pt::int32_t radY, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBegin, float degEnd, bool useAntiAliasing, Pt::int32_t& quartersX, Pt::int32_t& quartersY, Pt::int32_t& x1, Pt::int32_t& y1, Pt::int32_t& x2, Pt::int32_t& y2, Scanlines& scanlines);
               void arcUtilDrawCircumferencePixels(Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t radX, Pt::int32_t radY, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBegin, float degEnd, Pt::int32_t quartersX, Pt::int32_t quartersY, Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Scanlines& scanlinesRef);
               */

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

inline bool ImagePainter2::pointIsInsideArcDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBegin, float degEnd)
{
    // IMPORTANT NOTES:
    //     * The Y coordinate goes from low to high according to the coordinate system being used:
    //           - cartesian coordinate system: from the horizontal axis (the X axis) to the top;
    //           - computer  coordinate system: from the top of the screen to the bottom of the screen;
    //       This will cause sign inversion for trigonometry-based calculations in the Y coordinate.
    //     * The movement from begin angle to end angle must be in counter-clockwise (CCW), otherwise
    //       something wrong will be drawn.

    const float angle = convertCartesianToPolarCoordinate( x - ctrX, -(y - ctrY) );

    if(degBegin < 0 && degEnd < 0) {
        return angle >= (degBegin + 360) && angle <= (degEnd + 360);
    }

    if(degBegin < 0 && degEnd >= 0) {
        if( angle >= (degBegin + 360) && angle <= 360   ) return true;
        if( angle >= 0              && angle <= degEnd) return true;
        return false;
    }

    return angle >= degBegin && angle <= degEnd;
}

/*
inline void ImagePainter2::arcUtilDetermineHoleDirection(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, bool& faceL, bool& faceR, bool& faceT, bool& faceB)
{
    // Calculate the direction vector
    const Pt::int32_t vx = x2 - x1;           // Vector from the begin point to the end point
    const Pt::int32_t vy = y2 - y1;           // ---
    const Pt::int32_t vz = 0;                 // ---
    const Pt::int32_t rx = 0;                 // Vector from the point of origin (0, 0, 0) that points out of the monitor
    const Pt::int32_t ry = 0;                 // ---
    const Pt::int32_t rz = 1;                 // ---
    const Pt::int32_t cx = vy * rz - vz * ry; // Cross product of the above vectors
    const Pt::int32_t cy = vz * rx - vx * rz; // ---
  //const Pt::int32_t cz = vx * ry - vy * rx; // ---

    // Determine where the direction that the hole faces to
    faceT = cy < 0;
    faceB = cy > 0;
    faceL = cx < 0;
    faceR = cx > 0;
}

inline void ImagePainter2::arcUtilMarkOutsideScanlinesRL(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, bool faceL, bool faceR, bool faceT, bool faceB, Scanlines& scanlines)
{
    // Mark the all scanlines to the left and right side that will be completely outside the shape
    const Pt::int32_t xlMin = std::min(x1, x2);
    const Pt::int32_t xlMax = std::max(x1, x2);

    // Smaller Y
    for(Scanlines::iterator it = scanlines.begin(); it != scanlines.lower_bound(std::min(y1, y2) + 1); ++it) {
        if(faceL && it->second.to < xlMin) {
            it->second.from =  Painter::MaximumCoordinate;
            it->second.to   = -Painter::MaximumCoordinate;
        }
        if(faceR && it->second.from > xlMax) {
            it->second.from =  Painter::MaximumCoordinate;
            it->second.to   = -Painter::MaximumCoordinate;
        }
    }

    // Larger Y
    for(Scanlines::iterator it = scanlines.upper_bound(std::max(y1, y2) - 1); it != scanlines.end(); ++it) {
        if(faceL && it->second.to < xlMin) {
            it->second.from =  Painter::MaximumCoordinate;
            it->second.to   = -Painter::MaximumCoordinate;
        }
        if(faceR && it->second.from > xlMax) {
            it->second.from =  Painter::MaximumCoordinate;
            it->second.to   = -Painter::MaximumCoordinate;
        }
    }
}
*/


} // namespace
} // namespace

#endif

