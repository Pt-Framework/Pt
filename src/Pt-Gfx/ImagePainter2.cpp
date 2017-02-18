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


const PointF      ImagePainter2::PolygonSeparatorPointF(99999, 99999);
const Pt::int32_t ImagePainter2::MaximumCoordinate = 65535;

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
    _aaMode = mode;
}

FontMetrics ImagePainter2::fontMetrics( const Font& font, const Pt::String& text )
{
    return Rasterizer2::fontMetrics(font, text);
}

void ImagePainter2::setFontDir(const Pt::System::Path& path)
{
    FreeType::instance().setFontDir(path);
}

std::string ImagePainter2::defaultFont()
{
    return  FreeType::instance().defaultFont();
}

void ImagePainter2::setDefaultFont(const std::string& f)
{
    FreeType::instance().setDefaultFont(f);
}

std::vector<std::string> ImagePainter2::fontNames()
{
    return FreeType::instance().fontNames();
}

void ImagePainter2::setImage(Image& image)
{
    _rasterizer->setImage(image);
}

const ImageFormat& ImagePainter2::format() const
{
    return _rasterizer->format();
}

const CompositionMode& ImagePainter2::compositionMode() const
{
    return _rasterizer->compositionMode();
}

void ImagePainter2::setCompositionMode(const CompositionMode& mode)
{
    _rasterizer->setCompositionMode(mode);
}

