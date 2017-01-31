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
#include "ClipPolygon.h"

#include "Rasterizer2.h"

#include <stdio.h> // Just for easy debugging ;)

namespace Pt {
namespace Gfx {


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
    Rect imageRect( Point(0,0), img.size() );
    image( to, img, imageRect );
}

void Rasterizer2::image(const Point& to, const Image& from, const Rect& fromRect)
{
    // Clip fromRect to fit into the clip/image rect
    Point d = _currentClip.topLeft() - to;
    Point fromPos = fromRect.topLeft() + d;

    Rect fromClip(fromPos, _currentClip.size());
    fromClip = fromRect.intersect(fromClip);

    if( fromClip.isNull() ) return;

    // Take account for smaller fromRect
    Point toClip = to + (fromClip.topLeft() - fromRect.topLeft());

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

void Rasterizer2::fillTriangles(Point* points, size_t pointCount)
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
            rasterSolidTriangles(points, pointCount);
            break;
    }
}


// ======================================================================================
// ===== Protected Member Functions =====================================================
// ======================================================================================

#define FIXED_POINT_USE_16_16_FORMAT

// Use 16.16 format
#if defined(FIXED_POINT_USE_16_16_FORMAT)
    #define FIXED_POINT_SHIFT_FACTOR 16         // Shift factor
    #define FIXED_POINT_ALPHA_DIVFAC 257        // Must be ( (2 ^ FIXED_POINT_SHIFT_FACTOR - 1) / 255 )
    #define FIXED_POINT_FRACT_VAL_BM 0x0000FFFF // Bit mask for the fractional value; must be (2 ^ FIXED_POINT_SHIFT_FACTOR - 1)
// Use 24.8 format
#elif defined(FIXED_POINT_USE_24_8_FORMAT)
    #define FIXED_POINT_SHIFT_FACTOR 8          // Shift factor
    #define FIXED_POINT_ALPHA_MULFAC 1          // Must be ( 255 / (2 ^ FIXED_POINT_SHIFT_FACTOR - 1) )
    #define FIXED_POINT_FRACT_VAL_BM 0x000000FF // Bit mask for the fractional value; must be (2 ^ FIXED_POINT_SHIFT_FACTOR - 1)
// Use 28.4 format
#else
    #define FIXED_POINT_SHIFT_FACTOR 4          // Shift factor
    #define FIXED_POINT_ALPHA_MULFAC 17         // Must be ( 255 / (2 ^ FIXED_POINT_SHIFT_FACTOR - 1) )
    #define FIXED_POINT_FRACT_VAL_BM 0x0000000F // Bit mask for the fractional value; must be (2 ^ FIXED_POINT_SHIFT_FACTOR - 1)
#endif

void Rasterizer2::updateClip()
{
    Rect imageRect( Point(0,0) , _image->size() );
    _currentClip = _clip.isNull() ? imageRect : _clip.intersect( imageRect );

    // Resize the work buffer to match the size of the clip region
    _alphas.resize(_currentClip.width() * _currentClip.height());
}

void Rasterizer2::prepWorkBuffer(Pt::int32_t sizeX, Pt::int32_t sizeY)
{ memset(&_alphas[0], 0, sizeX * sizeY); }

