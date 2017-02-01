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

#define FIXED_POINT_USE_16_16_FORMAT
#define FILL_POLYGON_PRECISION_AA

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

void Rasterizer2::fillPolygon(const Point* points, const size_t pointCount)
{
    Pt::int32_t minX;
    Pt::int32_t minY;
    Pt::int32_t maxX;
    Pt::int32_t maxY;

#if 1

    std::vector<Point> clipped;
    genClippedPolygonPoints(clipped, points, pointCount);

    std::vector<Point> tris;
    if(!Triangulate::process(tris, clipped)) return;

    rasterFillTriangles(tris.data(), tris.size(), minX, minY, maxX, maxY);
    rasterPolygonOutline(clipped.data(), clipped.size(), minX, minY, maxX, maxY);

#else

    std::vector<Point> tris;

    // bot
    tris.push_back(Point(200, 100));
    tris.push_back(Point(100, 200));
    tris.push_back(Point(300, 200));
    // top
    tris.push_back(Point(100, 300));
    tris.push_back(Point(300, 300));
    tris.push_back(Point(200, 400));
    // all
    tris.push_back(Point(450, 100));
    tris.push_back(Point(350, 300));
    tris.push_back(Point(650, 400));

    rasterTriangles(tris.data(), tris.size(), minX, minY, maxX, maxY);

#endif
}


// ======================================================================================
// ===== Protected Member Functions =====================================================
// ======================================================================================

void Rasterizer2::updateClip()
{
    Rect imageRect( Point(0,0) , _image->size() );
    _currentClip = _clip.isNull() ? imageRect : _clip.intersect( imageRect );

    // Resize the work buffer to match the size of the image
    _alphas.resize(_image->width() * _image->height());
}

void Rasterizer2::prepWorkBuffer(Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t sizeX, Pt::int32_t sizeY)
{
    Pt::uint8_t* alphas = &_alphas[0] + minY * _image->width() + minX;

    for(int r = 0; r < sizeY; ++r) {
        memset(alphas, 0, sizeX);
        alphas += _image->width();
    }
}

void Rasterizer2::blitWorkBufferToImage(Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t sizeX, Pt::int32_t sizeY, const Color& color)
{
    const Pt::uint8_t* alphas = _alphas.data() + minY * _image->width() + minX;

    for(int r = 0; r < sizeY; ++r) {
        Pixel destPixel( _image->view(), minX, minY + r);
        _image->format().copy(destPixel, alphas, sizeX, color, _compositionMode);
        alphas += _image->width();
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
    const Pt::int32_t imgW = _image->width();
    const Pt::int32_t imgH = _image->height();

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
                                     _alphas[ly * imgW + lx] += (fly * flx + 255) >> 8;
        if( rx < imgW              ) _alphas[ly * imgW + rx] += (fly * frx + 255) >> 8;
        if(              ry < imgH ) _alphas[ry * imgW + lx] += (fry * flx + 255) >> 8;
        if( rx < imgW && ry < imgH ) _alphas[ry * imgW + rx] += (fry * frx + 255) >> 8;
        // Increment the drawing coordinate
        fx1 += chgX;
        fy1 += chgY;
    }
}

void Rasterizer2::rasterPolygonOutline(const Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY)
{
    // Convert the coordinates to fixed-points
    std::vector<Pt::int32_t> lineX(pointCount);
    std::vector<Pt::int32_t> lineY(pointCount);

    for(size_t i = 0; i < pointCount; ++i) {
        lineX[i] = points[i].x() << FIXED_POINT_SHIFT_FACTOR;
        lineY[i] = points[i].y() << FIXED_POINT_SHIFT_FACTOR;
    }

    // Calculate the size of the rectangle
    const Pt::int32_t sizeX = maxX - minX + 1;
    const Pt::int32_t sizeY = maxY - minY + 1;

    // Prepare the work buffer
    prepWorkBuffer(minX, minY, sizeX, sizeY);

    // Raster the outlines as multiple one-pixel lines
    Pt::int32_t xm, ym;
    size_t      pc1 = pointCount - 1;

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
    blitWorkBufferToImage(minX, minY, sizeX, sizeY, _brush.color());
}

