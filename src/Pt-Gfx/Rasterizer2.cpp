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

#include <cmath>

#include <Pt/Gfx/Algorithm.h>
#include <Pt/Gfx/ImagePainter2.h>

#include "DrawText.h"
#include "ClipShape.h"
#include "Rasterizer2.h"

#include <stdio.h> // Just for easy debugging ;)


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Fixed-Point 16.16 Settings and Helper Macros ===================================
// ======================================================================================

#define FIXED_POINT_SHIFT_FACTOR  16         // Shift factor
#define FIXED_POINT_FRACT_BITMASK 0x0000FFFF // Bit mask for the fractional value; must be (2 ^ FIXED_POINT_SHIFT_FACTOR - 1)
#define FIXED_POINT_CONSTANT_ONE  65536      // The value 1.0 in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR    )
#define FIXED_POINT_CONSTANT_HALF 32768      // The value 0.5 in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR / 2)

#define FIXED_POINT_IPART(V)        ( (V) & ~FIXED_POINT_FRACT_BITMASK )
#define FIXED_POINT_FPART(V)        ( (V) &  FIXED_POINT_FRACT_BITMASK )
#define FIXED_POINT_RFPART(V)       ( FIXED_POINT_FRACT_BITMASK - FIXED_POINT_FPART(V) )
#define FIXED_POINT_ROUND(V)        ( FIXED_POINT_IPART( (V) + FIXED_POINT_CONSTANT_HALF ) )
#define FIXED_POINT_FPART_TO_A8(V)  ( FIXED_POINT_FPART (V) >> 8 )
#define FIXED_POINT_RFPART_TO_A8(V) ( FIXED_POINT_RFPART(V) >> 8 )
#define FIXED_POINT_MUL_TO_A8(A, B) ( ( ( (Pt::uint32_t)(A) * (Pt::uint32_t)(B) + FIXED_POINT_FRACT_BITMASK ) >> FIXED_POINT_SHIFT_FACTOR ) )
#define FIXED_POINT_FROM_INT(V)     ( (V) << FIXED_POINT_SHIFT_FACTOR )
#define FIXED_POINT_TO_INT(V)       ( (V) >> FIXED_POINT_SHIFT_FACTOR )


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

Rasterizer2::Rasterizer2(Image& image)
: _image(&image)
, _text( new DrawText() )
, _font()
, _compositionMode(CompositionMode::SourceCopy)
, _penPixel(_image->view(), 0, 0)
, _brushPixel(_image->view(), 0, 0)
{
    _text->setFont(_font);
    updateClip();
}

Rasterizer2::~Rasterizer2()
{ delete _text; }

void Rasterizer2::setImage( Image& image )
{
    _image = &image;
    _brushBuffer.reset(_image->format(), _brushBuffer.size());
    updateClip();
}

const ImageFormat& Rasterizer2::format() const
{ return _image->format(); }

void Rasterizer2::setPen( const Pen& pen )
{
    _pen = pen;
    _penBuffer.reset(_image->format(), Size(64, 1));
    Gfx::fill(_penBuffer.begin(), _penBuffer.end(), pen.color());

    _penPixel.reset(_penBuffer.view(), 0, 0);
}

void Rasterizer2::setBrush( const Brush& brush )
{
    _brush = brush;
    _isGradient = false;

    switch( brush.fillStyle() ) {
        case Brush::Solid:
            _brushBuffer.reset( _image->format(), Size(64, 1) );
            Gfx::fill(_brushBuffer.begin(), _brushBuffer.end(), brush.color());
            _brushImage = &_brushBuffer;
            break;

        case Brush::Texture:
            if( brush.texture().format() != _image->format() ) {
                _brushBuffer.reset( _image->format(), brush.texture().size() );
                Gfx::copy( brush.texture().begin(), brush.texture().end(), _brushBuffer.begin() );
                _brushImage = &_brushBuffer;
            }
            else {
                _brushImage = &_brush.texture();
            }
            break;

        case Brush::HorizontalGradient:
        case Brush::VerticalGradient:
            _isGradient = true;
            _brushImage = &_brushBuffer;
            break;
    }

    _brushPixel.reset(_brushImage->view(), 0, 0);
}

void Rasterizer2::setFont(const Font& font)
{
    _font = font;
    _text->setFont(_font);
}

void Rasterizer2::setClip( const Rect& clip )
{
    _clip = clip;
    updateClip();
}