void Rasterizer2::blitWorkBufferToImage(Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t sizeX, Pt::int32_t sizeY, const Color& color)
{
    for(int r = 0; r < sizeY; ++r) {
        Pixel destPixel( _image->view(), minX, minY + r);
        _image->format().copy(destPixel, _alphas.data() + r * sizeX, sizeX, color, _compositionMode);
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

    // Translate the coordinates to (0, 0) and convert them to fixed-points
    Pt::int32_t fx1, fy1, fx2, fy2;

    if(x2 > x1) {
        fx1 = 0;
        fx2 = (maxX - minX) << FIXED_POINT_SHIFT_FACTOR;
    }
    else {
        fx1 = (maxX - minX) << FIXED_POINT_SHIFT_FACTOR;
        fx2 = 0;
    }

    if(y2 > y1) {
        fy1 = 0;
        fy2 = (maxY - minY) << FIXED_POINT_SHIFT_FACTOR;
    }
    else {
        fy1 = (maxY - minY) << FIXED_POINT_SHIFT_FACTOR;
        fy2 = 0;
    }

    // Prepare the work buffer
    prepWorkBuffer(sizeX, sizeY);

    // Raster the line
    rasterOnePixelLineSegment(fx1, fy1, fx2, fy2, steps, sizeX, sizeY);

    // Blit the work buffer to the image
    blitWorkBufferToImage(minX, minY, sizeX, sizeY, _pen.color());
}

void Rasterizer2::rasterOnePixelLineSegment(Pt::int32_t fx1, Pt::int32_t fy1, Pt::int32_t fx2, Pt::int32_t fy2, Pt::int32_t steps, Pt::int32_t sizeX, Pt::int32_t sizeY)
{
    // Calculate the change factors
    const Pt::int32_t chgX = (fx2 - fx1) / steps;
    const Pt::int32_t chgY = (fy2 - fy1) / steps;

    // Draw the line
    for(int i = 0; i <= steps; ++i) {
        // Calculate the alpha factors (0 - 255) of the block
#ifdef FIXED_POINT_ALPHA_DIVFAC
        Pt::int32_t frx = (fx1 & FIXED_POINT_FRACT_VAL_BM) / FIXED_POINT_ALPHA_DIVFAC;
        Pt::int32_t fry = (fy1 & FIXED_POINT_FRACT_VAL_BM) / FIXED_POINT_ALPHA_DIVFAC;
#else
        Pt::int32_t frx = (fx1 & FIXED_POINT_FRACT_VAL_BM) * FIXED_POINT_ALPHA_MULFAC;
        Pt::int32_t fry = (fy1 & FIXED_POINT_FRACT_VAL_BM) * FIXED_POINT_ALPHA_MULFAC;
#endif
        Pt::int32_t flx = 255 - frx;
        Pt::int32_t fly = 255 - fry;
        // Calculate the top-left coordinate of the block
        Pt::int32_t lx = fx1 >> FIXED_POINT_SHIFT_FACTOR;
        Pt::int32_t ly = fy1 >> FIXED_POINT_SHIFT_FACTOR;
        // Calculate the bottom-right coordinate of the block
        Pt::int32_t rx = frx ? (lx + 1) : lx;
        Pt::int32_t ry = fry ? (ly + 1) : ly;
        // Draw the block
                                       _alphas[ly * sizeX + lx] += (fly * flx + 255) >> 8;
        if( rx < sizeX               ) _alphas[ly * sizeX + rx] += (fly * frx + 255) >> 8;
        if(               ry < sizeY ) _alphas[ry * sizeX + lx] += (fry * flx + 255) >> 8;
        if( rx < sizeX && ry < sizeY ) _alphas[ry * sizeX + rx] += (fry * frx + 255) >> 8;
        // Increment the drawing coordinate
        fx1 += chgX;
        fy1 += chgY;
    }
}

void Rasterizer2::rasterSolidTriangles(Point* points, size_t pointCount)
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

    // Translate the coordinates to (0, 0)
    for(size_t i = 0; i < pointCount; ++i) {
        points[i].set(
            (points[i].x() - minX),
            (points[i].y() - minY)
        );
    }

    // Calculate the size of the rectangle
    const Pt::int32_t sizeX = maxX - minX + 1;
    const Pt::int32_t sizeY = maxY - minY + 1;

    // Prepare the work buffer
    prepWorkBuffer(sizeX, sizeY);

    // Raster the triangles
    for(size_t i = 0; i < pointCount; i += 3)
        rasterOneSolidTriangle(points[i], points[i + 1], points[i + 2], sizeX);

    // Blit the work buffer to the image
    blitWorkBufferToImage(minX, minY, sizeX, sizeY, _brush.color());
}

void Rasterizer2::rasterOneSolidTriangleBottomFlat(const Point& v1, const Point& v2, const Point& v3, Pt::int32_t sizeX)
{
  float invslope1 = (float)(v2.x() - v1.x()) / (v2.y() - v1.y());
  float invslope2 = (float)(v3.x() - v1.x()) / (v3.y() - v1.y());

  float curx1 = v1.x();
  float curx2 = v1.x();


    Pt::uint8_t* alphas = &_alphas[0] + v1.y() * sizeX;

  for (int scanlineY = v1.y(); scanlineY <= v2.y(); scanlineY++)
  {
      for(int i = curx1; i <= curx2; ++i) alphas[i] = 255;

      alphas += sizeX;

    curx1 += invslope1;
    curx2 += invslope2;
  }
}

