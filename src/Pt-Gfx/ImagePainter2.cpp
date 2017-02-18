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

#include <Pt/Math.h>
#include <Pt/Gfx/ImagePainter2.h>

#include "FreeType.h"

#include "Rasterizer2.h"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Static Public Member Functions =================================================
// ======================================================================================

const PointF      ImagePainter2::PolygonSeparatorPointF(99999, 99999);
const Pt::int32_t ImagePainter2::MaximumCoordinate = 65535;


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
    _rasterizer->strokeOnePixelSolidLine(a, b);
}

void ImagePainter2::drawRect( const RectF& rect )
{
    const Point tl( rect.topLeft    ().x(),rect.topLeft    ().y() );
    const Point br( rect.bottomRight().x(),rect.bottomRight().y() );
    _rasterizer->strokeOnePixelSolidRect(tl, br);
}

void ImagePainter2::drawPolyline( const PointF* ps, const size_t pointCount )
{
}

void ImagePainter2::drawEllipse( const PointF& topLeft, const SizeF& size )
{
}

void ImagePainter2::drawArc( const PointF& topLeft, const SizeF& size, float degBegin, float degEnd )
{
}

void ImagePainter2::fillRect( const RectF& rect )
{
    const Point tl( rect.topLeft    ().x(),rect.topLeft    ().y() );
    const Point br( rect.bottomRight().x(),rect.bottomRight().y() );
    _rasterizer->fillRect(tl, br);
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

void ImagePainter2::fillEllipse( const PointF& topLeft, const SizeF& size )
{
    // Call the fast non-AA rasterizer as needed
    if(_rasterizer->antiAliasingMode() == AntiAliasingMode::None) {
        fillEllipseImplNoAA(topLeft, size, _rasterizer->brush().color());
        return;
    }

    // Generate a polygon that approximates the ellipse
    std::vector<Point> points;

    // This is the original algorithm used for renderering ellipse
    // Its result should be more consistent with ImagePainter2::fillArc()
    genEllipseGeometryQSC(points, topLeft, size);

    // This algorithm seems to render worse images in some cases, but it is a bit faster
    //genEllipseGeometryXWU(points, topLeft, size);

    // This algorithm seems to render the most crisp image, but it is more than two times
    // slower than the original algorithm
    //genEllipseGeometryXMI(points, topLeft, size);

    // Rasterize the polygon
    _rasterizer->fillPolygon(points.data(), points.size());
}

void ImagePainter2::fillArc( const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, bool createPie )
{
    // Generate a polygon that approximates the arc
    std::vector<Point> points;
    genArcGeometryQSC(points, topLeft, size, degBegin, degEnd, createPie);

    // Rasterize the polygon
    _rasterizer->fillPolygon(points.data(), points.size());
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void ImagePainter2::fillEllipseImplNoAA( const PointF& topLeft, const SizeF& size, const Color& color )
{
    // Update the gradient as needed
    _rasterizer->updateGradientBrushAsNeeded(size.width(), size.height());

    // Draw the ellipse's spans as per this equation:
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
            _rasterizer->strokeScanlineNoAA(xc - x, xc - x + width - errorX - 1, yc - y,          minX, minY, color);
            _rasterizer->strokeScanlineNoAA(xc - x, xc - x + width - errorX - 1, yc + y - errorY, minX, minY, color);
            --y;
            dyt += d2yt;
            t   += dyt;
        }
        else {
            _rasterizer->strokeScanlineNoAA(xc - x, xc - x + width - errorX - 1, yc - y,          minX, minY, color);
            _rasterizer->strokeScanlineNoAA(xc - x, xc - x + width - errorX - 1, yc + y - errorY, minX, minY, color);
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
        _rasterizer->strokeScanlineNoAA(xc - a,  xc + a, yc, minX, minY, color);
}

void ImagePainter2::genEllipseGeometryQSC( std::vector<Point>& points, const PointF& topLeft, const SizeF& size )
{
    // Calculate the ellipse's parameters
    const Pt::int32_t radiusX = size.width () / 2;
    const Pt::int32_t radiusY = size.height() / 2;
    const Pt::int32_t radiusM = std::max(radiusX, radiusY);
    const Pt::int32_t centerX = topLeft.x() + radiusX;
    const Pt::int32_t centerY = topLeft.y() + radiusY;
    const Pt::int32_t numSegD = (radiusM / 20) * 20;
    const Pt::int32_t numSegs = (numSegD >= 16) ? numSegD : 16;
    const Pt::int32_t qtrSegs = (numSegs / 4);
    const Pt::int32_t qtrSeg1 = qtrSegs - 1;

    // Calculate the coordinate displacements
    std::vector<float> disX(qtrSegs);
    std::vector<float> disY(qtrSegs);
    for(Pt::int32_t i = 0; i < qtrSegs; ++i) {
        // Calculate the angle
        const float angle = 0.5f * Pt::Pi * i / qtrSegs;
        // Calculate the displacements
        disX[i] =  radiusX * fastCos<float, true>(angle);
        disY[i] = -radiusY * fastSin<float, true>(angle);
    }

    // Generate a polygon that approximates the ellipse
    Pt::int32_t prevX = ImagePainter2::MaximumCoordinate;
    Pt::int32_t prevY = ImagePainter2::MaximumCoordinate;
    Pt::int32_t p     = 0;

    points.resize(numSegs);

    for(Pt::int32_t i = 0; i < qtrSegs; ++i) { // Quadrant I
        // Calculate the coordinates
        const Pt::int32_t x = centerX + disX[i];
        const Pt::int32_t y = centerY + disY[i];
        // Skip duplicated points
        if(prevX == x && prevY == y) continue;
        prevX = x;
        prevY = y;
        // Store the point and increment the index
        points[p++].set(x, y);
    }

    for(Pt::int32_t i = 0; i < qtrSegs; ++i) { // Quadrant II
        // Calculate the coordinates
        const Pt::int32_t x = centerX - disX[qtrSeg1 - i];
        const Pt::int32_t y = centerY + disY[qtrSeg1 - i];
        // Skip duplicated points
        if(prevX == x && prevY == y) continue;
        prevX = x;
        prevY = y;
        // Store the point and increment the index
        points[p++].set(x, y);
    }

    for(Pt::int32_t i = 0; i < qtrSegs; ++i) { // Quadrant III
        // Calculate the coordinates
        const Pt::int32_t x = centerX - disX[i];
        const Pt::int32_t y = centerY - disY[i];
        // Skip duplicated points
        if(prevX == x && prevY == y) continue;
        prevX = x;
        prevY = y;
        // Store the point and increment the index
        points[p++].set(x, y);
    }

    for(Pt::int32_t i = 0; i < qtrSegs; ++i) { // Quadrant IV
        // Calculate the coordinates
        const Pt::int32_t x = centerX + disX[qtrSeg1 - i];
        const Pt::int32_t y = centerY - disY[qtrSeg1 - i];
        // Skip duplicated points
        if(prevX == x && prevY == y) continue;
        prevX = x;
        prevY = y;
        // Store the point and increment the index
        points[p++].set(x, y);
    }

    // Resize the vector to remove extra elements that may exist
    points.resize(p);
}

void ImagePainter2::genEllipseGeometryXWU( std::vector<Point>& points, const PointF& topLeft, const SizeF& size )
{
    // Calculate the ellipse's parameters
    const Pt::int32_t radiusX  = size.width () / 2;
    const Pt::int32_t radiusY  = size.height() / 2;
    const Pt::int32_t radiusX2 = radiusX * radiusX;
    const Pt::int32_t radiusY2 = radiusY * radiusY;
    const Pt::int32_t centerX  = topLeft.x() + radiusX;
    const Pt::int32_t centerY  = topLeft.y() + radiusY;
    const Pt::int32_t qtrDivF  = 8;
    const Pt::int32_t qtrSegsX = round( (float) radiusX2 / sqrt(radiusX2 + radiusY2) / qtrDivF );
    const Pt::int32_t qtrSegsY = round( (float) radiusY2 / sqrt(radiusX2 + radiusY2) / qtrDivF );
    const Pt::int32_t numSegs  = (qtrSegsX + 1 + qtrSegsY + 1) * 4;

    // Calculate the coordinate displacements
    std::vector<float> disX(qtrSegsY + 1);
    std::vector<float> disY(qtrSegsX + 1);

    for(Pt::int32_t x = 0; x <= qtrSegsX; ++x) {
        disY[x] = ceil(radiusY * sqrt(1 - (float) x * x * qtrDivF * qtrDivF / radiusX2));
    }

    for(Pt::int32_t y = 0; y <= qtrSegsY; ++y) {
        disX[y] = ceil(radiusX * sqrt(1 - (float) y * y * qtrDivF * qtrDivF / radiusY2));
    }

    // Generate a polygon that approximates the ellipse
    Pt::int32_t prevX = ImagePainter2::MaximumCoordinate;
    Pt::int32_t prevY = ImagePainter2::MaximumCoordinate;
    Pt::int32_t p     = 0;

    points.resize(numSegs);

    // --- Top-right ---
    for(Pt::int32_t x = 0; x <= qtrSegsX; ++x) {
        // Calculate the coordinates
        const Pt::int32_t px = centerX + x * qtrDivF;
        const Pt::int32_t py = centerY - disY[x];
        // Skip duplicated points
        if(prevX == px && prevY == py) continue;
        prevX = px;
        prevY = py;
        // Store the point and increment the index
        points[p++].set(px, py);
    }
    for(Pt::int32_t y = qtrSegsY; y >= 0; --y) {
        // Calculate the coordinates
        const Pt::int32_t px = centerX + disX[y];
        const Pt::int32_t py = centerY - y * qtrDivF;
        // Skip duplicated points
        if(prevX == px && prevY == py) continue;
        prevX = px;
        prevY = py;
        // Store the point and increment the index
        points[p++].set(px, py);
    }

    // --- Bottom-right ---
    for(Pt::int32_t y = 0; y <= qtrSegsY; ++y) {
        // Calculate the coordinates
        const Pt::int32_t px = centerX + disX[y];
        const Pt::int32_t py = centerY + y * qtrDivF;
        // Skip duplicated points
        if(prevX == px && prevY == py) continue;
        prevX = px;
        prevY = py;
        // Store the point and increment the index
        points[p++].set(px, py);
    }
    for(Pt::int32_t x = qtrSegsX; x >=0 ; --x) {
        // Calculate the coordinates
        const Pt::int32_t px = centerX + x * qtrDivF;
        const Pt::int32_t py = centerY + disY[x];
        // Skip duplicated points
        if(prevX == px && prevY == py) continue;
        prevX = px;
        prevY = py;
        // Store the point and increment the index
        points[p++].set(px, py);
    }

    // --- Bottom-left ---
    for(Pt::int32_t x = 0; x <= qtrSegsX; ++x) {
        // Calculate the coordinates
        const Pt::int32_t px = centerX - x * qtrDivF;
        const Pt::int32_t py = centerY + disY[x];
        // Skip duplicated points
        if(prevX == px && prevY == py) continue;
        prevX = px;
        prevY = py;
        // Store the point and increment the index
        points[p++].set(px, py);
    }
    for(Pt::int32_t y = qtrSegsY; y >= 0; --y) {
        // Calculate the coordinates
        const Pt::int32_t px = centerX - disX[y];
        const Pt::int32_t py = centerY + y * qtrDivF;
        // Skip duplicated points
        if(prevX == px && prevY == py) continue;
        prevX = px;
        prevY = py;
        // Store the point and increment the index
        points[p++].set(px, py);
    }

    // --- Top-left ---
    for(Pt::int32_t y = 0; y <= qtrSegsY; ++y) {
        // Calculate the coordinates
        const Pt::int32_t px = centerX - disX[y];
        const Pt::int32_t py = centerY - y * qtrDivF;
        // Skip duplicated points
        if(prevX == px && prevY == py) continue;
        prevX = px;
        prevY = py;
        // Store the point and increment the index
        points[p++].set(px, py);
    }
    for(Pt::int32_t x = qtrSegsX; x >= 0; --x) {
        // Calculate the coordinates
        const Pt::int32_t px = centerX - x * qtrDivF;
        const Pt::int32_t py = centerY - disY[x];
        // Skip duplicated points
        if(prevX == px && prevY == py) continue;
        prevX = px;
        prevY = py;
        // Store the point and increment the index
        points[p++].set(px, py);
    }

    // Resize the vector to remove extra elements that may exist
    points.resize(p);
}

void ImagePainter2::genEllipseGeometryXMI( std::vector<Point>& points, const PointF& topLeft, const SizeF& size )
{
    // Calculate the coordinate displacements as per this equation:
    //     e(X, Y) = ( b^2 * X^2 ) + ( a^2 * Y^2 ) - ( a^2 * b^2 )

    std::vector<Pt::int32_t> disX;

    const Pt::int32_t minX   =  (Pt::int32_t) topLeft.x();
    const Pt::int32_t minY   =  (Pt::int32_t) topLeft.y();
    const Pt::int32_t a      =  size.width () / 2;
    const Pt::int32_t b      =  size.height() / 2;
    const Pt::int32_t a2     =  a * a;
    const Pt::int32_t b2     =  b * b;
    const Pt::int32_t xc     =  a;
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
            disX.push_back(xc - x);
            --y;
            dyt += d2yt;
            t   += dyt;
        }
        else {
            disX.push_back(xc - x);
            ++x;
            dxt   += d2xt;
            t     += dxt;
            width += 2;
            --y;
            dyt   += d2yt;
            t     += dyt;
        }
    }

    if( !b ) disX.push_back(xc - a);

    // Sort the coordinates
    std::sort(disX.begin(), disX.end());

    // Generate a polygon that approximates the ellipse
    const Pt::int32_t addY  = ((Pt::int32_t) size.height() % 2) ? 1 : 0;
    const Pt::int32_t incY  = (size.height() + addY) * 65536 / disX.size() / 2;
          Pt::int32_t iterY = minY * 65536;

    points.clear();

    for(size_t iterX = 0; iterX < disX.size(); ++iterX) { // Top-left
        points.push_back( Point( minX + disX[disX.size() - 1 - iterX], iterY / 65536 ) );
        iterY += incY;
    }
    for(size_t iterX = 0; iterX < disX.size(); ++iterX) { // Bottom-left
        points.push_back( Point( minX + disX[iterX], iterY / 65536 ) );
        iterY += incY;
    }
    iterY -= incY;
    for(size_t iterX = 0; iterX < disX.size(); ++iterX) { // Bottom-right
        points.push_back( Point( minX + 2 * a - disX[disX.size() - 1 - iterX], iterY / 65536 ) );
        iterY -= incY;
    }
    for(size_t iterX = 0; iterX < disX.size(); ++iterX) { // Top-right
        points.push_back( Point( minX + 2 * a - disX[iterX], iterY / 65536 ) );
        iterY -= incY;
    }
}

