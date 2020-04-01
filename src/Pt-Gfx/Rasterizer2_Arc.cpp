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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include <map>

#include <Pt/Math.h>

#include "ClipShape.h"
#include "Rasterizer2.h"

namespace Pt {

namespace Gfx {


struct XY {
    Pt::int32_t x;
    Pt::int32_t y;

    inline XY(Pt::int32_t x_, Pt::int32_t y_)
    : x(x_), y(y_)
    {}

    inline bool operator < (const XY& c) const {
        if(x < c.x) return true;
        if(x > c.x) return false;
        return (y < c.y);
    }
};

typedef std::map<XY, Pt::int32_t> XYAlphaMap;


// Inspired by: Drawing Antialiased Circles and Ellipses
//              http://create.stephan-brumme.com/antialiased-circle
//              Original code by Stephan Brumme, 2011
void Rasterizer2::rasterNarrowArc(const Point& topLeft, const Size& size,
                                  float degBegin, float degEnd, const ArcMode& arcMode)
{
/*
    // Shall we draw an ellipse or arc?
    const bool drawArc = (degBegin != 0) || (degEnd != 0);

    if(drawArc) {
        // Ensure that the begin angle is within the acceptable range
        while(degBegin < -360) degBegin += 360;
        while(degBegin >  360) degBegin -= 360;
        // Ensure that the end angle is within the acceptable range
        while(degEnd < -360) degEnd += 360;
        while(degEnd >  360) degEnd -= 360;
    }
    else {
        degBegin =   0.0f;
        degEnd   = 360.0f;
    }

    // Calculate the ellipse's parameters
    const float minX = topLeft.x();
    const float minY = topLeft.y();
    const float radX = (size.width () - 1) * 0.5f;
    const float radY = (size.height() - 1) * 0.5f;
    const float ctrX = minX + radX;
    const float ctrY = minY + radY;

    const float       dTot = fabs(degEnd - degBegin);
    const float       cCir = 2.0f * Pt::pi<float>() * sqrtf( (radX * radX + radY * radY) * 0.5f );
    const Pt::int32_t cRes = ceil( cCir * dTot / 360.0f );

    const float dInc = degToRad(degEnd - degBegin) / float(cRes);
          float dItr = degToRad(degBegin);

    //fprintf(stderr, "BOX (%6.2f, %6.2f) (%6.2f, %6.2f)\n", (float) topLeft.x(), (float) topLeft.y(), (float) topLeft.x() + size.width() - 1, (float) topLeft.y() + size.height() - 1);
    //fprintf(stderr, "PAR (%6.2f, %6.2f) (%6.2f, %6.2f)\n\n", ctrX, ctrY, radX, radY);

    //
    XYAlphaMap xyam;

    for(Pt::int32_t i = 0; i < cRes; ++i) {
        //
        //  std::cerr << dItr << std::endl;
        const float xc = ctrX + radX * cos(dItr);
        const float yc = ctrY - radY * sin(dItr);
        dItr += dInc;
        //
        const Pt::int32_t xl = floor(xc);
        const Pt::int32_t yt = floor(yc);
        const Pt::int32_t xr = ceil (xc);
        const Pt::int32_t yb = ceil (yc);
        //
        const float       alphaXr = xc - xl;
        const float       alphaXl = 1.0f - alphaXr;
        const float       alphaYb = yc - yt;
        const float       alphaYt = 1.0f - alphaYb;
        const Pt::int32_t alphaLT = XWAA_WFILTER[ 255 - lround( alphaXl * alphaYt * 255.0f ) ];
        const Pt::int32_t alphaLB = XWAA_WFILTER[ 255 - lround( alphaXl * alphaYb * 255.0f ) ];
        const Pt::int32_t alphaRT = XWAA_WFILTER[ 255 - lround( alphaXr * alphaYt * 255.0f ) ];
        const Pt::int32_t alphaRB = XWAA_WFILTER[ 255 - lround( alphaXr * alphaYb * 255.0f ) ];

        //
        const bool xlValid = ClipShapeI::insideXRange(xl, _currentClip);
        const bool ytValid = ClipShapeI::insideYRange(yt, _currentClip);
        const bool xrValid = ClipShapeI::insideXRange(xr, _currentClip) && (xr != xl);
        const bool ybValid = ClipShapeI::insideYRange(yb, _currentClip) && (yb != yt);

        //
        Pt::int32_t& alt = xyam[XY(xl, yt)];
        Pt::int32_t& alb = xyam[XY(xl, yb)];
        Pt::int32_t& art = xyam[XY(xr, yt)];
        Pt::int32_t& arb = xyam[XY(xr, yb)];

        if(ytValid) {
            if(xlValid) {
                alt += alphaLT;
                if(alt > 255) alt = 255;
            }
            if(xrValid) {
                art += alphaRT;
                if(art > 255) art = 255;
            }
        }

        if(ybValid) {
            if(xlValid) {
                alb += alphaLB;
                if(alb > 255) alb = 255;
            }
            if(xrValid) {
                arb += alphaRB;
                if(arb > 255) arb = 255;
            }
        }
    }

    //
    for(XYAlphaMap::const_iterator it = xyam.begin(); it != xyam.end(); ++it) {
        Pixel pixel(_image->view(), it->first.x, it->first.y);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, it->second);
    }
*/

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

    const float       radX  = floor( size.width () * 0.5f );
    const float       radY  = floor( size.height() * 0.5f );

    const float       ctrX  = minX + radX;
    const float       ctrY  = minY + radY;

    const float       radX2 = radX * radX;
    const float       radY2 = radY * radY;

    const float       xyRat = radX / radY;

    // Adjustment for even sizes
    const bool        wEven = !(size.width () & 1);
    const bool        hEven = !(size.height() & 1);

    const Pt::int32_t sfX   = wEven ? 1 : 0;
    const Pt::int32_t sfY   = hEven ? 1 : 0;

    //fprintf(stderr, "BOX (%6.2f, %6.2f) (%6.2f, %6.2f)\n", (float) topLeft.x(), (float) topLeft.y(), (float) topLeft.x() + size.width() - 1, (float) topLeft.y() + size.height() - 1);
    //fprintf(stderr, "PAR (%6.2f, %6.2f) (%6.2f, %6.2f)\n\n", ctrX, ctrY, radX, radY);

    // Draw using solid pen?
    const bool solid = pen().isSolid();

    // Calculate the scaling factor for retrieving alphas from the pattern buffer
    const float pbScale = solid ? 1.0f : std::max(radX, radY) / (float) (64 / PATTERN_BUFFER_1P_SCALE_FACTOR);

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
        const float rBeg = degToRad(degBegin);
        bx = lround(ctrX + radX * std::cos(rBeg));
        by = lround(ctrY - radY * std::sin(rBeg)); // Sign inversion due to differences between cartesian and computer coordinate systems
        // Calculate the approximate coordinate of the point which is located at the end angle
        const float rEnd = degToRad(degEnd);
        ex = lround(ctrX + radX * std::cos(rEnd));
        ey = lround(ctrY - radY * std::sin(rEnd)); // Sign inversion due to differences between cartesian and computer coordinate systems
    }