const Gfx::RectF& ImagePainter2::clip() const
{
    return _clip;
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

void ImagePainter2::drawText( const PointF& toIn, const String& text )
{
    const Point to( (Pt::int32_t)(toIn.x()), (Pt::int32_t)(toIn.y()) );
    _rasterizer->strokeText( to, text );
}

void ImagePainter2::drawLine(const PointF& from, const PointF& to)
{
    const Point points[] = {
        Point( (Pt::int32_t)(from.x()), (Pt::int32_t)(from.y()) ),
        Point( (Pt::int32_t)(to  .x()), (Pt::int32_t)(to  .y()) )
    };
    _rasterizer->strokePolygon( points, 2 );
}

void ImagePainter2::drawRect( const RectF& rect )
{
    const Point tl( rect.topLeft    ().x(),rect.topLeft    ().y() );
    const Point br( rect.bottomRight().x(),rect.bottomRight().y() );
    _rasterizer->strokeRect(tl, br);
}

void ImagePainter2::fillRect( const RectF& rect )
{
    const Point tl( rect.topLeft    ().x(),rect.topLeft    ().y() );
    const Point br( rect.bottomRight().x(),rect.bottomRight().y() );
    _rasterizer->fillRect(tl, br);
}

void ImagePainter2::drawEllipse( const PointF& topLeft, const SizeF& size )
{
}

void ImagePainter2::fillEllipse( const PointF& topLeft, const SizeF& size )
{
    // Call the fast non-AA rasterizer as needed
    if(_aaMode == AntiAliasingMode::None) {
        // Convert the coordinates and size
        const Point topLeft_( topLeft.x    (), topLeft.y     () );
        const Size  size_   ( size   .width(), size   .height() );
        // Rasterize the ellipse
        _rasterizer->fillEllipseNoAA(topLeft_, size_);
        // Done
        return;
    }

#if 0

    // Calculate the ellipse's parameters
    const Pt::int32_t radiusX  = size.width () / 2;
    const Pt::int32_t radiusY  = size.height() / 2;
    const Pt::int32_t radiusX2 = radiusX * radiusX;
    const Pt::int32_t radiusY2 = radiusY * radiusY;
    const Pt::int32_t centerX  = topLeft.x() + radiusX;
    const Pt::int32_t centerY  = topLeft.y() + radiusY;
    const Pt::int32_t qtrSegsX = round((float) radiusX2 / sqrt(radiusX2 + radiusY2));
    const Pt::int32_t qtrSegsY = round((float) radiusY2 / sqrt(radiusX2 + radiusY2));
    const Pt::int32_t numSegs  = (qtrSegsX + 1 + qtrSegsY + 1) * 20;

    // Generate a polygon that approximates the ellipse
    std::vector<Point> points(numSegs);
    Pt::int32_t        p = 0;

    // Top-right
    for(Pt::int32_t x = 0; x <= qtrSegsX; ++x) {
        //
        Pt::int32_t y = round(radiusY * sqrt(1 - (float) x * x / radiusX2));
        // Store the point and increment the index
        points[p++].set(centerX + x, centerY - y);
    }
    for(Pt::int32_t y = qtrSegsY; y >= 0; --y) {
        //
        Pt::int32_t x = round(radiusX * sqrt(1 - (float) y * y / radiusY2));
        // Store the point and increment the index
        points[p++].set(centerX + x, centerY - y);
    }

    // Bottom-right
    for(Pt::int32_t y = 0; y <= qtrSegsY; ++y) {
        //
        Pt::int32_t x = round(radiusX * sqrt(1 - (float) y * y / radiusY2));
        // Store the point and increment the index
        points[p++].set(centerX + x, centerY + y);
    }
    for(Pt::int32_t x = qtrSegsX; x >=0 ; --x) {
        //
        Pt::int32_t y = round(radiusY * sqrt(1 - (float) x * x / radiusX2));
        // Store the point and increment the index
        points[p++].set(centerX + x, centerY + y);
    }

    // Bottom-left
    for(Pt::int32_t x = 0; x < qtrSegsX; ++x) {
        //
        Pt::int32_t y = round(radiusY * sqrt(1 - (float) x * x / radiusX2));
        // Store the point and increment the index
        points[p++].set(centerX - x, centerY + y);
    }
    for(Pt::int32_t y = qtrSegsY; y >= 0; --y) {
        //
        Pt::int32_t x = round(radiusX * sqrt(1 - (float) y * y / radiusY2));
        // Store the point and increment the index
        points[p++].set(centerX - x, centerY + y);
    }

    // Top-left
    for(Pt::int32_t y = 0; y < qtrSegsY; ++y) {
        //
        Pt::int32_t x = round(radiusX * sqrt(1 - (float) y * y / radiusY2));
        // Store the point and increment the index
        points[p++].set(centerX - x, centerY - y);
    }
    for(Pt::int32_t x = qtrSegsX; x >= 0; --x) {
        //
        Pt::int32_t y = round(radiusY * sqrt(1 - (float) x * x / radiusX2));
        // Store the point and increment the index
        points[p++].set(centerX - x, centerY - y);
    }

    // Rasterize the polygon
    _rasterizer->fillPolygon(points.data(), p);

#else

    // Calculate the ellipse's parameters
    const Pt::int32_t radiusX = size.width () / 2;
    const Pt::int32_t radiusY = size.height() / 2;
    const Pt::int32_t radiusM = std::max(radiusX, radiusY);
    const Pt::int32_t centerX = topLeft.x() + radiusX;
    const Pt::int32_t centerY = topLeft.y() + radiusY;
    const Pt::int32_t numSegs = (radiusM * 4 / 3 / 20) * 20;
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
    std::vector<Point> points(numSegs);
    Pt::int32_t        prevX = ImagePainter2::MaximumCoordinate;
    Pt::int32_t        prevY = ImagePainter2::MaximumCoordinate;
    Pt::int32_t        p     = 0;

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

    // Rasterize the polygon
    _rasterizer->fillPolygon(points.data(), p);

#endif
}

void ImagePainter2::drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}

void ImagePainter2::fillArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, bool createPie)
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
    const Pt::int32_t degFac  = (degEnd - degBegin) / 36 * 2;
    const Pt::int32_t numSegs = (radiusM * (degFac ? degFac : 1) / 3 / 20) * 20;
    const float       fdegInc = (Pt::Pi * (degEnd -  degBegin) / 180) / (numSegs - 1);

    // Generate a polygon that approximates the arc
    std::vector<Point> points(numSegs + (createPie ? 1 : 0));
    Pt::int32_t        prevX = ImagePainter2::MaximumCoordinate;
    Pt::int32_t        prevY = ImagePainter2::MaximumCoordinate;
    float              angle = Pt::Pi * degBegin / 180;
    Pt::int32_t        p     = 0;
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
        points[numSegs].set(centerX, centerY);

    // Rasterize the polygon
    _rasterizer->fillPolygon(points.data(), p);
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


} // namespace
} // namespace
