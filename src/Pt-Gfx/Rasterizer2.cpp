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
#include "ClipPolygon2.h"
#include "Triangulate.h"

#include "Rasterizer2.h"

#include <stdio.h> // Just for easy debugging ;)


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Settings =======================================================================
// ======================================================================================

// Comment this to reduce the precision of the polygon AA
#define FILL_POLYGON_PRECISION_AA

// Use 16.16 fixed-point format
#define FIXED_POINT_SHIFT_FACTOR 16         // Shift factor
#define FIXED_POINT_ALPHA_DIVFAC 257        // Must be ( (2 ^ FIXED_POINT_SHIFT_FACTOR - 1) / 255 )
#define FIXED_POINT_FRACT_VAL_BM 0x0000FFFF // Bit mask for the fractional value; must be (2 ^ FIXED_POINT_SHIFT_FACTOR - 1)
#define FIXED_POINT_CONSTANT_ONE 65536      // The value 1.0 in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR )
#define FIXED_POINT_CONSTANT_HLF 32768      // The value 0.5 in fixed-point ( 2 ^ (FIXED_POINT_SHIFT_FACTOR - 1) )

// The number of margin pixels
#define MARGIN_PIXELS 2


// ======================================================================================
// ===== Internal Functions =============================================================
// ======================================================================================

// Cohen–Sutherland clipping algorithm outcode
typedef int CSOutcode;

static const int CS_Inside = 0; // 0000
static const int CS_Left   = 1; // 0001
static const int CS_Right  = 2; // 0010
static const int CS_Bottom = 4; // 0100
static const int CS_Top    = 8; // 1000

// Compute the bit code for a point (x, y) using the clip rectangle
static CSOutcode csComputeOutcode(Pt::int32_t x, Pt::int32_t y, const Rect& clip)
{
    CSOutcode code = CS_Inside; // Initialised as being inside of the clip region

         if(x < clip.left  ()) code |= CS_Left;   // to the left of clip region
    else if(x > clip.right ()) code |= CS_Right;  // to the right of clip region
         if(y < clip.top   ()) code |= CS_Top;    // above the clip region
    else if(y > clip.bottom()) code |= CS_Bottom; // below the clip region

    return code;
}