    // Top and bottom halves
    const Pt::int32_t quartersX = Pt::lround( radX2 * invSqrtf(radX2 + radY2) );

    for(Pt::int32_t x = 0; x <= quartersX; ++x) {
        // Adjustment for even size
        if(sfX && !x) continue;
        // Calculate the coordinate and alpha
        const float       y     = radY * sqrt(1 - (float) x * x / radX2);
        const float       error = y - floor(y);
        const Pt::uint8_t alpha = lround(error * 255);
        // Without anti-aliasing
        if( ! _aaMode ) {
            // Calculate the coordinates
            const Pt::int32_t xl = ctrX - x;
            const Pt::int32_t xr = ctrX + x - sfX;
            const Pt::int32_t yt = ctrY - lround(y);
            const Pt::int32_t yb = ctrY + lround(y) - sfY;
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
                                          ? patternBuffer1PAlphaPolar(x, y, pbScale, xyRat)
                                          : patternBuffer1PAlphaPolar(x, y, pbScale       );
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
                                          ? patternBuffer1PAlphaPolar(x, y, pbScale, xyRat)
                                          : patternBuffer1PAlphaPolar(x, y, pbScale       );
                    if(pba) stroke4Pixels(xl, yt, xr, yb);
                }
            }
        }
        // With anti-aliasing
        else {
            // Calculate the coordinates
            const Pt::int32_t xl = ctrX - x;
            const Pt::int32_t xr = ctrX + x - sfX;
            const float       yt = ctrY - y;
            const float       yb = ctrY + y - sfY;
            const Pt::int32_t yt0 = ceil (yt);
            const Pt::int32_t yb0 = floor(yb);
            const Pt::int32_t yt1 = floor(yt);
            const Pt::int32_t yb1 = ceil (yb);
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
                    if(scaleWP) patternBuffer1PAlphaPolar(pba0, pba1, x, y, pbScale, xyRat, a0, a1);
                    else        patternBuffer1PAlphaPolar(pba0, pba1, x, y, pbScale,        a0, a1);
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
                    if(scaleWP) patternBuffer1PAlphaPolar(pba0, pba1, x, y, pbScale, xyRat, a0, a1);
                    else        patternBuffer1PAlphaPolar(pba0, pba1, x, y, pbScale,        a0, a1);
                    stroke4Pixels(xl, yt0, xr, yb0, pba0);
                    stroke4Pixels(xl, yt1, xr, yb1, pba1);
                }
            }
        }
    }

    // Left and right halves
    const Pt::int32_t quartersY = Pt::lround( radY2 * invSqrtf(radX2 + radY2) );

    for(Pt::int32_t y = 0; y <= quartersY; ++y) {
        // Adjustment for even size
        if(sfY && !y) continue;
        // Calculate the coordinate and alpha
        const float       x     = radX * sqrt(1 - (float) y * y / radY2);
        const float       error = x - floor(x);
        const Pt::uint8_t alpha = lround(error * 255);
        // Without anti-aliasing
        if( ! _aaMode ) {
            // Calculate the coordinates
            const Pt::int32_t xl = ctrX - lround(x);
            const Pt::int32_t xr = ctrX + lround(x) - sfX;
            const Pt::int32_t yt = ctrY - y;
            const Pt::int32_t yb = ctrY + y - sfY;
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
                                          ? patternBuffer1PAlphaPolar(x, y, pbScale, xyRat)
                                          : patternBuffer1PAlphaPolar(x, y, pbScale       );
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
                                          ? patternBuffer1PAlphaPolar(x, y, pbScale, xyRat)
                                          : patternBuffer1PAlphaPolar(x, y, pbScale       );
                    if(pba) stroke4Pixels(xl, yt, xr, yb);
                }
            }
        }
        // With anti-aliasing
        else {
            const float       xl  = ctrX - x;
            const float       xr  = ctrX + x - sfX;
            const Pt::int32_t xl0 = ceil (xl);
            const Pt::int32_t xr0 = floor(xr);
            const Pt::int32_t xl1 = floor(xl);
            const Pt::int32_t xr1 = ceil (xr);
            const Pt::int32_t yt  = ctrY - y;
            const Pt::int32_t yb  = ctrY + y - sfY;
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
                    if(scaleWP) patternBuffer1PAlphaPolar(pba0, pba1, x, y, pbScale, xyRat, a0, a1);
                    else        patternBuffer1PAlphaPolar(pba0, pba1, x, y, pbScale,        a0, a1);
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
                    if(scaleWP) patternBuffer1PAlphaPolar(pba0, pba1, x, y, pbScale, xyRat, a0, a1);
                    else        patternBuffer1PAlphaPolar(pba0, pba1, x, y, pbScale,        a0, a1);
                    stroke4Pixels(xl0, yt, xr0, yb, pba0);
                    stroke4Pixels(xl1, yt, xr1, yb, pba1);
                }
            }
        }
    }

    // Draw the arc's closing lines
    if(drawArc) {
        if(arcMode == ArcMode::Chord) {
            // Adjustment for even sizes
            if(wEven) {
                if(x1 > ctrX) x1 -= 1;
                if(x2 > ctrX) x2 -= 1;
            }

            if(hEven) {
                if(y1 > ctrY) y1 -= 1;
                if(y2 > ctrY) y2 -= 1;
            }            // Draw it
            const Point a(x1, y1);
            const Point b(x2, y2);
            drawNarrowLine(a, b, 0);
        }
        else if(arcMode == ArcMode::Pie) {
            // Adjustment for even sizes
            if(wEven) {
                if(bx > ctrX) bx -= 1;
                if(ex > ctrX) ex -= 1;
            }

            if(hEven) {
                if(by > ctrY) by -= 1;
                if(ey > ctrY) ey -= 1;
            }
            // Draw it
            Rasterizer2::DrawLineMask mask;
            const Point               a(bx,   by  );
            const Point               b(ex,   ey  );
            const Point               o(ctrX, ctrY);
            memcpy(mask, Rasterizer2::NullLineMask, sizeof(DrawLineMask));
            drawNarrowLine(a, o, &mask);
            drawNarrowLine(b, o, &mask);
        }
    }
}


