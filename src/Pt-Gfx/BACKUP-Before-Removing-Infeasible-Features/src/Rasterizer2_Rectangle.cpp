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
// ===== Public Member Functions ========================================================
// ======================================================================================

void Rasterizer2::strokeOnePixelRect(const Point& tl, const Point& br)
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

    // Solid line
    if(_pen.style() == Pen::Solid) {
        // Draw the rectangle's horizontal lines
        rasterOnePixelSolidHLineSegment(minX, maxX, minY, _pen.color(), 0);
        rasterOnePixelSolidHLineSegment(minX, maxX, maxY, _pen.color(), 0);
        // Draw the rectangle's vertical lines
        rasterOnePixelSolidVLineSegment(minX, minY + 1, maxY - 1, _pen.color(), 0);
        rasterOnePixelSolidVLineSegment(maxX, minY + 1, maxY - 1, _pen.color(), 0);
    }

    // Patterned line
    else {
        Pt::int32_t  fpiCtrInOut = PATTERN_BUFFER_COUNTER_START;
        DrawLineMask mask;
        memcpy(mask, Rasterizer2::NullLineMask, sizeof(DrawLineMask));
        rasterOnePixelPatternedLine(minX, minY, maxX, minY, _pen.color(), fpiCtrInOut, &mask);
        rasterOnePixelPatternedLine(maxX, minY, maxX, maxY, _pen.color(), fpiCtrInOut, &mask);
        rasterOnePixelPatternedLine(maxX, maxY, minX, maxY, _pen.color(), fpiCtrInOut, &mask);
        rasterOnePixelPatternedLine(minX, maxY, minX, minY, _pen.color(), fpiCtrInOut, &mask);
    }
}

void Rasterizer2::fillRect(const Point& tl, const Point& br)
{
    // Update the gradient as needed
    if(_isGradient)
        updateGradientBrush(br.x() - tl.x() + 1, br.y() - tl.y() + 1);

    // Draw the rectangle
    rasterRectArea(tl, br);
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void Rasterizer2::rasterRectArea(const Point& tl, const Point& br)
{
    // Get the minimum and maximum coordinates
    Pt::int32_t minX = tl.x();
    Pt::int32_t minY = tl.y();
    Pt::int32_t maxX = br.x();
    Pt::int32_t maxY = br.y();

    // Clip the coordinates
    minX = ClipShapeI::clipLeft  (minX, _currentClip);
    minY = ClipShapeI::clipTop   (minY, _currentClip);
    maxX = ClipShapeI::clipRight (maxX, _currentClip);
    maxY = ClipShapeI::clipBottom(maxY, _currentClip);

    // Calculate the width of the rectangle
    const Pt::int32_t sizeX = maxX - minX + 1;

    // Draw the rectangle using texture (or gradient texture)
    if(_isTexture) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        for(Pt::int32_t iterY = minY; iterY <= maxY; ++iterY) {
            Pt::int32_t iterX     = minX;
            Pt::int32_t spanWidth = sizeX;
            // Fill the spans
            while(spanWidth > 0) {
                const Pt::int32_t dx = iterX - minX;
                const Pt::int32_t dy = iterY - minY;
                const Pt::int32_t tx = _isGradient ? std::min(bw - 1, dx) : (dx % bw);
                const Pt::int32_t ty = _isGradient ? std::min(bh - 1, dy) : (dy % bh);
                const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, bw - tx);
                if(n) {
                    ConstPixel srcPixel(_brushImage->view(), tx, ty);
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
                const Pt::int32_t textureY = std::min<Pt::int32_t>(iterY - minY, _brushImage->height() - 1);
                ConstPixel        srcPixel(_brushImage->view(), 0, textureY);
                Pixel             dstPixel(_image->view(), iterX, iterY);
                _image->format().setPixels(dstPixel, srcPixel, spanWidth, _compositionMode);
            }
            // Fill the spans - horizontal gradient
            else {
                while(spanWidth > 0) {
                    const Pt::int32_t textureX = std::min<Pt::int32_t>(iterX - minX, _brushImage->width() - 1);
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
        Pixel pixel(_image->view(), minX, y);
        _image->format().setPixels(pixel, _brush.color(), sizeX, _compositionMode);
    }
}


} // namespace
} // namespace
