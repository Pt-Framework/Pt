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

    // Calculate the general arc's parameters
    FilledArcInfo fai;

    fai.antiAlias = (_rasterizer->antiAliasingMode() != AntiAliasingMode::None);

    fai.degBegin  = degBegin;
    fai.degEnd    = degEnd;

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

    // Find the exact coordinate of the begin and end point
    arcUtil_findExactBegEndPointsCoordinate(fai);

    // Draw based on the mode
    switch(arcMode) {
        case ArcMode::Chord:
            fillArcChordImpl(fai);
            break;

        case ArcMode::Pie:
            fillArcPieImpl(fai);
            drawArc(topLeft, size, degBegin, degEnd, arcMode); // Just for easier debugging & verification
            break;
    }

}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void ImagePainter2::fillArcChordImpl(FilledArcInfo& fai)
{
    // Calculate points for the closing line
    XWLineData line;
    arcUtil_runXWLineAlgorithm(line, fai.x1, fai.y1, fai.x2, fai.y2);

    // Find the direction that the line is facing to
    arcUtil_detXWLineDirection(line, fai.x1, fai.y1, fai.x2, fai.y2);

    //lprintf("l=%d r=%d t=%d b=%d\n", line.faceL, line.faceR, line.faceT, line.faceB);

    // Generate scanlines data
    Scanlines scanlines, scanlinesRef;
    arcUtil_genScanlinesForChord(fai, line, scanlines, scanlinesRef);

    // Draw the scanlines
    for(Scanlines::const_iterator it = scanlines.begin(); it != scanlines.end(); ++it) {
        _rasterizer->fillOneScanlineNoAA(it->second.from, it->second.to, it->first, fai.minX, fai.minY);
    }

    // Exit here if we are not doing anti-aliasing
    if(!fai.antiAlias) return;

    // Draw the anti-aliased circumference pixels
    arcUtil_drawCircumferencePixels(fai, scanlinesRef);
}

void ImagePainter2::fillArcPieImpl(FilledArcInfo& fai)
{
    // Calculate points for the closing lines
    XWLineData line1, line2;
    arcUtil_runXWLineAlgorithm(line1, fai.x1, fai.y1, fai.ctrX, fai.ctrY);
    arcUtil_runXWLineAlgorithm(line2, fai.x2, fai.y2, fai.ctrX, fai.ctrY);

    // Find the direction that the lines are facing to
    arcUtil_detXWLineDirection(line1, fai.x1, fai.y1, fai.ctrX, fai.ctrY);
    arcUtil_detXWLineDirection(line2, fai.x2, fai.y2, fai.ctrX, fai.ctrY);

    // ### TODO ###
}

void ImagePainter2::arcUtil_findExactBegEndPointsCoordinate(FilledArcInfo& fai)
{
    // Calculate the approximate coordinate of the point which is located at the begin angle
    const Pt::int32_t bx = round(fai.ctrX + fai.radX * fastCos(fai.degBegin * Pt::Pi / 180));
    const Pt::int32_t by = round(fai.ctrY - fai.radY * fastSin(fai.degBegin * Pt::Pi / 180)); // Sign inversion due to differences between cartesian and computer coordinate systems

    // Calculate the approximate coordinate of the point which is located at the end angle
    const Pt::int32_t ex = round(fai.ctrX + fai.radX * fastCos(fai.degEnd   * Pt::Pi / 180));
    const Pt::int32_t ey = round(fai.ctrY - fai.radY * fastSin(fai.degEnd   * Pt::Pi / 180)); // Sign inversion due to differences between cartesian and computer coordinate systems

    // Used for finding the exact coordinate of the points which are located at the begin and end angle
    Pt::int32_t x1d = MAXIMUM_COORD; // Begin point
    Pt::int32_t y1d = MAXIMUM_COORD;
    Pt::int32_t x2d = MAXIMUM_COORD; // End point
    Pt::int32_t y2d = MAXIMUM_COORD;

    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= fai.quartersX; ++x) {
        // Calculate the coordinate
        const float       y  = fai.radY * fastSqrt(1 - (float) x * x / fai.radX2);
        const Pt::int32_t xl = fai.ctrX - x;
        const Pt::int32_t xr = fai.ctrX + x;
        const Pt::int32_t yt = fai.ctrY - ( fai.antiAlias ? floor(y) : round(y) );
        const Pt::int32_t yb = fai.ctrY + ( fai.antiAlias ? floor(y) : round(y) );
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
        // Calculate the coordinate
        const float       x  = fai.radX * fastSqrt(1 - (float) y * y / fai.radY2);
        const Pt::int32_t xl = fai.ctrX - ( fai.antiAlias ? floor(x) : round(x) );
        const Pt::int32_t xr = fai.ctrX + ( fai.antiAlias ? floor(x) : round(x) );
        const Pt::int32_t yt = fai.ctrY - y;
        const Pt::int32_t yb = fai.ctrY + y;
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

// Use Xiaolin Wu's anti-aliased line algorithm to calculate the line's points
// https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
void ImagePainter2::arcUtil_runXWLineAlgorithm(XWLineData& dst, Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2)
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
            dst.points.insert( std::make_pair(
                i,
                XWLineData::XWPointXAA( FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)), a1, a2 )
            ) );
            ypxli += grad;
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            dst.points.insert( std::make_pair(
                FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)),
                XWLineData::XWPointXAA( i, a1, a2 )
            ) );
            ypxli += grad;
        }
    }
}