void Rasterizer2::rasterArcArea(const Point& topLeft, const Size& size,
                                float degBegin, float degEnd, const ArcMode& arcMode)
{
    // Update the gradient as needed
    if(_isGradient)
        updateGradientBrush(size.width(), size.height());

    // Ensure that the begin angle is within the acceptable range
    while(degBegin < -360.0f) degBegin += 360.0f;
    while(degBegin >  360.0f) degBegin -= 360.0f;

    // Ensure that the end angle is within the acceptable range
    while(degEnd < -360.0f) degEnd += 360.0f;
    while(degEnd >  360.0f) degEnd -= 360.0f;

    // Calculate the general arc's parameters
    FilledArcInfo fai;

    fai.antiAlias = _aaMode;

    fai.degBegin  = degBegin;
    fai.degEnd    = degEnd;

    fai.minX      = topLeft.x();
    fai.minY      = topLeft.y();

    fai.radX      = floor( size.width () * 0.5f );
    fai.radY      = floor( size.height() * 0.5f );

    fai.ctrX      = fai.minX + fai.radX;
    fai.ctrY      = fai.minY + fai.radY;

    fai.radX2     = fai.radX * fai.radX;
    fai.radY2     = fai.radY * fai.radY;

    fai.xyRat     = (float) fai.radX / (float) fai.radY;

    fai.quartersX = Pt::lround( fai.radX2 * invSqrtf(fai.radX2 + fai.radY2) );
    fai.quartersY = Pt::lround( fai.radY2 * invSqrtf(fai.radX2 + fai.radY2) );

    fai.wEven     = !(size.width () & 1);
    fai.hEven     = !(size.height() & 1);

    // Find the exact coordinate of the begin and end point
    arcUtil_findExactBegEndPointsCoordinate(fai);

    // Draw based on the mode
         if(arcMode == ArcMode::Chord) rasterArcAreaChord(fai);
    else if(arcMode == ArcMode::Pie  ) rasterArcAreaPie  (fai);
}


void Rasterizer2::rasterArcAreaChord(FilledArcInfo& fai)
{
    // Calculate points for the closing line
    ArcXWLineData line;

    arcUtil_runXWLineAlgorithm(line, fai, fai.x1, fai.y1, fai.x2, fai.y2);

    // Find the direction that the line is facing to
    arcUtil_detXWLineDirection(line);

    // Generate the scanlines data
    EAScanlines scanlines(fai.radY * 2 + 2);

    arcUtil_genScanlinesForChord(scanlines, fai, line);

    // Draw the scanlines
    for(size_t i = 0; i < scanlines.size(); ++i) {
        const ScanlineElement32& sle = scanlines[i];
        if(sle.isNull()) continue;
        rasterScanlineClipped(sle.from, sle.to, i + fai.minY - 1, fai.minX, fai.minY);
    }

    scanlines.clear();

    // Exit here if we are not doing anti-aliasing
    if(!fai.antiAlias) return;

    // Draw the anti-aliased circumference pixels
    arcUtil_rasterCircumferencePixels(fai);

    // Draw the closing line
    Point maskInOut[4] = {
        maxPoint(), maxPoint(),
        maxPoint(), maxPoint()
    };

    arcUtil_rasterClosingXWLine(fai, line, maskInOut);
}


void Rasterizer2::rasterArcAreaPie(FilledArcInfo& fai)
{
    // Calculate points for the closing lines
    ArcXWLineData line1, line2;

    if(fai.x1 < fai.x2) {
        arcUtil_runXWLineAlgorithm(line1, fai, fai.x1, fai.y1, fai.ctrX, fai.ctrY);
        arcUtil_runXWLineAlgorithm(line2, fai, fai.ctrX, fai.ctrY, fai.x2, fai.y2);
    }
    else {
        arcUtil_runXWLineAlgorithm(line2, fai, fai.x1, fai.y1, fai.ctrX, fai.ctrY);
        arcUtil_runXWLineAlgorithm(line1, fai, fai.ctrX, fai.ctrY, fai.x2, fai.y2);
    }

    // Find the direction that the lines are facing to
    arcUtil_detXWLineDirection(line1);
    arcUtil_detXWLineDirection(line2);

    // Generate the scanlines data
    EAScanlines scanlines1(fai.radY * 2 + 2);
    EAScanlines scanlines2(fai.radY * 2 + 2);

    arcUtil_genScanlinesForPie(scanlines1, scanlines2, fai, line1, line2);

    // Draw the scanlines
    for(size_t i = 0; i < scanlines1.size(); ++i) {
        const ScanlineElement32& sle = scanlines1[i];
        if(sle.isNull()) continue;
        rasterScanlineClipped(sle.from, sle.to, i + fai.minY - 1, fai.minX, fai.minY);
    }

    for(size_t i = 0; i < scanlines2.size(); ++i) {
        const ScanlineElement32& sle = scanlines2[i];
        if(sle.isNull()) continue;
        if(!scanlines1[i].isNull() && sle.from >= scanlines1[i].from && sle.to <= scanlines1[i].to) continue;
        rasterScanlineClipped(sle.from, sle.to, i + fai.minY - 1, fai.minX, fai.minY);
    }

    scanlines1.clear();
    scanlines2.clear();

    // Exit here if we are not doing anti-aliasing
    if(!fai.antiAlias) return;

    // Draw the anti-aliased circumference pixels
    arcUtil_rasterCircumferencePixels(fai);

    // Draw the closing lines
    Point maskInOut[4] = {
        maxPoint(), maxPoint(),
        maxPoint(), maxPoint()
    };

    arcUtil_rasterClosingXWLine(fai, line2, maskInOut);
    arcUtil_rasterClosingXWLine(fai, line1, maskInOut);
}