// Cohen–Sutherland clipping algorithm clips a line from (x0, y0) to (x1, y1)
// against a clip rectangle (https://en.wikipedia.org/wiki/Cohen–Sutherland_algorithm)
static bool csClipLine(Pt::int32_t& x0, Pt::int32_t& y0, Pt::int32_t& x1, Pt::int32_t& y1, const Rect& clip)
{
    // Compute the initial outcodes for the endpoints
    CSOutcode outcode0 = csComputeOutcode(x0, y0, clip);
    CSOutcode outcode1 = csComputeOutcode(x1, y1, clip);

    bool        accept = false;
    Pt::int32_t x, y;

    while(true) {
        // Both endpoints are inside the clip region
        if(!(outcode0 | outcode1)) {
            accept = true;
            break;
        }
        // Both endpoints are outside the clip region
        else if (outcode0 & outcode1) {
            break;
        }
        // At least one endpoint is outside the clip rectangle
        else {
            // Pick the one that is outside the clip rectangle
            // and find the intersection point using:
            //     y = y0 + (x - x0) * slope
            //     x = x0 + (y - y0) * slope
            CSOutcode outcodeOut = outcode0 ? outcode0 : outcode1;
            // Endpoint is above the clip rectangle
            if(outcodeOut & CS_Top) {
                x = x0 + (x1 - x0) * (clip.top   () - y0) / (y1 - y0);
                y = clip.top();
            }
            // Endpoint is below the clip rectangle
            else if(outcodeOut & CS_Bottom) {
                x = x0 + (x1 - x0) * (clip.bottom() - y0) / (y1 - y0);
                y = clip.bottom();
            }
            // Endpoint is to the right of clip rectangle
            else if(outcodeOut & CS_Right) {
                y = y0 + (y1 - y0) * (clip.right () - x0) / (x1 - x0);
                x = clip.right();
            }
            // Endpoint is to the left of clip rectangle
            else if(outcodeOut & CS_Left) {
                y = y0 + (y1 - y0) * (clip.left  () - x0) / (x1 - x0);
                x = clip.left();
            }
            // Replace the endpoint which is outside the clip rectangle
            // with the intersection point and run the next pass
            if(outcodeOut == outcode0) {
                x0 = x;
                y0 = y;
                outcode0 = csComputeOutcode(x0, y0, clip);
            }
            else {
                x1 = x;
                y1 = y;
                outcode1 = csComputeOutcode(x1, y1, clip);
            }
        }
    }

    return accept;
}


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
, _aaLevel(0)
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

    switch( brush.fillStyle() )
    {
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

void Rasterizer2::fillPolygon(const Point* points, const size_t pointCount)
{
    std::vector<Point> clipped;
    genClippedPolygonPoints(clipped, points, pointCount);

    std::vector<Point> tris;
    if(!Triangulate::process(tris, clipped)) return;

    Pt::int32_t minX;
    Pt::int32_t minY;
    Pt::int32_t maxX;
    Pt::int32_t maxY;
    Pt::int32_t sizeX;
    Pt::int32_t sizeY;

    rasterFillTriangles(tris.data(), tris.size(), minX, minY, maxX, maxY, sizeX, sizeY);
    rasterPolygonOutline(clipped.data(), clipped.size(), minX, minY, maxX, maxY, sizeX, sizeY, _brush.color());

    /*
    std::vector<Point> tris;

    // Bottom-flat
    tris.push_back(Point(200, 100));
    tris.push_back(Point(100, 200));
    tris.push_back(Point(300, 200));
    // Top-flat
    tris.push_back(Point(100, 300));
    tris.push_back(Point(300, 300));
    tris.push_back(Point(200, 400));
    // Non-flat
    tris.push_back(Point(450, 100));
    tris.push_back(Point(350, 300));
    tris.push_back(Point(650, 400));

    rasterFillTriangles(tris.data(), tris.size(), minX, minY, maxX, maxY, sizeX, sizeY);
    */
}


// ======================================================================================
// ===== Protected Member Functions =====================================================
// ======================================================================================

void Rasterizer2::updateClip()
{
    const Rect imageRect( Point(0,0) , _image->size() );
    _currentClip = _clip.isNull() ? imageRect : _clip.intersect( imageRect );

    // Resize the work buffer so it slightly larger than the size of the image
    _alphas.resize( ( _image->width() + MARGIN_PIXELS * 2 ) * ( _image->height() + MARGIN_PIXELS * 2) );
    _wbXSize = _image->width() + MARGIN_PIXELS * 2;
}

void Rasterizer2::prepWorkBuffer(Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t sizeX, Pt::int32_t sizeY)
{
    Pt::uint16_t* alphas = &_alphas[0] + (minY + MARGIN_PIXELS) * _wbXSize + minX + MARGIN_PIXELS;

    for(int r = 0; r < sizeY; ++r) {
        memset(alphas, 0, sizeX * sizeof(*alphas));
        alphas += _wbXSize;
    }
}

void Rasterizer2::blitWorkBufferToImage(Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t sizeX, Pt::int32_t sizeY, const Color& color)
{
    const Pt::uint16_t* alphas = _alphas.data() + (minY + MARGIN_PIXELS) * _wbXSize + minX + MARGIN_PIXELS;

    for(int r = 0; r < sizeY; ++r) {
        Pixel destPixel( _image->view(), minX, minY + r);
        _image->format().copy(destPixel, alphas, sizeX, color, _compositionMode);
        alphas += _wbXSize;
    }
}

void Rasterizer2::rasterOnePixelLine(const Point& a, const Point& b)
{
    // Clip the points
    Pt::int32_t x1 = a.x();
    Pt::int32_t y1 = a.y();
    Pt::int32_t x2 = b.x();
    Pt::int32_t y2 = b.y();

    if(!csClipLine(x1, y1, x2, y2, _currentClip)) return;

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

    // Calculate the size of the rectangle
    const Pt::int32_t sizeX = maxX - minX + 1;
    const Pt::int32_t sizeY = maxY - minY + 1;

    // Caculate the number of steps
    const Pt::int32_t steps = std::max(sizeX, sizeY) - 1;
    if(!steps) return;

    // Convert the coordinates to fixed-points
    const Pt::int32_t fx1 = x1 << FIXED_POINT_SHIFT_FACTOR;
    const Pt::int32_t fy1 = y1 << FIXED_POINT_SHIFT_FACTOR;
    const Pt::int32_t fx2 = x2 << FIXED_POINT_SHIFT_FACTOR;
    const Pt::int32_t fy2 = y2 << FIXED_POINT_SHIFT_FACTOR;

    // Prepare the work buffer
    prepWorkBuffer(minX, minY, sizeX, sizeY);

    // Raster the line
    rasterOnePixelLineSegment(fx1, fy1, fx2, fy2, steps);

    // Blit the work buffer to the image
    blitWorkBufferToImage(minX, minY, sizeX, sizeY, _pen.color());
}

void Rasterizer2::rasterOnePixelLineSegment(Pt::int32_t fx1, Pt::int32_t fy1, Pt::int32_t fx2, Pt::int32_t fy2, Pt::int32_t steps)
{
#if 1

    // https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm


//     function plot(x, y, c) is
//         plot the pixel at (x, y) with brightness c (where 0 ≤ c ≤ 1)
//
//     // integer part of x
//     function ipart(x) is
//         return int(x)
//
//     function round(x) is
//         return ipart(x + 0.5)
//
//     // fractional part of x
//     function fpart(x) is
//         if x < 0
//             return 1 - (x - floor(x))
//         return x - floor(x)
//
//     function rfpart(x) is
//         return 1 - fpart(x)
//
//     function drawLine(x0,y0,x1,y1) is

//      boolean steep := abs(y1 - y0) > abs(x1 - x0)
        Pt::int32_t deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
        Pt::int32_t deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);
        bool        steep  = deltaY > deltaX;

//      if steep then
//          swap(x0, y0)
//          swap(x1, y1)
//      end if
        if(steep) {
            std::swap(fx1, fy1);
            std::swap(fx2, fy2);
        }

//      if x0 > x1 then
//          swap(x0, x1)
//          swap(y0, y1)
//      end if
        if(fx1 > fx2) {
            std::swap(fx1, fx2);
            std::swap(fy1, fy2);
        }

//      dx := x1 - x0
//      dy := y1 - y0
        deltaX = fx2 - fx1;
        deltaY = fy2 - fy1;

//      gradient := dy / dx
//      if dx == 0.0 then
//          gradient := 1.0
//      end if

        Pt::int32_t gradient = deltaY / (deltaX >> FIXED_POINT_SHIFT_FACTOR);
        if(!gradient) gradient = (1 << FIXED_POINT_SHIFT_FACTOR);

#define FIXED_POINT_FPART(V)  ( (V) &  FIXED_POINT_FRACT_VAL_BM )
#define FIXED_POINT_RFPART(V) ( FIXED_POINT_CONSTANT_ONE - FIXED_POINT_FPART(V) )
#define FIXED_POINT_IPART(V)  ( (V) & ~FIXED_POINT_FRACT_VAL_BM )
#define FIXED_POINT_ROUND(V)  ( FIXED_POINT_IPART( (V) + FIXED_POINT_CONSTANT_HLF ) )

        // Handle the first endpoint
//      xend := round(x0)
//      yend := y0 + gradient * (xend - x0)
//      xgap := rfpart(x0 + 0.5)
        Pt::int32_t xend = FIXED_POINT_ROUND(fx1);
        Pt::int32_t yend = fy1 + gradient * ((xend - fx1) >> FIXED_POINT_SHIFT_FACTOR);
        Pt::int32_t xgap = FIXED_POINT_RFPART(fx1 + FIXED_POINT_CONSTANT_HLF);

//      xpxl1 := xend // this will be used in the main loop
//      ypxl1 := ipart(yend)
        Pt::int32_t xpxl1 = xend; // Will be used in the main loop
        Pt::int32_t ypxl1 = yend & ~FIXED_POINT_FRACT_VAL_BM;

//      if steep then
//          plot(ypxl1,   xpxl1, rfpart(yend) * xgap)
//          plot(ypxl1+1, xpxl1,  fpart(yend) * xgap)
//      else
//          plot(xpxl1, ypxl1  , rfpart(yend) * xgap)
//          plot(xpxl1, ypxl1+1,  fpart(yend) * xgap)
//      end if

        Color color = _pen.color();

#define FIXED_POINT_TO_INT(V) ( (V) >> FIXED_POINT_SHIFT_FACTOR )

        if(steep) {
            Pt::int32_t a1 = ((((Pt::int64_t)FIXED_POINT_RFPART(yend) * (Pt::int64_t)xgap) >> (FIXED_POINT_SHIFT_FACTOR)) - 1) / FIXED_POINT_ALPHA_DIVFAC;
            Pixel p1(_image->view(), FIXED_POINT_TO_INT(ypxl1), FIXED_POINT_TO_INT(xpxl1));
            _image->format().setPixel(p1, color, _compositionMode);

            Pt::int32_t a2 = ((((Pt::int64_t)FIXED_POINT_FPART(yend) * (Pt::int64_t)xgap) >> (FIXED_POINT_SHIFT_FACTOR)) - 1) / FIXED_POINT_ALPHA_DIVFAC;
            Pixel p2(_image->view(), FIXED_POINT_TO_INT(ypxl1 + 1), FIXED_POINT_TO_INT(xpxl1));
            _image->format().setPixel(p2, color, _compositionMode);

        }
        else {
        }


//      intery := yend + gradient // first y-intersection for the main loop


//
//         // handle second endpoint
//         xend := round(x1)
//         yend := y1 + gradient * (xend - x1)
//         xgap := fpart(x1 + 0.5)
//         xpxl2 := xend //this will be used in the main loop
//         ypxl2 := ipart(yend)
//         if steep then
//             plot(ypxl2  , xpxl2, rfpart(yend) * xgap)
//             plot(ypxl2+1, xpxl2,  fpart(yend) * xgap)
//         else
//             plot(xpxl2, ypxl2,  rfpart(yend) * xgap)
//             plot(xpxl2, ypxl2+1, fpart(yend) * xgap)
//         end if
//
//         // main loop
//         if steep then
//             for x from xpxl1 + 1 to xpxl2 - 1 do
//                begin
//                     plot(ipart(intery)  , x, rfpart(intery))
//                     plot(ipart(intery)+1, x,  fpart(intery))
//                     intery := intery + gradient
//                end
//         else
//             for x from xpxl1 + 1 to xpxl2 - 1 do
//                begin
//                     plot(x, ipart(intery),  rfpart(intery))
//                     plot(x, ipart(intery)+1, fpart(intery))
//                     intery := intery + gradient
//                end
//         end if
//     end function

#else
    // Calculate the change factors
    const Pt::int32_t chgX = (fx2 - fx1) / steps;
    const Pt::int32_t chgY = (fy2 - fy1) / steps;

    // Draw the line
    for(Pt::int32_t i = 0; i <= steps; ++i) {
        // Calculate the alpha factors (0 - 255) of the block
        const Pt::int32_t frx = (fx1 & FIXED_POINT_FRACT_VAL_BM) / FIXED_POINT_ALPHA_DIVFAC;
        const Pt::int32_t fry = (fy1 & FIXED_POINT_FRACT_VAL_BM) / FIXED_POINT_ALPHA_DIVFAC;
        const Pt::int32_t flx = 255 - frx;
        const Pt::int32_t fly = 255 - fry;
        // Calculate the top-left coordinate of the block
        const Pt::int32_t lx = fx1 >> FIXED_POINT_SHIFT_FACTOR;
        const Pt::int32_t ly = fy1 >> FIXED_POINT_SHIFT_FACTOR;
        // Calculate the bottom-right coordinate of the block
        const Pt::int32_t rx = frx ? (lx + 1) : lx;
        const Pt::int32_t ry = fry ? (ly + 1) : ly;
        // Draw the block
        _alphas[ (ly + MARGIN_PIXELS) * _wbXSize + lx + MARGIN_PIXELS ] += (fly * flx) >> 8;
        _alphas[ (ly + MARGIN_PIXELS) * _wbXSize + rx + MARGIN_PIXELS ] += (fly * frx) >> 8;
        _alphas[ (ry + MARGIN_PIXELS) * _wbXSize + lx + MARGIN_PIXELS ] += (fry * flx) >> 8;
        _alphas[ (ry + MARGIN_PIXELS) * _wbXSize + rx + MARGIN_PIXELS ] += (fry * frx) >> 8;
        // Increment the drawing coordinate
        fx1 += chgX;
        fy1 += chgY;
    }
#endif
}

