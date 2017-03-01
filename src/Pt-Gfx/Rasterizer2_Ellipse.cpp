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

#include "Rasterizer2.h"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

// Inspired by: Drawing Antialiased Circles and Ellipses
//              http://create.stephan-brumme.com/antialiased-circle
//              Original code by Stephan Brumme, 2011
void Rasterizer2::strokeOnePixelEllipseArc(const Point& topLeft, const Size& size, float degBegin, float degEnd, const ArcMode& arcMode)
{
    // IMPORTANT NOTES:
    //     * The Y coordinate goes from low to high according to the coordinate system being used:
    //           - cartesian coordinate system: from the horizontal axis (the X axis) to the top;
    //           - computer  coordinate system: from the top of the screen to the bottom of the screen;
    //       This will cause sign inversion for trigonometry-based calculations in the Y coordinate.
    //     * The movement from begin angle to end angle must be in counter-clockwise (CCW), otherwise
    //       something wrong will be drawn.

    // Shall we draw an ellipse or arc?
    const bool drawArc = (degBegin != 0) || (degEnd != 0);

    // Calculate the ellipse's parameters
    const Pt::int32_t minX  = topLeft.x();
    const Pt::int32_t minY  = topLeft.y();
    const Pt::int32_t radX  = size.width () / 2;
    const Pt::int32_t radY  = size.height() / 2;
    const Pt::int32_t ctrX  = minX + radX;
    const Pt::int32_t ctrY  = minY + radY;
    const Pt::int32_t radX2 = radX * radX;
    const Pt::int32_t radY2 = radY * radY;
    const float       xyRat = (float) radX / (float) radY;

    // Draw using solid pen?
    const bool solid = (pen().style() == Pen::Solid);

    // Calculate the scaling factor for retrieving alphas from the pattern buffer
    const float pbScale = solid ? 1.0f : std::max(radX, radY) / (float) PATTERN_BUFFER_SCALE_FACTOR;

    // Determine we need to scale the indexes to the pattern buffer
    const bool scaleWP = (radX != radY);

    // Drawing an arc requires more parameters and calculation
    Pt::int32_t bx = 0, x1 = 0, x1d = MAXIMUM_COORD; // Begin point
    Pt::int32_t by = 0, y1 = 0, y1d = MAXIMUM_COORD;
    Pt::int32_t ex = 0, x2 = 0, x2d = MAXIMUM_COORD; // End point
    Pt::int32_t ey = 0, y2 = 0, y2d = MAXIMUM_COORD;

    if(drawArc) {
        // Ensure that the begin angle is within the acceptable range
        while(degBegin < -360) degBegin += 360;
        while(degBegin >  360) degBegin -= 360;
        // Ensure that the end angle is within the acceptable range
        while(degEnd < -360) degEnd += 360;
        while(degEnd >  360) degEnd -= 360;
        // Calculate the approximate coordinate of the point which is located at the begin angle
        bx = round(ctrX + radX * Gfx::Math::fastCos(degBegin * Pt::Pi / 180));
        by = round(ctrY - radY * Gfx::Math::fastSin(degBegin * Pt::Pi / 180)); // See the notes on the beginning of this function
        // Calculate the approximate coordinate of the point which is located at the end angle
        ex = round(ctrX + radX * Gfx::Math::fastCos(degEnd   * Pt::Pi / 180));
        ey = round(ctrY - radY * Gfx::Math::fastSin(degEnd   * Pt::Pi / 180)); // See the notes on the beginning of this function
    }

    // Top and bottom halves
    const Pt::int32_t quartersX = round( radX2 * Gfx::Math::fastInvSqrt(radX2 + radY2) );

    for(Pt::int32_t x = 0; x <= quartersX; ++x) {
        // Calculate the coordinate and alpha
        const float       y     = radY * Gfx::Math::fastSqrt(1 - (float) x * x / radX2);
        const float       error = y - floor(y);
        const Pt::uint8_t alpha = round(error * 255);
        // Without anti-aliasing
        if(_aaMode == AntiAliasingMode::None) {
            // Calculate the coordinates
            const Pt::int32_t xl = ctrX - x;
            const Pt::int32_t xr = ctrX + x;
            const Pt::int32_t yt = ctrY - round(y);
            const Pt::int32_t yb = ctrY + round(y);
            // Arc
            if(drawArc) {
                // Draw the pixels
                const bool mask[4] = {
                    arcUtil_pointIsInsideDegRange(xl, yt, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xl, yb, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xr, yt, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xr, yb, ctrX, ctrY, degBegin, degEnd, xyRat)
                };
                if(solid)
                    stroke4Pixels(xl, yt, xr, yb, mask);
                else {
                    const Pt::uint8_t pba = scaleWP
                                          ? patternBufferAlphaPolar(x, y, pbScale, xyRat)
                                          : patternBufferAlphaPolar(x, y, pbScale       );
                    if(pba) stroke4Pixels(xl, yt, xr, yb, mask);
                }
                // Determine the exact coordinates of the closing lines
                if(arcMode == ArcMode::Open) continue;
                if(abs(xl - bx) < x1d) { x1d = abs(xl - bx); x1 = xl; }
                if(abs(xl - ex) < x2d) { x2d = abs(xl - ex); x2 = xl; }
                if(abs(xr - bx) < x1d) { x1d = abs(xr - bx); x1 = xr; }
                if(abs(xr - ex) < x2d) { x2d = abs(xr - ex); x2 = xr; }
                if(abs(yt - by) < y1d) { y1d = abs(yt - by); y1 = yt; }
                if(abs(yt - ey) < y2d) { y2d = abs(yt - ey); y2 = yt; }
                if(abs(yb - by) < y1d) { y1d = abs(yb - by); y1 = yb; }
                if(abs(yb - ey) < y2d) { y2d = abs(yb - ey); y2 = yb; }
            }
            // Ellipse
            else {
                if(solid)
                   stroke4Pixels(xl, yt, xr, yb);
                else {
                    const Pt::uint8_t pba = scaleWP
                                          ? patternBufferAlphaPolar(x, y, pbScale, xyRat)
                                          : patternBufferAlphaPolar(x, y, pbScale       );
                    if(pba) stroke4Pixels(xl, yt, xr, yb);
                }
            }
        }
        // With anti-aliasing
        else {
            // Calculate the coordinates
            const Pt::int32_t xl  = ctrX - x;
            const Pt::int32_t xr  = ctrX + x;
            const Pt::int32_t yt0 = ctrY - floor(y);
            const Pt::int32_t yb0 = ctrY + floor(y);
            const Pt::int32_t yt1 = ctrY - floor(y) - 1;
            const Pt::int32_t yb1 = ctrY + floor(y) + 1;
            // Arc
            if(drawArc) {
                // Draw the pixels
                const bool mask0[4] = {
                    arcUtil_pointIsInsideDegRange(xl, yt0, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xl, yb0, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xr, yt0, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xr, yb0, ctrX, ctrY, degBegin, degEnd, xyRat)
                };
                const bool mask1[4] = {
                    arcUtil_pointIsInsideDegRange(xl, yt1, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xl, yb1, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xr, yt1, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xr, yb1, ctrX, ctrY, degBegin, degEnd, xyRat)
                };
                const Pt::uint8_t a0 = Rasterizer2::XWAA_WFILTER[      alpha];
                const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[255 - alpha];
                if(solid) {
                    stroke4Pixels(xl, yt0, xr, yb0, a0, mask0);
                    stroke4Pixels(xl, yt1, xr, yb1, a1, mask1);
                }
                else {
                    Pt::uint8_t pba0, pba1;
                    if(scaleWP) patternBufferAlphaPolar(pba0, pba1, x, y, pbScale, xyRat, a0, a1);
                    else        patternBufferAlphaPolar(pba0, pba1, x, y, pbScale,        a0, a1);
                    stroke4Pixels(xl, yt0, xr, yb0, pba0, mask0);
                    stroke4Pixels(xl, yt1, xr, yb1, pba1, mask1);
                }
                // Determine the exact coordinates of the closing lines
                if(arcMode == ArcMode::Open) continue;
                if(abs(xl  - bx) < x1d) { x1d = abs(xl  - bx); x1 = xl;  }
                if(abs(xl  - ex) < x2d) { x2d = abs(xl  - ex); x2 = xl;  }
                if(abs(xr  - bx) < x1d) { x1d = abs(xr  - bx); x1 = xr;  }
                if(abs(xr  - ex) < x2d) { x2d = abs(xr  - ex); x2 = xr;  }
                if(abs(yt0 - by) < y1d) { y1d = abs(yt0 - by); y1 = yt0; }
                if(abs(yt0 - ey) < y2d) { y2d = abs(yt0 - ey); y2 = yt0; }
                if(abs(yb0 - by) < y1d) { y1d = abs(yb0 - by); y1 = yb0; }
                if(abs(yb0 - ey) < y2d) { y2d = abs(yb0 - ey); y2 = yb0; }
                if(abs(yt1 - by) < y1d) { y1d = abs(yt1 - by); y1 = yt1; }
                if(abs(yt1 - ey) < y2d) { y2d = abs(yt1 - ey); y2 = yt1; }
                if(abs(yb1 - by) < y1d) { y1d = abs(yb1 - by); y1 = yb1; }
                if(abs(yb1 - ey) < y2d) { y2d = abs(yb1 - ey); y2 = yb1; }
            }
            // Ellipse
            else {
                const Pt::uint8_t a0 = Rasterizer2::XWAA_WFILTER[      alpha];
                const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[255 - alpha];
                if(solid) {
                    stroke4Pixels(xl, yt0, xr, yb0, a0);
                    stroke4Pixels(xl, yt1, xr, yb1, a1);
                }
                else {
                    Pt::uint8_t pba0, pba1;
                    if(scaleWP) patternBufferAlphaPolar(pba0, pba1, x, y, pbScale, xyRat, a0, a1);
                    else        patternBufferAlphaPolar(pba0, pba1, x, y, pbScale,        a0, a1);
                    stroke4Pixels(xl, yt0, xr, yb0, pba0);
                    stroke4Pixels(xl, yt1, xr, yb1, pba1);
                }
            }
        }
    }

    // Left and right halves
    const Pt::int32_t quartersY = round( radY2 * Gfx::Math::fastInvSqrt(radX2 + radY2) );

    for(Pt::int32_t y = 0; y <= quartersY; ++y) {
        // Calculate the coordinate and alpha
        const float       x     = radX * Gfx::Math::fastSqrt(1 - (float) y * y / radY2);
        const float       error = x - floor(x);
        const Pt::uint8_t alpha = round(error * 255);
        // Without anti-aliasing
        if(_aaMode == AntiAliasingMode::None) {
            // Calculate the coordinates
            const Pt::int32_t xl = ctrX - round(x);
            const Pt::int32_t xr = ctrX + round(x);
            const Pt::int32_t yt = ctrY - y;
            const Pt::int32_t yb = ctrY + y;
            // Arc
            if(drawArc) {
                // Draw the pixels
                const bool mask[4] = {
                    arcUtil_pointIsInsideDegRange(xl, yt, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xl, yb, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xr, yt, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xr, yb, ctrX, ctrY, degBegin, degEnd, xyRat)
                };
                if(solid)
                    stroke4Pixels(xl, yt, xr, yb, mask);
                else {
                    const Pt::uint8_t pba = scaleWP
                                          ? patternBufferAlphaPolar(x, y, pbScale, xyRat)
                                          : patternBufferAlphaPolar(x, y, pbScale       );
                    if(pba) stroke4Pixels(xl, yt, xr, yb, mask);
                }
                // Determine the exact coordinates of the closing lines
                if(arcMode == ArcMode::Open) continue;
                if(abs(xl - bx) < x1d) { x1d = abs(xl - bx); x1 = xl; }
                if(abs(xl - ex) < x2d) { x2d = abs(xl - ex); x2 = xl; }
                if(abs(xr - bx) < x1d) { x1d = abs(xr - bx); x1 = xr; }
                if(abs(xr - ex) < x2d) { x2d = abs(xr - ex); x2 = xr; }
                if(abs(yt - by) < y1d) { y1d = abs(yt - by); y1 = yt; }
                if(abs(yt - ey) < y2d) { y2d = abs(yt - ey); y2 = yt; }
                if(abs(yb - by) < y1d) { y1d = abs(yb - by); y1 = yb; }
                if(abs(yb - ey) < y2d) { y2d = abs(yb - ey); y2 = yb; }
            }
            // Ellipse
            else {
                if(solid)
                    stroke4Pixels(xl, yt, xr, yb);
                else {
                    const Pt::uint8_t pba = scaleWP
                                          ? patternBufferAlphaPolar(x, y, pbScale, xyRat)
                                          : patternBufferAlphaPolar(x, y, pbScale       );
                    if(pba) stroke4Pixels(xl, yt, xr, yb);
                }
            }
        }
        // With anti-aliasing
        else {
            // Calculate the coordinates
            const Pt::int32_t xl0 = ctrX - floor(x);
            const Pt::int32_t xr0 = ctrX + floor(x);
            const Pt::int32_t xl1 = ctrX - floor(x) - 1;
            const Pt::int32_t xr1 = ctrX + floor(x) + 1;
            const Pt::int32_t yt  = ctrY - y;
            const Pt::int32_t yb  = ctrY + y;
            // Arc
            if(drawArc) {
                // Draw the pixels
                const bool mask0[4] = {
                    arcUtil_pointIsInsideDegRange(xl0, yt, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xl0, yb, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xr0, yt, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xr0, yb, ctrX, ctrY, degBegin, degEnd, xyRat)
                };
                const bool mask1[4] = {
                    arcUtil_pointIsInsideDegRange(xl1, yt, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xl1, yb, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xr1, yt, ctrX, ctrY, degBegin, degEnd, xyRat),
                    arcUtil_pointIsInsideDegRange(xr1, yb, ctrX, ctrY, degBegin, degEnd, xyRat)
                };
                const Pt::uint8_t a0 = Rasterizer2::XWAA_WFILTER[      alpha];
                const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[255 - alpha];
                if(solid) {
                    stroke4Pixels(xl0, yt, xr0, yb, a0, mask0);
                    stroke4Pixels(xl1, yt, xr1, yb, a1, mask1);
                }
                else {
                    Pt::uint8_t pba0, pba1;
                    if(scaleWP) patternBufferAlphaPolar(pba0, pba1, x, y, pbScale, xyRat, a0, a1);
                    else        patternBufferAlphaPolar(pba0, pba1, x, y, pbScale,        a0, a1);
                    stroke4Pixels(xl0, yt, xr0, yb, pba0, mask0);
                    stroke4Pixels(xl1, yt, xr1, yb, pba1, mask1);
                }
                // Determine the exact coordinates of the closing lines
                if(arcMode == ArcMode::Open) continue;
                if(abs(xl0 - bx) < x1d) { x1d = abs(xl0 - bx); x1 = xl0; }
                if(abs(xl0 - ex) < x2d) { x2d = abs(xl0 - ex); x2 = xl0; }
                if(abs(xr0 - bx) < x1d) { x1d = abs(xr0 - bx); x1 = xr0; }
                if(abs(xr0 - ex) < x2d) { x2d = abs(xr0 - ex); x2 = xr0; }
                if(abs(xl1 - bx) < x1d) { x1d = abs(xl1 - bx); x1 = xl1; }
                if(abs(xl1 - ex) < x2d) { x2d = abs(xl1 - ex); x2 = xl1; }
                if(abs(xr1 - bx) < x1d) { x1d = abs(xr1 - bx); x1 = xr1; }
                if(abs(xr1 - ex) < x2d) { x2d = abs(xr1 - ex); x2 = xr1; }
                if(abs(yt  - by) < y1d) { y1d = abs(yt  - by); y1 = yt;  }
                if(abs(yt  - ey) < y2d) { y2d = abs(yt  - ey); y2 = yt;  }
                if(abs(yb  - by) < y1d) { y1d = abs(yb  - by); y1 = yb;  }
                if(abs(yb  - ey) < y2d) { y2d = abs(yb  - ey); y2 = yb;  }
            }
            // Ellipse
            else {
                const Pt::uint8_t a0 = Rasterizer2::XWAA_WFILTER[      alpha];
                const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[255 - alpha];
                if(solid) {
                    stroke4Pixels(xl0, yt, xr0, yb, a0);
                    stroke4Pixels(xl1, yt, xr1, yb, a1);
                }
                else {
                    Pt::uint8_t pba0, pba1;
                    if(scaleWP) patternBufferAlphaPolar(pba0, pba1, x, y, pbScale, xyRat, a0, a1);
                    else        patternBufferAlphaPolar(pba0, pba1, x, y, pbScale,        a0, a1);
                    stroke4Pixels(xl0, yt, xr0, yb, pba0);
                    stroke4Pixels(xl1, yt, xr1, yb, pba1);
                }
            }
        }
    }

    // Draw the arc's closing lines
    if(drawArc) {
        if(arcMode == ArcMode::Chord) {
            const Point a(x1, y1);
            const Point b(x2, y2);
            strokeOnePixelLine(a, b, 0);
        }
        else if(arcMode == ArcMode::Pie) {
            Rasterizer2::DrawLineMask mask = Rasterizer2::NullLineMask;
            const Point               a(bx,   by  );
            const Point               b(ex,   ey  );
            const Point               o(ctrX, ctrY);
            strokeOnePixelLine(a, o, &mask);
            strokeOnePixelLine(b, o, &mask);
        }
    }
}