FontMetrics Rasterizer2::fontMetrics( const String& text ) const
{ return _text->fontMetrics( text ); }

FontMetrics Rasterizer2::fontMetrics( const Font& font, const Pt::String& text )
{
    DrawText textRender;
    textRender.setFont(font);

    return textRender.fontMetrics(text);
}

void Rasterizer2::image(const Point& to, const Image& img)
{
    const Rect imageRect( Point(0,0), img.size() );
    image( to, img, imageRect );
}

void Rasterizer2::image(const Point& to, const Image& from, const Rect& fromRect)
{
    // Clip fromRect to fit into the clip/image rect
    const Point d       = _currentClip.topLeft() - to;
    const Point fromPos = fromRect.topLeft() + d;

    Rect fromClip(fromPos, _currentClip.size());
    fromClip = fromRect.intersect(fromClip);

    if( fromClip.isNull() ) return;

    // Take account for smaller fromRect
    const Point toClip = to + (fromClip.topLeft() - fromRect.topLeft());

    _image->format().copy(_image->view(), toClip, from.view(), fromClip, _compositionMode);
}

void Rasterizer2::strokeText( const Point& to, const Pt::String& text )
{
    _text->setClip(_currentClip);
    _text->draw( *_image, _pen.color(), to, text, _compositionMode );
}

void Rasterizer2::strokeRect(const Point& tl, const Point& br)
{
    switch( _pen.style() ) {
        case Pen::Solid:
            rasterOnePixelRectOutline(tl, br);
            break;

        case Pen::Dash:
        case Pen::DoubleDash:
            break;
    }
}

void Rasterizer2::strokeOutline(const Point* points, size_t pointCount)
{
    switch( _pen.style() ) {
        case Pen::Solid:
            if( _pen.size() == 1 && pointCount == 2 ) {
                rasterOnePixelLine(points[0], points[1]);
            }
            break;

        case Pen::Dash:
        case Pen::DoubleDash:
            break;
    }
}

void Rasterizer2::fillRect(const Point& tl, const Point& br)
{
    rasterRectArea(tl, br);
}

void Rasterizer2::fillPolygon(const Point* points, const size_t pointCount)
{
    std::vector<Point> clipped;

#if 1
    genClippedPolygonPoints(clipped, points, pointCount);
#else
    clipped.push_back(Point(450, 100));
    clipped.push_back(Point(350, 300));
    clipped.push_back(Point(650, 400));
#endif

    rasterPolygonArea(clipped.data(), clipped.size(), _brush.color());
    rasterPolygonOutline(clipped.data(), clipped.size(), _brush.color());
}


// ======================================================================================
// ===== Protected Member Functions =====================================================
// ======================================================================================

void Rasterizer2::updateClip()
{
    const Rect imageRect( Point(0,0) , _image->size() );
    _currentClip = _clip.isNull() ? imageRect : _clip.intersect( imageRect );
}

void Rasterizer2::rasterOnePixelLine(const Point& a, const Point& b)
{
    // Clip the points
    Pt::int32_t x1 = a.x();
    Pt::int32_t y1 = a.y();
    Pt::int32_t x2 = b.x();
    Pt::int32_t y2 = b.y();

    if(!ClipShape::clipLine(x1, y1, x2, y2, _currentClip)) return;

    // Find the minimum and maximum coordinates
    Pt::int32_t minX, minY, maxX, maxY;

    if(x2 > x1) {
        minX = x1;
        maxX = x2;
    }
    else {
        minX = x2;
        maxX = x1;
    }

    if(y2 > y1) {
        minY = y1;
        maxY = y2;
    }
    else {
        minY = y2;
        maxY = y1;
    }

    // Calculate the size of the line
    const Pt::int32_t sizeX = maxX - minX + 1;
    const Pt::int32_t sizeY = maxY - minY + 1;
    if(!sizeX && !sizeY) return;

    // Convert the coordinates to fixed-points
    const Pt::int32_t fx1 = FIXED_POINT_FROM_INT(x1);
    const Pt::int32_t fy1 = FIXED_POINT_FROM_INT(y1);
    const Pt::int32_t fx2 = FIXED_POINT_FROM_INT(x2);
    const Pt::int32_t fy2 = FIXED_POINT_FROM_INT(y2);

    // Raster the line
    rasterOnePixelLineSegment(fx1, fy1, fx2, fy2, _pen.color(), false);
}