void ImagePainter2::arcUtil_genScanlinesForChord(const FilledArcInfo& fai, XWLineData& xwLine, Scanlines& scanlines, Scanlines& scanlinesRef)
{
    // Find the line's minimum and maximum Y coordinates
    Pt::int32_t lineMinY, lineMaxY;

    lineMinY = xwLine.points.begin ()->first;
    lineMaxY = xwLine.points.rbegin()->first;

    //lprintf("%d %d\n", lineMinY, lineMaxY);

    // Minimum and maximum X coordinates of the shape
    const Pt::int32_t xlMin = std::min(fai.x1, fai.x2);
    const Pt::int32_t xlMax = std::max(fai.x1, fai.x2);

    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= fai.quartersX; ++x) {
        // Calculate the coordinate
        const float       y  = fai.radY * fastSqrt(1 - (float) x * x / fai.radX2);
        const Pt::int32_t yt = fai.ctrY - ( fai.antiAlias ? floor(y) : round(y) );
        const Pt::int32_t yb = fai.ctrY + ( fai.antiAlias ? floor(y) : round(y) );
        const Pt::int32_t xl = fai.ctrX - x;
        const Pt::int32_t xr = fai.ctrX + x;
        // Skip if the scanline will be completely outside the shape
        if(xwLine.faceL && xr < xlMin) continue;
        if(xwLine.faceR && xl > xlMax) continue;
        // Store/update the scanline coordinates
        arcUtil_cropAndStoreScanlineForChord(xwLine, scanlines, scanlinesRef, lineMinY, lineMaxY, xl, xr, yt, yb);
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= fai.quartersY; ++y) {
        // Calculate the coordinate
        const float       x  = fai.radX * fastSqrt(1 - (float) y * y / fai.radY2);
        const Pt::int32_t yt = fai.ctrY - y;
        const Pt::int32_t yb = fai.ctrY + y;
        const Pt::int32_t xl = fai.ctrX - ( fai.antiAlias ? floor(x) : round(x) );
        const Pt::int32_t xr = fai.ctrX + ( fai.antiAlias ? floor(x) : round(x) );
        // Skip if the scanline will be completely outside the shape
        if(xwLine.faceL && xr < xlMin) continue;
        if(xwLine.faceR && xl > xlMax) continue;
        // Store/update the scanline coordinates
        arcUtil_cropAndStoreScanlineForChord(xwLine, scanlines, scanlinesRef, lineMinY, lineMaxY, xl, xr, yt, yb);
    }
}

