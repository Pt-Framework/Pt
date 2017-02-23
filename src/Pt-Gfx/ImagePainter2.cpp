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
// ===== Static Public Member Functions =================================================
// ======================================================================================

void ImagePainter2::setFontDir(const Pt::System::Path& path)
{
    FreeType::instance().setFontDir(path);
}

void ImagePainter2::setDefaultFont(const std::string& f)
{
    FreeType::instance().setDefaultFont(f);
}

std::string ImagePainter2::defaultFont()
{
    return FreeType::instance().defaultFont();
}

std::vector<std::string> ImagePainter2::fontNames()
{
    return FreeType::instance().fontNames();
}

FontMetrics ImagePainter2::fontMetrics( const Font& font, const Pt::String& text )
{
    return Rasterizer2::fontMetrics(font, text);
}


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

ImagePainter2::ImagePainter2(Image& image)
: _rasterizer(new Rasterizer2(image))
{
    setAntiAliasingMode();
}

ImagePainter2::~ImagePainter2()
{
    delete _rasterizer;
}

void ImagePainter2::setAntiAliasingMode(AntiAliasingMode mode)
{
    _rasterizer->setAntiAliasingMode(mode);
}

void ImagePainter2::setImage(Image& image)
{
    _rasterizer->setImage(image);
}

const ImageFormat& ImagePainter2::format() const
{
    return _rasterizer->format();
}

void ImagePainter2::setCompositionMode(const CompositionMode& mode)
{
    _rasterizer->setCompositionMode(mode);
}

const CompositionMode& ImagePainter2::compositionMode() const
{
    return _rasterizer->compositionMode();
}

void ImagePainter2::setClip( const RectF& clipIn )
{
     Rect clip(
         Point( (Pt::int32_t)(clipIn.x    ()), (Pt::int32_t)(clipIn.y     ()) ),
         Size ( (Pt::int32_t)(clipIn.width()), (Pt::int32_t)(clipIn.height()) )
     );
    _rasterizer->setClip( clip );
    _clip = clipIn;
}

const Gfx::RectF& ImagePainter2::clip() const
{
    return _clip;
}

void ImagePainter2::setPen( const Pen& pen )
{
    _rasterizer->setPen( pen ) ;
}

const Pen& ImagePainter2::pen() const
{
    return _rasterizer->pen();
}

void ImagePainter2::setBrush(const Brush& brush)
{
    _rasterizer->setBrush(brush);
}

const Brush& ImagePainter2::brush() const
{
    return _rasterizer->brush();
}

void ImagePainter2::setFont(const Font& font)
{
    _rasterizer->setFont( font );
}

const Font& ImagePainter2::font() const
{
    return _rasterizer->font();
}

FontMetrics ImagePainter2::fontMetrics(const String& text) const
{
    return _rasterizer->fontMetrics( text );
}

void ImagePainter2::drawImage( const PointF& toIn, const Image& image)
{
    Point to( (Pt::int32_t)(toIn.x()), (Pt::int32_t)(toIn.y()) );
    _rasterizer->image(to, image);
}

void ImagePainter2::drawImage(const PointF& toIn, const Image& image, const RectF& imageRectIn)
{
    Point to( (Pt::int32_t)(toIn.x()), (Pt::int32_t)(toIn.y()) );

    Rect imageRect(
        Point( (Pt::int32_t)(imageRectIn.    x()), (Pt::int32_t)(imageRectIn.     y()) ),
        Size ( (Pt::int32_t)(imageRectIn.width()), (Pt::int32_t)(imageRectIn.height()) )
    );

    _rasterizer->image(to, image, imageRect);
}

void ImagePainter2::drawText( const PointF& toIn, const String& text )
{
    const Point to( (Pt::int32_t)(toIn.x()), (Pt::int32_t)(toIn.y()) );

    _rasterizer->strokeText( to, text );
}