void Rasterizer2::rasterOnePixelHLineSegment(Pt::int32_t x1, Pt::int32_t x2, Pt::int32_t y, const Color& color)
{
    // Calculate the length of the line
    const Pt::int32_t sizeL = x2 - x1 + 1;

    // Draw the line
    Pixel pixel(_image->view(), x1, y);

    for(Pt::int32_t i = 0; i < sizeL; ++i) {
        _image->format().setPixel(pixel, color, _compositionMode);
        pixel.advance();
    }
}

void Rasterizer2::rasterOnePixelVLineSegment(Pt::int32_t x, Pt::int32_t y1, Pt::int32_t y2, const Color& color)
{
    // Calculate the length of the line
    const Pt::int32_t sizeL = y2 - y1 + 1;

    // Draw the line
    Pixel pixel(_image->view(), x, y1);

    for(Pt::int32_t i = 0; i < sizeL; ++i) {
        _image->format().setPixel(pixel, color, _compositionMode);
        pixel.advance(_image->width() - x);;
    }
}

void Rasterizer2::rasterOnePixelLineSegment(Pt::int32_t fx1, Pt::int32_t fy1, Pt::int32_t fx2, Pt::int32_t fy2, const Color& color, bool skipLastPoint)
{
    // TODO: hline, vline, xline

    // Xiaolin Wu's Anti-Aliased Line Algorithm
    // https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm

    // A helper macro to set pixel
    #define XW_SET_PIXEL(IMG, COL, X, Y, A)                                        \
        do {                                                                       \
            if( X < 0 || X >= IMG->width() || Y < 0 || Y >= IMG->height() ) break; \
            Pixel PIX(IMG->view(), X, Y);                                          \
            IMG->format().setPixel(PIX, COL, _compositionMode, A);                 \
        } while(false)

    // Swap the values as needed
    const Pt::int32_t deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
    const Pt::int32_t deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);
    bool        steep  = deltaY > deltaX;

    if(steep) {
        std::swap(fx1, fy1);
        std::swap(fx2, fy2);
    }

    if(fx1 > fx2) {
        std::swap(fx1, fx2);
        std::swap(fy1, fy2);
    }

    // Calculate the gradient
    Pt::int32_t gradient = (fy2 - fy1) / ((fx2 - fx1) >> FIXED_POINT_SHIFT_FACTOR);

    // Handle the first endpoint
    Pt::int32_t xend  = FIXED_POINT_ROUND(fx1);
    Pt::int32_t yend  = fy1 + gradient * FIXED_POINT_TO_INT(xend - fx1);
    Pt::int32_t xgap  = FIXED_POINT_RFPART(fx1 + FIXED_POINT_CONSTANT_HALF);
    Pt::int32_t xpxl1 = xend; // Will be used in the main loop
    Pt::int32_t ypxl1 = FIXED_POINT_IPART(yend);
    Pt::uint8_t a1    = FIXED_POINT_MUL_TO_A8(FIXED_POINT_RFPART(yend), xgap);
    Pt::uint8_t a2    = FIXED_POINT_MUL_TO_A8(FIXED_POINT_FPART (yend), xgap);
    if(steep) {
        XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(ypxl1                           ), FIXED_POINT_TO_INT(xpxl1), a1);
        XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(ypxl1 + FIXED_POINT_CONSTANT_ONE), FIXED_POINT_TO_INT(xpxl1), a2);
    }
    else {
        XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(xpxl1), FIXED_POINT_TO_INT(ypxl1                           ), a1);
        XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(xpxl1), FIXED_POINT_TO_INT(ypxl1 + FIXED_POINT_CONSTANT_ONE), a2);
    }

    // First y-intersection for the main loop
    Pt::int32_t intery = yend + gradient;

    // Handle the second endpoint
                xend  = FIXED_POINT_ROUND(fx2);
                yend  = fy2 + gradient * FIXED_POINT_TO_INT(xend - fx2);
                xgap  = FIXED_POINT_RFPART(fx2 + FIXED_POINT_CONSTANT_HALF);
    Pt::int32_t xpxl2 = xend; // Will be used in the main loop
    Pt::int32_t ypxl2 = FIXED_POINT_IPART(yend);
                a1    = FIXED_POINT_MUL_TO_A8(FIXED_POINT_RFPART(yend), xgap);
                a2    = FIXED_POINT_MUL_TO_A8(FIXED_POINT_FPART (yend), xgap);
    if(!skipLastPoint) {
        if(steep) {
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(ypxl2                           ), FIXED_POINT_TO_INT(xpxl2), a1);
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(ypxl2 + FIXED_POINT_CONSTANT_ONE), FIXED_POINT_TO_INT(xpxl2), a2);
        }
        else {
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(xpxl2), FIXED_POINT_TO_INT(ypxl2                           ), a1);
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(xpxl2), FIXED_POINT_TO_INT(ypxl2 + FIXED_POINT_CONSTANT_ONE), a2);
        }
    }

    // Loop through the rest of the pixels
    Pt::int32_t from = FIXED_POINT_TO_INT(xpxl1 + FIXED_POINT_CONSTANT_ONE);
    Pt::int32_t to   = FIXED_POINT_TO_INT(xpxl2 - FIXED_POINT_CONSTANT_ONE);
    if(steep) {
        for(Pt::int32_t i = from; i <= to; ++i) {
            a1 = FIXED_POINT_RFPART_TO_A8(intery);
            a2 = FIXED_POINT_FPART_TO_A8 (intery);
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(FIXED_POINT_IPART(intery)                           ), i, a1);
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(FIXED_POINT_IPART(intery) + FIXED_POINT_CONSTANT_ONE), i, a2);
            intery += gradient;
        }
    }
    else {
        for(Pt::int32_t i = from; i <= to; ++i) {
            a1 = FIXED_POINT_RFPART_TO_A8(intery);
            a2 = FIXED_POINT_FPART_TO_A8 (intery);
            XW_SET_PIXEL(_image, color, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(intery)                           ), a1);
            XW_SET_PIXEL(_image, color, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(intery) + FIXED_POINT_CONSTANT_ONE), a2);
            intery += gradient;
        }
    }
}

