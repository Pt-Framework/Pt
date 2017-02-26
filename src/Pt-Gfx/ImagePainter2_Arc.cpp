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
         if(arcMode == ArcMode::Chord) fillArcChordImpl(fai);
    else if(arcMode == ArcMode::Pie  ) fillArcPieImpl  (fai);
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================
/*
Pt::Gfx - CompositionMode::SourceCopy
    Solid-filled    ellipse          @ ImagePainter  =      6
    Solid-filled    ellipse NOAA     @ ImagePainter2 =      7 ( 1.167)
    Solid-filled    ellipse XWAA     @ ImagePainter2 =     18 ( 3.000)

    Solid-filled    arc     NOAA     @ ImagePainter2 =     73
    Solid-filled    arc     XWAA     @ ImagePainter2 =    143 ( 1.959)

Pt::Gfx - CompositionMode::SourceOver
    Solid-filled    ellipse          @ ImagePainter  =     40
    Solid-filled    ellipse NOAA     @ ImagePainter2 =     26 ( 0.650)
    Solid-filled    ellipse XWAA     @ ImagePainter2 =     36 ( 0.900)

    Solid-filled    arc     NOAA     @ ImagePainter2 =    139
    Solid-filled    arc     XWAA     @ ImagePainter2 =    205 ( 1.475)
*/

void ImagePainter2::fillArcChordImpl(FilledArcInfo& fai)
{
    // Calculate points for the closing line
    XWLineData line;

    line.points.resize(fai.radY * 2 + 2);
    arcUtil_runXWLineAlgorithm(line, fai, fai.x1, fai.y1, fai.x2, fai.y2);

    // Find the direction that the line is facing to
    arcUtil_detXWLineDirection(line);

    // Generate the scanlines data
    Scanlines scanlines(fai.radY * 2 + 2);

    arcUtil_genScanlinesForChord(scanlines, fai, line);

    // Draw the scanlines
    for(size_t i = 0; i < scanlines.size(); ++i) {
        const ScanlineElement& sle = scanlines[i];
        if(sle.isNull()) continue;
        _rasterizer->fillOneScanlineNoAA(sle.from, sle.to, i + fai.minY - 1, fai.minX, fai.minY);
    }

    scanlines.clear();

    // Exit here if we are not doing anti-aliasing
    if(!fai.antiAlias) return;

    // Draw the anti-aliased circumference pixels
    // ### TODO: There are out-of-place brighter-pixel artifacts in SourceOverMode !!! ###
    arcUtil_drawCircumferencePixels(fai);

    // Draw the closing line
    Point maskInOut[4] = {
        Painter::MaximumPointCoordinate, Painter::MaximumPointCoordinate,
        Painter::MaximumPointCoordinate, Painter::MaximumPointCoordinate
    };

    arcUtil_drawXWLine(fai, line, maskInOut);
}

void ImagePainter2::fillArcPieImpl(FilledArcInfo& fai)
{
    // Calculate points for the closing lines
    XWLineData line1, line2;

    line1.points.resize(fai.radY * 2 + 2);
    line2.points.resize(fai.radY * 2 + 2);

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
    Scanlines scanlines1(fai.radY * 2 + 2);
    Scanlines scanlines2(fai.radY * 2 + 2);

    arcUtil_genScanlinesForPie(scanlines1, scanlines2, fai, line1, line2);

    // Draw the scanlines
    for(size_t i = 0; i < scanlines1.size(); ++i) {
        const ScanlineElement& sle = scanlines1[i];
        if(sle.isNull()) continue;
        _rasterizer->fillOneScanlineNoAA(sle.from, sle.to, i + fai.minY - 1, fai.minX, fai.minY);
    }

    for(size_t i = 0; i < scanlines2.size(); ++i) {
        const ScanlineElement& sle = scanlines2[i];
        if(sle.isNull()) continue;
        if(!scanlines1[i].isNull() && sle.from >= scanlines1[i].from && sle.to <= scanlines1[i].to) continue;
        _rasterizer->fillOneScanlineNoAA(sle.from, sle.to, i + fai.minY - 1, fai.minX, fai.minY);
    }

    scanlines1.clear();
    scanlines2.clear();

    // Exit here if we are not doing anti-aliasing
    if(!fai.antiAlias) return;

    // Draw the anti-aliased circumference pixels
    // ### TODO: There are out-of-place brighter-pixel artifacts in SourceOverMode !!! ###
    arcUtil_drawCircumferencePixels(fai);

    // Draw the closing lines
    Point maskInOut[4] = {
        Painter::MaximumPointCoordinate, Painter::MaximumPointCoordinate,
        Painter::MaximumPointCoordinate, Painter::MaximumPointCoordinate
    };

    arcUtil_drawXWLine(fai, line2, maskInOut);
    arcUtil_drawXWLine(fai, line1, maskInOut);
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
void ImagePainter2::arcUtil_runXWLineAlgorithm(XWLineData& xwLine, const FilledArcInfo& fai, Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2)
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
            xwLine.points[ i - fai.minY + 1 ].push_back(
                XWLineData::XWPoint( FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)), a1, a2 )
            );
            ypxli += grad;
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            xwLine.points[ FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)) - fai.minY + 1 ].push_back(
                XWLineData::XWPoint( i, a1, a2 )
            );
            ypxli += grad;
        }
    }
}