void Rasterizer2::arcUtil_findExactBegEndPointsCoordinate(FilledArcInfo& fai)
{
    // NOTE: Gfx::Math::fastCos() and Gfx::Math::fastSin() will produce artifacts!

    // Calculate the approximate coordinate of the point which is located at the begin angle
    const float rBeg = fai.degBegin * DegToRadF;
    const Pt::int32_t bx = lround(fai.ctrX + fai.radX * ::cosf(rBeg));
    const Pt::int32_t by = lround(fai.ctrY - fai.radY * ::sinf(rBeg)); // Sign inversion due to differences between cartesian and computer coordinate systems

    // Calculate the approximate coordinate of the point which is located at the end angle
    const float rEnd = fai.degEnd * DegToRadF;
    const Pt::int32_t ex = lround(fai.ctrX + fai.radX * ::cosf(rEnd));
    const Pt::int32_t ey = lround(fai.ctrY - fai.radY * ::sinf(rEnd)); // Sign inversion due to differences between cartesian and computer coordinate systems

    // Used for finding the exact coordinate of the points which are located at the begin and end angle
    Pt::int32_t x1d = MAXIMUM_COORD; // Begin point
    Pt::int32_t y1d = MAXIMUM_COORD;
    Pt::int32_t x2d = MAXIMUM_COORD; // End point
    Pt::int32_t y2d = MAXIMUM_COORD;

    // Adjustment for even sizes
    const Pt::int32_t sfX = fai.wEven ? 1 : 0;
    const Pt::int32_t sfY = fai.hEven ? 1 : 0;

    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= fai.quartersX; ++x) {
        // Adjustment for even size
        if(sfX && !x) continue;
        // Calculate the coordinate
        const float       y  = fai.radY * sqrt(1 - (float) x * x / fai.radX2);
        const Pt::int32_t xl = fai.ctrX - x;
        const Pt::int32_t xr = fai.ctrX + x - sfX;
        const Pt::int32_t yt = fai.ctrY - ( fai.antiAlias ? Pt::lround(floor(y)) : lround(y) );
        const Pt::int32_t yb = fai.ctrY + ( fai.antiAlias ? Pt::lround(floor(y)) : lround(y) ) - sfY;
        // Determine the exact coordinates of the closing lines
        if(abs(xl - bx) < x1d) { x1d = abs(xl - bx); fai.x1 = xl; }
        if(abs(xl - ex) < x2d) { x2d = abs(xl - ex); fai.x2 = xl; }
        if(abs(xr - bx) < x1d) { x1d = abs(xr - bx); fai.x1 = xr; }
        if(abs(xr - ex) < x2d) { x2d = abs(xr - ex); fai.x2 = xr; }
        if(abs(yt - by) < y1d) { y1d = abs(yt - by); fai.y1 = yt; }
        if(abs(yt - ey) < y2d) { y2d = abs(yt - ey); fai.y2 = yt; }
        if(abs(yb - by) < y1d) { y1d = abs(yb - by); fai.y1 = yb; }
        if(abs(yb - ey) < y2d) { y2d = abs(yb - ey); fai.y2 = yb; }
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= fai.quartersY; ++y) {
        // Adjustment for even size
        if(sfY && !y) continue;
        // Calculate the coordinate
        const float       x  = fai.radX * sqrt(1 - (float) y * y / fai.radY2);
        const Pt::int32_t xl = fai.ctrX - ( fai.antiAlias ? Pt::lround(floor(x)) : lround(x) );
        const Pt::int32_t xr = fai.ctrX + ( fai.antiAlias ? Pt::lround(floor(x)) : lround(x) ) - sfX;
        const Pt::int32_t yt = fai.ctrY - y;
        const Pt::int32_t yb = fai.ctrY + y - sfY;
        // Determine the exact coordinates of the closing lines
        if(abs(xl - bx) < x1d) { x1d = abs(xl - bx); fai.x1 = xl; }
        if(abs(xl - ex) < x2d) { x2d = abs(xl - ex); fai.x2 = xl; }
        if(abs(xr - bx) < x1d) { x1d = abs(xr - bx); fai.x1 = xr; }
        if(abs(xr - ex) < x2d) { x2d = abs(xr - ex); fai.x2 = xr; }
        if(abs(yt - by) < y1d) { y1d = abs(yt - by); fai.y1 = yt; }
        if(abs(yt - ey) < y2d) { y2d = abs(yt - ey); fai.y2 = yt; }
        if(abs(yb - by) < y1d) { y1d = abs(yb - by); fai.y1 = yb; }
        if(abs(yb - ey) < y2d) { y2d = abs(yb - ey); fai.y2 = yb; }
    }
}

// Using algorithm from: Xiaolin Wu's Line Algorithm
//                       https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
//                       Last modified on January 19, 2017
void Rasterizer2::arcUtil_runXWLineAlgorithm(ArcXWLineData& xwLine, const FilledArcInfo& fai, Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2)
{
    // Copy the coordinates
    xwLine.x1 = x1;
    xwLine.y1 = y1;
    xwLine.x2 = x2;
    xwLine.y2 = y2;

    xwLine.minY = std::min(y1, y2);
    xwLine.maxY = std::max(y1, y2);

    // Prepare the buffer
    xwLine.points.resize(xwLine.maxY - xwLine.minY + 1 + 2);

    // Convert the coordinates to fixed-points
    Pt::int32_t fx1 = FIXED_POINT_FROM_INT(x1);
    Pt::int32_t fy1 = FIXED_POINT_FROM_INT(y1);
    Pt::int32_t fx2 = FIXED_POINT_FROM_INT(x2);
    Pt::int32_t fy2 = FIXED_POINT_FROM_INT(y2);

    // Swap the values as needed
    const Pt::int32_t deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
    const Pt::int32_t deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);

    xwLine.steep = deltaY > deltaX;

    if(xwLine.steep) {
        std::swap(fx1, fy1);
        std::swap(fx2, fy2);
    }

    xwLine.swapDir = (fx1 > fx2);

    if(xwLine.swapDir) {
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

    if(xwLine.steep) {
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            xwLine.points[ i - xwLine.minY + 1 ].push_back(
                ArcXWLineData::XWPoint( FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)), a1, a2 )
            );
            ypxli += grad;
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            xwLine.points[ FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)) - xwLine.minY + 1 ].push_back(
                ArcXWLineData::XWPoint( i, a1, a2 )
            );
            ypxli += grad;
        }
    }
}