void Rasterizer2::rasterRectArea(const Point& tl, const Point& br)
{
    // Get the minimum and maximum coordinates
    Pt::int32_t minX = tl.x();
    Pt::int32_t minY = tl.y();
    Pt::int32_t maxX = br.x();
    Pt::int32_t maxY = br.y();

    // Clip the coordinates
    if(minX < _currentClip.left  ()) minX = _currentClip.left  ();
    if(minY < _currentClip.top   ()) minY = _currentClip.top   ();
    if(maxX > _currentClip.right ()) maxX = _currentClip.right ();
    if(maxY > _currentClip.bottom()) maxY = _currentClip.bottom();

    // Calculate the width of the rectangle
    const Pt::int32_t sizeX = maxX - minX + 1;

    // Draw the rectangles
    for(Pt::int32_t y = minY; y <= maxY; ++y) {
        Pt::int32_t spanWidth = sizeX;
        while(spanWidth > 0) {
            const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
            Pixel             dstPixel(_image->view(), minX + sizeX - spanWidth, y);
            ConstPixel        srcPixel(_brushBuffer.view(), 0, 0);
            _image->format().copy(dstPixel, srcPixel, n, _compositionMode);
            spanWidth -= n;
        }
    }
}

void Rasterizer2::rasterOnePixelRectOutline(const Point& tl, const Point& br)
{
    // Get the minimum and maximum coordinates
    Pt::int32_t minX = tl.x();
    Pt::int32_t minY = tl.y();
    Pt::int32_t maxX = br.x();
    Pt::int32_t maxY = br.y();

    // Clip the coordinates
    if(minX < _currentClip.left  ()) minX = _currentClip.left  ();
    if(minY < _currentClip.top   ()) minY = _currentClip.top   ();
    if(maxX > _currentClip.right ()) maxX = _currentClip.right ();
    if(maxY > _currentClip.bottom()) maxY = _currentClip.bottom();

    // Draw the rectangle's horizontal lines
    rasterOnePixelHLineSegment(minX, maxX, minY, _pen.color());
    rasterOnePixelHLineSegment(minX, maxX, maxY, _pen.color());

    // Draw the rectangle's vertical lines
    rasterOnePixelVLineSegment(minX, minY + 1, maxY - 1, _pen.color());
    rasterOnePixelVLineSegment(maxX, minY + 1, maxY - 1, _pen.color());
}

