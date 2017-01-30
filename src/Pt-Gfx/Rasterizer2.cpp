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

#include "Rasterizer2.h"

#include "ClipPolygon.h"
#include "DrawText.h"

#include <stdio.h>

namespace Pt {

namespace Gfx {

// ======================================================================================
// ===== Internal Functions =============================================================
// ======================================================================================


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
{
    delete _text;
}

void Rasterizer2::setImage( Image& image )
{
    _image = &image;
    _brushBuffer.reset(_image->format(), _brushBuffer.size());
    updateClip();
}

const ImageFormat& Rasterizer2::format() const
{
    return _image->format();
}

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
            if( brush.texture().format() != _image->format() )
            {
                _brushBuffer.reset( _image->format(), brush.texture().size() );
                Gfx::copy( brush.texture().begin(), brush.texture().end(), _brushBuffer.begin() );
                _brushImage = &_brushBuffer;
            }
            else
            {
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
{
    return _text->fontMetrics( text );
}

FontMetrics Rasterizer2::fontMetrics( const Font& font, const Pt::String& text )
{
    DrawText textRender;
    textRender.setFont(font);

    return textRender.fontMetrics(text);
}

void Rasterizer2::image( const Point& to, const Image& img)
{
    Rect imageRect( Point(0,0), img.size() );
    image( to, img, imageRect );
}

void Rasterizer2::image(const Point& to, const Image& from, const Rect& fromRect)
{
    // clip fromRect to fit into the clip/image rect
    Point d = _currentClip.topLeft() - to;
    Point fromPos = fromRect.topLeft() + d;

    Rect fromClip( fromPos, _currentClip.size() );
    fromClip = fromRect.intersect(fromClip);

    if( fromClip.isNull() )
      return;

    // account for smaller fromRect
    Point toClip = to + (fromClip.topLeft() - fromRect.topLeft());

    _image->format().copy(_image->view(), toClip, from.view(), fromClip, _compositionMode);
}

void Rasterizer2::strokeText( const Point& to, const Pt::String& text )
{
    _text->setClip(_currentClip);
    _text->draw( *_image, _pen.color(), to, text );
}


void Rasterizer2::strokeOutline(const PointT* points, size_t pointCount)
{
    switch( _pen.style() )
    {
        case Pen::Solid:
            if( _pen.size() == 1 && pointCount == 2 )
                rasterOnePixelLine(points[0].x(), points[0].y(), points[1].x(), points[1].y());

            /*
            if( _pen.size() == 1 )
                drawThinSolidPolyline( points, pointCount );
            else
                drawWideSolidPolyline( points, pointCount );
            */
            break;

        case Pen::Dash:
        case Pen::DoubleDash:
            /*
            if( _pen.size() == 1 )
                drawThinDashPolyline(points, pointCount );
            else
                drawWideDashPolyline( points, pointCount );
            */
            break;
    }
}

// ======================================================================================
// ===== Protected Member Functions =====================================================
// ======================================================================================

#define FIXED_POINT_MUL_FACTOR 256
#define FIXED_POINT_AAA_FACTOR (FIXED_POINT_MUL_FACTOR - 1)
#define FIXED_POINT_FRACT_MASK 0x000000FF

void Rasterizer2::updateClip()
{
    Rect imageRect( Point(0,0) , _image->size() );
    _currentClip = _clip.isNull() ? imageRect : _clip.intersect( imageRect );
}

void Rasterizer2::rasterOnePixelLine( float x1_, float y1_, float x2_, float y2_ )
{
    // Clip the points
    // ### !!! TODO !!! ###
    float fx1 = x1_;
    float fy1 = y1_;
    float fx2 = x2_;
    float fy2 = y2_;

    // Find the minimum and maximum coordinates
    Pt::int32_t minX, minY, maxX, maxY;

    if(fx1 < fx2) {
        minX = floor(fx1);
        maxX = ceil (fx2);
    }
    else {
        minX = floor(fx2);
        maxX = ceil (fx1);
    }

    if(fy1 < fy2) {
        minY = floor(fy1);
        maxY = ceil (fy2);
    }
    else {
        minY = floor(fy2);
        maxY = ceil (fy1);
    }

    // Calculate the work buffer size
    Pt::int32_t sizeX = maxX - minX + 1;
    Pt::int32_t sizeY = maxY - minY + 1;

    // Initialize the work buffer
    _alphas.resize(sizeX * sizeY);
    memset(&_alphas[0], 0, _alphas.size());

    // Caculate the number of steps
    Pt::int32_t steps = std::min(sizeX, sizeY) * 1;

    // Translate and convert the coordinates
    Pt::int32_t x1, y1, x2, y2;

    if(fx2 > fx1) {
        x1 = 0;
        x2 = (fx2 - fx1) * FIXED_POINT_MUL_FACTOR;
    }
    else {
        x1 = (fx1 - fx2) * FIXED_POINT_MUL_FACTOR;
        x2 = 0;
    }

    if(fy2 > fy1) {
        y1 = 0;
        y2 = (fy2 - fy1) * FIXED_POINT_MUL_FACTOR;
    }
    else {
        y1 = (fy1 - fy2) * FIXED_POINT_MUL_FACTOR;
        y2 = 0;
    }

    // Calculate the increment factors
    Pt::int32_t ix = (x2 - x1) / steps;
    Pt::int32_t iy = (y2 - y1) / steps;

    // Draw the line
    for(int i = 0; i <= steps; ++i) {
        // Calculate the alpha factor of the block
        Pt::int32_t frx = x1 & FIXED_POINT_FRACT_MASK;
        Pt::int32_t fry = y1 & FIXED_POINT_FRACT_MASK;
        Pt::int32_t flx = FIXED_POINT_AAA_FACTOR - frx;
        Pt::int32_t fly = FIXED_POINT_AAA_FACTOR - fry;
        // Calculate the top-left position of the block
        Pt::int32_t lx = x1 / FIXED_POINT_MUL_FACTOR;
        Pt::int32_t ly = y1 / FIXED_POINT_MUL_FACTOR;
        // Calculate the bottom-right position of the block
        Pt::int32_t rx = frx ? (lx + 1) : lx;
        Pt::int32_t ry = fry ? (ly + 1) : ly;
        // Draw the block
                                       _alphas[ly * sizeX + lx] += (fly * flx) / FIXED_POINT_AAA_FACTOR;
        if( rx < sizeX               ) _alphas[ly * sizeX + rx] += (fly * frx) / FIXED_POINT_AAA_FACTOR;
        if(               ry < sizeY ) _alphas[ry * sizeX + lx] += (fry * flx) / FIXED_POINT_AAA_FACTOR;
        if( rx < sizeX && ry < sizeY ) _alphas[ry * sizeX + rx] += (fry * frx) / FIXED_POINT_AAA_FACTOR;
        // Increment the drawing coordinate
        x1 += ix;
        y1 += iy;
    }

    // Blit the draw buffer to the image
    for(int r = 0; r < sizeY; ++r) {
        Pixel destPixel( _image->view(), minX, minY + r);
        _image->format().copy(destPixel, _alphas.data() + r * sizeX, sizeX, _pen.color(), _compositionMode);
    }
}


} // namespace

} // namespace
