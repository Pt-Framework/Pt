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

void ImagePainter2::drawEllipse( const PointF& topLeft, const SizeF& size )
{
    drawOnePixelSolidEllipseArcImpl(topLeft, size, 0, 0, ArcMode::Open);
}

// Inspired by http://create.stephan-brumme.com/antialiased-circle
void ImagePainter2::fillEllipse( const PointF& topLeft, const SizeF& size )
{
    // Update the gradient as needed
    _rasterizer->updateGradientBrushAsNeeded(size.width(), size.height());

    // Call the fast non-AA rasterizer as needed
    if(_rasterizer->antiAliasingMode() == AntiAliasingMode::None) {
        fillEllipseImplNoAA(topLeft, size);
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
    Scanlines scanlines;

    // Top and bottom halves
    const Pt::int32_t quartersX = round( radX2 * fastInvSqrt(radX2 + radY2) );

    for(Pt::int32_t x = 0; x <= quartersX; ++x) {
        // Calculate the Y coordinate
        const float       y   = radY * fastSqrt(1 - (float) x * x / radX2);
        const Pt::int32_t fly = floor(y);
        // Store/update the scanline coordinates
        Scanlines::iterator it1 = scanlines.find(ctrY - fly);
        Scanlines::iterator it2 = scanlines.find(ctrY + fly);
        if(it1 == scanlines.end()) { // Insert a new element
            scanlines.insert( std::make_pair( ctrY - fly, ScanlineElement(ctrX - x, ctrX + x) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( ctrX - x < it1->second.from ) it1->second.from = ctrX - x;
            if( ctrX + x > it1->second.to   ) it1->second.to   = ctrX + x;
        }
        if(it2 == scanlines.end()) { // Insert a new element
            scanlines.insert( std::make_pair( ctrY + fly, ScanlineElement(ctrX - x, ctrX + x) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( ctrX - x < it2->second.from ) it2->second.from = ctrX - x;
            if( ctrX + x > it2->second.to   ) it2->second.to   = ctrX + x;
        }
    }

    // Left and right halves
    const Pt::int32_t quartersY = round( radY2 * fastInvSqrt(radX2 + radY2) );

    for(Pt::int32_t y = 0; y <= quartersY; ++y) {
        // Calculate the X coordinate
        const float       x   = radX * fastSqrt(1 - (float) y * y / radY2);
        const Pt::int32_t flx = floor(x);
        // Store/update the scanline coordinates
        Scanlines::iterator it1 = scanlines.find(ctrY - y);
        Scanlines::iterator it2 = scanlines.find(ctrY + y);
        if(it1 == scanlines.end()) { // Insert a new element
            scanlines.insert( std::make_pair( ctrY - y, ScanlineElement(ctrX - flx, ctrX + flx) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( ctrX - flx < it1->second.from ) it1->second.from = ctrX - flx;
            if( ctrX + flx > it1->second.to   ) it1->second.to   = ctrX + flx;
        }
        if(it2 == scanlines.end()) { // Insert a new element
            scanlines.insert( std::make_pair( ctrY + y, ScanlineElement(ctrX - flx, ctrX + flx) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( ctrX - flx < it2->second.from ) it2->second.from = ctrX - flx;
            if( ctrX + flx > it2->second.to   ) it2->second.to   = ctrX + flx;
        }
    }

    // Draw the scanlines
    for(Scanlines::const_iterator it = scanlines.begin(); it != scanlines.end(); ++it) {
        _rasterizer->fillOneScanlineNoAA(it->second.from, it->second.to, it->first, minX, minY);
    }

    // === Process the circumference's pixels ===

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
        Scanlines::const_iterator it10 = scanlines.find(y10);
        Scanlines::const_iterator it20 = scanlines.find(y20);
        if( ( it10 == scanlines.end() || (it10->second.from > x1 || it10->second.to < x2) ) ||
            ( it20 == scanlines.end() || (it20->second.from > x1 || it20->second.to < x2) )
        ) _rasterizer->fill4Pixels(x1, y10, x2, y20, minX, minY, alpha);
        // Draw the second part of the pixels
        const Pt::int32_t y11 = ctrY - fly - 1;
        const Pt::int32_t y21 = ctrY + fly + 1;
        Scanlines::const_iterator it11 = scanlines.find(y11);
        Scanlines::const_iterator it21 = scanlines.find(y21);
        if( ( it11 == scanlines.end() || (it11->second.from > x1 || it11->second.to < x2) ) ||
            ( it21 == scanlines.end() || (it21->second.from > x1 || it21->second.to < x2) )
        ) _rasterizer->fill4Pixels(x1, y11, x2, y21, minX, minY, alpha);
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
        Scanlines::const_iterator it1 = scanlines.find(y1);
        Scanlines::const_iterator it2 = scanlines.find(y2);
        if( ( it1 == scanlines.end() || (it1->second.from > x10 || it1->second.to < x20) ) ||
            ( it2 == scanlines.end() || (it2->second.from > x10 || it2->second.to < x20) )
        ) _rasterizer->fill4Pixels(x10, y1, x20, y2, minX, minY, alpha);
        // Draw the second part of the pixels
        const Pt::int32_t x11 = ctrX - flx - 1;
        const Pt::int32_t x21 = ctrX + flx + 1;
        if( ( it1 == scanlines.end() || (it1->second.from > x11 || it1->second.to < x21) ) ||
            ( it2 == scanlines.end() || (it2->second.from > x11 || it2->second.to < x21) )
        ) _rasterizer->fill4Pixels(x11, y1, x21, y2, minX, minY, alpha);
    }
}

void ImagePainter2::drawArc( const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, const ArcMode& arcMode )
{
    drawOnePixelSolidEllipseArcImpl(topLeft, size, degBegin, degEnd, arcMode);
}

void ImagePainter2::fillArc( const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, const ArcMode& arcMode )
{
    // Draw based on the mode
    switch(arcMode) {
        case ArcMode::Chord:
            fillArcChordImpl(topLeft, size, degBegin, degEnd);
            break;

        case ArcMode::Pie:
            fillArcPieImpl(topLeft, size, degBegin, degEnd);
            break;
    }
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

// Inspired by http://create.stephan-brumme.com/antialiased-circle
void ImagePainter2::drawOnePixelSolidEllipseArcImpl(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, const ArcMode& arcMode)
{
    // IMPORTANT NOTES:
    //     * In Cartesian coordinate system, the Y coordinate goes from low to high,
    //       from the middle axis (X) to the top
    //     * In computer coordinate system, the Y coordinate goes from low to high,
    //       from the top of the screen to the bottom
    //     * This will cause addition and subtraction to be interchanged when calculating
    //       for the Y coordinate using trigonometry

    // Shall we draw an ellipse or arc?
    const bool drawArc = (degBegin != 0) || (degEnd != 0);

    // Calculate the ellipse's parameters
    Pt::int32_t minX  = topLeft.x();
    Pt::int32_t minY  = topLeft.y();
    Pt::int32_t radX  = size.width () / 2;
    Pt::int32_t radY  = size.height() / 2;
    Pt::int32_t ctrX  = minX + radX;
    Pt::int32_t ctrY  = minY + radY;
    Pt::int32_t radX2 = radX * radX;
    Pt::int32_t radY2 = radY * radY;

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
        bx = round(ctrX + radX * fastCos(degBegin * Pt::Pi / 180));
        by = round(ctrY - radY * fastSin(degBegin * Pt::Pi / 180)); // See the notes on the beginning of this function
        // Calculate the approximate coordinate of the point which is located at the end angle
        ex = round(ctrX + radX * fastCos(degEnd   * Pt::Pi / 180));
        ey = round(ctrY - radY * fastSin(degEnd   * Pt::Pi / 180)); // See the notes on the beginning of this function
    }

    // Top and bottom halves
    const Pt::int32_t quartersX = round( radX2 * fastInvSqrt(radX2 + radY2) );

    for(Pt::int32_t x = 0; x <= quartersX; ++x) {
        // Calculate the coordinate and alpha
        const float       y     = radY * fastSqrt(1 - (float) x * x / radX2);
        const float       error = y - floor(y);
        const Pt::uint8_t alpha = round(error * 255);
        // Without anti-aliasing
        if(_rasterizer->antiAliasingMode() == AntiAliasingMode::None) {
            // Calculate the coordinates
            const Pt::int32_t xl = ctrX - x;
            const Pt::int32_t xr = ctrX + x;
            const Pt::int32_t yt = ctrY - round(y);
            const Pt::int32_t yb = ctrY + round(y);
            // Arc
            if(drawArc) {
                // Draw the pixels
                const bool mask[4] = {
                    insideDegRange(xl, yt, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xl, yb, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xr, yt, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xr, yb, ctrX, ctrY, degBegin, degEnd)
                };
                _rasterizer->stroke4Pixels(xl, yt, xr, yb, mask);
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
               _rasterizer->stroke4Pixels(xl, yt, xr, yb);
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
                    insideDegRange(xl, yt0, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xl, yb0, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xr, yt0, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xr, yb0, ctrX, ctrY, degBegin, degEnd)
                };
                const bool mask1[4] = {
                    insideDegRange(xl, yt1, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xl, yb1, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xr, yt1, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xr, yb1, ctrX, ctrY, degBegin, degEnd)
                };
                const Pt::uint8_t a0 = Rasterizer2::XWAA_WFILTER[      alpha];
                const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[255 - alpha];
                _rasterizer->stroke4Pixels(xl, yt0, xr, yb0, a0, mask0);
                _rasterizer->stroke4Pixels(xl, yt1, xr, yb1, a1, mask1);
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
                _rasterizer->stroke4Pixels(xl, yt0, xr, yb0, a0);
                _rasterizer->stroke4Pixels(xl, yt1, xr, yb1, a1);
            }
        }
    }

    // Left and right halves
    const Pt::int32_t quartersY = round( radY2 * fastInvSqrt(radX2 + radY2) );

    for(Pt::int32_t y = 0; y <= quartersY; ++y) {
        // Calculate the coordinate and alpha
        const float       x     = radX * fastSqrt(1 - (float) y * y / radY2);
        const float       error = x - floor(x);
        const Pt::uint8_t alpha = round(error * 255);
        // Without anti-aliasing
        if(_rasterizer->antiAliasingMode() == AntiAliasingMode::None) {
            // Calculate the coordinates
            const Pt::int32_t xl = ctrX - round(x);
            const Pt::int32_t xr = ctrX + round(x);
            const Pt::int32_t yt = ctrY - y;
            const Pt::int32_t yb = ctrY + y;
            // Arc
            if(drawArc) {
                // Draw the pixels
                const bool mask[4] = {
                    insideDegRange(xl, yt, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xl, yb, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xr, yt, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xr, yb, ctrX, ctrY, degBegin, degEnd)
                };
                _rasterizer->stroke4Pixels(xl, yt, xr, yb, mask);
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
                _rasterizer->stroke4Pixels(xl, yt, xr, yb);
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
                    insideDegRange(xl0, yt, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xl0, yb, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xr0, yt, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xr0, yb, ctrX, ctrY, degBegin, degEnd)
                };
                const bool mask1[4] = {
                    insideDegRange(xl1, yt, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xl1, yb, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xr1, yt, ctrX, ctrY, degBegin, degEnd),
                    insideDegRange(xr1, yb, ctrX, ctrY, degBegin, degEnd)
                };
                const Pt::uint8_t a0 = Rasterizer2::XWAA_WFILTER[      alpha];
                const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[255 - alpha];
                _rasterizer->stroke4Pixels(xl0, yt, xr0, yb, a0, mask0);
                _rasterizer->stroke4Pixels(xl1, yt, xr1, yb, a1, mask1);
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
                _rasterizer->stroke4Pixels(xl0, yt, xr0, yb, a0);
                _rasterizer->stroke4Pixels(xl1, yt, xr1, yb, a1);
            }
        }
    }

    // Draw the arc's closing lines
    if(drawArc) {
        Rasterizer2::DrawLineMask mask = Rasterizer2::NullLineMask;
        if(arcMode == ArcMode::Chord) {
            const Point a(x1, y1);
            const Point b(x2, y2);
            _rasterizer->strokeOnePixelSolidLine(a, b, &mask);
        }
        else if(arcMode == ArcMode::Pie) {
            const Point a(bx, by);
            const Point b(ex, ey);
            const Point o(ctrX, ctrY);
            _rasterizer->strokeOnePixelSolidLine(a, o, &mask);
            _rasterizer->strokeOnePixelSolidLine(b, o, &mask);
        }
    }
}

void ImagePainter2::fillEllipseImplNoAA( const PointF& topLeft, const SizeF& size )
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
            _rasterizer->fillOneScanlineNoAA(xc - x, xc - x + width - errorX - 1, yc - y,          minX, minY);
            _rasterizer->fillOneScanlineNoAA(xc - x, xc - x + width - errorX - 1, yc + y - errorY, minX, minY);
            --y;
            dyt += d2yt;
            t   += dyt;
        }
        else {
            _rasterizer->fillOneScanlineNoAA(xc - x, xc - x + width - errorX - 1, yc - y,          minX, minY);
            _rasterizer->fillOneScanlineNoAA(xc - x, xc - x + width - errorX - 1, yc + y - errorY, minX, minY);
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
        _rasterizer->fillOneScanlineNoAA(xc - a,  xc + a, yc, minX, minY);
}

// Inspired by http://create.stephan-brumme.com/antialiased-circle
void ImagePainter2::fillArcChordImpl(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    // IMPORTANT NOTES:
    //     * In Cartesian coordinate system, the Y coordinate goes from low to high,
    //       from the middle axis (X) to the top
    //     * In computer coordinate system, the Y coordinate goes from low to high,
    //       from the top of the screen to the bottom
    //     * This will cause addition and subtraction to be reversed when calculating
    //       for the Y coordinate using trigonometry


    // XXXXX    * This will also cause >= and < comparison operators to be interchanged when
    //      XXXXXX determining quadrant using the Y coordinate

    // Update the gradient as needed
    _rasterizer->updateGradientBrushAsNeeded(size.width(), size.height());

    // Ensure that the begin angle is within the acceptable range
    while(degBegin < -360) degBegin += 360;
    while(degBegin >  360) degBegin -= 360;

    // Ensure that the end angle is within the acceptable range
    while(degEnd < -360) degEnd += 360;
    while(degEnd >  360) degEnd -= 360;

    // Calculate the ellipse's parameters
    Pt::int32_t minX  = topLeft.x();
    Pt::int32_t minY  = topLeft.y();
    Pt::int32_t radX  = size.width () / 2;
    Pt::int32_t radY  = size.height() / 2;
    Pt::int32_t ctrX  = minX + radX;
    Pt::int32_t ctrY  = minY + radY;
    Pt::int32_t radX2 = radX * radX;
    Pt::int32_t radY2 = radY * radY;

    // Calculate the approximate coordinate of the point which is located at the begin angle
    const Pt::int32_t bx = round(ctrX + radX * fastCos(degBegin * Pt::Pi / 180));
    const Pt::int32_t by = round(ctrY - radY * fastSin(degBegin * Pt::Pi / 180)); // See the notes on the beginning of this function

    // Calculate the approximate coordinate of the point which is located at the end angle
    const Pt::int32_t ex = round(ctrX + radX * fastCos(degEnd   * Pt::Pi / 180));
    const Pt::int32_t ey = round(ctrY - radY * fastSin(degEnd   * Pt::Pi / 180)); // See the notes on the beginning of this function

    // Used for finding the exact coordinate of the points which are located at the begin and end angle
    Pt::int32_t x1 = 0, x1d = MAXIMUM_COORD; // Begin point
    Pt::int32_t y1 = 0, y1d = MAXIMUM_COORD;
    Pt::int32_t x2 = 0, x2d = MAXIMUM_COORD; // End point
    Pt::int32_t y2 = 0, y2d = MAXIMUM_COORD;

    // === Process the scanlines ===

    // List of scanlines to be drawn later
    Scanlines scanlines;

    // Top and bottom halves
    const Pt::int32_t quartersX = round( radX2 * fastInvSqrt(radX2 + radY2) );

    for(Pt::int32_t x = 0; x <= quartersX; ++x) {
        // Calculate the coordinate and alpha
        const float y = radY * fastSqrt(1 - (float) x * x / radX2);
        // Without anti-aliasing
        if(_rasterizer->antiAliasingMode() == AntiAliasingMode::None) {
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
    const Pt::int32_t quartersY = round( radY2 * fastInvSqrt(radX2 + radY2) );

    for(Pt::int32_t y = 0; y <= quartersY; ++y) {
        // Calculate the coordinate and alpha
        const float x = radX * fastSqrt(1 - (float) y * y / radY2);
        // Without anti-aliasing
        if(_rasterizer->antiAliasingMode() == AntiAliasingMode::None) {
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

    /*
    // Determine the location of the begin and end points
    Pt::int32_t qBeg = 0, qEnd = 0;

         if(x1 >= ctrX && y1 <  ctrY) qBeg = 1; // See the notes on the beginning of this function
    else if(x1 <  ctrX && y1 <  ctrY) qBeg = 2; // See the notes on the beginning of this function
    else if(x1 <  ctrX && y1 >= ctrY) qBeg = 3; // See the notes on the beginning of this function
    else if(x1 >= ctrX && y1 >= ctrY) qBeg = 4; // See the notes on the beginning of this function

         if(x2 >= ctrX && y2 <  ctrY) qEnd = 1; // See the notes on the beginning of this function
    else if(x2 <  ctrX && y2 <  ctrY) qEnd = 2; // See the notes on the beginning of this function
    else if(x2 <  ctrX && y2 >= ctrY) qEnd = 3; // See the notes on the beginning of this function
    else if(x2 >= ctrX && y2 >= ctrY) qEnd = 4; // See the notes on the beginning of this function

    // Determine where the direction that the hole faces to
    bool faceL = false, faceR = false, faceT = false, faceB = false;

    switch(qBeg) {
        case 1 : switch(qEnd) {
                     case 1 : faceL = true ; faceR = false; faceT = false; faceB = true ; break;
                     case 2 : faceL = true ; faceR = false; faceT = false; faceB = true ; break;
                     case 3 : faceL = false; faceR = true ; faceT = false; faceB = true ; break;
                     case 4 : faceL = false; faceR = true ; faceT = false; faceB = true ; break;
                 }
                 break;
        case 2 : switch(qEnd) {
                     case 1 : faceL = false; faceR = true ; faceT = true ; faceB = false; break;
                     case 2 : faceL = false; faceR = true ; faceT = false; faceB = true ; break;
                     case 3 : faceL = false; faceR = true ; faceT = false; faceB = true ; break;
                     case 4 : faceL = false; faceR = true ; faceT = true ; faceB = false; break;
                 }
                 break;
        case 3 : switch(qEnd) {
                     case 1 : faceL = true ; faceR = false; faceT = true ; faceB = false; break;
                     case 2 : faceL = true ; faceR = false; faceT = true ; faceB = false; break;
                     case 3 : faceL = false; faceR = true ; faceT = true ; faceB = false; break;
                     case 4 : faceL = false; faceR = true ; faceT = true ; faceB = false; break;
                 }
                 break;
        case 4 : switch(qEnd) {
                     case 1 : faceL = true ; faceR = false; faceT = true ; faceB = false; break;
                     case 2 : faceL = true ; faceR = false; faceT = false; faceB = true ; break;
                     case 3 : faceL = true ; faceR = false; faceT = false; faceB = true ; break;
                     case 4 : faceL = true ; faceR = false; faceT = true ; faceB = false; break;
                 }
                 break;
    }

    if(x1 == x2) {
        faceT = false;
        faceB = false;
    }

    if(y1 == y2) {
        faceL = false;
        faceR = false;
    }
    switch(qBeg) {
        case 1 : switch(qEnd) {
                     case 1 : faceL = x2 < x1; faceR = x2 > x1; faceT = y2 > y1; faceB = y2 > y1; break;
                     case 2 : faceL = x2 < x1; faceR = x2 > x1; faceT = y2 > y1; faceB = y2 > y1; break;
                     case 3 : faceL = x2 > x1; faceR = x2 < x1; faceT = y2 > y1; faceB = y2 > y1; break;
                     case 4 : faceL = x2 > x1; faceR = x2 < x1; faceT = y2 > y1; faceB = y2 > y1; break;
                 }
                 break;
        case 2 : switch(qEnd) {
                     case 1 : faceL = x2 > x1; faceR = x2 > x1; faceT = y2 > y1; faceB = y2 > y1; break;
                     case 2 : faceL = x2 > x1; faceR = x2 > x1; faceT = y2 > y1; faceB = y2 > y1; break;
                     case 3 : faceL = x2 > x1; faceR = x2 > x1; faceT = y2 > y1; faceB = y2 > y1; break;
                     case 4 : faceL = x2 > x1; faceR = x2 > x1; faceT = y2 > y1; faceB = y2 > y1; break;
                 }
                 break;
        case 3 : switch(qEnd) {
                     case 1 : faceL = x2 > x1; faceR = x2 > x1; faceT = y2 > y1; faceB = y2 > y1; break;
                     case 2 : faceL = x2 > x1; faceR = x2 > x1; faceT = y2 > y1; faceB = y2 > y1; break;
                     case 3 : faceL = x2 > x1; faceR = x2 > x1; faceT = y2 > y1; faceB = y2 > y1; break;
                     case 4 : faceL = x2 > x1; faceR = x2 > x1; faceT = y2 > y1; faceB = y2 > y1; break;
                 }
                 break;
        case 4 : switch(qEnd) {
                     case 1 : faceL = x2 > x1; faceR = x2 > x1; faceT = y2 > y1; faceB = y2 > y1; break;
                     case 2 : faceL = x2 > x1; faceR = x2 > x1; faceT = y2 > y1; faceB = y2 > y1; break;
                     case 3 : faceL = x2 > x1; faceR = x2 > x1; faceT = y2 > y1; faceB = y2 > y1; break;
                     case 4 : faceL = x2 > x1; faceR = x2 > x1; faceT = y2 > y1; faceB = y2 > y1; break;
                 }
                 break;
    }
    */

    // Determine where the direction that the hole faces to

    const Pt::int32_t vx = x2 - x1;           // Vector from the begin point to the end point
    const Pt::int32_t vy = y2 - y1;           // ---
    const Pt::int32_t vz = 0;                 // ---
    const Pt::int32_t rx = 0;                 // Vector from the point of origin (0, 0, 0) that points out of the monitor
    const Pt::int32_t ry = 0;                 // ---
    const Pt::int32_t rz = 1;                 // ---
    const Pt::int32_t cx = vy * rz - vz * ry; // Cross product of the above vectors
    const Pt::int32_t cy = vz * rx - vx * rz; // ---
    const Pt::int32_t cz = vx * ry - vy * rx; // ---

    bool faceT = cy < 0;
    bool faceB = cy > 0;
    bool faceL = cx < 0;
    bool faceR = cx > 0;

    //lprintf("qb=%d qe=%d : l=%d r=%d t=%d b=%d\n", qBeg, qEnd, faceL, faceR, faceT, faceB);
    lprintf("l=%d r=%d t=%d b=%d\n", faceL, faceR, faceT, faceB);

    // Crop the spans to the top and bottom as needed
    if(faceL || faceR) {
        scanlines.erase(scanlines.begin(),                       scanlines.lower_bound(std::min(y1, y2)));
        scanlines.erase(scanlines.upper_bound(std::max(y1, y2)), scanlines.end()                        );
    }
    else {
        if(faceT) scanlines.erase(scanlines.begin(),                       scanlines.lower_bound(std::min(y1, y2)));
        if(faceB) scanlines.erase(scanlines.upper_bound(std::max(y1, y2)), scanlines.end()                        );
    }

    // Crop the spans to the left and right by running the Xiaolin Wu's anti-aliased line algorithm
    if(faceL || faceR) {
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

    // Draw the scanlines
    for(Scanlines::const_iterator it = scanlines.begin(); it != scanlines.end(); ++it) {
        _rasterizer->fillOneScanlineNoAA(it->second.from, it->second.to, it->first, minX, minY);
    }

    // Exit here if we are not doing anti aliasing
    if(_rasterizer->antiAliasingMode() == AntiAliasingMode::None)  return;

    // === Process the circumference's pixels ===

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
        Scanlines::const_iterator it10 = scanlines.find(y10);
        Scanlines::const_iterator it20 = scanlines.find(y20);
        if( ( it10 == scanlines.end() || (it10->second.from > x1 || it10->second.to < x2) ) ||
            ( it20 == scanlines.end() || (it20->second.from > x1 || it20->second.to < x2) )
        ) {
            const bool mask[4] = {
                insideDegRange(x1, y10, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x1, y20, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x2, y10, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x2, y20, ctrX, ctrY, degBegin, degEnd)
            };
            const Pt::uint8_t a = Rasterizer2::XWAA_WFILTER[alpha];
            _rasterizer->fill4Pixels(x1, y10, x2, y20, minX, minY, a, mask);
        }
        // Draw the second part of the pixels
        const Pt::int32_t y11 = ctrY - fly - 1;
        const Pt::int32_t y21 = ctrY + fly + 1;
        Scanlines::const_iterator it11 = scanlines.find(y11);
        Scanlines::const_iterator it21 = scanlines.find(y21);
        if( ( it11 == scanlines.end() || (it11->second.from > x1 || it11->second.to < x2) ) ||
            ( it21 == scanlines.end() || (it21->second.from > x1 || it21->second.to < x2) )
        ) {
            const bool mask[4] = {
                insideDegRange(x1, y11, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x1, y21, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x2, y11, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x2, y21, ctrX, ctrY, degBegin, degEnd)
            };
            const Pt::uint8_t a = Rasterizer2::XWAA_WFILTER[255 - alpha];
            _rasterizer->fill4Pixels(x1, y11, x2, y21, minX, minY, a, mask);
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
        Scanlines::const_iterator it1 = scanlines.find(y1);
        Scanlines::const_iterator it2 = scanlines.find(y2);
        if( ( it1 == scanlines.end() || (it1->second.from > x10 || it1->second.to < x20) ) ||
            ( it2 == scanlines.end() || (it2->second.from > x10 || it2->second.to < x20) )
        ) {
            const bool mask[4] = {
                insideDegRange(x10, y1, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x10, y2, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x20, y1, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x20, y2, ctrX, ctrY, degBegin, degEnd)
            };
            const Pt::uint8_t a = Rasterizer2::XWAA_WFILTER[alpha];
            _rasterizer->fill4Pixels(x10, y1, x20, y2, minX, minY, a, mask);
        }
        // Draw the second part of the pixels
        const Pt::int32_t x11 = ctrX - flx - 1;
        const Pt::int32_t x21 = ctrX + flx + 1;
        if( ( it1 == scanlines.end() || (it1->second.from > x11 || it1->second.to < x21) ) ||
            ( it2 == scanlines.end() || (it2->second.from > x11 || it2->second.to < x21) )
        ) {
            const bool mask[4] = {
                insideDegRange(x11, y1, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x11, y2, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x21, y1, ctrX, ctrY, degBegin, degEnd),
                insideDegRange(x21, y2, ctrX, ctrY, degBegin, degEnd)
            };
            const Pt::uint8_t a = Rasterizer2::XWAA_WFILTER[255 - alpha];
            _rasterizer->fill4Pixels(x11, y1, x21, y2, minX, minY, a, mask);
        }
    }

    // Draw the closing line
    _rasterizer->fillOnePixelGLineSegmentXWAA(x1, y1, x2, y2, minX, minY, 0);
}

void ImagePainter2::fillArcPieImpl(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


} // namespace
} // namespace