void ImagePainter2::arcUtil_genScanlinesForChord(Scanlines& scanlines, const FilledArcInfo& fai, const XWLineData& xwLine)
{
    // Find the line's minimum and maximum Y coordinates
    const Pt::int32_t lineMinY = std::min(xwLine.y1, xwLine.y2) + 1;
    const Pt::int32_t lineMaxY = std::max(xwLine.y1, xwLine.y2) - 1;

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
        arcUtil_cropAndStoreScanlineForChord(scanlines, fai, xwLine, lineMinY, lineMaxY, xl, xr, yt);
        arcUtil_cropAndStoreScanlineForChord(scanlines, fai, xwLine, lineMinY, lineMaxY, xl, xr, yb);
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
        arcUtil_cropAndStoreScanlineForChord(scanlines, fai, xwLine, lineMinY, lineMaxY, xl, xr, yt);
        arcUtil_cropAndStoreScanlineForChord(scanlines, fai, xwLine, lineMinY, lineMaxY, xl, xr, yb);
    }
}

void ImagePainter2::arcUtil_cropAndStoreScanlineForChord(Scanlines& scanlines, const FilledArcInfo& fai, const XWLineData& xwLine, Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t y)
{
    // For convenience
    typedef std::vector<XWLineData::XWPoint> XWPoints;
    typedef XWPoints::const_iterator         XWPointsIterator;

    // Check if the scanline will be completely outside the shape
    if( (xwLine.faceT && y < lineMinY) || (xwLine.faceB && y > lineMaxY) ) return;

    // Get the element with the wanted coordinate from the closing line
    const XWPoints&  xwPoints = xwLine.points[y - fai.minY + 1];
    XWPointsIterator lit      = xwPoints.end();
    for(XWPointsIterator cit = xwPoints.begin(); cit != xwPoints.end(); ++cit) {
        if(cit->isNull()) continue;
        if( lit == xwPoints.end() || (xwLine.faceL && cit->x > lit->x) || (xwLine.faceR && cit->x < lit->x) ) lit = cit;
    }

    // Crop the scanline coordinates to the closing line
    Pt::int32_t xlc = xl;
    Pt::int32_t xrc = xr;
    if(lit != xwPoints.end()) {
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

void ImagePainter2::arcUtil_genScanlinesForPie(Scanlines& scanlines1, Scanlines& scanlines2, const FilledArcInfo& fai, const XWLineData& xwLine1, const XWLineData& xwLine2)
{
    // Find the line's minimum and maximum Y coordinates
    Pt::int32_t lineMinY = fai.minY;
    Pt::int32_t lineMaxY = fai.minY + fai.radY * 2;

    if( (xwLine1.faceT && xwLine2.faceB) || (xwLine1.faceB && xwLine2.faceT) ) {
        Pt::int32_t y1avg = (xwLine1.y1 + xwLine1.y2) / 2;
        Pt::int32_t y2avg = (xwLine2.y1 + xwLine2.y2) / 2;
        if(y1avg < y2avg) { // The first line is on lower Y coordinate
            if(xwLine1.faceT) {
                const Pt::int32_t cy = std::min(xwLine1.y1, xwLine1.y2);
                if(lineMinY < cy) lineMinY = cy;
            }
            if(xwLine2.faceB) {
                const Pt::int32_t cy = std::max(xwLine2.y1, xwLine2.y2);
                if(lineMaxY > cy) lineMaxY = cy;
            }
        }
        else { // The second line is on lower Y coordinate
            if(xwLine1.faceB) {
                const Pt::int32_t cy = std::max(xwLine1.y1, xwLine1.y2);
                if(lineMaxY > cy) lineMaxY = cy;
            }
            if(xwLine2.faceT) {
                const Pt::int32_t cy = std::min(xwLine2.y1, xwLine2.y2);
                if(lineMinY < cy) lineMinY = cy;
            }
        }
    }
    else {
        // Face top
        if(xwLine1.faceT && xwLine2.faceT) {
            const Pt::int32_t cy1 = std::min(xwLine1.y1, xwLine1.y2);
            const Pt::int32_t cy2 = std::min(xwLine2.y1, xwLine2.y2);
            const Pt::int32_t cy  = std::min(cy1, cy2);
            if(lineMinY < cy) lineMinY = cy;
        }
        else if(xwLine1.faceT) {
            const Pt::int32_t cy = std::min(xwLine1.y1, xwLine1.y2);
            if(lineMinY < cy) lineMinY = cy;
        }
        else if(xwLine2.faceT) {
            const Pt::int32_t cy = std::min(xwLine2.y1, xwLine2.y2);
            if(lineMinY < cy) lineMinY = cy;
        }
        // Face bottom
        if(xwLine1.faceB && xwLine2.faceB) {
            const Pt::int32_t cy1 = std::max(xwLine1.y1, xwLine1.y2);
            const Pt::int32_t cy2 = std::max(xwLine2.y1, xwLine2.y2);
            const Pt::int32_t cy  = std::max(cy1, cy2);
            if(lineMaxY > cy) lineMaxY = cy;
        }
        else if(xwLine1.faceB) {
            const Pt::int32_t cy = std::max(xwLine1.y1, xwLine1.y2);
            if(lineMaxY > cy) lineMaxY = cy;
        }
        else if(xwLine2.faceB) {
            const Pt::int32_t cy = std::max(xwLine2.y1, xwLine2.y2);
            if(lineMaxY > cy) lineMaxY = cy;
        }
    }

    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= fai.quartersX; ++x) {
        // Calculate the coordinate
        const float       y  = fai.radY * fastSqrt(1 - (float) x * x / fai.radX2);
        const Pt::int32_t yt = fai.ctrY - ( fai.antiAlias ? floor(y) : round(y) );
        const Pt::int32_t yb = fai.ctrY + ( fai.antiAlias ? floor(y) : round(y) );
        const Pt::int32_t xl = fai.ctrX - x;
        const Pt::int32_t xr = fai.ctrX + x;
        // Store/update the scanline coordinates
        arcUtil_cropAndStoreScanlineForPie(scanlines1, scanlines2, fai, xwLine1, xwLine2, lineMinY, lineMaxY, xl, xr, yt);
        arcUtil_cropAndStoreScanlineForPie(scanlines1, scanlines2, fai, xwLine1, xwLine2, lineMinY, lineMaxY, xl, xr, yb);
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
        arcUtil_cropAndStoreScanlineForPie(scanlines1, scanlines2, fai, xwLine1, xwLine2, lineMinY, lineMaxY, xl, xr, yt);
        arcUtil_cropAndStoreScanlineForPie(scanlines1, scanlines2, fai, xwLine1, xwLine2, lineMinY, lineMaxY, xl, xr, yb);
    }
}

void ImagePainter2::arcUtil_cropAndStoreScanlineForPie(Scanlines& scanlines1, Scanlines& scanlines2, const FilledArcInfo& fai, const XWLineData& xwLine1, const XWLineData& xwLine2, Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t y)
{
    // For convenience
    typedef std::vector<XWLineData::XWPoint> XWPoints;
    typedef XWPoints::const_iterator         XWPointsIterator;

    // Check if the scanline will be completely outside the shape
    if( (xwLine1.faceT && y <= lineMinY) || (xwLine2.faceT && y <= lineMinY) ||
        (xwLine1.faceB && y >= lineMaxY) || (xwLine2.faceB && y >= lineMaxY)
    ) return;

    // Get the element with the wanted coordinate from the left-side closing line
    const XWPoints&  xwPoints1 = xwLine1.points[y - fai.minY + 1];
    XWPointsIterator lit1      = xwPoints1.end();
    for(XWPointsIterator cit = xwPoints1.begin(); cit != xwPoints1.end(); ++cit) {
        if(cit->isNull()) continue;
        if( lit1 == xwPoints1.end() || (xwLine1.faceL && cit->x > lit1->x) || (xwLine1.faceR && cit->x < lit1->x) ) lit1 = cit;
    }

    // Get the element with the wanted coordinate from the right-side closing line
    const XWPoints&  xwPoints2 = xwLine2.points[y - fai.minY + 1];
    XWPointsIterator lit2      = xwPoints2.end();
    for(XWPointsIterator cit = xwPoints2.begin(); cit != xwPoints2.end(); ++cit) {
        if(cit->isNull()) continue;
        if( lit2 == xwPoints2.end() || (xwLine2.faceL && cit->x > lit2->x) || (xwLine2.faceR && cit->x < lit2->x) ) lit2 = cit;
    }

    // Copy the coordinates for cropping
    Pt::int32_t xlc1 = xl;
    Pt::int32_t xrc1 = xr;

    Pt::int32_t xlc2 = -1; // By default we do not store a second scanline
    Pt::int32_t xrc2 = xr;

    // Both lines are facing left
    if( xwLine1.faceL && xwLine2.faceL ) {
        // Left
        if( lit1 != xwPoints1.end() && xwLine1.insideYRange(y) ) {
            if(xwLine1.steep) { if(xlc1 <  lit1->x + 1) xlc1 = lit1->x + 1; } // (X), (X + 1)
            else              { if(xlc1 <= lit1->x    ) xlc1 = lit1->x + 1; } // (X)
        }
        // Left
        if( lit2 != xwPoints2.end() && xwLine2.insideYRange(y) ) {
            if(xwLine2.steep) { if(xlc1 <  lit2->x + 1) xlc1 = lit2->x + 1; } // (X), (X + 1)
            else              { if(xlc1 <= lit2->x    ) xlc1 = lit2->x + 1; } // (X)
        }
    }

    // Both lines are facing right
    else if( xwLine1.faceR && xwLine2.faceR ) {
        // Right
        if( lit1 != xwPoints1.end() && xwLine1.insideYRange(y) ) {
            if(xwLine1.steep) { if(xrc1 >  lit1->x) xrc1 = lit1->x;     } // (X), (X + 1)
            else              { if(xrc1 >= lit1->x) xrc1 = lit1->x - 1; } // (X)
        }
        // Right
        if( lit2 != xwPoints2.end() && xwLine2.insideYRange(y) ) {
            if(xwLine2.steep) { if(xrc1 >  lit2->x) xrc1 = lit2->x;     } // (X), (X + 1)
            else              { if(xrc1 >= lit2->x) xrc1 = lit2->x - 1; } // (X)
        }
    }

    // Left-side line is facing left and right-side line is facing right
    else if( xwLine1.faceL && xwLine2.faceR ) {
        // Left
        if( lit1 != xwPoints1.end() && xwLine1.insideYRange(y) ) {
            if(xwLine1.steep) { if(xlc1 <  lit1->x + 1) xlc1 = lit1->x + 1; } // (X), (X + 1)
            else              { if(xlc1 <= lit1->x    ) xlc1 = lit1->x + 1; } // (X)
        }
        // Right
        if( lit2 != xwPoints2.end() && xwLine2.insideYRange(y) ) {
            if(xwLine2.steep) { if(xrc1 >  lit2->x) xrc1 = lit2->x;     } // (X), (X + 1)
            else              { if(xrc1 >= lit2->x) xrc1 = lit2->x - 1; } // (X)
        }
    }

    // Left-side line is facing right and right-side line is facing left => we have got a vertical "v-shape"
    else if( xwLine1.faceR && xwLine2.faceL ) {
        // Right
        if( lit1 != xwPoints1.end() && xwLine1.insideYRange(y) ) {
            if(xwLine1.steep) { if(xrc1 >  lit1->x) xrc1 = lit1->x;     } // (X), (X + 1)
            else              { if(xrc1 >= lit1->x) xrc1 = lit1->x - 1; } // (X)
        }
        // Left
        if( lit2 != xwPoints2.end() && xwLine2.insideYRange(y) ) {
            if(xwLine2.steep) { if(xlc2 <  lit2->x + 1) xlc2 = lit2->x + 1; } // (X), (X + 1)
            else              { if(xlc2 <= lit2->x    ) xlc2 = lit2->x + 1; } // (X)
        }
        if( lit2 != xwPoints2.end() && xwLine2.insideYRange(y) && !xwLine1.insideYRange(y) ) {
            if(xwLine2.steep) { if(xlc1 <  lit2->x + 1) xlc1 = lit2->x + 1; } // (X), (X + 1)
            else              { if(xlc1 <= lit2->x    ) xlc1 = lit2->x + 1; } // (X)
        }
    }

    // Other conditions
    else {
        // Left
        if( xwLine1.faceL ) {
            if( lit1 != xwPoints1.end() && xwLine1.insideYRange(y) ) {
                if(xwLine1.steep) { if(xlc1 <  lit1->x + 1) xlc1 = lit1->x + 1; } // (X), (X + 1)
                else              { if(xlc1 <= lit1->x    ) xlc1 = lit1->x + 1; } // (X)
            }
        }
        // Right
        else if( xwLine1.faceR ) {
            if( lit1 != xwPoints1.end() && xwLine1.insideYRange(y) ) {
                if(xwLine1.steep) { if(xrc1 >  lit1->x) xrc1 = lit1->x;     } // (X), (X + 1)
                else              { if(xrc1 >= lit1->x) xrc1 = lit1->x - 1; } // (X)
            }
        }
        // Left
        if( xwLine2.faceL ) {
            if( lit2 != xwPoints2.end() && xwLine2.insideYRange(y) ) {
                if(xwLine2.steep) { if(xlc1 <  lit2->x + 1) xlc1 = lit2->x + 1; } // (X), (X + 1)
                else              { if(xlc1 <= lit2->x    ) xlc1 = lit2->x + 1; } // (X)
            }
        }
        // Right
        else if( xwLine2.faceR ) {
            if( lit2 != xwPoints2.end() && xwLine2.insideYRange(y) ) {
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

void ImagePainter2::arcUtil_drawXWLine(const FilledArcInfo& fai, const XWLineData& xwLine, Point maskInOut[4])
{
    // For convenience
    typedef std::vector<XWLineData::XWPoint> XWPoints;
    typedef XWPoints::const_iterator         XWPointsIterator;

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
    Pt::int32_t pCnt = 0;
    Pt::int32_t lx[4];
    Pt::int32_t ly[4];

    // A helper macro to skip a pixel
    #define CHECK_SKIP_PIXEL(X, Y, A)              \
        {                                          \
            bool skipPixel = false;                \
            for(Pt::int32_t i = 0; i < 4; ++i) {   \
                if(mx[i] == (X) && my[i] == (Y)) { \
                    skipPixel = true;              \
                    break;                         \
                }                                  \
            }                                      \
            if(skipPixel) continue;                \
            if(!(A)) continue;                     \
            lx[2] = lx[3]; lx[3] = x;              \
            ly[2] = ly[3]; ly[3] = y;              \
            if(pCnt < 2) {                         \
                lx[pCnt] = x;                      \
                ly[pCnt] = y;                      \
                ++pCnt;                            \
            }                                      \
        }                                          \
        do {} while(false)

    // Draw the pixels in all coordinates
    Pt::int32_t y = fai.minY - 1;

    for(std::vector<XWPoints>::const_iterator it = xwLine.points.begin(); it != xwLine.points.end(); ++it) {
        // Skip if this Y coordinate has no pixel
        const XWPoints& xwPoint = *it;
        if(xwPoint.empty()) {
            ++y;
            continue;
        }
        // Draw the pixels in this Y coordinate
        for(XWPointsIterator pt = xwPoint.begin(); pt != xwPoint.end(); ++pt) {
            // Skip if this is a null pixel
            if(pt->isNull()) continue;
            // Get the coordinate and alpha
            const Pt::int32_t x  = pt->x;
            const Pt::int32_t a1 = pt->a1;
            const Pt::int32_t a2 = pt->a2;
            // deltaY > deltaX
            if(xwLine.steep) {
                if( xwLine.faceL && (x != fai.x1 || y != fai.y1) && (x != fai.x2 || y != fai.y2) ) {
                    CHECK_SKIP_PIXEL(x, y, a1);
                    _rasterizer->fillPixel(x, y, fai.minX, fai.minY, a1);
                }
                if( xwLine.faceR && (x + 1 != fai.x1 || y != fai.y1) && (x + 1 != fai.x2 || y != fai.y2) ) {
                    CHECK_SKIP_PIXEL(x + 1, y, a2);
                    _rasterizer->fillPixel(x + 1, y, fai.minX, fai.minY, a2);
                }
            }
            // deltaY <= deltaX
            else {
                if( (x != fai.x1 || y != fai.y1) && (x != fai.x2 || y != fai.y2) ) {
                    if(xwLine.faceT) {
                        CHECK_SKIP_PIXEL(x, y, a1);
                        _rasterizer->fillPixel(x, y, fai.minX, fai.minY, a1);
                    }
                    if(xwLine.faceB) {
                        CHECK_SKIP_PIXEL(x, y, a2);
                        _rasterizer->fillPixel(x, y, fai.minX, fai.minY, a2);
                    }
                }
            }
        }
        // Increment the Y coordinate
        ++y;
    }

    // Store back the start and end coordinates to the mask
    for(Pt::int32_t i = 0; i < 4; ++i) {
        maskInOut[i].set(lx[i], ly[i]);
    }

    // Undefine the helper macro
    #undef CHECK_SKIP_PIXEL
}


} // namespace
} // namespace
