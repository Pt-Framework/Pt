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
    arcUtil_findExactBegEndPointsCoordinate(fai, degBegin, degEnd);

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
    arcUtil_detXWLineDirection(line, fai.x1, fai.y1, fai.x2, fai.y2);

    // Generate scanlines data
    Scanlines scanlines;
    arcUtil_genScanlinesForChord(fai, line, scanlines);

    // Draw the scanlines
    for(Scanlines::const_iterator it = scanlines.begin(); it != scanlines.end(); ++it) {
        _rasterizer->fillOneScanlineNoAA(it->second.from, it->second.to, it->first, fai.minX, fai.minY);
    }
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
}

void ImagePainter2::arcUtil_findExactBegEndPointsCoordinate(FilledArcInfo& fai, float degBegin, float degEnd)
{
    // Calculate the approximate coordinate of the point which is located at the begin angle
    const Pt::int32_t bx = round(fai.ctrX + fai.radX * fastCos(degBegin * Pt::Pi / 180));
    const Pt::int32_t by = round(fai.ctrY - fai.radY * fastSin(degBegin * Pt::Pi / 180)); // Sign inversion due to differences between cartesian and computer coordinate systems

    // Calculate the approximate coordinate of the point which is located at the end angle
    const Pt::int32_t ex = round(fai.ctrX + fai.radX * fastCos(degEnd   * Pt::Pi / 180));
    const Pt::int32_t ey = round(fai.ctrY - fai.radY * fastSin(degEnd   * Pt::Pi / 180)); // Sign inversion due to differences between cartesian and computer coordinate systems

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

void ImagePainter2::arcUtil_genScanlinesForChord(FilledArcInfo& fai, XWLineData& xwLine, Scanlines& scanlines)
{
    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= fai.quartersX; ++x) {
        // Calculate the coordinate
        const float       y  = fai.radY * fastSqrt(1 - (float) x * x / fai.radX2);
        const Pt::int32_t xl = fai.ctrX - x;
        const Pt::int32_t xr = fai.ctrX + x;
        const Pt::int32_t yt = fai.ctrY - ( fai.antiAlias ? floor(y) : round(y) );
        const Pt::int32_t yb = fai.ctrY + ( fai.antiAlias ? floor(y) : round(y) );
        // Store/update the scanline coordinates
        Scanlines::iterator it1 = scanlines.find(yt);
        Scanlines::iterator it2 = scanlines.find(yb);
        if(it1 == scanlines.end()) { // Insert a new element
            scanlines.insert( std::make_pair( yt, ScanlineElement(xl, xr) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( xl < it1->second.from ) it1->second.from = xl;
            if( xr > it1->second.to   ) it1->second.to   = xr;
        }
        if(it2 == scanlines.end()) { // Insert a new element
            scanlines.insert( std::make_pair( yb, ScanlineElement(xl, xr) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( xl < it2->second.from ) it2->second.from = xl;
            if( xr > it2->second.to   ) it2->second.to   = xr;
        }
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= fai.quartersY; ++y) {
        // Calculate the coordinate
        const float       x  = fai.radX * fastSqrt(1 - (float) y * y / fai.radY2);
        const Pt::int32_t xl = fai.ctrX - ( fai.antiAlias ? floor(x) : round(x) );
        const Pt::int32_t xr = fai.ctrX + ( fai.antiAlias ? floor(x) : round(x) );
        const Pt::int32_t yt = fai.ctrY - y;
        const Pt::int32_t yb = fai.ctrY + y;
        // Store/update the scanline coordinates
        Scanlines::iterator it1 = scanlines.find(yt);
        Scanlines::iterator it2 = scanlines.find(yb);
        if(it1 == scanlines.end()) { // Insert a new element
            scanlines.insert( std::make_pair( yt, ScanlineElement(xl, xr) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( xl < it1->second.from ) it1->second.from = xl;
            if( xr > it1->second.to   ) it1->second.to   = xr;
        }
        if(it2 == scanlines.end()) { // Insert a new element
            scanlines.insert( std::make_pair( yb, ScanlineElement(xl, xr) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( xl < it2->second.from ) it2->second.from = xl;
            if( xr > it2->second.to   ) it2->second.to   = xr;
        }
    }

    /*
    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= quartersX; ++x) {
        // Calculate the coordinate
        const float y = fai.radY * fastSqrt(1 - (float) x * x / fai.radX2);
        const Pt::int32_t xl = fai.ctrX - x;
        const Pt::int32_t xr = fai.ctrX + x;
        const Pt::int32_t yt = fai.ctrY - ( fai.antiAlias ? floor(y) : round(y) );
        const Pt::int32_t yb = fai.ctrY + ( fai.antiAlias ? floor(y) : round(y) );

        // Without anti-aliasing
        if(!useAntiAliasing) {
            // Calculate the coordinates
            const Pt::int32_t xl = ctrX - x;
            const Pt::int32_t xr = ctrX + x;
            const Pt::int32_t yt = ctrY - round(y);
            const Pt::int32_t yb = ctrY + round(y);
            // Store/update the scanline coordinates
            Scanlines::iterator it1 = scanlines.find(yt);
            Scanlines::iterator it2 = scanlines.find(yb);
            if(it1 == scanlines.end()) { // Insert a new element
                scanlines.insert( std::make_pair( yt, ScanlineElement(xl, xr) ) );
            }
            else { // Update the scanline's "from" and "to" coordinates
                if( xl < it1->second.from ) it1->second.from = xl;
                if( xr > it1->second.to   ) it1->second.to   = xr;
            }
            if(it2 == scanlines.end()) { // Insert a new element
                scanlines.insert( std::make_pair( yb, ScanlineElement(xl, xr) ) );
            }
            else { // Update the scanline's "from" and "to" coordinates
                if( xl < it2->second.from ) it2->second.from = xl;
                if( xr > it2->second.to   ) it2->second.to   = xr;
            }
        }
        // With anti-aliasing
        else {
            // Calculate the coordinates
            const Pt::int32_t xl = ctrX - x;
            const Pt::int32_t xr = ctrX + x;
            const Pt::int32_t yt = ctrY - floor(y);
            const Pt::int32_t yb = ctrY + floor(y);
            // Store/update the scanline coordinates
            Scanlines::iterator it1 = scanlines.find(yt);
            Scanlines::iterator it2 = scanlines.find(yb);
            if(it1 == scanlines.end()) { // Insert a new element
                scanlines.insert( std::make_pair( yt, ScanlineElement(xl, xr) ) );
            }
            else { // Update the scanline's "from" and "to" coordinates
                if( xl < it1->second.from ) it1->second.from = xl;
                if( xr > it1->second.to   ) it1->second.to   = xr;
            }
            if(it2 == scanlines.end()) { // Insert a new element
                scanlines.insert( std::make_pair( yb, ScanlineElement(xl, xr) ) );
            }
            else { // Update the scanline's "from" and "to" coordinates
                if( xl < it2->second.from ) it2->second.from = xl;
                if( xr > it2->second.to   ) it2->second.to   = xr;
            }
        }
    }

    /*
    // Left and right halves
    quartersY = round( radY2 * fastInvSqrt(radX2 + radY2) );

    for(Pt::int32_t y = 0; y <= quartersY; ++y) {
        // Calculate the coordinate
        const float x = radX * fastSqrt(1 - (float) y * y / radY2);
        // Without anti-aliasing
        if(!useAntiAliasing) {
            // Calculate the coordinates
            const Pt::int32_t xl = ctrX - round(x);
            const Pt::int32_t xr = ctrX + round(x);
            const Pt::int32_t yt = ctrY - y;
            const Pt::int32_t yb = ctrY + y;
            // Determine the exact coordinates of the closing lines
            if(abs(xl - bx) < x1d) { x1d = abs(xl - bx); x1 = xl; }
            if(abs(xl - ex) < x2d) { x2d = abs(xl - ex); x2 = xl; }
            if(abs(xr - bx) < x1d) { x1d = abs(xr - bx); x1 = xr; }
            if(abs(xr - ex) < x2d) { x2d = abs(xr - ex); x2 = xr; }
            if(abs(yt - by) < y1d) { y1d = abs(yt - by); y1 = yt; }
            if(abs(yt - ey) < y2d) { y2d = abs(yt - ey); y2 = yt; }
            if(abs(yb - by) < y1d) { y1d = abs(yb - by); y1 = yb; }
            if(abs(yb - ey) < y2d) { y2d = abs(yb - ey); y2 = yb; }
            // Store/update the scanline coordinates
            Scanlines::iterator it1 = scanlines.find(yt);
            Scanlines::iterator it2 = scanlines.find(yb);
            if(it1 == scanlines.end()) { // Insert a new element
                scanlines.insert( std::make_pair( yt, ScanlineElement(xl, xr) ) );
            }
            else { // Update the scanline's "from" and "to" coordinates
                if( xl < it1->second.from ) it1->second.from = xl;
                if( xr > it1->second.to   ) it1->second.to   = xr;
            }
            if(it2 == scanlines.end()) { // Insert a new element
                scanlines.insert( std::make_pair( yb, ScanlineElement(xl, xr) ) );
            }
            else { // Update the scanline's "from" and "to" coordinates
                if( xl < it2->second.from ) it2->second.from = xl;
                if( xr > it2->second.to   ) it2->second.to   = xr;
            }
        }
        // With anti-aliasing
        else {
            // Calculate the coordinates
            const Pt::int32_t xl = ctrX - floor(x);
            const Pt::int32_t xr = ctrX + floor(x);
            const Pt::int32_t yt = ctrY - y;
            const Pt::int32_t yb = ctrY + y;
            // Determine the exact coordinates of the closing lines
            if(abs(xl - bx) < x1d) { x1d = abs(xl - bx); x1 = xl; }
            if(abs(xl - ex) < x2d) { x2d = abs(xl - ex); x2 = xl; }
            if(abs(xr - bx) < x1d) { x1d = abs(xr - bx); x1 = xr; }
            if(abs(xr - ex) < x2d) { x2d = abs(xr - ex); x2 = xr; }
            if(abs(yt - by) < y1d) { y1d = abs(yt - by); y1 = yt; }
            if(abs(yt - ey) < y2d) { y2d = abs(yt - ey); y2 = yt; }
            if(abs(yb - by) < y1d) { y1d = abs(yb - by); y1 = yb; }
            if(abs(yb - ey) < y2d) { y2d = abs(yb - ey); y2 = yb; }
            // Store/update the scanline coordinates
            Scanlines::iterator it1 = scanlines.find(yt);
            Scanlines::iterator it2 = scanlines.find(yb);
            if(it1 == scanlines.end()) { // Insert a new element
                scanlines.insert( std::make_pair( yt, ScanlineElement(xl, xr) ) );
            }
            else { // Update the scanline's "from" and "to" coordinates
                if( xl < it1->second.from ) it1->second.from = xl;
                if( xr > it1->second.to   ) it1->second.to   = xr;
            }
            if(it2 == scanlines.end()) { // Insert a new element
                scanlines.insert( std::make_pair( yb, ScanlineElement(xl, xr) ) );
            }
            else { // Update the scanline's "from" and "to" coordinates
                if( xl < it2->second.from ) it2->second.from = xl;
                if( xr > it2->second.to   ) it2->second.to   = xr;
            }
        }
    }
    */
}


} // namespace
} // namespace