void ImagePainter2::arcUtil_cropAndStoreScanlineForChord(XWLineData& xwLine, Scanlines& scanlines, Scanlines& scanlinesRef, Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t yt, Pt::int32_t yb)
{
    // Store/update the reference scanline coordinates
    Scanlines::iterator rit1 = scanlinesRef.find(yt);
    if(rit1 == scanlinesRef.end()) { // Insert a new element
        scanlinesRef.insert( std::make_pair( yt, ScanlineElement(xl, xr) ) );
    }
    else { // Update the scanline's "from" and "to" coordinates
        if( xl < rit1->second.from ) rit1->second.from = xl;
        if( xr > rit1->second.to   ) rit1->second.to   = xr;
    }

    Scanlines::iterator rit2 = scanlinesRef.find(yb);
    if(rit2 == scanlinesRef.end()) { // Insert a new element
        scanlinesRef.insert( std::make_pair( yb, ScanlineElement(xl, xr) ) );
    }
    else { // Update the scanline's "from" and "to" coordinates
        if( xl < rit2->second.from ) rit2->second.from = xl;
        if( xr > rit2->second.to   ) rit2->second.to   = xr;
    }

    // Store/update the scanline coordinates
    if( (!xwLine.faceT && !xwLine.faceB) || (xwLine.faceT && yt >= lineMinY) || (xwLine.faceB && yt <= lineMaxY) ) {
        // Crop the coordinates
        XWLineData::XWPoints::iterator lit = xwLine.points.find(yt);
        Pt::int32_t                    xlc = xl;
        Pt::int32_t                    xrc = xr;
        if(lit != xwLine.points.end()) {
            if(xwLine.faceL) {
                if(xwLine.steep) { // (X), (X + 1)
                    if(xlc < lit->second.x + 1) xlc = lit->second.x + 1;
                    lit->second.a2 = 0;
                }
                else { // (X)
                    if(xlc <= lit->second.x) xlc = lit->second.x + 1;
                }
            }
            if(xwLine.faceR) {
                if(xwLine.steep) { // (X), (X + 1)
                    if(xrc > lit->second.x) xrc = lit->second.x;
                    lit->second.a1 = 0;
                }
                else { // (X)
                    if(xrc >= lit->second.x) xrc = lit->second.x - 1;
                }
            }
        }
        // Store/update the scanline coordinates as needed
        if(xrc >= xlc) {
            Scanlines::iterator sit = scanlines.find(yt);
            if(sit == scanlines.end()) { // Insert a new element
                scanlines.insert( std::make_pair( yt, ScanlineElement(xlc, xrc) ) );
            }
            else { // Update the scanline's "from" and "to" coordinates
                if( xlc < sit->second.from ) sit->second.from = xlc;
                if( xrc > sit->second.to   ) sit->second.to   = xrc;
            }
        }
    }

    if( (!xwLine.faceT && !xwLine.faceB) || (xwLine.faceT && yb >= lineMinY) || (xwLine.faceB && yb <= lineMaxY) ) {
        XWLineData::XWPoints::iterator lit = xwLine.points.find(yb);
        Pt::int32_t                    xlc = xl;
        Pt::int32_t                    xrc = xr;
        if(lit != xwLine.points.end()) {
            if(xwLine.faceL) {
                if(xwLine.steep) { // (X), (X + 1)
                    if(xlc < lit->second.x + 1) xlc = lit->second.x + 1;
                    lit->second.a2 = 0;
                }
                else { // (X)
                    if(xlc <= lit->second.x) xlc = lit->second.x + 1;
                }
            }
            if(xwLine.faceR) {
                if(xwLine.steep) { // (X), (X + 1)
                    if(xrc > lit->second.x) xrc = lit->second.x;
                    lit->second.a1 = 0;
                }
                else { // (X)
                    if(xrc >= lit->second.x) xrc = lit->second.x - 1;
                }
            }
        }
        // Store/update the scanline coordinates as needed
        if(xrc >= xlc) {
            Scanlines::iterator sit = scanlines.find(yb);
            if(sit == scanlines.end()) { // Insert a new element
                scanlines.insert( std::make_pair( yb, ScanlineElement(xlc, xrc) ) );
            }
            else { // Update the scanline's "from" and "to" coordinates
                if( xlc < sit->second.from ) sit->second.from = xlc;
                if( xrc > sit->second.to   ) sit->second.to   = xrc;
            }
        }
    }
}