void Rasterizer2::rasterSolidTriangles(const Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY, Pt::int32_t& sizeX, Pt::int32_t& sizeY)
{
    // Find the minimum and maximum coordinates
    minX =  65535;
    minY =  65535;
    maxX = -65535;
    maxY = -65535;

    for(size_t i = 0; i < pointCount; ++i) {
        const Pt::int32_t x = points[i].x();
        const Pt::int32_t y = points[i].y();
        if(x < minX) minX = x;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(y > maxY) maxY = y;
    }

    // Calculate the size of the rectangle
    sizeX = maxX - minX + 1;
    sizeY = maxY - minY + 1;

    // Prepare the work buffer
    prepWorkBuffer(minX, minY, sizeX, sizeY);

    // Raster the triangles
    for(size_t i = 0; i < pointCount; i += 3) {
        rasterOneSolidTriangle(points[i], points[i + 1], points[i + 2]);
    }

    // Do not blit the work buffer to the image because the outline-raster function which
    // will be called after this function exits is the one which will do it!

    // blitWorkBufferToImage(minX, minY, sizeX, sizeY, _brush.color());
}

// Based on http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
void Rasterizer2::rasterOneSolidTriangle(const Point& v1, const Point& v2, const Point& v3)
{
    // Sort the vertices by its Y coordinates
    const Point* vs[3] = { &v1, &v2, &v3 };

    if( vs[1]->y() < vs[0]->y() ) std::swap( vs[1], vs[0] );
    if( vs[2]->y() < vs[0]->y() ) std::swap( vs[2], vs[0] );
    if( vs[2]->y() < vs[1]->y() ) std::swap( vs[2], vs[1] );

    // Check for bottom-flat triangle
    if(vs[1]->y() == vs[2]->y()) {
        rasterOneSolidTriangleBottomFlat(*vs[0], *vs[1], *vs[2]);
    }
    // Check for top-flat triangle
    else if(vs[0]->y() == vs[1]->y()) {
        rasterOneSolidTriangleTopFlat(*vs[0], *vs[1], *vs[2]);
    }
    // Split the triangle to a bottom-flat and top-flat
    else {
        const Pt::int32_t deltaY10   = vs[1]->y() - vs[0]->y();
        const Pt::int32_t deltaY20   = vs[2]->y() - vs[0]->y();
        const Pt::int32_t deltaX20   = vs[2]->x() - vs[0]->x();
        const Pt::int32_t ratioY1020 = ( (deltaY10 << FIXED_POINT_SHIFT_FACTOR) / deltaY20 );
        const Point       vm( vs[0]->x() + ( (ratioY1020 * deltaX20) >> FIXED_POINT_SHIFT_FACTOR ),
                              vs[1]->y()
                            );
        rasterOneSolidTriangleBottomFlat(*vs[0], *vs[1], vm);
        rasterOneSolidTriangleTopFlat(*vs[1], vm, *vs[2]);
    }
}

