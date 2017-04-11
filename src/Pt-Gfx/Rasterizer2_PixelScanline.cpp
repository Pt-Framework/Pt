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

#include "Rasterizer2.h"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void Rasterizer2::stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2)
{
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    if( x1Valid && y1Valid ) {
        Pixel pixel(_image->view(), x1, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }

    if( x1Valid && y2Valid ) {
        Pixel pixel(_image->view(), x1, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }

    if( x2Valid && y1Valid ) {
        Pixel pixel(_image->view(), x2, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }

    if( x2Valid && y2Valid ) {
        Pixel pixel(_image->view(), x2, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }
}

void Rasterizer2::stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const bool mask[4])
{
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    if( x1Valid && y1Valid && mask[0] ) {
        Pixel pixel(_image->view(), x1, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }

    if( x1Valid && y2Valid && mask[1] ) {
        Pixel pixel(_image->view(), x1, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }

    if( x2Valid && y1Valid && mask[2] ) {
        Pixel pixel(_image->view(), x2, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }

    if( x2Valid && y2Valid && mask[3] ) {
        Pixel pixel(_image->view(), x2, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }
}

void Rasterizer2::stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::uint8_t alpha)
{
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    if (x1Valid && y1Valid ) {
        Pixel pixel(_image->view(), x1, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }

    if( x1Valid && y2Valid ) {
        Pixel pixel(_image->view(), x1, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }

    if( x2Valid && y1Valid ) {
        Pixel pixel(_image->view(), x2, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }

    if( x2Valid && y2Valid ) {
        Pixel pixel(_image->view(), x2, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }
}

void Rasterizer2::stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::uint8_t alpha, const bool mask[4])
{
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    if( x1Valid && y1Valid && mask[0] ) {
        Pixel pixel(_image->view(), x1, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }

    if( x1Valid && y2Valid && mask[1] ) {
        Pixel pixel(_image->view(), x1, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }

    if( x2Valid && y1Valid && mask[2] ) {
        Pixel pixel(_image->view(), x2, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }

    if( x2Valid && y2Valid && mask[3] ) {
        Pixel pixel(_image->view(), x2, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }
}

void Rasterizer2::fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY)
{
    // Check the clipping
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    // Draw the pixels using texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        if( x1Valid && y1Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
        if( x1Valid && y2Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
        if( x2Valid && y1Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
        if( x2Valid && y2Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
    }

    // Draw the pixels using solid color
    else {
        if( x1Valid && y1Valid ) {
            Pixel pixel(_image->view(), x1, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
        if( x1Valid && y2Valid ) {
            Pixel pixel(_image->view(), x1, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
        if( x2Valid && y1Valid ) {
            Pixel pixel(_image->view(), x2, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
        if( x2Valid && y2Valid ) {
            Pixel pixel(_image->view(), x2, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
    }
}

void Rasterizer2::fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, const bool mask[4])
{
    // Check the clipping
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    // Draw the pixels using texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        if( x1Valid && y1Valid && mask[0] ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
        if( x1Valid && y2Valid && mask[1] ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
        if( x2Valid && y1Valid && mask[2] ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
        if( x2Valid && y2Valid && mask[3] ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
    }

    // Draw the pixels using solid color
    else {
        if( x1Valid && y1Valid && mask[0] ) {
            Pixel pixel(_image->view(), x1, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
        if( x1Valid && y2Valid && mask[1] ) {
            Pixel pixel(_image->view(), x1, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
        if( x2Valid && y1Valid && mask[2] ) {
            Pixel pixel(_image->view(), x2, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
        if( x2Valid && y2Valid && mask[3] ) {
            Pixel pixel(_image->view(), x2, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
    }
}

void Rasterizer2::fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, Pt::uint8_t alpha)
{
    // Check the clipping
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    // Draw the pixels using texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        if( x1Valid && y1Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
        if( x1Valid && y2Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
        if( x2Valid && y1Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
        if( x2Valid && y2Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
    }

    // Draw the pixels using solid color
    else {
        if( x1Valid && y1Valid ) {
            Pixel pixel(_image->view(), x1, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
        if( x1Valid && y2Valid ) {
            Pixel pixel(_image->view(), x1, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
        if( x2Valid && y1Valid ) {
            Pixel pixel(_image->view(), x2, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
        if( x2Valid && y2Valid ) {
            Pixel pixel(_image->view(), x2, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
    }
}

void Rasterizer2::fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, Pt::uint8_t alpha, const bool mask[4])
{
    // Check the clipping
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    // Draw the pixels using texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        if( x1Valid && y1Valid && mask[0] ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
        if( x1Valid && y2Valid && mask[1] ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
        if( x2Valid && y1Valid && mask[2] ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
        if( x2Valid && y2Valid && mask[3] ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
    }

    // Draw the pixels using solid color
    else {
        if( x1Valid && y1Valid && mask[0] ) {
            Pixel pixel(_image->view(), x1, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
        if( x1Valid && y2Valid && mask[1] ) {
            Pixel pixel(_image->view(), x1, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
        if( x2Valid && y1Valid && mask[2] ) {
            Pixel pixel(_image->view(), x2, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
        if( x2Valid && y2Valid && mask[3] ) {
            Pixel pixel(_image->view(), x2, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
    }
}

void Rasterizer2::fill4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, const Pt::uint8_t alphaMask[4])
{
    // Check the clipping
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    // Draw the pixels using texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        if( x1Valid && y1Valid && alphaMask[0] ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alphaMask[0]);
        }
        if( x1Valid && y2Valid && alphaMask[1] ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alphaMask[1]);
        }
        if( x2Valid && y1Valid && alphaMask[2] ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alphaMask[2]);
        }
        if( x2Valid && y2Valid && alphaMask[3] ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alphaMask[3]);
        }
    }

    // Draw the pixels using solid color
    else {
        if( x1Valid && y1Valid && alphaMask[0] ) {
            Pixel pixel(_image->view(), x1, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alphaMask[0]);
        }
        if( x1Valid && y2Valid && alphaMask[1] ) {
            Pixel pixel(_image->view(), x1, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alphaMask[1]);
        }
        if( x2Valid && y1Valid && alphaMask[2] ) {
            Pixel pixel(_image->view(), x2, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alphaMask[2]);
        }
        if( x2Valid && y2Valid && alphaMask[3] ) {
            Pixel pixel(_image->view(), x2, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alphaMask[3]);
        }
    }
}

void Rasterizer2::rasterScanline(
    Pt::int32_t  iterL, Pt::int32_t iterR, Pt::int32_t pixelY,
    Pt::int32_t  minX,  Pt::int32_t minY,
    const Color& color
)
{
    // Draw the span using texture (or gradient texture)
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
    _image->format().setPixels(pixel, color, iterR - iterL + 1, _compositionMode);
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

void Rasterizer2::rasterScanlineWithClipping(Pt::int32_t from, Pt::int32_t to, Pt::int32_t pixelY, Pt::int32_t minX, Pt::int32_t minY)
{
    // Check if the Y coordinate is outside the clipping region
    if( !ClipShapeI::insideYRange(pixelY, _currentClip) ) return;

    // Check and limit the X coordinates
    from = ClipShapeI::clipLeft  (from, _currentClip);
    to   = ClipShapeI::clipRight (to,   _currentClip);

    if(to < from) return;

    // Draw the scanline
    rasterScanline(from - minX, to - minX, pixelY - minY, minX, minY, _brush.color());
}


} // namespace
} // namespace
