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

namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

Rasterizer2::Rasterizer2(Image& image)
: _aaLevel(1)
, _image(&image)
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
    _brush      = brush;
    _isGradient = false;
    _isTexture  = false;

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
            _isTexture = true;
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


// ======================================================================================
// ===== Private Member Functions - Utilities ===========================================
// ======================================================================================

void Rasterizer2::updateGradientBrush(Pt::int32_t width, Pt::int32_t height)
{
    // Start colors
    Pt::uint8_t rs, gs, bs, as;

    // End colors
    Pt::uint8_t re, ge, be, ae;

    // Resize the brush buffer and the start-end colors
    switch(_brush.fillStyle()) {
        case Pt::Gfx::Brush::HorizontalGradient:
            // Resize the brush buffer
            height = 1;
            _brushBuffer.reset(_image->format(), Size(width, 1));
            // Determine the colors
            rs = _brush.color        ().red  () / 257;
            gs = _brush.color        ().green() / 257;
            bs = _brush.color        ().blue () / 257;
            as = _brush.color        ().alpha() / 257;
            re = _brush.gradientColor().red  () / 257;
            ge = _brush.gradientColor().green() / 257;
            be = _brush.gradientColor().blue () / 257;
            ae = _brush.gradientColor().alpha() / 257;
            break;

        case Pt::Gfx::Brush::VerticalGradient:
            // Resize the brush buffer
            width = 1;
            _brushBuffer.reset(_image->format(), Size(1, height));
            // Determine the colors
            rs = _brush.gradientColor().red  () / 257;
            gs = _brush.gradientColor().green() / 257;
            bs = _brush.gradientColor().blue () / 257;
            as = _brush.gradientColor().alpha() / 257;
            re = _brush.color        ().red  () / 257;
            ge = _brush.color        ().green() / 257;
            be = _brush.color        ().blue () / 257;
            ae = _brush.color        ().alpha() / 257;
            break;

        default:
            return;
    }

    // Create the gradient
    const Pt::int32_t  length = width + height - 1 - 1;
          Pt::uint8_t* pixel  = _brushBuffer.data();
    for(int n = 0; n <= length; ++n) {
        const Pt::int32_t f2 = FIXED_POINT_FROM_INT(n) / length;
        const Pt::int32_t f1 = FIXED_POINT_CONSTANT_ONE - f2;
        const Pt::uint8_t r1 = FIXED_POINT_TO_INT(rs * f1);
        const Pt::uint8_t r2 = FIXED_POINT_TO_INT(re * f2);
        const Pt::uint8_t g1 = FIXED_POINT_TO_INT(gs * f1);
        const Pt::uint8_t g2 = FIXED_POINT_TO_INT(ge * f2);
        const Pt::uint8_t b1 = FIXED_POINT_TO_INT(bs * f1);
        const Pt::uint8_t b2 = FIXED_POINT_TO_INT(be * f2);
        const Pt::uint8_t a1 = FIXED_POINT_TO_INT(as * f1);
        const Pt::uint8_t a2 = FIXED_POINT_TO_INT(ae * f2);
        *pixel++ = b1 + b2;
        *pixel++ = g1 + g2;
        *pixel++ = r1 + r2;
        *pixel++ = a1 + a2;
    }
}

void Rasterizer2::updateClip()
{
    const Rect imageRect( Point(0,0) , _image->size() );
    _currentClip = _clip.isNull() ? imageRect : _clip.intersect( imageRect );
}

void Rasterizer2::genClippedPolygonPoints(std::vector<Point>& dst, const Point* src, const size_t pointCount) const
{
    for(size_t i = 0; i < pointCount; ++i)
        dst.push_back( Point( src[i].x(), src[i].y() ) );

    ClipShape::clipPolygon(dst, _currentClip);
}

void Rasterizer2::getPolygonRectMinMax(const Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY)
{
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
}

void Rasterizer2::rasterScanline(
    Pt::int32_t  iterL, Pt::int32_t iterR, Pt::int32_t pixelY,
    Pt::int32_t  minX,  Pt::int32_t minY,
    const Color& color
)
{
    // Draw the span using texture
    if(_isTexture) {
        Pt::int32_t iterX     = iterL;
        Pt::int32_t spanWidth = iterR - iterL + 1;
        while(spanWidth > 0) {
            const Pt::int32_t tX = iterX  % _brushImage->width ();
            const Pt::int32_t tY = pixelY % _brushImage->height();
            const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tX);
            if(n) {
                ConstPixel srcPixel(_brushImage->view(), tX, tY);
                Pixel      dstPixel(_image->view(), minX + iterX, minY + pixelY);
                _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
            }
            spanWidth -= n;
            iterX     += n;
        }
        return;
    }

    // Draw the span using gradient
    if(_isGradient) {
        Pt::int32_t iterX     = iterL;
        Pt::int32_t spanWidth = iterR - iterL + 1;
        // Fill the span - vertical gradient
        if(_brush.fillStyle() == Pt::Gfx::Brush::VerticalGradient) {
            const Pt::int32_t textureY = pixelY % _brushImage->height();
            ConstPixel        srcPixel(_brushImage->view(), 0, textureY);
            Pixel             dstPixel(_image->view(), minX + iterX, minY + pixelY);
            _image->format().setPixels(dstPixel, srcPixel, spanWidth, _compositionMode);
        }
        // Fill the span - horizontal gradient
        else {
            while(spanWidth > 0) {
                const Pt::int32_t tX = iterX  % _brushImage->width ();
                const Pt::int32_t tY = pixelY % _brushImage->height();
                const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tX);
                if(n) {
                    ConstPixel srcPixel(_brushImage->view(), tX, tY);
                    Pixel      dstPixel(_image->view(), minX + iterX, minY + pixelY);
                    _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
                }
                spanWidth -= n;
                iterX     += n;
            }
        }
        return;
    }

    // Draw the span using solid color
    Pixel pixel(_image->view(), minX + iterL, minY + pixelY);
    _image->format().setPixels(pixel, _brush.color(), iterR - iterL + 1, _compositionMode);
    //Pt::int32_t iterX     = iterL;
    //Pt::int32_t spanWidth = iterR - iterL + 1;
    //while(spanWidth > 0) {
    //    const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
    //    if(n) {
    //        Pixel pixel(_image->view(), minX + iterX, minY + pixelY);
    //        _image->format().copy(pixel, _brushPixel, n, _compositionMode);
    //    }
    //    spanWidth -= n;
    //    iterX     += n;
    //}
}


} // namespace
} // namespace