// Based on http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
void Rasterizer2::rasterOneSolidTriangleBottomFlat(const Point& v1, const Point& v2, const Point& v3)
{
    /*        v1
     *
     *
     *     v2    v3
     */

    const Pt::int32_t chgX1 = ( ( ((Pt::int32_t)v2.x() - (Pt::int32_t)v1.x()) << FIXED_POINT_SHIFT_FACTOR ) /
                                  ((Pt::int32_t)v2.y() - (Pt::int32_t)v1.y())
                              );
    const Pt::int32_t chgX2 = ( ( ((Pt::int32_t)v3.x() - (Pt::int32_t)v1.x()) << FIXED_POINT_SHIFT_FACTOR ) /
                                  ((Pt::int32_t)v3.y() - (Pt::int32_t)v1.y())
                              );

    Pt::uint16_t* alphas = &_alphas[0] + (v1.y() + MARGIN_PIXELS) * _wbXSize + MARGIN_PIXELS;

/*
 * TODO : try this
spanWidth = ...
CompositionMode mode = ...;

while(spanWidth > 0)
{
    n = min(_brushBuffer.width(), spanWidth);
    _image->format()->copy(to, _brushBuffer, n, mode);
    spanWidth -= n;
}
 */

    // More precise
    if(_aaLevel) {
        Pt::int32_t curX1 = (v1.x() << FIXED_POINT_SHIFT_FACTOR);
        Pt::int32_t curX2 = (v1.x() << FIXED_POINT_SHIFT_FACTOR);

        if(chgX1 <= 0) curX1 += FIXED_POINT_FRACT_VAL_BM / 2;
        else           curX1 -= FIXED_POINT_FRACT_VAL_BM / 2;

        if(chgX2 >= 0) curX2 -= FIXED_POINT_FRACT_VAL_BM / 2;
        else           curX2 += FIXED_POINT_FRACT_VAL_BM / 2;

        for(Pt::int32_t i = v1.y(); i <= v2.y(); ++i) {
            const Pt::int32_t steps = (curX2 > curX1)
                                    ? (curX2 - curX1 + FIXED_POINT_FRACT_VAL_BM) >> FIXED_POINT_SHIFT_FACTOR
                                    : (curX1 - curX2 + FIXED_POINT_FRACT_VAL_BM) >> FIXED_POINT_SHIFT_FACTOR;
            if(!steps) {
                alphas[curX1 >> FIXED_POINT_SHIFT_FACTOR] = 255;
            }
            else {
                const Pt::int32_t chgX  = (curX2 - curX1) / steps;
                      Pt::int32_t iterX = curX1;
                for(int j = 0; j <= steps; ++j) {
                    if(j >= 1 && j <= steps - 1) {
                        alphas[iterX >> FIXED_POINT_SHIFT_FACTOR] = 255;
                    }
                    if(j <= 1 || j >= steps - 1) {
                        const Pt::int32_t frx = (iterX & FIXED_POINT_FRACT_VAL_BM) / FIXED_POINT_ALPHA_DIVFAC;
                        const Pt::int32_t flx = 255 - frx;
                        // Calculate the left and right coordinate of the span
                        const Pt::int32_t lx = iterX >> FIXED_POINT_SHIFT_FACTOR;
                        const Pt::int32_t rx = frx ? (lx + 1) : lx;
                        // Draw the span
                        alphas[lx] += flx;
                        alphas[rx] += frx;
                    }
                    iterX += chgX;
                }
            }
            alphas += _wbXSize;
            curX1  += chgX1;
            curX2  += chgX2;
        }
    }

    // Normal
    else {
        Pt::int32_t curX1 = (v1.x() << FIXED_POINT_SHIFT_FACTOR) + FIXED_POINT_FRACT_VAL_BM / 2;
        Pt::int32_t curX2 = (v1.x() << FIXED_POINT_SHIFT_FACTOR) - FIXED_POINT_FRACT_VAL_BM / 2;

        for(Pt::int32_t i = v1.y(); i <= v2.y(); ++i) {
            const Pt::int32_t from = std::min(curX1, curX2) >> FIXED_POINT_SHIFT_FACTOR;
            const Pt::int32_t to   = std::max(curX1, curX2) >> FIXED_POINT_SHIFT_FACTOR;
            for(int j = from; j <= to; ++j) {
                *(alphas + j) = 255;
            }
            alphas += _wbXSize;
            curX1  += chgX1;
            curX2  += chgX2;
        }
    }
}