void Rasterizer2::arcUtil_genScanlinesForChord(EAScanlines& scanlines, const FilledArcInfo& fai, const ArcXWLineData& xwLine)
{
    // Find the line's minimum and maximum Y coordinates
    Pt::int32_t lineMinY = fai.minY;
    Pt::int32_t lineMaxY = fai.minY + fai.radY * 2;

    // Face top
    if(xwLine.faceT) {
        if(xwLine.faceL) {
            const float angle = toPolar(
                (std::max(xwLine.x1, xwLine.x2) - fai.ctrX), -(xwLine.minY - fai.ctrY) * fai.xyRat
            );
            if((angle < 90 || angle > 180) && lineMinY < xwLine.minY + 1) lineMinY = xwLine.minY + 1;
        }
        else if(xwLine.faceR) {
            const float angle = toPolar(
                (std::min(xwLine.x1, xwLine.x2) - fai.ctrX), -(xwLine.minY - fai.ctrY) * fai.xyRat
            );
            if(angle > 90 && lineMinY < xwLine.minY + 1) lineMinY = xwLine.minY + 1;
        }
        else {
            if(lineMinY < xwLine.minY + 1) lineMinY = xwLine.minY + 1;
        }
    }

    // Face bottom
    if(xwLine.faceB) {
        if(xwLine.faceL) {
            const float angle = toPolar(
                (std::max(xwLine.x1, xwLine.x2) - fai.ctrX), -(xwLine.maxY - fai.ctrY) * fai.xyRat
            );
            if((angle < 180 || angle > 270) && lineMaxY > xwLine.maxY - 1) lineMaxY = xwLine.maxY - 1;
        }
        else if(xwLine.faceR) {
            const float angle = toPolar(
                (std::min(xwLine.x1, xwLine.x2) - fai.ctrX), -(xwLine.maxY - fai.ctrY) * fai.xyRat
            );
            if(angle < 270 && lineMaxY > xwLine.maxY - 1) lineMaxY = xwLine.maxY - 1;
        }
        else {
            if(lineMaxY > xwLine.maxY - 1) lineMaxY = xwLine.maxY - 1;
        }
    }

    // Minimum and maximum X coordinates of the shape
    const Pt::int32_t xlMin = std::min(fai.x1, fai.x2);
    const Pt::int32_t xlMax = std::max(fai.x1, fai.x2);

    // Adjustment for even sizes
    const Pt::int32_t sfX = fai.wEven ? 1 : 0;
    const Pt::int32_t sfY = fai.hEven ? 1 : 0;

    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= fai.quartersX; ++x) {
        // Adjustment for even size
        if(sfX && !x) continue;
        // Calculate the coordinate
        const float       y  = fai.radY * sqrt(1 - (float) x * x / fai.radX2);
        const Pt::int32_t yt = fai.ctrY - ( fai.antiAlias ? Pt::lround(floor(y)) : lround(y) );
        const Pt::int32_t yb = fai.ctrY + ( fai.antiAlias ? Pt::lround(floor(y)) : lround(y) ) - sfX;
        const Pt::int32_t xl = fai.ctrX - x;
        const Pt::int32_t xr = fai.ctrX + x - sfY;
        // Skip if the scanline will be completely outside the shape
        if(xwLine.faceL && xr < xlMin) continue;
        if(xwLine.faceR && xl > xlMax) continue;
        // Store/update the scanline coordinates
        arcUtil_cropAndStoreScanlineForChord(scanlines, fai, xwLine, lineMinY, lineMaxY, xl, xr, yt);
        arcUtil_cropAndStoreScanlineForChord(scanlines, fai, xwLine, lineMinY, lineMaxY, xl, xr, yb);
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= fai.quartersY; ++y) {
        // Adjustment for even size
        if(sfY && !y) continue;
        // Calculate the coordinate
        const float       x  = fai.radX * sqrt(1 - (float) y * y / fai.radY2);
        const Pt::int32_t yt = fai.ctrY - y;
        const Pt::int32_t yb = fai.ctrY + y - sfX;
        const Pt::int32_t xl = fai.ctrX - ( fai.antiAlias ? Pt::lround(floor(x)) : lround(x) );
        const Pt::int32_t xr = fai.ctrX + ( fai.antiAlias ? Pt::lround(floor(x)) : lround(x) ) - sfY;
        // Skip if the scanline will be completely outside the shape
        if(xwLine.faceL && xr < xlMin) continue;
        if(xwLine.faceR && xl > xlMax) continue;
        // Store/update the scanline coordinates
        arcUtil_cropAndStoreScanlineForChord(scanlines, fai, xwLine, lineMinY, lineMaxY, xl, xr, yt);
        arcUtil_cropAndStoreScanlineForChord(scanlines, fai, xwLine, lineMinY, lineMaxY, xl, xr, yb);
    }
}


void Rasterizer2::arcUtil_cropAndStoreScanlineForChord(EAScanlines& scanlines, const FilledArcInfo& fai, const ArcXWLineData& xwLine, Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t y)
{
    // For convenience
    typedef std::vector<ArcXWLineData::XWPoint> XWPoints;
    typedef XWPoints::const_iterator            XWPointsIterator;

    // Check if the scanline will be completely outside the shape
    if( (xwLine.faceT && y < lineMinY) || (xwLine.faceB && y > lineMaxY) ) return;

    // Get the element with the wanted coordinate from the closing line
    const Pt::int32_t lineY    = y - xwLine.minY + 1;
    const bool        lineYvld = ( lineY >= 0 && lineY < (Pt::int32_t) xwLine.points.size() );
    const XWPoints*   xwPoints = lineYvld ? &xwLine.points[lineY] : 0;
    XWPointsIterator  lit      = xwPoints ? xwPoints->end() : XWPointsIterator();
    if(xwPoints) {
        for(XWPointsIterator cit = xwPoints->begin(); cit != xwPoints->end(); ++cit) {
            if(cit->isNull()) continue;
            if( lit == xwPoints->end() || (xwLine.faceL && cit->x > lit->x) || (xwLine.faceR && cit->x < lit->x) ) lit = cit;
        }
    }

    // Crop the scanline coordinates to the closing line
    Pt::int32_t xlc = xl;
    Pt::int32_t xrc = xr;
    if(xwPoints && lit != xwPoints->end()) {
        if(xwLine.faceL) {
            if(xwLine.steep) { if(xlc <  lit->x + 1) xlc = lit->x + 1; } // (X), (X + 1)
            else             { if(xlc <= lit->x    ) xlc = lit->x + 1; } // (X)
        }
        if(xwLine.faceR) {
            if(xwLine.steep) { if(xrc >  lit->x) xrc = lit->x;     } // (X), (X + 1)
            else             { if(xrc >= lit->x) xrc = lit->x - 1; } // (X)
        }
    }

    // Store/update the scanline coordinates as needed
    if(xrc >= xlc) {
        y = y - fai.minY + 1;
        if(scanlines[y].isNull()) { // Insert a new element
            scanlines[y].from = xlc;
            scanlines[y].to   = xrc;
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( xlc < scanlines[y].from ) scanlines[y].from = xlc;
            if( xrc > scanlines[y].to   ) scanlines[y].to   = xrc;
        }
    }
}