void Rasterizer2::rasterOneSolidTriangleTopFlat(const Point& v1, const Point& v2, const Point& v3, Pt::int32_t sizeX)
{
  float invslope1 = (float)(v3.x() - v1.x()) / (v3.y() - v1.y());
  float invslope2 = (float)(v3.x() - v2.x()) / (v3.y() - v2.y());

  float curx1 = v3.x();
  float curx2 = v3.x();

    Pt::uint8_t* alphas = &_alphas[0] + v3.y() * sizeX;

  for (int scanlineY = v3.y(); scanlineY > v1.y(); scanlineY--)
  {
      for(int i = curx1; i <= curx2; ++i) alphas[i] = 255;

      alphas -= sizeX;

    curx1 -= invslope1;
    curx2 -= invslope2;
  }
}

void Rasterizer2::rasterOneSolidTriangle(const Point& v1, const Point& v2, const Point& v3, Pt::int32_t sizeX)
{
    // http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
    // Sort the vertices by its Y coordinates
    const Point* vs[3] = { &v1, &v2, &v3 };

    //    printf("%d %d     %d %d     %d %d\n", vs[0]->x(), vs[0]->y(), vs[1]->x(), vs[1]->y(), vs[2]->x(), vs[2]->y());

    if( vs[1]->y() < vs[0]->y() ) std::swap( vs[1], vs[0] );
    if( vs[2]->y() < vs[0]->y() ) std::swap( vs[2], vs[0] );
    if( vs[2]->y() < vs[1]->y() ) std::swap( vs[2], vs[1] );

    // Check for bottom-flat triangle
    if(vs[1]->y() == vs[2]->y()) {
        rasterOneSolidTriangleBottomFlat(*vs[0], *vs[1], *vs[2], sizeX);
        //if(vs[1]->x() <= vs[2]->x()) rasterOneSolidTriangleBottomFlat(*vs[0], *vs[1], *vs[2], sizeX);
        //else                         rasterOneSolidTriangleBottomFlat(*vs[0], *vs[2], *vs[1], sizeX);

    }
    // Check for top-flat triangle
    else if(vs[0]->y() == vs[1]->y()) {
        rasterOneSolidTriangleTopFlat(*vs[0], *vs[1], *vs[2], sizeX);
        //if(vs[0]->x() <= vs[1]->x()) rasterOneSolidTriangleTopFlat(*vs[0], *vs[1], *vs[2], sizeX);
        //else                         rasterOneSolidTriangleTopFlat(*vs[1], *vs[0], *vs[2], sizeX);
    }
    // Split the triangle to a bottom-flat and top-flat
    else {

        float y2my1 = vs[1]->y() - vs[0]->y();
        float y3my1 = vs[2]->y() - vs[0]->y();
        float x3mx1 = vs[2]->x() - vs[0]->x();

        float x4    = vs[0]->x() + y2my1 / y3my1 * x3mx1;

        //((float)(vs[1]->y() - vs[0]->y()) / (float)(vs[2]->y() - vs[0]->y())) * (vs[2]->x() - vs[0]->x()),
        Point vm(
            x4,
            vs[1]->y()
        );

       // printf("%d %d     %d %d     %d %d\n", vs[0]->x(), vs[0]->y(), vs[1]->x(), vs[1]->y(), vm.x(), vm.y());

        //if(vs[1]->x() <= vm.x()) rasterOneSolidTriangleBottomFlat(*vs[0], *vs[1], vm, sizeX);
        //else                     rasterOneSolidTriangleBottomFlat(*vs[0], vm, *vs[1], sizeX);
//
        //if(vs[1]->x() <= vm.x()) rasterOneSolidTriangleTopFlat(*vs[1], vm, *vs[2], sizeX);
        //else                     rasterOneSolidTriangleTopFlat(vm, *vs[1], *vs[2], sizeX);

        rasterOneSolidTriangleBottomFlat(*vs[0],* vs[1], vm, sizeX);
        rasterOneSolidTriangleTopFlat(*vs[1], vm, *vs[2], sizeX);

    }
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void Rasterizer2::genClippedPolygonPoints(std::vector<Point>& dst, const PointF* src, const size_t pointCount) const
{
   dst.clear();

   for(size_t i = 0; i < pointCount; ++i)
       dst.push_back( Point( src[i].x(), src[i].y() ) );

    ClipPolygon clipper;
    clipper(dst, _currentClip);
}


} // namespace
} // namespace