void Rasterizer2::rasterSolidTriangles(const Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY)
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
    const Pt::int32_t sizeX = maxX - minX + 1;
    const Pt::int32_t sizeY = maxY - minY + 1;

    // Prepare the work buffer
    prepWorkBuffer(minX, minY, sizeX, sizeY);

    // Raster the triangles
    for(size_t i = 0; i < pointCount; i += 3) {
        rasterOneSolidTriangle(points[i], points[i + 1], points[i + 2]);
    }

    // Blit the work buffer to the image
    blitWorkBufferToImage(minX, minY, sizeX, sizeY, _brush.color());
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
        Pt::int32_t y2my1 = vs[1]->y() - vs[0]->y();
        Pt::int32_t y3my1 = vs[2]->y() - vs[0]->y();
        Pt::int32_t x3mx1 = vs[2]->x() - vs[0]->x();
        Pt::int32_t y2ry3 = ( (y2my1 << FIXED_POINT_SHIFT_FACTOR) / y3my1 );
        Pt::int32_t offst = (y2ry3 * x3mx1) >> FIXED_POINT_SHIFT_FACTOR;

        Point vm(
            vs[0]->x() + offst,
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

    const Pt::int32_t imgW = _image->width();

    const Pt::int32_t chgX1 = ( ( ((Pt::int32_t)v2.x() - (Pt::int32_t)v1.x()) << FIXED_POINT_SHIFT_FACTOR ) / ((Pt::int32_t)v2.y() - (Pt::int32_t)v1.y()) );
    const Pt::int32_t chgX2 = ( ( ((Pt::int32_t)v3.x() - (Pt::int32_t)v1.x()) << FIXED_POINT_SHIFT_FACTOR ) / ((Pt::int32_t)v3.y() - (Pt::int32_t)v1.y()) );

#ifdef FILL_POLYGON_PRECISION_AA

    Pt::int32_t curX1 = (v1.x() << FIXED_POINT_SHIFT_FACTOR);
    Pt::int32_t curX2 = (v1.x() << FIXED_POINT_SHIFT_FACTOR);

    if(chgX1 < 0) curX1 -= FIXED_POINT_FRACT_VAL_BM / 2;
    else          curX1 += FIXED_POINT_FRACT_VAL_BM / 2;

    if(chgX2 < 0) curX2 += FIXED_POINT_FRACT_VAL_BM / 2;
    else          curX2 -= FIXED_POINT_FRACT_VAL_BM / 2;

    Pt::uint8_t* alphas = &_alphas[0] + v1.y() * imgW;

    for(int i = v1.y(); i <= v2.y(); ++i) {
        Pt::int32_t steps;
        if(curX2 > curX1) steps = (curX2 - curX1 + FIXED_POINT_FRACT_VAL_BM) >> FIXED_POINT_SHIFT_FACTOR;
        else              steps = (curX1 - curX2 + FIXED_POINT_FRACT_VAL_BM) >> FIXED_POINT_SHIFT_FACTOR;
        if(!steps) {
            alphas[curX1 >> FIXED_POINT_SHIFT_FACTOR] = 255;
        }
        else {
            Pt::int32_t iterX = curX1;
            Pt::int32_t chgX  = (curX2 - curX1) / steps;
            for(int j = 0; j <= steps; ++j) {
                if(j >= 1 && j <= steps - 1) {
                    alphas[iterX >> FIXED_POINT_SHIFT_FACTOR] = 255;
                }
                if(j <= 1 || j >= steps - 1) {
#ifdef FIXED_POINT_ALPHA_DIVFAC
                    Pt::int32_t frx = (iterX & FIXED_POINT_FRACT_VAL_BM) / FIXED_POINT_ALPHA_DIVFAC;
#else
                    Pt::int32_t frx = (iterX & FIXED_POINT_FRACT_VAL_BM) * FIXED_POINT_ALPHA_MULFAC;
#endif
                    Pt::int32_t flx = 255 - frx;
                    // Calculate the left and right coordinate of the span
                    Pt::int32_t lx = iterX >> FIXED_POINT_SHIFT_FACTOR;
                    Pt::int32_t rx = frx ? (lx + 1) : lx;
                    // Draw the block
                    if( ((Pt::int32_t) alphas[lx] + flx) <= 255 ) alphas[lx] += flx;
                    else                                          alphas[lx]  = 255;
                    if( rx < imgW ) {
                        if( ((Pt::int32_t) alphas[rx] + frx) <= 255 ) alphas[rx] += frx;
                        else                                          alphas[rx]  = 255;
                    }
                }
                iterX += chgX;
            }
        }
        alphas += imgW;
        curX1  += chgX1;
        curX2  += chgX2;
    }

#else

    Pt::int32_t curX1 = (v1.x() << FIXED_POINT_SHIFT_FACTOR) + FIXED_POINT_FRACT_VAL_BM / 2;
    Pt::int32_t curX2 = (v1.x() << FIXED_POINT_SHIFT_FACTOR) - FIXED_POINT_FRACT_VAL_BM / 2;

    Pt::uint8_t* alphas = &_alphas[0] + v1.y() * imgW;

    for(int i = v1.y(); i <= v2.y(); ++i) {
        if(curX1 <= curX2) {
            for(int j = (curX1 >> FIXED_POINT_SHIFT_FACTOR); j <= (curX2 >> FIXED_POINT_SHIFT_FACTOR); ++j) {
                *(alphas + j) = 255;
            }
        }
        else {
            for(int j = (curX2 >> FIXED_POINT_SHIFT_FACTOR); j <= (curX1 >> FIXED_POINT_SHIFT_FACTOR); ++j) {
                *(alphas + j) = 255;
            }
        }
        alphas += imgW;
        curX1  += chgX1;
        curX2  += chgX2;
    }

#endif
}