void Rasterizer2::arcUtil_genScanlinesForPie(EAScanlines& scanlines1, EAScanlines& scanlines2, const FilledArcInfo& fai, const ArcXWLineData& xwLine1, const ArcXWLineData& xwLine2)
{
    // Find the line's minimum and maximum Y coordinates
    Pt::int32_t lineMinY = fai.minY;
    Pt::int32_t lineMaxY = fai.minY + fai.radY * 2;

    // Facing top-bottom or bottom-top
    if( (xwLine1.faceT && xwLine2.faceB) || (xwLine1.faceB && xwLine2.faceT) ) {
        Pt::int32_t y1avg = (xwLine1.y1 + xwLine1.y2) / 2;
        Pt::int32_t y2avg = (xwLine2.y1 + xwLine2.y2) / 2;
        if(y1avg < y2avg) { // The first line is on lower Y coordinate
            if(xwLine1.faceT) {
                if(lineMinY < xwLine1.minY) lineMinY = xwLine1.minY;
            }
            if(xwLine2.faceB) {
                if(lineMaxY > xwLine2.maxY) lineMaxY = xwLine2.maxY;
            }
        }
        else { // The second line is on lower Y coordinate
            if(xwLine1.faceB) {
                if(lineMaxY > xwLine1.maxY) lineMaxY = xwLine1.maxY;
            }
            if(xwLine2.faceT) {
                if(lineMinY < xwLine2.minY) lineMinY = xwLine2.minY;
            }
        }
    }
    // Facing other directions
    else {
        // Face top
        if(xwLine1.faceT && xwLine2.faceT) {
            const Pt::int32_t cy = std::min(xwLine1.minY, xwLine2.minY);
            if(lineMinY < cy) lineMinY = cy + 1;
        }
        else if(xwLine1.faceT) {
            if(lineMinY < xwLine1.minY) lineMinY = xwLine1.minY;
        }
        else if(xwLine2.faceT) {
            if(lineMinY < xwLine2.minY) lineMinY = xwLine2.minY;
        }
        // Face bottom
        if(xwLine1.faceB && xwLine2.faceB) {
            const Pt::int32_t cy = std::max(xwLine1.maxY, xwLine2.maxY);
            if(lineMaxY > cy) lineMaxY = cy - 1;
        }
        else if(xwLine1.faceB) {
            if(lineMaxY > xwLine1.maxY) lineMaxY = xwLine1.maxY;
        }
        else if(xwLine2.faceB) {
            if(lineMaxY > xwLine2.maxY) lineMaxY = xwLine2.maxY;
        }
    }

    // Adjustment for even sizes
    const Pt::int32_t sfX = fai.wEven ? 1 : 0;
    const Pt::int32_t sfY = fai.hEven ? 1 : 0;

    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= fai.quartersX; ++x) {
        // Adjustment for even size
        if(sfX && !x) continue;
        // Calculate the coordinate
        const float       y  = fai.radY * sqrt(1 - (float) x * x / fai.radX2);
        const Pt::int32_t yt = fai.ctrY - ( fai.antiAlias ? Pt::lround(floor(y)) : lround(y) );
        const Pt::int32_t yb = fai.ctrY + ( fai.antiAlias ? Pt::lround(floor(y)) : lround(y) ) - sfX;
        const Pt::int32_t xl = fai.ctrX - x;
        const Pt::int32_t xr = fai.ctrX + x - sfY;
        // Store/update the scanline coordinates
        arcUtil_cropAndStoreScanlineForPie(scanlines1, scanlines2, fai, xwLine1, xwLine2, lineMinY, lineMaxY, xl, xr, yt);
        arcUtil_cropAndStoreScanlineForPie(scanlines1, scanlines2, fai, xwLine1, xwLine2, lineMinY, lineMaxY, xl, xr, yb);
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= fai.quartersY; ++y) {
        // Adjustment for even size
        if(sfY && !y) continue;
        // Calculate the coordinate
        const float       x  = fai.radX * sqrt(1 - (float) y * y / fai.radY2);
        const Pt::int32_t yt = fai.ctrY - y;
        const Pt::int32_t yb = fai.ctrY + y - sfX;
        const Pt::int32_t xl = fai.ctrX - ( fai.antiAlias ? Pt::lround(floor(x)) : lround(x) );
        const Pt::int32_t xr = fai.ctrX + ( fai.antiAlias ? Pt::lround(floor(x)) : lround(x) ) - sfY;
        // Store/update the scanline coordinates
        arcUtil_cropAndStoreScanlineForPie(scanlines1, scanlines2, fai, xwLine1, xwLine2, lineMinY, lineMaxY, xl, xr, yt);
        arcUtil_cropAndStoreScanlineForPie(scanlines1, scanlines2, fai, xwLine1, xwLine2, lineMinY, lineMaxY, xl, xr, yb);
    }
}