// Based on http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
void Rasterizer2::rasterOneSolidTriangleTopFlat(const Point& v1, const Point& v2, const Point& v3)
{
    /*     v1    v2
     *
     *
     *        v3
     */

    const Pt::int32_t chgX1 = ( ( ((Pt::int32_t)v1.x() - (Pt::int32_t)v3.x()) << FIXED_POINT_SHIFT_FACTOR ) /
                                  ((Pt::int32_t)v1.y() - (Pt::int32_t)v3.y())
                              );
    const Pt::int32_t chgX2 = ( ( ((Pt::int32_t)v2.x() - (Pt::int32_t)v3.x()) << FIXED_POINT_SHIFT_FACTOR ) /
                                  ((Pt::int32_t)v2.y() - (Pt::int32_t)v3.y())
                              );

    Pt::uint16_t* alphas = &_alphas[0] + (v3.y() + MARGIN_PIXELS) * _wbXSize + MARGIN_PIXELS;

    // More precise
    if(_aaLevel) {
        Pt::int32_t curX1 = (v3.x() << FIXED_POINT_SHIFT_FACTOR);
        Pt::int32_t curX2 = (v3.x() << FIXED_POINT_SHIFT_FACTOR);

        if(chgX1 <= 0) curX1 += FIXED_POINT_FRACT_VAL_BM / 2;
        else           curX1 -= FIXED_POINT_FRACT_VAL_BM / 2;

        if(chgX2 >= 0) curX2 -= FIXED_POINT_FRACT_VAL_BM / 2;
        else           curX2 += FIXED_POINT_FRACT_VAL_BM / 2;

        for(Pt::int32_t i = v3.y(); i > v1.y(); --i) {
            const Pt::int32_t steps = (curX2 > curX1)
                                    ? (curX2 - curX1 + FIXED_POINT_FRACT_VAL_BM) >> FIXED_POINT_SHIFT_FACTOR
                                    : (curX1 - curX2 + FIXED_POINT_FRACT_VAL_BM) >> FIXED_POINT_SHIFT_FACTOR;
            if(!steps) {
                alphas[curX1 >> FIXED_POINT_SHIFT_FACTOR] = 255;
            }
            else {
                const Pt::int32_t chgX  = (curX2 - curX1) / steps;
                      Pt::int32_t iterX = curX1;
                for(int j = 0; j <= steps; ++j) {
                    if(j >= 1 && j <= steps - 1) {
                        alphas[iterX >> FIXED_POINT_SHIFT_FACTOR] = 255;
                    }
                    if(j <= 1 || j >= steps - 1) {
                        const Pt::int32_t frx = (iterX & FIXED_POINT_FRACT_VAL_BM) / FIXED_POINT_ALPHA_DIVFAC;
                        const Pt::int32_t flx = 255 - frx;
                        // Calculate the left and right coordinate of the span
                        const Pt::int32_t lx = iterX >> FIXED_POINT_SHIFT_FACTOR;
                        const Pt::int32_t rx = frx ? (lx + 1) : lx;
                        // Draw the span
                        alphas[lx] += flx;
                        alphas[rx] += frx;
                    }
                    iterX += chgX;
                }
            }
            alphas -= _wbXSize;
            curX1  -= chgX1;
            curX2  -= chgX2;
        }
    }

    // Normal
    else {
        Pt::int32_t curX1 = (v3.x() << FIXED_POINT_SHIFT_FACTOR) + FIXED_POINT_FRACT_VAL_BM / 2;
        Pt::int32_t curX2 = (v3.x() << FIXED_POINT_SHIFT_FACTOR) - FIXED_POINT_FRACT_VAL_BM / 2;

        for(Pt::int32_t i = v3.y(); i > v1.y(); --i) {
            const Pt::int32_t from = std::min(curX1, curX2) >> FIXED_POINT_SHIFT_FACTOR;
            const Pt::int32_t to   = std::max(curX1, curX2) >> FIXED_POINT_SHIFT_FACTOR;
            for(int j = from; j <= to; ++j) {
                *(alphas + j) = 255;
            }
            alphas -= _wbXSize;
            curX1  -= chgX1;
            curX2  -= chgX2;
        }
    }
}

