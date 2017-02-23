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

#include <Pt/Gfx/ImagePainter2.h>

#include "FreeType.h"
#include "Rasterizer2.h"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

void ImagePainter2::drawArc( const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, const ArcMode& arcMode )
{
    drawOnePixelSolidEllipseArcImpl(topLeft, size, degBegin, degEnd, arcMode);
}

struct ImagePainter2::FilledArcInfo {
    bool        antiAlias;    // A flag that indicate if the arc will be anti-aliased

    Pt::int32_t minX, minY;   // Top-left coordinate of the arc
    Pt::int32_t ctrX, ctrY;   // Center coordinate of the arc
    Pt::int32_t radX, radY;   // Radius of the arc
    Pt::int32_t radX2, radY2; // Squared radius of the arc

    Pt::int32_t x1, y1;       // Coordinate of the begin point
    Pt::int32_t x2, y2;       // Coordinate of the end point

    Pt::int32_t quartersX;    // The number of quarter points in the X direction
    Pt::int32_t quartersY;    // The number of quarter points in the Y direction
};

struct ImagePainter2::XWLineData {
    struct XWPoint {
        Pt::int32_t x, y;
        Pt::uint8_t a1, a2;

        XWPoint(Pt::int32_t x_, Pt::int32_t y_, Pt::uint8_t a1_, Pt::uint8_t a2_)
        : x(x_), y(y_), a1(a1_), a2(a2_)
        {}
    };

    bool steep; // If "true"  then the a2 belongs to (x + 1, y)
                // If "false" then the a2 belongs to (x, y + 1)

    std::vector<XWPoint> points; // The line's points
};





void ImagePainter2::fillArc( const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, const ArcMode& arcMode )
{
    // Update the gradient as needed
    _rasterizer->updateGradientBrushAsNeeded(size.width(), size.height());

    // Ensure that the begin angle is within the acceptable range
    while(degBegin < -360) degBegin += 360;
    while(degBegin >  360) degBegin -= 360;

    // Ensure that the end angle is within the acceptable range
    while(degEnd < -360) degEnd += 360;
    while(degEnd >  360) degEnd -= 360;

    // Calculate the arc's parameters
    FilledArcInfo fai;

    fai.antiAlias = (_rasterizer->antiAliasingMode() != AntiAliasingMode::None);

    fai.minX      = topLeft.x();
    fai.minY      = topLeft.y();
    fai.radX      = size.width () / 2;
    fai.radY      = size.height() / 2;
    fai.radX2     = fai.radX * fai.radX;
    fai.radY2     = fai.radY * fai.radY;
    fai.ctrX      = fai.minX + fai.radX;
    fai.ctrY      = fai.minY + fai.radY;

    fai.quartersX = round( fai.radX2 * fastInvSqrt(fai.radX2 + fai.radY2) );
    fai.quartersY = round( fai.radY2 * fastInvSqrt(fai.radX2 + fai.radY2) );

    // Draw based on the mode
    switch(arcMode) {
        case ArcMode::Chord:
            //fillArcChordImpl(topLeft, size, degBegin, degEnd);
            break;

        case ArcMode::Pie:
            //fillArcPieImpl(topLeft, size, degBegin, degEnd);
            break;
    }

}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

// Xiaolin Wu's Anti-Aliased Line Algorithm
// https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
void ImagePainter2::arcUtil_runXWLineAlgorithm(XWLineData& dst, Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY)
{
    // Convert the coordinates to fixed-points
    Pt::int32_t fx1 = FIXED_POINT_FROM_INT(x1);
    Pt::int32_t fy1 = FIXED_POINT_FROM_INT(y1);
    Pt::int32_t fx2 = FIXED_POINT_FROM_INT(x2);
    Pt::int32_t fy2 = FIXED_POINT_FROM_INT(y2);

    // Swap the values as needed
    const Pt::int32_t deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
    const Pt::int32_t deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);

    dst.steep = deltaY > deltaX;

    if(dst.steep) {
        std::swap(fx1, fy1);
        std::swap(fx2, fy2);
    }

    if(fx1 > fx2) {
        std::swap(fx1, fx2);
        std::swap(fy1, fy2);
    }

    // Handle the gradient, starting point, and ending point
    const Pt::int32_t grad = (fy2 - fy1) / FIXED_POINT_TO_INT(fx2 - fx1);
    const Pt::int32_t xpxl1 = FIXED_POINT_ROUND(fx1);
    const Pt::int32_t xpxl2 = FIXED_POINT_ROUND(fx2);
    const Pt::int32_t ypxl  = fy1 + grad * FIXED_POINT_TO_INT(xpxl1 - fx1);

    // Calculate the pixels
    Pt::int32_t from  = FIXED_POINT_TO_INT(FIXED_POINT_ROUND(fx1));
    Pt::int32_t to    = FIXED_POINT_TO_INT(xpxl2);
    Pt::int32_t ypxli = ypxl;

    if(dst.steep) {
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            dst.points.push_back( XWLineData::XWPoint( FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)), i, a1, a2 ) );
            ypxli += grad;
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            dst.points.push_back( XWLineData::XWPoint( i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)), a1, a2 ) );
            ypxli += grad;
        }
    }
}


} // namespace
} // namespace