// Based on http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
void Rasterizer2::rasterOneSolidTriangleTopFlat(const Point& v1, const Point& v2, const Point& v3)
{
    /*     v1    v2
     *
     *
     *        v3
     */

    const Pt::int32_t imgW = _image->width();

    const Pt::int32_t chgX1 = ( ( ((Pt::int32_t)v3.x() - (Pt::int32_t)v1.x()) << FIXED_POINT_SHIFT_FACTOR ) / ((Pt::int32_t)v3.y() - (Pt::int32_t)v1.y()) ) ;
    const Pt::int32_t chgX2 = ( ( ((Pt::int32_t)v3.x() - (Pt::int32_t)v2.x()) << FIXED_POINT_SHIFT_FACTOR ) / ((Pt::int32_t)v3.y() - (Pt::int32_t)v2.y()) ) ;

#ifdef FILL_POLYGON_PRECISION_AA

    Pt::int32_t curX1 = (v3.x() << FIXED_POINT_SHIFT_FACTOR);
    Pt::int32_t curX2 = (v3.x() << FIXED_POINT_SHIFT_FACTOR);

    if(chgX1 < 0) curX1 += FIXED_POINT_FRACT_VAL_BM / 2;
    else          curX1 -= FIXED_POINT_FRACT_VAL_BM / 2;

    if(chgX2 < 0) curX2 -= FIXED_POINT_FRACT_VAL_BM / 2;
    else          curX2 += FIXED_POINT_FRACT_VAL_BM / 2;

    Pt::uint8_t* alphas = &_alphas[0] + v3.y() * imgW;

    for(int i = v3.y(); i > v1.y(); --i) {
        Pt::int32_t steps;
        if(curX2 > curX1) steps = (curX2 - curX1 + FIXED_POINT_FRACT_VAL_BM) >> FIXED_POINT_SHIFT_FACTOR;
        else              steps = (curX1 - curX2 + FIXED_POINT_FRACT_VAL_BM) >> FIXED_POINT_SHIFT_FACTOR;
        if(!steps) {
            alphas[curX1 >> FIXED_POINT_SHIFT_FACTOR] = 255;
        }
        else {
            Pt::int32_t iterX = curX1;
            Pt::int32_t chgX  = (curX2 - curX1) / steps;
            for(int j = 0; j <= steps; ++j) {
                if(j >= 1 && j <= steps - 1) {
                    alphas[iterX >> FIXED_POINT_SHIFT_FACTOR] = 255;
                }
                if(j <= 1 || j >= steps - 1) {
#ifdef FIXED_POINT_ALPHA_DIVFAC
                    Pt::int32_t frx = (iterX & FIXED_POINT_FRACT_VAL_BM) / FIXED_POINT_ALPHA_DIVFAC;
#else
                    Pt::int32_t frx = (iterX & FIXED_POINT_FRACT_VAL_BM) * FIXED_POINT_ALPHA_MULFAC;
#endif
                    Pt::int32_t flx = 255 - frx;
                    // Calculate the left and right coordinate of the span
                    Pt::int32_t lx = iterX >> FIXED_POINT_SHIFT_FACTOR;
                    Pt::int32_t rx = frx ? (lx + 1) : lx;
                    // Draw the block
                    if( ((Pt::int32_t) alphas[lx] + flx) <= 255 ) alphas[lx] += flx;
                    else                                          alphas[lx]  = 255;
                    if( rx < imgW ) {
                        if( ((Pt::int32_t) alphas[rx] + frx) <= 255 ) alphas[rx] += frx;
                        else                                          alphas[rx]  = 255;
                    }
                }
                iterX += chgX;
            }
        }
        alphas -= imgW;
        curX1  -= chgX1;
        curX2  -= chgX2;
    }

#else

    Pt::int32_t curX1 = (v3.x() << FIXED_POINT_SHIFT_FACTOR) + FIXED_POINT_FRACT_VAL_BM / 2;
    Pt::int32_t curX2 = (v3.x() << FIXED_POINT_SHIFT_FACTOR) - FIXED_POINT_FRACT_VAL_BM / 2;

    Pt::uint8_t* alphas = &_alphas[0] + v3.y() * imgW;

    for(int i = v3.y(); i > v1.y(); --i) {
        if(curX1 <= curX2) {
            for(int j = (curX1 >> FIXED_POINT_SHIFT_FACTOR); j <= (curX2 >> FIXED_POINT_SHIFT_FACTOR); ++j) {
                alphas[j] = 255;
            }
        }
        else {
            for(int j = (curX2 >> FIXED_POINT_SHIFT_FACTOR); j <= (curX1 >> FIXED_POINT_SHIFT_FACTOR); ++j) {
                alphas[j] = 255;
            }
        }
        alphas -= imgW;
        curX1  -= chgX1;
        curX2  -= chgX2;
    }

#endif
}

void Rasterizer2::rasterFillTriangles(Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY)
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
            rasterSolidTriangles(points, pointCount, minX, minY, maxX, maxY);
            break;
    }
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