// http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonArea(const Point* points, size_t pointCount, const Color& color)
{
    // Find the minimum and maximum coordinates
    Pt::int32_t minX =  65535;
    Pt::int32_t minY =  65535;
    Pt::int32_t maxX = -65535;
    Pt::int32_t maxY = -65535;

    for(size_t i = 0; i < pointCount; ++i) {
        const Pt::int32_t x = points[i].x();
        const Pt::int32_t y = points[i].y();
        if(x < minX) minX = x;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(y > maxY) maxY = y;
    }

    // List of nodes in normal integers and fixed-points
    //std::vector<Pt::int32_t> nodeX (pointCount, 0);
    std::vector<Pt::int32_t> nodeXf(pointCount, 0);

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = minY; pixelY <= maxY; ++pixelY) {
        // Build a list of nodes
        Pt::int32_t j     = pointCount - 1;
        Pt::int32_t nodes = 0;
        for(size_t i = 0; i < pointCount; ++i) {
            if( ( points[i].y() < pixelY && points[j].y() >= pixelY ) ||
                ( points[j].y() < pixelY && points[i].y() >= pixelY )
            ) {
                Pt::int32_t deltaYp = pixelY        - points[i].y();
                Pt::int32_t deltaYj = points[j].y() - points[i].y();
                Pt::int32_t deltaXj = points[j].x() - points[i].x();
                nodeXf[nodes] = FIXED_POINT_FROM_INT(points[i].x()) + FIXED_POINT_FROM_INT(deltaYp) / deltaYj * deltaXj;
                //nodeX [nodes] = FIXED_POINT_TO_INT(nodeXf[nodes]);
                ++nodes;
            }
            j = i;
        }
        // Sort the nodes using a simple bubble sort
        for(Pt::int32_t i = 0; i < nodes - 1;) {
            if(nodeXf[i] > nodeXf[i + 1]) {
                //std::swap(nodeX [i], nodeX [i + 1]);
                std::swap(nodeXf[i], nodeXf[i + 1]);
                if(i) --i;
            }
            else {
                ++i;
            }
        }
        // Fill the pixels between node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            // Determine the X coordinates
            Pt::int32_t from;
            Pt::int32_t to;
            if(true) {
                Pt::int32_t x1 = FIXED_POINT_TO_INT(nodeXf[i]                           );
                Pt::int32_t x2 = FIXED_POINT_TO_INT(nodeXf[i] + FIXED_POINT_CONSTANT_ONE);
                from = (x1 == x2) ? x1 : x2;
            }
            if(true) {
                Pt::int32_t x1 = FIXED_POINT_TO_INT(nodeXf[i + 1] - FIXED_POINT_CONSTANT_ONE);
                Pt::int32_t x2 = FIXED_POINT_TO_INT(nodeXf[i + 1]                           );
                to = (x1 == x2) ? x1 : x2;
            }
            if(to < from) continue;
            // Draw the spans
            Pt::int32_t spanWidth = to - from + 1;
            while(spanWidth > 0) {
                const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
                Pixel             dstPixel(_image->view(), from, pixelY);
                ConstPixel        srcPixel(_brushBuffer.view(), 0, 0);
                _image->format().copy(dstPixel, srcPixel, n, _compositionMode);
                from      += n;
                spanWidth -= n;
            }
        }
    }
}

void Rasterizer2::rasterPolygonOutline(const Point* points, size_t pointCount, const Color& color)
{
    // Convert the coordinates to fixed-points
    std::vector<Pt::int32_t> lineX(pointCount);
    std::vector<Pt::int32_t> lineY(pointCount);

    for(size_t i = 0; i < pointCount; ++i) {
        lineX[i] = FIXED_POINT_FROM_INT(points[i].x());
        lineY[i] = FIXED_POINT_FROM_INT(points[i].y());
    }

    // Raster the outlines as multiple one-pixel lines
    const size_t pc1 = pointCount - 1;

    for(size_t i = 0; i < pc1; ++i) {
        rasterOnePixelLineSegment(lineX[i], lineY[i], lineX[i + 1], lineY[i + 1], color, true);
    }
    rasterOnePixelLineSegment(lineX[0], lineY[0], lineX[pc1], lineY[pc1], color, false);
}

void Rasterizer2::genClippedPolygonPoints(std::vector<Point>& dst, const Point* src, const size_t pointCount) const
{
    dst.clear();

    for(size_t i = 0; i < pointCount; ++i)
        dst.push_back( Point( src[i].x(), src[i].y() ) );

    ClipShape::clipPolygon(dst, _currentClip);
}


} // namespace
} // namespace