void Rasterizer2::arcUtil_cropAndStoreScanlineForPie(EAScanlines& scanlines1, EAScanlines& scanlines2, const FilledArcInfo& fai, const ArcXWLineData& xwLine1, const ArcXWLineData& xwLine2, Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t y)
{
    // For convenience
    typedef std::vector<ArcXWLineData::XWPoint> XWPoints;
    typedef XWPoints::const_iterator            XWPointsIterator;

    // Check if the scanline will be completely outside the shape
    if( (xwLine1.faceT && y < lineMinY) || (xwLine2.faceT && y < lineMinY) ||
        (xwLine1.faceB && y > lineMaxY) || (xwLine2.faceB && y > lineMaxY)
    ) return;

    // Get the element with the wanted coordinate from the left-side closing line
    const Pt::int32_t lineY1    = y - xwLine1.minY + 1;
    const bool        lineYvld1 = ( lineY1 >= 0 && lineY1 < (Pt::int32_t) xwLine1.points.size() );
    const XWPoints*   xwPoints1 = lineYvld1 ? &xwLine1.points[lineY1] : 0;
    XWPointsIterator  lit1      = xwPoints1 ? xwPoints1->end() : XWPointsIterator();
    if(xwPoints1) {
        for(XWPointsIterator cit = xwPoints1->begin(); cit != xwPoints1->end(); ++cit) {
            if(cit->isNull()) continue;
            if( lit1 == xwPoints1->end() || (xwLine1.faceL && cit->x > lit1->x) || (xwLine1.faceR && cit->x < lit1->x) ) lit1 = cit;
        }
    }

    // Get the element with the wanted coordinate from the right-side closing line
    const Pt::int32_t lineY2    = y - xwLine2.minY + 1;
    const bool        lineYvld2 = ( lineY2 >= 0 && lineY2 < (Pt::int32_t) xwLine2.points.size() );
    const XWPoints*   xwPoints2 = lineYvld2 ? &xwLine2.points[lineY2] : 0;
    XWPointsIterator  lit2      = xwPoints2 ? xwPoints2->end() : XWPointsIterator();
    if(xwPoints2) {
        for(XWPointsIterator cit = xwPoints2->begin(); cit != xwPoints2->end(); ++cit) {
            if(cit->isNull()) continue;
            if( lit2 == xwPoints2->end() || (xwLine2.faceL && cit->x > lit2->x) || (xwLine2.faceR && cit->x < lit2->x) ) lit2 = cit;
        }
    }

    // Copy the coordinates for cropping
    Pt::int32_t xlc1 = xl;
    Pt::int32_t xrc1 = xr;

    Pt::int32_t xlc2 = -1; // By default we do not store a second scanline
    Pt::int32_t xrc2 = xr;

    // Both lines are facing left
    if( xwLine1.faceL && xwLine2.faceL ) {
        // Left
        if( xwPoints1 && lit1 != xwPoints1->end() && xwLine1.insideYRange(y) ) {
            if(xwLine1.steep) { if(xlc1 <  lit1->x + 1) xlc1 = lit1->x + 1; } // (X), (X + 1)
            else              { if(xlc1 <= lit1->x    ) xlc1 = lit1->x + 1; } // (X)
        }
        // Left
        if( xwPoints2 && lit2 != xwPoints2->end() && xwLine2.insideYRange(y) ) {
            if(xwLine2.steep) { if(xlc1 <  lit2->x + 1) xlc1 = lit2->x + 1; } // (X), (X + 1)
            else              { if(xlc1 <= lit2->x    ) xlc1 = lit2->x + 1; } // (X)
        }
    }

    // Both lines are facing right
    else if( xwLine1.faceR && xwLine2.faceR ) {
        // Right
        if( xwPoints1 && lit1 != xwPoints1->end() && xwLine1.insideYRange(y) ) {
            if(xwLine1.steep) { if(xrc1 >  lit1->x) xrc1 = lit1->x;     } // (X), (X + 1)
            else              { if(xrc1 >= lit1->x) xrc1 = lit1->x - 1; } // (X)
        }
        // Right
        if( xwPoints2 && lit2 != xwPoints2->end() && xwLine2.insideYRange(y) ) {
            if(xwLine2.steep) { if(xrc1 >  lit2->x) xrc1 = lit2->x;     } // (X), (X + 1)
            else              { if(xrc1 >= lit2->x) xrc1 = lit2->x - 1; } // (X)
        }
    }

    // Left-side line is facing left and right-side line is facing right
    else if( xwLine1.faceL && xwLine2.faceR ) {
        // Left
        if( xwPoints1 && lit1 != xwPoints1->end() && xwLine1.insideYRange(y) ) {
            if(xwLine1.steep) { if(xlc1 <  lit1->x + 1) xlc1 = lit1->x + 1; } // (X), (X + 1)
            else              { if(xlc1 <= lit1->x    ) xlc1 = lit1->x + 1; } // (X)
        }
        // Right
        if( xwPoints2 && lit2 != xwPoints2->end() && xwLine2.insideYRange(y) ) {
            if(xwLine2.steep) { if(xrc1 >  lit2->x) xrc1 = lit2->x;     } // (X), (X + 1)
            else              { if(xrc1 >= lit2->x) xrc1 = lit2->x - 1; } // (X)
        }
    }

    // Left-side line is facing right and right-side line is facing left => we have got a vertical "v-shape"
    else if( xwLine1.faceR && xwLine2.faceL ) {
        // Right
        if( xwPoints1 && lit1 != xwPoints1->end() && xwLine1.insideYRange(y) ) {
            if(xwLine1.steep) { if(xrc1 >  lit1->x) xrc1 = lit1->x;     } // (X), (X + 1)
            else              { if(xrc1 >= lit1->x) xrc1 = lit1->x - 1; } // (X)
        }
        // Left
        if( xwPoints2 && lit2 != xwPoints2->end() && xwLine2.insideYRange(y) ) {
            if(xwLine2.steep) { if(xlc2 <  lit2->x + 1) xlc2 = lit2->x + 1; } // (X), (X + 1)
            else              { if(xlc2 <= lit2->x    ) xlc2 = lit2->x + 1; } // (X)
        }
        if( xwPoints2 && lit2 != xwPoints2->end() && xwLine2.insideYRange(y) && !xwLine1.insideYRange(y) ) {
            if(xwLine2.steep) { if(xlc1 <  lit2->x + 1) xlc1 = lit2->x + 1; } // (X), (X + 1)
            else              { if(xlc1 <= lit2->x    ) xlc1 = lit2->x + 1; } // (X)
        }
    }

    // Other conditions
    else {
        // Left
        if( xwLine1.faceL ) {
            if( xwPoints1 && lit1 != xwPoints1->end() && xwLine1.insideYRange(y) ) {
                if(xwLine1.steep) { if(xlc1 <  lit1->x + 1) xlc1 = lit1->x + 1; } // (X), (X + 1)
                else              { if(xlc1 <= lit1->x    ) xlc1 = lit1->x + 1; } // (X)
            }
        }
        // Right
        else if( xwLine1.faceR ) {
            if( xwPoints1 && lit1 != xwPoints1->end() && xwLine1.insideYRange(y) ) {
                if(xwLine1.steep) { if(xrc1 >  lit1->x) xrc1 = lit1->x;     } // (X), (X + 1)
                else              { if(xrc1 >= lit1->x) xrc1 = lit1->x - 1; } // (X)
            }
        }
        // Left
        if( xwLine2.faceL ) {
            if( xwPoints2 && lit2 != xwPoints2->end() && xwLine2.insideYRange(y) ) {
                if(xwLine2.steep) { if(xlc1 <  lit2->x + 1) xlc1 = lit2->x + 1; } // (X), (X + 1)
                else              { if(xlc1 <= lit2->x    ) xlc1 = lit2->x + 1; } // (X)
            }
        }
        // Right
        else if( xwLine2.faceR ) {
            if( xwPoints2 && lit2 != xwPoints2->end() && xwLine2.insideYRange(y) ) {
                if(xwLine2.steep) { if(xrc1 >  lit2->x) xrc1 = lit2->x;     } // (X), (X + 1)
                else              { if(xrc1 >= lit2->x) xrc1 = lit2->x - 1; } // (X)
            }
        }
    }

    // Store/update the first scanline coordinates as needed
    y = y - fai.minY + 1;

    if(xrc1 >= xlc1) {
        if(scanlines1[y].isNull()) { // Insert a new element
            scanlines1[y].from = xlc1;
            scanlines1[y].to   = xrc1;
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( xlc1 < scanlines1[y].from ) scanlines1[y].from = xlc1;
            if( xrc1 > scanlines1[y].to   ) scanlines1[y].to   = xrc1;
        }
    }

    // Store/update the second scanline coordinates as needed
    if(xlc2 != -1 && xrc2 >= xlc2) {
        if(scanlines2[y].isNull()) { // Insert a new element
            scanlines2[y].from = xlc2;
            scanlines2[y].to   = xrc2;
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( xlc2 < scanlines2[y].from ) scanlines2[y].from = xlc2;
            if( xrc2 > scanlines2[y].to   ) scanlines2[y].to   = xrc2;
        }
    }
}

void Rasterizer2::arcUtil_rasterCircumferencePixels(FilledArcInfo& fai)
{
    // Adjustment for even sizes
    const Pt::int32_t sfX = fai.wEven ? 1 : 0;
    const Pt::int32_t sfY = fai.hEven ? 1 : 0;

    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= fai.quartersX; ++x) {
        // Adjustment for even size
        if(sfX && !x) continue;
        // Calculate the Y coordinate and alpha
        const float       y     = fai.radY * sqrt(1 - (float) x * x / fai.radX2);
        const Pt::int32_t fly   = Pt::lround(floor(y));
        const float       error = y - fly;
        const Pt::uint8_t alpha = lround(error * 255);
        // Draw the pixels
        const Pt::int32_t x1 = fai.ctrX - x;
        const Pt::int32_t x2 = fai.ctrX + x - sfX;
        const Pt::int32_t y1 = fai.ctrY - fly - 1;
        const Pt::int32_t y2 = fai.ctrY + fly + 1 - sfY;
        const Pt::uint8_t alphas[4] = {
            arcUtil_pointIsInsideDegRange(x1, y1, fai.ctrX, fai.ctrY, alpha, fai.degBegin, fai.degEnd, fai.xyRat),
            arcUtil_pointIsInsideDegRange(x1, y2, fai.ctrX, fai.ctrY, alpha, fai.degBegin, fai.degEnd, fai.xyRat),
            arcUtil_pointIsInsideDegRange(x2, y1, fai.ctrX, fai.ctrY, alpha, fai.degBegin, fai.degEnd, fai.xyRat),
            arcUtil_pointIsInsideDegRange(x2, y2, fai.ctrX, fai.ctrY, alpha, fai.degBegin, fai.degEnd, fai.xyRat)
        };
        fill4Pixels(x1, y1, x2, y2, fai.minX, fai.minY, alphas);
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= fai.quartersY; ++y) {
        // Adjustment for even size
        if(sfY && !y) continue;
        // Calculate the X coordinate and alpha
        const float       x     = fai.radX * sqrt(1 - (float) y * y / fai.radY2);
        const Pt::int32_t flx   = Pt::lround(floor(x));
        const float       error = x - flx;
        const Pt::uint8_t alpha = lround(error * 255);
        // Draw the pixels
        const Pt::int32_t x1 = fai.ctrX - flx - 1;
        const Pt::int32_t x2 = fai.ctrX + flx + 1 - sfX;
        const Pt::int32_t y1 = fai.ctrY - y;
        const Pt::int32_t y2 = fai.ctrY + y - sfY;
        Pt::uint8_t alphas[4] = {
            arcUtil_pointIsInsideDegRange(x1, y1, fai.ctrX, fai.ctrY, alpha, fai.degBegin, fai.degEnd, fai.xyRat),
            arcUtil_pointIsInsideDegRange(x1, y2, fai.ctrX, fai.ctrY, alpha, fai.degBegin, fai.degEnd, fai.xyRat),
            arcUtil_pointIsInsideDegRange(x2, y1, fai.ctrX, fai.ctrY, alpha, fai.degBegin, fai.degEnd, fai.xyRat),
            arcUtil_pointIsInsideDegRange(x2, y2, fai.ctrX, fai.ctrY, alpha, fai.degBegin, fai.degEnd, fai.xyRat)
        };
        fill4Pixels(x1, y1, x2, y2, fai.minX, fai.minY, alphas);
    }
}