void ImagePainter2::arcUtil_drawCircumferencePixels(FilledArcInfo& fai, const Scanlines& scanlinesRef)
{
    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= fai.quartersX; ++x) {
        // Calculate the Y coordinate and alpha
        const float       y     = fai.radY * fastSqrt(1 - (float) x * x / fai.radX2);
        const Pt::int32_t fly   = floor(y);
        const float       error = y - fly;
        const Pt::uint8_t alpha = round(error * 255);
        // Draw the first part of the pixels
        const Pt::int32_t x1  = fai.ctrX - x;
        const Pt::int32_t x2  = fai.ctrX + x;
        const Pt::int32_t y10 = fai.ctrY - fly;
        const Pt::int32_t y20 = fai.ctrY + fly;
        Scanlines::const_iterator it10 = scanlinesRef.find(y10);
        Scanlines::const_iterator it20 = scanlinesRef.find(y20);
        if( ( it10 == scanlinesRef.end() || (it10->second.from > x1+1 && it10->second.to < x2-1) ) ||
            ( it20 == scanlinesRef.end() || (it20->second.from > x1+1 && it20->second.to < x2-1) )
        ) {
            const bool mask[4] = {
                pointIsInsideArcDegRange(x1, y10, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x1, y20, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x2, y10, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x2, y20, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd)
            };
            _rasterizer->fill4Pixels(x1, y10, x2, y20, fai.minX, fai.minY, 255 - alpha, mask);
        }
        // Draw the second part of the pixels
        const Pt::int32_t y11 = fai.ctrY - fly - 1;
        const Pt::int32_t y21 = fai.ctrY + fly + 1;
        Scanlines::const_iterator it11 = scanlinesRef.find(y11);
        Scanlines::const_iterator it21 = scanlinesRef.find(y21);
        if( ( it11 == scanlinesRef.end() || (it11->second.from > x1 && it11->second.to < x2) ) ||
            ( it21 == scanlinesRef.end() || (it21->second.from > x1 && it21->second.to < x2) )
        ) {
            const bool mask[4] = {
                pointIsInsideArcDegRange(x1, y11, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x1, y21, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x2, y11, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x2, y21, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd)
            };
            _rasterizer->fill4Pixels(x1, y11, x2, y21, fai.minX, fai.minY, alpha, mask);
        }
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= fai.quartersY; ++y) {
        // Calculate the X coordinate and alpha
        const float       x     = fai.radX * fastSqrt(1 - (float) y * y / fai.radY2);
        const Pt::int32_t flx   = floor(x);
        const float       error = x - flx;
        const Pt::uint8_t alpha = round(error * 255);
        // Draw the first part of the pixels
        const Pt::int32_t x10 = fai.ctrX - flx;
        const Pt::int32_t x20 = fai.ctrX + flx;
        const Pt::int32_t y1  = fai.ctrY - y;
        const Pt::int32_t y2  = fai.ctrY + y;
        Scanlines::const_iterator it1 = scanlinesRef.find(y1);
        Scanlines::const_iterator it2 = scanlinesRef.find(y2);
        if( ( it1 == scanlinesRef.end() || (it1->second.from > x10 && it1->second.to < x20) ) ||
            ( it2 == scanlinesRef.end() || (it2->second.from > x10 && it2->second.to < x20) )
        ) {
            const bool mask[4] = {
                pointIsInsideArcDegRange(x10, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x10, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x20, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x20, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd)
            };
            _rasterizer->fill4Pixels(x10, y1, x20, y2, fai.minX, fai.minY, 255 - alpha, mask);
        }
        // Draw the second part of the pixels
        const Pt::int32_t x11 = fai.ctrX - flx - 1;
        const Pt::int32_t x21 = fai.ctrX + flx + 1;
        if( ( it1 == scanlinesRef.end() || (it1->second.from > x11 && it1->second.to < x21) ) ||
            ( it2 == scanlinesRef.end() || (it2->second.from > x11 && it2->second.to < x21) )
        ) {
            const bool mask[4] = {
                pointIsInsideArcDegRange(x11, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x11, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x21, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x21, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd)
            };
            _rasterizer->fill4Pixels(x11, y1, x21, y2, fai.minX, fai.minY, alpha, mask);
        }
    }
}


} // namespace
} // namespace