void ImagePainter2::genArcGeometryQSC( std::vector<Point>& points, const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, bool createPie )
{
    // Ensure that the begin and end degrees are within acceptable range
    while(degBegin <   0) degBegin += 360;
    while(degBegin > 360) degBegin -= 360;

    while(degEnd <   0) degEnd += 360;
    while(degEnd > 360) degEnd -= 360;

    if(degEnd < degBegin) std::swap(degEnd, degBegin);

    // Calculate the arc's parameters
    const Pt::int32_t radiusX = size.width () / 2;
    const Pt::int32_t radiusY = size.height() / 2;
    const Pt::int32_t radiusM = std::max(radiusX, radiusY);
    const Pt::int32_t centerX = topLeft.x() + radiusX;
    const Pt::int32_t centerY = topLeft.y() + radiusY;
    const Pt::int32_t degFac  = (degEnd - degBegin) / 36;
    const Pt::int32_t numSegD = (radiusM * (degFac ? degFac : 1) / 10 / 20) * 20;
    const Pt::int32_t numSegs = (numSegD >= 16) ? numSegD : 16;
    const float       fdegInc = (Pt::Pi * (degEnd -  degBegin) / 180) / (numSegs - 1);

    // Generate a polygon that approximates the arc
    Pt::int32_t prevX = ImagePainter2::MaximumCoordinate;
    Pt::int32_t prevY = ImagePainter2::MaximumCoordinate;
    float       angle = Pt::Pi * degBegin / 180;
    Pt::int32_t p     = 0;

    points.resize(numSegs + (createPie ? 1 : 0));

    for(Pt::int32_t i = 0; i < numSegs; ++i) {
        // Calculate the coordinates
        const Pt::int32_t x = centerX + radiusX * fastCos<float, true>(angle);
        const Pt::int32_t y = centerY - radiusY * fastSin<float, true>(angle);
        // Update the angle
        angle += fdegInc;
        // Skip duplicated points
        if(prevX == x && prevY == y) continue;
        prevX = x;
        prevY = y;
        // Store the point and increment the index
        points[p++].set(x, y);
    }

    if(createPie) // For drawing a pie, add one more point at the center of the arc
        points[p++].set(centerX, centerY);

    // Resize the vector to remove extra elements that may exist
    points.resize(p);
}


} // namespace
} // namespace