void Rasterizer2::rasterFillTriangles(Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY, Pt::int32_t& sizeX, Pt::int32_t& sizeY)
{
    if(pointCount % 3) return;

    switch( _brush.fillStyle() ) {
        case Brush::Texture:
            break;

        case Brush::VerticalGradient:
            break;

        case Brush::HorizontalGradient:
            break;

        case Brush::Solid:
            rasterSolidTriangles(points, pointCount, minX, minY, maxX, maxY, sizeX, sizeY);
            break;
    }
}

void Rasterizer2::rasterPolygonOutline(const Point* points, size_t pointCount, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY, Pt::int32_t sizeX, Pt::int32_t sizeY, const Color& color)
{
    // Convert the coordinates to fixed-points
    std::vector<Pt::int32_t> lineX(pointCount);
    std::vector<Pt::int32_t> lineY(pointCount);

    for(size_t i = 0; i < pointCount; ++i) {
        lineX[i] = points[i].x() << FIXED_POINT_SHIFT_FACTOR;
        lineY[i] = points[i].y() << FIXED_POINT_SHIFT_FACTOR;
    }

    // Do not prepare the work buffer here because the area-filling function which was
    // called before this function has done it!

    // prepWorkBuffer(minX, minY, sizeX, sizeY);

    // Raster the outlines as multiple one-pixel lines
    const size_t      pc1 = pointCount - 1;
          Pt::int32_t xm, ym;

    for(size_t i = 0; i < pc1; ++i) {
        if(lineX[i] > lineX[i + 1]) xm = lineX[i    ] - lineX[i + 1];
        else                        xm = lineX[i + 1] - lineX[i    ];
        if(lineY[i] > lineY[i + 1]) ym = lineY[i    ] - lineY[i + 1];
        else                        ym = lineY[i + 1] - lineY[i    ];
        xm >>= FIXED_POINT_SHIFT_FACTOR;
        ym >>= FIXED_POINT_SHIFT_FACTOR;
        rasterOnePixelLineSegment(lineX[i], lineY[i], lineX[i + 1], lineY[i + 1], std::max(xm, ym) - 1);
    }

    if(lineX[0] > lineX[pc1]) xm = lineX[0  ] - lineX[pc1];
    else                      xm = lineX[pc1] - lineX[0  ];
    if(lineY[0] > lineY[pc1]) ym = lineY[0  ] - lineY[pc1];
    else                      ym = lineY[pc1] - lineY[0  ];
    xm >>= FIXED_POINT_SHIFT_FACTOR;
    ym >>= FIXED_POINT_SHIFT_FACTOR;
    rasterOnePixelLineSegment(lineX[0], lineY[0], lineX[pc1], lineY[pc1], std::max(xm, ym) - 1);

    // Blit the work buffer to the image
    blitWorkBufferToImage(minX, minY, sizeX, sizeY, color);
}

void Rasterizer2::genClippedPolygonPoints(std::vector<Point>& dst, const Point* src, const size_t pointCount) const
{
    dst.clear();

    for(size_t i = 0; i < pointCount; ++i)
        dst.push_back( Point( src[i].x(), src[i].y() ) );

    ClipPolygon2::clip(dst, _currentClip);
}


} // namespace
} // namespace