void ImagePainter2::drawLine(const PointF& from, const PointF& to)
{
    const Point a( (Pt::int32_t)(from.x()), (Pt::int32_t)(from.y()) );
    const Point b( (Pt::int32_t)(to  .x()), (Pt::int32_t)(to  .y()) );

    _rasterizer->strokeOnePixelSolidLine(a, b, 0);
}

void ImagePainter2::drawRect( const RectF& rect )
{
    const Point tl( rect.topLeft    ().x(),rect.topLeft    ().y() );
    const Point br( rect.bottomRight().x(),rect.bottomRight().y() );

    _rasterizer->strokeOnePixelSolidRect(tl, br);
}

void ImagePainter2::fillRect( const RectF& rect )
{
    const Point tl( rect.topLeft    ().x(),rect.topLeft    ().y() );
    const Point br( rect.bottomRight().x(),rect.bottomRight().y() );
    _rasterizer->fillRect(tl, br);
}

void ImagePainter2::drawPolyline( const PointF* ps, const size_t pointCount )
{
}

void ImagePainter2::fillPolygon( const PointF* ps, const size_t pointCount )
{
    // Copy the points
    std::vector<Point> points(pointCount);

    for(size_t i = 0; i < pointCount; ++i)
        points[i].set( ps[i].x(), ps[i].y() );

    // Rasterize the polygon
    _rasterizer->fillPolygon(points.data(), pointCount);
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

/*
// Inspired by http://create.stephan-brumme.com/antialiased-circle
void ImagePainter2::fillArcChordImpl(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    // Calculate the ellipse's parameters
    Pt::int32_t minX  = topLeft.x();
    Pt::int32_t minY  = topLeft.y();
    Pt::int32_t radX  = size.width () / 2;
    Pt::int32_t radY  = size.height() / 2;
    Pt::int32_t ctrX  = minX + radX;
    Pt::int32_t ctrY  = minY + radY;

    // === Process the scanlines ===

    // Exact coordinate of the points which are located at the begin and end angle
    Pt::int32_t x1 = 0; // Begin point
    Pt::int32_t y1 = 0;
    Pt::int32_t x2 = 0; // End point
    Pt::int32_t y2 = 0;

    // Generate a list of scanlines that will be drawn later
    const bool  useAntiAliasing = ( _rasterizer->antiAliasingMode() != AntiAliasingMode::None );
    Pt::int32_t quartersX;
    Pt::int32_t quartersY;
    Scanlines   scanlines;
    arcUtilCalcScanlines(radX, radY, ctrX, ctrY, degBegin, degEnd, useAntiAliasing, quartersX, quartersY, x1, y1, x2, y2, scanlines);

    // Determine where the direction that the hole faces to
    bool faceL, faceR, faceT, faceB;
    arcUtilDetermineHoleDirection(x1, y1, x2, y2, faceL, faceR, faceT, faceB);
    // lprintf("l=%d r=%d t=%d b=%d\n", faceL, faceR, faceT, faceB);

    // Copy the list of scanlines to be drawn later
    const Scanlines scanlinesRef = scanlines;

    // Remove the all scanlines to the top and bottom side that will be completely outside the shape
    if(faceT) scanlines.erase(scanlines.begin(),                           scanlines.lower_bound(std::min(y1, y2) + 1));
    if(faceB) scanlines.erase(scanlines.upper_bound(std::max(y1, y2) - 1), scanlines.end()                            );

    // Crop the scanlines to the left and right side by running the Xiaolin Wu's anti-aliased line algorithm
    if(faceL || faceR) {
        arcUtilCropScanlinesUsingXWu(x1, y1, x2, y2, faceL, faceR, faceT, faceB, scanlines);
    }

    // Mark the all scanlines to the left and right side that will be completely outside the shape
    arcUtilMarkOutsideScanlinesRL(x1, y1, x2, y2, faceL, faceR, faceT, faceB, scanlines);

    // Draw the scanlines
    for(Scanlines::const_iterator it = scanlines.begin(); it != scanlines.end(); ++it) {
        _rasterizer->fillOneScanlineNoAA(it->second.from, it->second.to, it->first, minX, minY);
    }

    // Exit here if we are not doing anti aliasing
    if(_rasterizer->antiAliasingMode() == AntiAliasingMode::None)  return;

    // === Process the circumference's pixels ===

    // Pixels from the ellipse's circumference
    arcUtilDrawCircumferencePixels(minX, minY, radX, radY, ctrX, ctrY, degBegin, degEnd, quartersX, quartersY, x1, y1, x2, y2, scanlinesRef);

    // Draw the closing line
    // --- SourceCopy ---
    if(_rasterizer->compositionMode() == CompositionMode::SourceCopy) {
        _rasterizer->fillOnePixelGLineSegmentXWAA(x1, y1, x2, y2, minX, minY, 0, 0);
    }

    // --- SourceOver ---
    std::map<Pt::int32_t, Pt::int32_t> exclusionZone;

    for(Scanlines::const_iterator it = scanlines.begin(); it != scanlines.end(); ++it) {
        if(faceL) exclusionZone[it->first] = it->second.from;
        if(faceR) exclusionZone[it->first] = it->second.to;
    }

    _rasterizer->fillOnePixelGLineSegmentXWAA(x1, y1, x2, y2, minX, minY, &exclusionZone, 0);
}

void ImagePainter2::fillArcPieImpl(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    // Calculate the ellipse's parameters
    Pt::int32_t minX  = topLeft.x();
    Pt::int32_t minY  = topLeft.y();
    Pt::int32_t radX  = size.width () / 2;
    Pt::int32_t radY  = size.height() / 2;
    Pt::int32_t ctrX  = minX + radX;
    Pt::int32_t ctrY  = minY + radY;

    // === Process the scanlines ===

    // Exact coordinate of the points which are located at the begin and end angle
    Pt::int32_t x1 = 0; // Begin point
    Pt::int32_t y1 = 0;
    Pt::int32_t x2 = 0; // End point
    Pt::int32_t y2 = 0;

    // Generate a list of scanlines that will be drawn later
    const bool  useAntiAliasing = ( _rasterizer->antiAliasingMode() != AntiAliasingMode::None );
    Pt::int32_t quartersX;
    Pt::int32_t quartersY;
    Scanlines   scanlines;
    arcUtilCalcScanlines(radX, radY, ctrX, ctrY, degBegin, degEnd, useAntiAliasing, quartersX, quartersY, x1, y1, x2, y2, scanlines);

    // Determine where the direction that the hole faces to
    bool faceL, faceR, faceT, faceB;
    arcUtilDetermineHoleDirection(x1, y1, x2, y2, faceL, faceR, faceT, faceB);
    // lprintf("l=%d r=%d t=%d b=%d\n", faceL, faceR, faceT, faceB);

    // Copy the list of scanlines to be drawn later
    const Scanlines scanlinesRef = scanlines;

    // Remove the all scanlines to the top and bottom side that will be completely outside the shape
    if(faceT) scanlines.erase(scanlines.begin(),                           scanlines.lower_bound(std::min(y1, y2) + 1));
    if(faceB) scanlines.erase(scanlines.upper_bound(std::max(y1, y2) - 1), scanlines.end()                            );

    // Crop the scanlines to the left and right side by running the Xiaolin Wu's anti-aliased line algorithm
    if(faceL || faceR) {
        // TODO: FAILED TO CROP PROPERLY FOR V SHAPES !!!
        // TODO: FAILED TO CROP PROPERLY IF THE LINE IS STEEP !!!
        arcUtilCropScanlinesUsingXWu(x1, y1, ctrX, ctrY, faceL, faceR, faceT, faceB, scanlines);
        arcUtilCropScanlinesUsingXWu(ctrX, ctrY, x2, y2, faceL, faceR, faceT, faceB, scanlines);
    }

    // Mark the all scanlines to the left and right side that will be completely outside the shape
    arcUtilMarkOutsideScanlinesRL(x1, y1, x2, y2, faceL, faceR, faceT, faceB, scanlines);

    // Draw the scanlines
    for(Scanlines::const_iterator it = scanlines.begin(); it != scanlines.end(); ++it) {
        _rasterizer->fillOneScanlineNoAA(it->second.from, it->second.to, it->first, minX, minY);
    }

    // Exit here if we are not doing anti aliasing
    if(_rasterizer->antiAliasingMode() == AntiAliasingMode::None)  return;

    // === Process the circumference's pixels ===

    // Pixels from the ellipse's circumference
    arcUtilDrawCircumferencePixels(minX, minY, radX, radY, ctrX, ctrY, degBegin, degEnd, quartersX, quartersY, x1, y1, x2, y2, scanlinesRef);

    // Draw the closing line
    Rasterizer2::DrawLineMask mask = Rasterizer2::NullLineMask;

    // --- SourceCopy ---
    if(_rasterizer->compositionMode() == CompositionMode::SourceCopy) {
        _rasterizer->fillOnePixelGLineSegmentXWAA(x1, y1, ctrX, ctrY, minX, minY, 0, &mask);
        _rasterizer->fillOnePixelGLineSegmentXWAA(ctrX, ctrY, x2, y2, minX, minY, 0, &mask);
    }

    // --- SourceOver ---

    // TODO: USE TWO EXCLUZION ZONE, ONE FOR EACH LINE !!!
    // TODO: FIRST, NEEDS TO DETERMINE TO WHICH SIDE THE LINES ARE FACING TO !!!

    std::map<Pt::int32_t, Pt::int32_t> exclusionZone;

    for(Scanlines::const_iterator it = scanlines.begin(); it != scanlines.end(); ++it) {
        if(faceL) exclusionZone[it->first] = it->second.from;
        if(faceR) exclusionZone[it->first] = it->second.to;
    }

    _rasterizer->fillOnePixelGLineSegmentXWAA(x1, y1, ctrX, ctrY, minX, minY, &exclusionZone, &mask);
    _rasterizer->fillOnePixelGLineSegmentXWAA(ctrX, ctrY, x2, y2, minX, minY, &exclusionZone, &mask);

}

void ImagePainter2::arcUtilCalcScanlines(Pt::int32_t radX, Pt::int32_t radY, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBegin, float degEnd, bool useAntiAliasing, Pt::int32_t& quartersX, Pt::int32_t& quartersY, Pt::int32_t& x1, Pt::int32_t& y1, Pt::int32_t& x2, Pt::int32_t& y2, Scanlines& scanlines)
{
    // IMPORTANT NOTES:
    //     * In Cartesian coordinate system, the Y coordinate goes from low to high,
    //       from the middle axis (X) to the top
    //     * In computer coordinate system, the Y coordinate goes from low to high,
    //       from the top of the screen to the bottom
    //     * This will cause addition and subtraction to be reversed when calculating
    //       for the Y coordinate using trigonometry

    // Calculate the square of the radius
    Pt::int32_t radX2 = radX * radX;
    Pt::int32_t radY2 = radY * radY;

    // Calculate the approximate coordinate of the point which is located at the begin angle
    const Pt::int32_t bx = round(ctrX + radX * fastCos(degBegin * Pt::Pi / 180));
    const Pt::int32_t by = round(ctrY - radY * fastSin(degBegin * Pt::Pi / 180)); // See the notes on the beginning of this function

    // Calculate the approximate coordinate of the point which is located at the end angle
    const Pt::int32_t ex = round(ctrX + radX * fastCos(degEnd   * Pt::Pi / 180));
    const Pt::int32_t ey = round(ctrY - radY * fastSin(degEnd   * Pt::Pi / 180)); // See the notes on the beginning of this function

    // Used for finding the exact coordinate of the points which are located at the begin and end angle
    Pt::int32_t x1d = MAXIMUM_COORD; // Begin point
    Pt::int32_t y1d = MAXIMUM_COORD;
    Pt::int32_t x2d = MAXIMUM_COORD; // End point
    Pt::int32_t y2d = MAXIMUM_COORD;

    // Top and bottom halves
    quartersX = round( radX2 * fastInvSqrt(radX2 + radY2) );

    for(Pt::int32_t x = 0; x <= quartersX; ++x) {
        // Calculate the coordinate
        const float y = radY * fastSqrt(1 - (float) x * x / radX2);
        // Without anti-aliasing
        if(!useAntiAliasing) {
            // Calculate the coordinates
            const Pt::int32_t xl = ctrX - x;
            const Pt::int32_t xr = ctrX + x;
            const Pt::int32_t yt = ctrY - round(y);
            const Pt::int32_t yb = ctrY + round(y);
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
            const Pt::int32_t xl = ctrX - x;
            const Pt::int32_t xr = ctrX + x;
            const Pt::int32_t yt = ctrY - floor(y);
            const Pt::int32_t yb = ctrY + floor(y);
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
}

void ImagePainter2::arcUtilCropScanlinesUsingXWu(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, bool faceL, bool faceR, bool faceT, bool faceB, Scanlines& scanlines)
{
    // Convert the coordinates to fixed-points
    Pt::int32_t fx1 = FIXED_POINT_FROM_INT(x1);
    Pt::int32_t fy1 = FIXED_POINT_FROM_INT(y1);
    Pt::int32_t fx2 = FIXED_POINT_FROM_INT(x2);
    Pt::int32_t fy2 = FIXED_POINT_FROM_INT(y2);

    // Swap the values as needed
    const Pt::int32_t deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
    const Pt::int32_t deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);
    const bool        steep  = deltaY > deltaX;

    if(steep) {
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

    // Walk through the coordinates
    Pt::int32_t from  = FIXED_POINT_TO_INT(FIXED_POINT_ROUND(fx1));
    Pt::int32_t to    = FIXED_POINT_TO_INT(xpxl2);
    Pt::int32_t ypxli = ypxl;
    if(steep) {
        for(Pt::int32_t i = from; i <= to; ++i) {
            // Calculate the coordinates
            Pt::int32_t refX = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli));
            Pt::int32_t refY = i;
            ypxli += grad;
            // Crop the corresponding scanline
            Scanlines::iterator it = scanlines.find(refY);
            if(it == scanlines.end()) continue;
            if(faceL && it->second.from < refX) it->second.from = refX;
            if(faceR && it->second.to   > refX) it->second.to   = refX;
        }
    }
    else {
        for(Pt::int32_t i = from; i <= to; ++i) {
            // Calculate the coordinates
            Pt::int32_t refX  = i;
            Pt::int32_t refYt = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           );
            Pt::int32_t refYb = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE);
            ypxli += grad;
            // Crop the corresponding scanlines
            Scanlines::iterator itt = scanlines.find(refYt);
            Scanlines::iterator itb = scanlines.find(refYb);
            if(itt != scanlines.end()) {
                if(faceL && itt->second.from < refX - 1) itt->second.from = refX - 1;
                if(faceR && itt->second.to   > refX    ) itt->second.to   = refX;
            }
            if(itb != scanlines.end()) {
                if(faceL && itb->second.from < refX - 1) itb->second.from = refX - 1;
                if(faceR && itb->second.to   > refX    ) itb->second.to   = refX;
            }
        }
    }
}

void ImagePainter2::arcUtilDrawCircumferencePixels(Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t radX, Pt::int32_t radY, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBegin, float degEnd, Pt::int32_t quartersX, Pt::int32_t quartersY, Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Scanlines& scanlinesRef)
{
    // Calculate the square of the radius
    Pt::int32_t radX2 = radX * radX;
    Pt::int32_t radY2 = radY * radY;

    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= quartersX; ++x) {
        // Calculate the Y coordinate and alpha
        const float       y     = radY * fastSqrt(1 - (float) x * x / radX2);
        const Pt::int32_t fly   = floor(y);
        const float       error = y - fly;
        const Pt::uint8_t alpha = round(error * 255);
        // Draw the first part of the pixels
        const Pt::int32_t x1  = ctrX - x;
        const Pt::int32_t x2  = ctrX + x;
        const Pt::int32_t y10 = ctrY - fly;
        const Pt::int32_t y20 = ctrY + fly;
        Scanlines::const_iterator it10 = scanlinesRef.find(y10);
        Scanlines::const_iterator it20 = scanlinesRef.find(y20);
        if( ( it10 == scanlinesRef.end() || (it10->second.from > x1 && it10->second.to < x2) ) ||
            ( it20 == scanlinesRef.end() || (it20->second.from > x1 && it20->second.to < x2) )
        ) {
            const bool mask[4] = {
                insideDegRange(x1, y10, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x1, y20, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x2, y10, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x2, y20, ctrX, ctrY, degBegin, degEnd)
            };
            _rasterizer->fill4Pixels(x1, y10, x2, y20, minX, minY, 255 - alpha, mask);
        }
        // Draw the second part of the pixels
        const Pt::int32_t y11 = ctrY - fly - 1;
        const Pt::int32_t y21 = ctrY + fly + 1;
        Scanlines::const_iterator it11 = scanlinesRef.find(y11);
        Scanlines::const_iterator it21 = scanlinesRef.find(y21);
        if( ( it11 == scanlinesRef.end() || (it11->second.from > x1 && it11->second.to < x2) ) ||
            ( it21 == scanlinesRef.end() || (it21->second.from > x1 && it21->second.to < x2) )
        ) {
            const bool mask[4] = {
                insideDegRange(x1, y11, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x1, y21, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x2, y11, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x2, y21, ctrX, ctrY, degBegin, degEnd)
            };
            _rasterizer->fill4Pixels(x1, y11, x2, y21, minX, minY, alpha, mask);
        }
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= quartersY; ++y) {
        // Calculate the X coordinate and alpha
        const float       x     = radX * fastSqrt(1 - (float) y * y / radY2);
        const Pt::int32_t flx   = floor(x);
        const float       error = x - flx;
        const Pt::uint8_t alpha = round(error * 255);
        // Draw the first part of the pixels
        const Pt::int32_t x10 = ctrX - flx;
        const Pt::int32_t x20 = ctrX + flx;
        const Pt::int32_t y1  = ctrY - y;
        const Pt::int32_t y2  = ctrY + y;
        Scanlines::const_iterator it1 = scanlinesRef.find(y1);
        Scanlines::const_iterator it2 = scanlinesRef.find(y2);
        if( ( it1 == scanlinesRef.end() || (it1->second.from > x10 && it1->second.to < x20) ) ||
            ( it2 == scanlinesRef.end() || (it2->second.from > x10 && it2->second.to < x20) )
        ) {
            const bool mask[4] = {
                insideDegRange(x10, y1, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x10, y2, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x20, y1, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x20, y2, ctrX, ctrY, degBegin, degEnd)
            };
            _rasterizer->fill4Pixels(x10, y1, x20, y2, minX, minY, 255 - alpha, mask);
        }
        // Draw the second part of the pixels
        const Pt::int32_t x11 = ctrX - flx - 1;
        const Pt::int32_t x21 = ctrX + flx + 1;
        if( ( it1 == scanlinesRef.end() || (it1->second.from > x11 && it1->second.to < x21) ) ||
            ( it2 == scanlinesRef.end() || (it2->second.from > x11 && it2->second.to < x21) )
        ) {
            const bool mask[4] = {
                insideDegRange(x11, y1, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x11, y2, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x21, y1, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x21, y2, ctrX, ctrY, degBegin, degEnd)
            };
            _rasterizer->fill4Pixels(x11, y1, x21, y2, minX, minY, alpha, mask);
        }
    }
}
*/


} // namespace
} // namespace
