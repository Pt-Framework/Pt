/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2017 Marc Boris Duerner
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include "Rasterizer2.h"
#include "ClipShape.h"

namespace Pt {

namespace Gfx {

/*
// REVIEW: Seems nothing actually uses these functions anymore?
void Rasterizer2::rasterNarrowRect(const Point& tl, const Point& br)
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

    if(_pen.isSolid())
    {
        // Draw the rectangle's horizontal lines
        rasterNarrowSolidHLineSegment(minX, maxX, minY, _pen.color(), 0);
        rasterNarrowSolidHLineSegment(minX, maxX, maxY, _pen.color(), 0);

        // Draw the rectangle's vertical lines
        rasterNarrowSolidVLineSegment(minX, minY + 1, maxY - 1, _pen.color(), 0);
        rasterNarrowSolidVLineSegment(maxX, minY + 1, maxY - 1, _pen.color(), 0);
    }
    else // Patterned line
    {
        Pt::int32_t  fpiCtrInOut = PATTERN_BUFFER_1P_COUNTER_START;
        DrawLineMask mask;
        memcpy(mask, Rasterizer2::NullLineMask, sizeof(DrawLineMask));
        rasterNarrowPatternedLine(minX, minY, maxX, minY, _pen.color(), fpiCtrInOut, &mask);
        rasterNarrowPatternedLine(maxX, minY, maxX, maxY, _pen.color(), fpiCtrInOut, &mask);
        rasterNarrowPatternedLine(maxX, maxY, minX, maxY, _pen.color(), fpiCtrInOut, &mask);
        rasterNarrowPatternedLine(minX, maxY, minX, minY, _pen.color(), fpiCtrInOut, &mask);
    }
}
*/


void Rasterizer2::drawRect(const RectF& rect)
{
    std::vector<PointF> ps;
    ps.reserve(5);

    ps.push_back(rect.bottomLeft ());
    ps.push_back(rect.bottomRight());
    ps.push_back(rect.topRight   ());
    ps.push_back(rect.topLeft    ());
    ps.push_back(rect.bottomLeft ());

    /*
    for (size_t i = 0; i < n; ++i)
    {
        // Floor the coordinates with an epsilon of 0.001
        PointF p( Pt::lround(ps[i].x() - 0.4999),
                  Pt::lround(ps[i].y() - 0.4999) );
        // Only store non-duplicated coordinates
        if( ! polygon.empty() && polygon.back() == p )
          continue;
    }
    */

    if(_pen.size() == 1)
       drawNarrowPolyline( &ps[0], ps.size() );
    else
       drawWidePolyline( &ps[0], ps.size(), false );
}


void Rasterizer2::rasterNarrowRoundedRect(const RectF& rect, float radius)
{
    const float x1 = rect.topLeft().x();
    const float y1 = rect.topLeft().y();
    const float x2 = rect.bottomRight().x();
    const float y2 = rect.bottomRight().y();

    // line end masks
    DrawLineMask mask_zero;
    memcpy(mask_zero, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    DrawLineMask mask_nnp1;
    memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    // pattern state
    Pt::int32_t fpiCtrInOut = PATTERN_BUFFER_1P_COUNTER_START;

    // bottom left corner
    rasterNarrowQuadraticBezier(
        x1 + radius, y2,
        x1         , y2,
        x1         , y2 - radius,
        _pen.color(),
        _pen.isSolid() ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );

    // left staight line
    rasterNarrowQuadraticBezier(
        x1, y2 - radius,
        x1, y1 + rect.height() / 2,
        x1, y1 + radius,
        _pen.color(),
        _pen.isSolid() ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );

    // top left corner
    rasterNarrowQuadraticBezier(
        x1        , y1 + radius,
        x1        , y1,
        x1 + radius, y1,
        _pen.color(),
        _pen.isSolid() ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );

    // top straight line
    rasterNarrowQuadraticBezier(
        x1 + radius          , y1,
        x1 + rect.width() / 2, y1,
        x2 - radius          , y1,
        _pen.color(),
        _pen.isSolid() ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );

    // top right corner
    rasterNarrowQuadraticBezier(
        x2 - radius, y1,
        x2         , y1,
        x2         , y1 + radius,
        _pen.color(),
        _pen.isSolid() ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );

    // right straight line
    rasterNarrowQuadraticBezier(
        x2, y1 + radius,
        x2, y1 + rect.height() / 2,
        x2, y2 - radius,
        _pen.color(),
        _pen.isSolid() ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );

    // bottom right corner
    rasterNarrowQuadraticBezier(
        x2         , y2 - radius,
        x2         , y2,
        x2 - radius, y2,
        _pen.color(),
        _pen.isSolid() ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );

    // bottom straight line
    rasterNarrowQuadraticBezier(
        x2 - radius          , y2,
        x1 + rect.width() / 2, y2,
        x1 + radius          , y2,
        _pen.color(),
        _pen.isSolid() ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );
}


void Rasterizer2::rasterRectArea(const Point& tl, const Point& br)
{
    // Clip the coordinates
    // REVIEW: clipping
    // INFO: Looks correct :)
    Rect rect(tl, br);

    Rect clipRect(_currentClip.topLeft(), Size(_currentClip.width() + 1, _currentClip.height() + 1));

    rect = rect.intersect(clipRect);

    if (rect.isNull())
        return;

    Pt::int32_t minX = rect.left();
    Pt::int32_t minY = rect.top();
    Pt::int32_t maxX = rect.right();
    Pt::int32_t maxY = rect.bottom();
    // REVIEW: end

    // Calculate the width of the rectangle
    const Pt::int32_t sizeX = maxX - minX;

    // Draw the rectangle using texture (or gradient texture)
    if(_isTexture)
    {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        for(Pt::int32_t iterY = minY; iterY < maxY; ++iterY)
        {
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
    if(_isGradient)
    {
        for(Pt::int32_t iterY = minY; iterY < maxY; ++iterY)
        {
            Pt::int32_t iterX     = minX;
            Pt::int32_t spanWidth = sizeX;

            if(_brush.gradient() == Pt::Gfx::Brush::Vertical)
            {
                const Pt::int32_t textureY = std::min<Pt::int32_t>(iterY - minY, _brushImage->height() - 1);
                ConstPixel        srcPixel(_brushImage->view(), 0, textureY);
                Pixel             dstPixel(_image->view(), iterX, iterY);
                _image->format().setPixels(dstPixel, srcPixel, spanWidth, _compositionMode);
            }
            else // Pt::Gfx::Brush::Horizontal
            {
                while(spanWidth > 0)
                {
                    const Pt::int32_t textureX = std::min<Pt::int32_t>(iterX - minX, _brushImage->width() - 1);
                    const Pt::int32_t n        = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - textureX);
                    if(n)
                    {
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
    for(Pt::int32_t y = minY; y < maxY; ++y)
    {
        Pixel pixel(_image->view(), minX, y);
        _image->format().setPixels(pixel, _brush.color(),
                                   sizeX, _compositionMode);
    }
}


} // namespace

} // namespace
