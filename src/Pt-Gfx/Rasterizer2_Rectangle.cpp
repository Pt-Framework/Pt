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

#include "Rasterizer2_Common.h"

namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

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

void Rasterizer2::fillRect(const Point& tl, const Point& br)
{
    if(_isGradient)
        updateGradientBrush(br.x() - tl.x() + 1, br.y() - tl.y() + 1);

    rasterRectArea(tl, br);
}

// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

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
    rasterOnePixelHLineSegment(minX, maxX, minY, _pen.color(), false);
    rasterOnePixelHLineSegment(minX, maxX, maxY, _pen.color(), false);

    // Draw the rectangle's vertical lines
    rasterOnePixelVLineSegment(minX, minY + 1, maxY - 1, _pen.color(), false);
    rasterOnePixelVLineSegment(maxX, minY + 1, maxY - 1, _pen.color(), false);
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

    // Draw the rectangle using texture
    if(_isTexture) {
        for(Pt::int32_t iterY = minY; iterY <= maxY; ++iterY) {
            Pt::int32_t iterX     = minX;
            Pt::int32_t spanWidth = sizeX;
            // Fill the spans
            while(spanWidth > 0) {
                const Pt::int32_t tX = (iterX - minX) % _brushImage->width ();
                const Pt::int32_t tY = (iterY - minY) % _brushImage->height();
                const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tX);
                if(n) {
                    ConstPixel srcPixel(_brushImage->view(), tX, tY);
                    Pixel      dstPixel(_image->view(), iterX, iterY);
                    _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
                }
                spanWidth -= n;
                iterX     += n;
            }
        }
        return;
    }

    // Draw the rectangle using gradient
    if(_isGradient) {
        for(Pt::int32_t iterY = minY; iterY <= maxY; ++iterY) {
            Pt::int32_t iterX     = minX;
            Pt::int32_t spanWidth = sizeX;
            // Fill the spans - vertical gradient
            if(_brush.fillStyle() == Pt::Gfx::Brush::VerticalGradient) {
                const Pt::int32_t textureY = (iterY - minY) % _brushImage->height();
                ConstPixel        srcPixel(_brushImage->view(), 0, textureY);
                Pixel             dstPixel(_image->view(), iterX, iterY);
                _image->format().setPixels(dstPixel, srcPixel, spanWidth, _compositionMode);
            }
            // Fill the spans - horizontal gradient
            else {
                while(spanWidth > 0) {
                    const Pt::int32_t textureX = (iterX - minX) % _brushImage->width ();
                    const Pt::int32_t n        = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - textureX);
                    if(n) {
                        ConstPixel srcPixel(_brushImage->view(), textureX, 0);
                        Pixel      dstPixel(_image->view(), iterX, iterY);
                        _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
                    }
                    spanWidth -= n;
                    iterX     += n;
                }
            }
        }
        return;
    }

    // Draw the rectangle using solid color
    for(Pt::int32_t y = minY; y <= maxY; ++y) {
        Pt::int32_t spanWidth = sizeX;
        while(spanWidth > 0) {
            const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
            Pixel             pixel(_image->view(), minX + sizeX - spanWidth, y);
            _image->format().copy(pixel, _brushPixel, n, _compositionMode);
            spanWidth -= n;
        }
    }
}


} // namespace
} // namespace
