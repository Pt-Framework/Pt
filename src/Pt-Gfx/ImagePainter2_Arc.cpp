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
    fai.xyRat     = (float) fai.radX / (float) fai.radY;

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
    arcUtil_detXWLineDirection(line);

    //lprintf("l=%d r=%d t=%d b=%d\n", line.faceL, line.faceR, line.faceT, line.faceB);

    // Generate scanlines data
    Scanlines scanlines;
    arcUtil_genScanlinesForChord(scanlines, fai, line);

    // Draw the scanlines
    for(Scanlines::const_iterator it = scanlines.begin(); it != scanlines.end(); ++it) {
        _rasterizer->fillOneScanlineNoAA(it->second.from, it->second.to, it->first, fai.minX, fai.minY);
    }

    // Exit here if we are not doing anti-aliasing
    if(!fai.antiAlias) return;

    // Draw the anti-aliased circumference pixels
    // ### TODO: There are out-of-place brighter-pixel artifacts in SourceOverMode !!! ###
    arcUtil_drawCircumferencePixels(fai);

    // Draw the closing line
    arcUtil_drawXWLine(fai, line, 0);
}

void ImagePainter2::fillArcPieImpl(FilledArcInfo& fai)
{
    // Calculate points for the closing lines
    XWLineData line1, line2;
    if(fai.x1 < fai.x2) {
        arcUtil_runXWLineAlgorithm(line1, fai.x1, fai.y1, fai.ctrX, fai.ctrY);
        arcUtil_runXWLineAlgorithm(line2, fai.ctrX, fai.ctrY, fai.x2, fai.y2);
    }
    else {
        arcUtil_runXWLineAlgorithm(line2, fai.x1, fai.y1, fai.ctrX, fai.ctrY);
        arcUtil_runXWLineAlgorithm(line1, fai.ctrX, fai.ctrY, fai.x2, fai.y2);
    }

    // Find the direction that the lines are facing to
    arcUtil_detXWLineDirection(line1);
    arcUtil_detXWLineDirection(line2);

    //lprintf("(%d, %d) (%d, %d) | (%d, %d) (%d, %d)\n", line1.x1, line1.y1, line1.x2, line1.y2, line2.x1, line2.y1, line2.x2, line2.y2);
    //lprintf("l=%d r=%d t=%d b=%d | l=%d r=%d t=%d b=%d\n", line1.faceL, line1.faceR, line1.faceT, line1.faceB, line2.faceL, line2.faceR, line2.faceT, line2.faceB);

    // Generate scanlines data
    Scanlines scanlines1, scanlines2;
    arcUtil_genScanlinesForPie(scanlines1, scanlines2, fai, line1, line2);

    // Draw the scanlines
    for(Scanlines::const_iterator it = scanlines1.begin(); it != scanlines1.end(); ++it) {
        _rasterizer->fillOneScanlineNoAA(it->second.from, it->second.to, it->first, fai.minX, fai.minY);
    }

    for(Scanlines::const_iterator it = scanlines2.begin(); it != scanlines2.end(); ++it) {
        _rasterizer->fillOneScanlineNoAA(it->second.from, it->second.to, it->first, fai.minX, fai.minY);
    }

    // Just for easier debugging & verification
    //drawArc(PointF(fai.minX, fai.minY), SizeF(fai.radX * 2, fai.radY * 2), fai.degBegin, fai.degEnd, ArcMode::Pie);

    // Exit here if we are not doing anti-aliasing
    if(!fai.antiAlias) return;

    // Draw the anti-aliased circumference pixels
    // ### TODO: There are out-of-place brighter-pixel artifacts in SourceOverMode !!! ###
    arcUtil_drawCircumferencePixels(fai);

    // Draw the closing lines
    arcUtil_drawXWLine(fai, line1, 0     );
    arcUtil_drawXWLine(fai, line2, &line1);
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
void ImagePainter2::arcUtil_runXWLineAlgorithm(XWLineData& xwLine, Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2)
{
    // Copy the coordinates
    xwLine.x1 = x1;
    xwLine.y1 = y1;
    xwLine.x2 = x2;
    xwLine.y2 = y2;

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

    if(xwLine.steep) {
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            xwLine.points.insert( std::make_pair(
                i,
                XWLineData::XWPointAA( FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)), a1, a2 )
            ) );
            ypxli += grad;
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            xwLine.points.insert( std::make_pair(
                FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)),
                XWLineData::XWPointAA( i, a1, a2 )
            ) );
            ypxli += grad;
        }
    }
}

