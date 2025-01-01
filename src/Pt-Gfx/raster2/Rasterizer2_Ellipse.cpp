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

#include "Rasterizer2.h"

namespace Pt {

namespace Gfx {

// Inspired by: Drawing Antialiased Circles and Ellipses
//              http://create.stephan-brumme.com/antialiased-circle
//              Original code by Stephan Brumme, 2011
void Rasterizer2::fillEllipse(const Point& topLeft, const Size& size)
{
    // Call the fast non-AA rasterizer as needed
    if( ! _aaMode ) {
        // Update the gradient as needed
        if(_isGradient)
            updateGradientBrush(size.width(), size.height());
        // Raster the ellipse
        rasterEllipseAreaNoAA(topLeft, size);
        return;
    }

    // Update the gradient as needed
    if(_isGradient)
        updateGradientBrush(size.width() + 2, size.height() + 2);

    // Calculate the ellipse's parameters
    const Pt::int32_t minX  = topLeft.x();
    const Pt::int32_t minY  = topLeft.y();

    const float       radX  = floor( size.width () * 0.5f );
    const float       radY  = floor( size.height() * 0.5f );

    const float       ctrX  = minX + radX;
    const float       ctrY  = minY + radY;

    const float       radX2 = radX * radX;
    const float       radY2 = radY * radY;

    // Adjustment for even sizes
    const bool        wEven = !(size.width () & 1);
    const bool        hEven = !(size.height() & 1);

    const Pt::int32_t sfX   = wEven ? 1 : 0;
    const Pt::int32_t sfY   = hEven ? 1 : 0;

    // === Process the scanlines ===

    // List of scanlines to be drawn later
    EAScanlines scanlines(radY * 2 + 2);

    // Top and bottom halves
    const Pt::int32_t quartersX = floor( radX2 * invSqrtf(radX2 + radY2) );

    for(Pt::int32_t x = 0; x <= quartersX; ++x) {
        // Adjustment for even size
        if(sfX && !x) continue;
        // Calculate the coordinates
        const float       y   = radY * sqrt(1 - (float) x * x / radX2);
        const Pt::int32_t fly = floor(y);
        const Pt::int32_t x1  = ctrX - x;
        const Pt::int32_t x2  = ctrX + x - sfX;
        const Pt::int32_t y1  = ctrY - fly - minY + 1;
        const Pt::int32_t y2  = ctrY + fly - minY + 1 - sfY;
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
    const Pt::int32_t quartersY = floor( radY2 * invSqrtf(radX2 + radY2) );

    for(Pt::int32_t y = 0; y <= quartersY; ++y) {
        // Adjustment for even size
        if(sfY && !y) continue;
        // Calculate the coordinates
        const float       x   = radX * sqrt(1 - (float) y * y / radY2);
        const Pt::int32_t flx = floor(x);
        const Pt::int32_t x1  = ctrX - flx;
        const Pt::int32_t x2  = ctrX + flx - sfX;
        const Pt::int32_t y1  = ctrY - y - minY + 1;
        const Pt::int32_t y2  = ctrY + y - minY + 1 - sfY;
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
        const ScanlineElement32& sle = scanlines[i];
        if(sle.isNull()) continue;
        rasterScanlineClipped(sle.from, sle.to, i + minY - 1, minX, minY);
    }

    scanlines.clear();

    // === Process the circumference's pixels ===

    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= quartersX; ++x) {
        // Adjustment for even size
        if(sfX && !x) continue;
        // Calculate the Y coordinate and alpha
        const float       y     = radY * sqrt(1 - (float) x * x / radX2);
        const Pt::int32_t fly   = floor(y);
        const float       error = y - fly;
        const Pt::uint8_t alpha = lround(error * 255);
        // Draw the pixels
        const Pt::int32_t x1 = ctrX - x;
        const Pt::int32_t x2 = ctrX + x - sfX;
        const Pt::int32_t y1 = ctrY - fly - 1;
        const Pt::int32_t y2 = ctrY + fly + 1 - sfY;
        fill4Pixels(x1, y1, x2, y2, minX - 1, minY - 1, alpha);
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= quartersY; ++y) {
        // Adjustment for even size
        if(sfY && !y) continue;
        // Calculate the X coordinate and alpha
        const float       x     = radX * sqrt(1 - (float) y * y / radY2);
        const Pt::int32_t flx   = floor(x);
        const float       error = x - flx;
        const Pt::uint8_t alpha = lround(error * 255);
        // Draw the pixels
        const Pt::int32_t x1 = ctrX - flx - 1;
        const Pt::int32_t x2 = ctrX + flx + 1 - sfX;
        const Pt::int32_t y1 = ctrY - y;
        const Pt::int32_t y2 = ctrY + y - sfY;
        fill4Pixels(x1, y1, x2, y2, minX - 1, minY - 1, alpha);
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
    const Pt::int32_t radX   =  size.width () / 2;
    const Pt::int32_t radY   =  size.height() / 2;
    const Pt::int32_t radX2  =  radX * radX;
    const Pt::int32_t radY2  =  radY * radY;
    const Pt::int32_t ctrX   =  minX + radX;
    const Pt::int32_t ctrY   =  minY + radY;
    const Pt::int32_t crit1  = -(radX2 / 4 + radX % 2 + radY2);
    const Pt::int32_t crit2  = -(radY2 / 4 + radY % 2 + radX2);
    const Pt::int32_t crit3  = -(radY2 / 4 + radY % 2     );
    const Pt::int32_t radX2t =  2 * radY2;
    const Pt::int32_t radY2t =  2 * radX2;
          Pt::int32_t incXt  =  0;
          Pt::int32_t incYt  = -2 * radX2 * radY;
          Pt::int32_t itrX   =  0;
          Pt::int32_t itrY   =  radY;
          Pt::int32_t itrT   = -radX2 * radY;
          Pt::int32_t itrW   =  1;

    while( itrY > 0 && itrX <= radX ) {
        if( (itrT + radY2 * itrX) <= crit1 || (itrT + radX2 * itrY) <= crit3 ) {
            ++itrX;
            incXt += radX2t;
            itrT  += incXt;
            itrW  += 2;
        }
        else if( (itrT - radX2 * itrY) > crit2 )  {
            rasterScanlineClipped(ctrX - itrX, ctrX - itrX + itrW - errorX - 1, ctrY - itrY,          minX, minY);
            rasterScanlineClipped(ctrX - itrX, ctrX - itrX + itrW - errorX - 1, ctrY + itrY - errorY, minX, minY);
            --itrY;
            incYt += radY2t;
            itrT  += incYt;
        }
        else {
            rasterScanlineClipped(ctrX - itrX, ctrX - itrX + itrW - errorX - 1, ctrY - itrY,          minX, minY);
            rasterScanlineClipped(ctrX - itrX, ctrX - itrX + itrW - errorX - 1, ctrY + itrY - errorY, minX, minY);
            ++itrX;
            incXt += radX2t;
            itrT  += incXt;
            itrW  += 2;
            --itrY;
            incYt += radY2t;
            itrT  += incYt;
        }
    }

    if( !errorY || !radY )
        rasterScanlineClipped(ctrX - radX,  ctrX + radX, ctrY, minX, minY);
}

} // namespace

} // namespace