// Inspired by: Drawing Antialiased Circles and Ellipses
//              http://create.stephan-brumme.com/antialiased-circle
//              Original code by Stephan Brumme, 2011
void Rasterizer2::fillEllipse(const Point& topLeft, const Size& size)
{
    // Update the gradient as needed
    if(_isGradient)
        updateGradientBrush(size.width(), size.height());

    // Call the fast non-AA rasterizer as needed
    if(_aaMode == AntiAliasingMode::None) {
        rasterEllipseAreaNoAA(topLeft, size);
        return;
    }

    // Calculate the ellipse's parameters
    Pt::int32_t minX  = topLeft.x();
    Pt::int32_t minY  = topLeft.y();
    Pt::int32_t radX  = size.width () / 2;
    Pt::int32_t radY  = size.height() / 2;
    Pt::int32_t ctrX  = minX + radX;
    Pt::int32_t ctrY  = minY + radY;
    Pt::int32_t radX2 = radX * radX;
    Pt::int32_t radY2 = radY * radY;

    // === Process the scanlines ===

    // List of scanlines to be drawn later
    Scanlines scanlines(radY * 2 + 2);

    // Top and bottom halves
    const Pt::int32_t quartersX = round( radX2 * Gfx::Math::fastInvSqrt(radX2 + radY2) );

    for(Pt::int32_t x = 0; x <= quartersX; ++x) {
        // Calculate the coordinates
        const float       y   = radY * Gfx::Math::fastSqrt(1 - (float) x * x / radX2);
        const Pt::int32_t fly = floor(y);
        const Pt::int32_t x1  = ctrX - x;
        const Pt::int32_t x2  = ctrX + x;
        const Pt::int32_t y1  = ctrY - fly - minY + 1;
        const Pt::int32_t y2  = ctrY + fly - minY + 1;
        // Store/update the scanline coordinates
        if(scanlines[y1].isNull()) { // Insert a new element
            scanlines[y1].from = x1;
            scanlines[y1].to   = x2;
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( x1 < scanlines[y1].from ) scanlines[y1].from = x1;
            if( x2 > scanlines[y1].to   ) scanlines[y1].to   = x2;
        }
        if(scanlines[y2].isNull()) { // Insert a new element
            scanlines[y2].from = x1;
            scanlines[y2].to   = x2;
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( x1 < scanlines[y2].from ) scanlines[y2].from = x1;
            if( x2 > scanlines[y2].to   ) scanlines[y2].to   = x2;
        }
    }

    // Left and right halves
    const Pt::int32_t quartersY = round( radY2 * Gfx::Math::fastInvSqrt(radX2 + radY2) );

    for(Pt::int32_t y = 0; y <= quartersY; ++y) {
        // Calculate the coordinates
        const float       x   = radX * Gfx::Math::fastSqrt(1 - (float) y * y / radY2);
        const Pt::int32_t flx = floor(x);
        const Pt::int32_t x1  = ctrX - flx;
        const Pt::int32_t x2  = ctrX + flx;
        const Pt::int32_t y1  = ctrY - y - minY + 1;
        const Pt::int32_t y2  = ctrY + y - minY + 1;
        // Store/update the scanline coordinates
        if(scanlines[y1].isNull()) { // Insert a new element
            scanlines[y1].from = x1;
            scanlines[y1].to   = x2;
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( x1 < scanlines[y1].from ) scanlines[y1].from = x1;
            if( x2 > scanlines[y1].to   ) scanlines[y1].to   = x2;
        }
        if(scanlines[y2].isNull()) { // Insert a new element
            scanlines[y2].from = x1;
            scanlines[y2].to   = x2;
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( x1 < scanlines[y2].from ) scanlines[y2].from = x1;
            if( x2 > scanlines[y2].to   ) scanlines[y2].to   = x2;
        }
    }

    // Draw the scanlines
    for(size_t i = 0; i < scanlines.size(); ++i) {
        const ScanlineElement& sle = scanlines[i];
        if(sle.isNull()) continue;
        fillOneScanlineNoAA(sle.from, sle.to, i + minY - 1, minX, minY);
    }

    scanlines.clear();

    // === Process the circumference's pixels ===

    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= quartersX; ++x) {
        // Calculate the Y coordinate and alpha
        const float       y     = radY * Gfx::Math::fastSqrt(1 - (float) x * x / radX2);
        const Pt::int32_t fly   = floor(y);
        const float       error = y - fly;
        const Pt::uint8_t alpha = round(error * 255);
        // Draw the pixels
        const Pt::int32_t x1 = ctrX - x;
        const Pt::int32_t x2 = ctrX + x;
        const Pt::int32_t y1 = ctrY - fly - 1;
        const Pt::int32_t y2 = ctrY + fly + 1;
        fill4Pixels(x1, y1, x2, y2, minX, minY, alpha);
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= quartersY; ++y) {
        // Calculate the X coordinate and alpha
        const float       x     = radX * Gfx::Math::fastSqrt(1 - (float) y * y / radY2);
        const Pt::int32_t flx   = floor(x);
        const float       error = x - flx;
        const Pt::uint8_t alpha = round(error * 255);
        // Draw the pixels
        const Pt::int32_t x1 = ctrX - flx - 1;
        const Pt::int32_t x2 = ctrX + flx + 1;
        const Pt::int32_t y1 = ctrY - y;
        const Pt::int32_t y2 = ctrY + y;
        fill4Pixels(x1, y1, x2, y2, minX, minY, alpha);
    }
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void Rasterizer2::rasterEllipseAreaNoAA(const Point& topLeft, const Size& size)
{
    // Draw the ellipse's scanlines as per this equation:
    //     e(X, Y) = ( b^2 * X^2 ) + ( a^2 * Y^2 ) - ( a^2 * b^2 )

    const Pt::int32_t minX   =  topLeft.x();
    const Pt::int32_t minY   =  topLeft.y();
    const Pt::int32_t errorX = ((Pt::int32_t) size.width () % 2) ? 0 : 1;
    const Pt::int32_t errorY = ((Pt::int32_t) size.height() % 2) ? 0 : 1;
    const Pt::int32_t a      =  size.width () / 2;
    const Pt::int32_t b      =  size.height() / 2;
    const Pt::int32_t a2     =  a * a;
    const Pt::int32_t b2     =  b * b;
    const Pt::int32_t xc     =  minX + a;
    const Pt::int32_t yc     =  minY + b;
    const Pt::int32_t crit1  = -(a2 / 4 + a % 2 + b2);
    const Pt::int32_t crit2  = -(b2 / 4 + b % 2 + a2);
    const Pt::int32_t crit3  = -(b2 / 4 + b % 2     );
    const Pt::int32_t d2xt   =  2 * b2;
    const Pt::int32_t d2yt   =  2 * a2;
          Pt::int32_t dxt    =  0;
          Pt::int32_t dyt    = -2 * a2 * b;
          Pt::int32_t x      =  0;
          Pt::int32_t y      =  b;
          Pt::int32_t width  =  1;
          Pt::int32_t t      = -a2 * b;

    while( y > 0 && x <= a ) {
        if( (t + b2 * x) <= crit1 || (t + a2 * y) <= crit3 ) {
            ++x;
            dxt   += d2xt;
            t     += dxt;
            width += 2;
        }
        else if( (t - a2 * y) > crit2 )  {
            fillOneScanlineNoAA(xc - x, xc - x + width - errorX - 1, yc - y,          minX, minY);
            fillOneScanlineNoAA(xc - x, xc - x + width - errorX - 1, yc + y - errorY, minX, minY);
            --y;
            dyt += d2yt;
            t   += dyt;
        }
        else {
            fillOneScanlineNoAA(xc - x, xc - x + width - errorX - 1, yc - y,          minX, minY);
            fillOneScanlineNoAA(xc - x, xc - x + width - errorX - 1, yc + y - errorY, minX, minY);
            ++x;
            dxt   += d2xt;
            t     += dxt;
            width += 2;
            --y;
            dyt   += d2yt;
            t     += dyt;
        }
    }

    if( !errorY || !b )
        fillOneScanlineNoAA(xc - a,  xc + a, yc, minX, minY);
}


} // namespace
} // namespace