void ImagePainter2::arcUtil_genScanlinesForChord(Scanlines& scanlines, const FilledArcInfo& fai, const XWLineData& xwLine)
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
        arcUtil_cropAndStoreScanlineForChord(scanlines, xwLine, lineMinY, lineMaxY, xl, xr, yt);
        arcUtil_cropAndStoreScanlineForChord(scanlines, xwLine, lineMinY, lineMaxY, xl, xr, yb);
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
        arcUtil_cropAndStoreScanlineForChord(scanlines, xwLine, lineMinY, lineMaxY, xl, xr, yt);
        arcUtil_cropAndStoreScanlineForChord(scanlines, xwLine, lineMinY, lineMaxY, xl, xr, yb);
    }
}

void ImagePainter2::arcUtil_cropAndStoreScanlineForChord(Scanlines& scanlines, const XWLineData& xwLine, Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t y)
{
    // For convenience
    typedef XWLineData::XWPoints::const_iterator XWPointsIterator;

    // Check if the scanline will be completely outside the shape
    if( (xwLine.faceT && y < lineMinY) || (xwLine.faceB && y > lineMaxY) ) return;

    // Get the element with the wanted coordinate from the closing line
    XWPointsIterator lwb = xwLine.points.lower_bound(y);
    XWPointsIterator upb = xwLine.points.upper_bound(y);
    XWPointsIterator lit = lwb;
    for(XWPointsIterator cit = lwb; cit != upb; ++cit) {
        if(xwLine.faceL && cit->second.x > lit->second.x) lit = cit;
        if(xwLine.faceR && cit->second.x < lit->second.x) lit = cit;
    }

    // Crop the scanline coordinates to the closing line
    Pt::int32_t xlc = xl;
    Pt::int32_t xrc = xr;
    if(lit != xwLine.points.end()) {
        if(xwLine.faceL) {
            if(xwLine.steep) { if(xlc <  lit->second.x + 1) xlc = lit->second.x + 1; } // (X), (X + 1)
            else             { if(xlc <= lit->second.x    ) xlc = lit->second.x + 1; } // (X)
        }
        if(xwLine.faceR) {
            if(xwLine.steep) { if(xrc >  lit->second.x) xrc = lit->second.x;     } // (X), (X + 1)
            else             { if(xrc >= lit->second.x) xrc = lit->second.x - 1; } // (X)
        }
    }

    // Store/update the scanline coordinates as needed
    if(xrc >= xlc) {
        Scanlines::iterator sit = scanlines.find(y);
        if(sit == scanlines.end()) { // Insert a new element
            scanlines.insert( std::make_pair( y, ScanlineElement(xlc, xrc) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( xlc < sit->second.from ) sit->second.from = xlc;
            if( xrc > sit->second.to   ) sit->second.to   = xrc;
        }
    }
}

void ImagePainter2::arcUtil_genScanlinesForPie(Scanlines& scanlines1, Scanlines& scanlines2, const FilledArcInfo& fai, const XWLineData& xwLine1, const XWLineData& xwLine2)
{
    // Find the line's minimum and maximum Y coordinates
    Pt::int32_t lineMinY, lineMaxY;

    lineMinY = std::min(xwLine1.points.begin ()->first, xwLine2.points.begin ()->first);
    lineMaxY = std::max(xwLine1.points.rbegin()->first, xwLine2.points.rbegin()->first);

    //lprintf("%d %d\n", lineMinY, lineMaxY);

    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= fai.quartersX; ++x) {
        // Calculate the coordinate
        const float       y  = fai.radY * fastSqrt(1 - (float) x * x / fai.radX2);
        const Pt::int32_t yt = fai.ctrY - ( fai.antiAlias ? floor(y) : round(y) );
        const Pt::int32_t yb = fai.ctrY + ( fai.antiAlias ? floor(y) : round(y) );
        const Pt::int32_t xl = fai.ctrX - x;
        const Pt::int32_t xr = fai.ctrX + x;
        // Store/update the scanline coordinates
        arcUtil_cropAndStoreScanlineForPie(scanlines1, scanlines2, xwLine1, xwLine2, lineMinY, lineMaxY, xl, xr, yt);
        arcUtil_cropAndStoreScanlineForPie(scanlines1, scanlines2, xwLine1, xwLine2, lineMinY, lineMaxY, xl, xr, yb);
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= fai.quartersY; ++y) {
        // Calculate the coordinate
        const float       x  = fai.radX * fastSqrt(1 - (float) y * y / fai.radY2);
        const Pt::int32_t yt = fai.ctrY - y;
        const Pt::int32_t yb = fai.ctrY + y;
        const Pt::int32_t xl = fai.ctrX - ( fai.antiAlias ? floor(x) : round(x) );
        const Pt::int32_t xr = fai.ctrX + ( fai.antiAlias ? floor(x) : round(x) );
        // Store/update the scanline coordinates
        arcUtil_cropAndStoreScanlineForPie(scanlines1, scanlines2, xwLine1, xwLine2, lineMinY, lineMaxY, xl, xr, yt);
        arcUtil_cropAndStoreScanlineForPie(scanlines1, scanlines2, xwLine1, xwLine2, lineMinY, lineMaxY, xl, xr, yb);
    }
}

void ImagePainter2::arcUtil_cropAndStoreScanlineForPie(Scanlines& scanlines1, Scanlines& scanlines2, const XWLineData& xwLine1, const XWLineData& xwLine2, Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t y)
{
    // For convenience
    typedef XWLineData::XWPoints::const_iterator XWPointsIterator;

    // Check if the scanline will be completely outside the shape
    if( (xwLine1.faceT && y <= lineMinY) || (xwLine2.faceT && y <= lineMinY) ||
        (xwLine1.faceB && y >= lineMaxY) || (xwLine2.faceB && y >= lineMaxY)
      ) return;

    // Get the element with the wanted coordinate from the left-side closing line
    XWPointsIterator lwb1 = xwLine1.points.lower_bound(y);
    XWPointsIterator upb1 = xwLine1.points.upper_bound(y);
    XWPointsIterator lit1 = lwb1;
    for(XWPointsIterator cit = lwb1; cit != upb1; ++cit) {
        if(xwLine1.faceL && cit->second.x > lit1->second.x) lit1 = cit;
        if(xwLine1.faceR && cit->second.x < lit1->second.x) lit1 = cit;
    }

    // Get the element with the wanted coordinate from the right-side closing line
    XWPointsIterator lwb2 = xwLine2.points.lower_bound(y);
    XWPointsIterator upb2 = xwLine2.points.upper_bound(y);
    XWPointsIterator lit2 = lwb2;
    for(XWPointsIterator cit = lwb2; cit != upb2; ++cit) {
        if(xwLine2.faceL && cit->second.x > lit2->second.x) lit2 = cit;
        if(xwLine2.faceR && cit->second.x < lit2->second.x) lit2 = cit;
    }

    // Copy the coordinates for cropping
    Pt::int32_t xlc1 = xl;
    Pt::int32_t xrc1 = xr;

    Pt::int32_t xlc2 = -1; // By default we do not have a second scanline
    Pt::int32_t xrc2 = xr;

    // Both lines are facing left
    if( xwLine1.faceL && xwLine2.faceL ) {
        // Left
        if( lit1 != xwLine1.points.end() && xwLine1.insideYRange(y) ) {
            if(xwLine1.steep) { if(xlc1 <  lit1->second.x + 1) xlc1 = lit1->second.x + 1; } // (X), (X + 1)
            else              { if(xlc1 <= lit1->second.x    ) xlc1 = lit1->second.x + 1; } // (X)
        }
        // Left
        if( lit2 != xwLine2.points.end() && xwLine2.insideYRange(y) ) {
            if(xwLine2.steep) { if(xlc1 <  lit2->second.x + 1) xlc1 = lit2->second.x + 1; } // (X), (X + 1)
            else              { if(xlc1 <= lit2->second.x    ) xlc1 = lit2->second.x + 1; } // (X)
        }
    }

    // Both lines are facing right
    else if( xwLine1.faceR && xwLine2.faceR ) {
        // Right
        if( lit1 != xwLine1.points.end() && xwLine1.insideYRange(y) ) {
            if(xwLine1.steep) { if(xrc1 >  lit1->second.x) xrc1 = lit1->second.x;     } // (X), (X + 1)
            else              { if(xrc1 >= lit1->second.x) xrc1 = lit1->second.x - 1; } // (X)
        }
        // Right
        if( lit2 != xwLine2.points.end() && xwLine2.insideYRange(y) ) {
            if(xwLine2.steep) { if(xrc1 >  lit2->second.x) xrc1 = lit2->second.x;     } // (X), (X + 1)
            else              { if(xrc1 >= lit2->second.x) xrc1 = lit2->second.x - 1; } // (X)
        }
    }

    // Left-side line is facing left and right-side line is facing right
    else if( xwLine1.faceL && xwLine2.faceR ) {
        // Left
        if( lit1 != xwLine1.points.end() && xwLine1.insideYRange(y) ) {
            if(xwLine1.steep) { if(xlc1 <  lit1->second.x + 1) xlc1 = lit1->second.x + 1; } // (X), (X + 1)
            else              { if(xlc1 <= lit1->second.x    ) xlc1 = lit1->second.x + 1; } // (X)
        }
        // Right
        if( lit2 != xwLine2.points.end() && xwLine2.insideYRange(y) ) {
            if(xwLine2.steep) { if(xrc1 >  lit2->second.x) xrc1 = lit2->second.x;     } // (X), (X + 1)
            else              { if(xrc1 >= lit2->second.x) xrc1 = lit2->second.x - 1; } // (X)
        }
    }

    // Left-side line is facing right and right-side line is facing left => we have got a vertical "V-shape"
    else if( xwLine1.faceR && xwLine2.faceL ) {
        // Right
        if( lit1 != xwLine1.points.end() && xwLine1.insideYRange(y) ) {
            if(xwLine1.steep) { if(xrc1 >  lit1->second.x) xrc1 = lit1->second.x;     } // (X), (X + 1)
            else              { if(xrc1 >= lit1->second.x) xrc1 = lit1->second.x - 1; } // (X)
        }
        // Left
        if( lit2 != xwLine2.points.end() && xwLine2.insideYRange(y) ) {
            if(xwLine2.steep) { if(xlc2 <  lit2->second.x + 1) xlc2 = lit2->second.x + 1; } // (X), (X + 1)
            else              { if(xlc2 <= lit2->second.x    ) xlc2 = lit2->second.x + 1; } // (X)
        }
    }

    // Store/update the first scanline coordinates as needed
    if(xrc1 >= xlc1) {
        Scanlines::iterator sit = scanlines1.find(y);
        if(sit == scanlines1.end()) { // Insert a new element
            scanlines1.insert( std::make_pair( y, ScanlineElement(xlc1, xrc1) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( xlc1 < sit->second.from ) sit->second.from = xlc1;
            if( xrc1 > sit->second.to   ) sit->second.to   = xrc1;
        }
    }

    // Store/update the second scanline coordinates as needed
    if(xlc2 != -1 && xrc2 >= xlc2) {
        Scanlines::iterator sit = scanlines2.find(y);
        if(sit == scanlines2.end()) { // Insert a new element
            scanlines2.insert( std::make_pair( y, ScanlineElement(xlc2, xrc2) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( xlc2 < sit->second.from ) sit->second.from = xlc2;
            if( xrc2 > sit->second.to   ) sit->second.to   = xrc2;
        }
    }
}

void ImagePainter2::arcUtil_drawCircumferencePixels(FilledArcInfo& fai)
{
    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= fai.quartersX; ++x) {
        // Calculate the Y coordinate and alpha
        const float       y     = fai.radY * fastSqrt(1 - (float) x * x / fai.radX2);
        const Pt::int32_t fly   = floor(y);
        const float       error = y - fly;
        const Pt::uint8_t alpha = round(error * 255);
        // Draw the pixels
        const Pt::int32_t x1 = fai.ctrX - x;
        const Pt::int32_t x2 = fai.ctrX + x;
        const Pt::int32_t y1 = fai.ctrY - fly - 1;
        const Pt::int32_t y2 = fai.ctrY + fly + 1;
        const bool mask[4] = {
            pointIsInsideArcDegRange(x1, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd, fai.xyRat),
            pointIsInsideArcDegRange(x1, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd, fai.xyRat),
            pointIsInsideArcDegRange(x2, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd, fai.xyRat),
            pointIsInsideArcDegRange(x2, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd, fai.xyRat)
        };
        _rasterizer->fill4Pixels(x1, y1, x2, y2, fai.minX, fai.minY, alpha, mask);
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= fai.quartersY; ++y) {
        // Calculate the X coordinate and alpha
        const float       x     = fai.radX * fastSqrt(1 - (float) y * y / fai.radY2);
        const Pt::int32_t flx   = floor(x);
        const float       error = x - flx;
        const Pt::uint8_t alpha = round(error * 255);
        // Draw the pixels
        const Pt::int32_t x1 = fai.ctrX - flx - 1;
        const Pt::int32_t x2 = fai.ctrX + flx + 1;
        const Pt::int32_t y1 = fai.ctrY - y;
        const Pt::int32_t y2 = fai.ctrY + y;
        const bool mask[4] = {
            pointIsInsideArcDegRange(x1, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd, fai.xyRat),
            pointIsInsideArcDegRange(x1, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd, fai.xyRat),
            pointIsInsideArcDegRange(x2, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd, fai.xyRat),
            pointIsInsideArcDegRange(x2, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd, fai.xyRat)
        };
        _rasterizer->fill4Pixels(x1, y1, x2, y2, fai.minX, fai.minY, alpha, mask);
    }
}

void ImagePainter2::arcUtil_drawXWLine(const FilledArcInfo& fai, const XWLineData& xwLine, const XWLineData* xwLineExclusion)
{
    for(XWLineData::XWPoints::const_iterator it = xwLine.points.begin(); it != xwLine.points.end(); ++it) {
        // Get the coordinate and alpha
        const Pt::int32_t y  = it->first;
        const Pt::int32_t x  = it->second.x;
        const Pt::int32_t a1 = it->second.a1;
        const Pt::int32_t a2 = it->second.a2;
        // Check for exclusion
        if(xwLineExclusion) {
            // Get the element with the wanted coordinate from the closing line
            for( XWLineData::XWPoints::const_iterator cit  = xwLineExclusion->points.lower_bound(y);
                                                      cit != xwLineExclusion->points.upper_bound(y);
                                                    ++cit
            ) {
                if(xwLine.steep && xwLine.faceR) {
                    if(cit->second.x == x + 1) return;
                }
                else {
                    if(cit->second.x == x) return;
                }
            }
        }
        // deltaY > deltaX
        if(xwLine.steep) {
            if( xwLine.faceL && (x != fai.x1 || y != fai.y1) && (x != fai.x2 || y != fai.y2) )
                _rasterizer->fillPixel(x, y, fai.minX, fai.minY, a1);
            if( xwLine.faceR && (x + 1 != fai.x1 || y != fai.y1) && (x + 1 != fai.x2 || y != fai.y2) )
                _rasterizer->fillPixel(x + 1, y, fai.minX, fai.minY, a2);
        }
        // deltaY <= deltaX
        else {
            if( (x != fai.x1 || y != fai.y1) && (x != fai.x2 || y != fai.y2) ) {
                if(xwLine.faceT) _rasterizer->fillPixel(x, y, fai.minX, fai.minY, a1);
                if(xwLine.faceB) _rasterizer->fillPixel(x, y, fai.minX, fai.minY, a2);
            }
        }
    }
}


} // namespace
} // namespace