void Rasterizer2::arcUtil_rasterClosingXWLine(const FilledArcInfo& fai, const ArcXWLineData& xwLine, Point maskInOut[4])
{
    // For convenience
    typedef std::vector<ArcXWLineData::XWPoint> XWPoints;
    typedef XWPoints::const_iterator            XWPointsIterator;

    // Get the mask's coordinate
    const Pt::int32_t mx[4] = {
        (Pt::int32_t) maskInOut[0].x(), (Pt::int32_t) maskInOut[1].x(),
        (Pt::int32_t) maskInOut[2].x(), (Pt::int32_t) maskInOut[3].x()
    };
    const Pt::int32_t my[4] = {
        (Pt::int32_t) maskInOut[0].y(), (Pt::int32_t) maskInOut[1].y(),
        (Pt::int32_t) maskInOut[2].y(), (Pt::int32_t) maskInOut[3].y()
    };

    // Used for storing back the mask's coordinates
    Pt::int32_t pCnt  = 0;
    Pt::int32_t lx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t ly[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    // A helper macro to skip a pixel
    #define CHECK_SKIP_PIXEL(X, Y, A)                       \
        {                                                   \
            /* Check if we should skip drawing the pixel */ \
            bool skipPixel = false;                         \
            for(Pt::int32_t i = 0; i < 4; ++i) {            \
                if(mx[i] != (X) || my[i] != (Y)) continue;  \
                skipPixel = true;                           \
                break;                                      \
            }                                               \
            if(skipPixel || !(A)) continue;                 \
            /* Store back the mask's coordinates */         \
            lx[2] = lx[3]; lx[3] = X;                       \
            ly[2] = ly[3]; ly[3] = Y;                       \
            if(pCnt < 2) {                                  \
                lx[pCnt] = X;                               \
                ly[pCnt] = Y;                               \
                ++pCnt;                                     \
            }                                               \
        }                                                   \
        do {} while(false)

    // Draw the pixels in all coordinates
    Pt::int32_t y = xwLine.minY - 1;

    for(std::vector<XWPoints>::const_iterator it = xwLine.points.begin(); it != xwLine.points.end(); ++it) {
        // Draw the pixels in this Y coordinate
        for(XWPointsIterator pt = it->begin(); pt != it->end(); ++pt) {
            // Skip if this is a null pixel
            if(pt->isNull()) continue;
            // Get the coordinate and alpha
            const Pt::int32_t x  = pt->x;
            const Pt::uint8_t a1 = pt->a1;
            const Pt::uint8_t a2 = pt->a2;
            // deltaY > deltaX
            if(xwLine.steep) {
                if( xwLine.faceL && (x != fai.x1 || y != fai.y1) && (x != fai.x2 || y != fai.y2) ) {
                    CHECK_SKIP_PIXEL(x, y, a1);
                    fillPixel(x, y, fai.minX, fai.minY, a1);
                }
                if( xwLine.faceR && (x + 1 != fai.x1 || y != fai.y1) && (x + 1 != fai.x2 || y != fai.y2) ) {
                    CHECK_SKIP_PIXEL(x + 1, y, a2);
                    fillPixel(x + 1, y, fai.minX, fai.minY, a2);
                }
            }
            // deltaY <= deltaX
            else {
                if( (x != fai.x1 || y != fai.y1) && (x != fai.x2 || y != fai.y2) ) {
                    if(xwLine.faceT) {
                        CHECK_SKIP_PIXEL(x, y, a1);
                        fillPixel(x, y, fai.minX, fai.minY, a1);
                    }
                    if(xwLine.faceB) {
                        CHECK_SKIP_PIXEL(x, y, a2);
                        fillPixel(x, y, fai.minX, fai.minY, a2);
                    }
                }
            }
        }
        // Increment the Y coordinate
        ++y;
    }

    // Store back the start and end coordinates to the mask
    if(xwLine.swapDir) {
        maskInOut[2].set(lx[0], ly[0]);
        maskInOut[3].set(lx[1], ly[1]);
        maskInOut[0].set(lx[2], ly[2]);
        maskInOut[1].set(lx[3], ly[3]);
    }
    else {
        maskInOut[0].set(lx[0], ly[0]);
        maskInOut[1].set(lx[1], ly[1]);
        maskInOut[2].set(lx[2], ly[2]);
        maskInOut[3].set(lx[3], ly[3]);
    }

    // Undefine the helper macro
    #undef CHECK_SKIP_PIXEL
}


void Rasterizer2::arcUtil_detXWLineDirection(ArcXWLineData& xwLineData)
{
    // Calculate the direction vector
    const Pt::int32_t vx = xwLineData.x2 - xwLineData.x1; // Vector from the begin point to the end point
    const Pt::int32_t vy = xwLineData.y2 - xwLineData.y1; //
    const Pt::int32_t vz = 0;                             //
    const Pt::int32_t rx = 0;                             // Vector from the point of origin (0, 0, 0) that points out of the monitor
    const Pt::int32_t ry = 0;                             //
    const Pt::int32_t rz = 1;                             //
    const Pt::int32_t cx = vy * rz - vz * ry;             // Cross product of the above two vectors
    const Pt::int32_t cy = vz * rx - vx * rz;             //
  //const Pt::int32_t cz = vx * ry - vy * rx;             //

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

    const float angle = toPolar( (x - ctrX), -(y - ctrY) * xyRatio );

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
    const float angle = toPolar(relX, -relY * xyRatio);
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
