/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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

#include "SkiaContext.h"

namespace Pt {

namespace Gfx {

SkiaContext::SkiaContext()
: PaintContext()
, _hasClip(false)
{
}


SkiaContext::~SkiaContext()
{
}


void SkiaContext::onSetCompositionMode(const Gfx::CompositionMode& mode) 
{
    _compositionMode = mode;
}


void SkiaContext::onSetPen(const Gfx::Pen& pen)
{
    _skiaPen = SkPaint();
    _skiaPen.setColor(toSkia(pen.color()));
    _skiaPen.setStyle(SkPaint::kStroke_Style);
    _skiaPen.setAntiAlias(true);
    _skiaPen.setStrokeWidth(pen.size());
    _skiaPen.setStrokeJoin(toSkia(pen.joinStyle()));
    _skiaPen.setStrokeCap(toSkia(pen.capStyle()));

    switch( pen.style() )
    {
        default:
        case Pt::Gfx::Pen::Solid:
        {
        }
        break;

        case Pt::Gfx::Pen::Dash:
        {
            if (pen.capStyle() == Gfx::Pen::RoundCap || pen.capStyle() == Gfx::Pen::SquareCap)
            {
                SkScalar dashes[] = { 2.0f * pen.size(), 2.0f * pen.size() };
                _skiaPen.setPathEffect(SkDashPathEffect::Make(dashes, 2, 0));
            }
            else
            {
                SkScalar dashes[] = { 3.0f * pen.size(), 1.0f * pen.size() };
                _skiaPen.setPathEffect(SkDashPathEffect::Make(dashes, 2, 0));
            }
        }
        break;

        case Pt::Gfx::Pen::Dot:
        {
            if (pen.capStyle() == Gfx::Pen::RoundCap ||
                pen.capStyle() == Gfx::Pen::SquareCap)
            {
                SkScalar dashes[] = { 1.0f, 2.0f * pen.size() };
                _skiaPen.setPathEffect(SkDashPathEffect::Make(dashes, 2, 0));
            }
            else
            {
                SkScalar dashes[] = { 1.0f * pen.size(), 1.0f * pen.size() };

                _skiaPen.setPathEffect(SkDashPathEffect::Make(dashes, 2, 0));
            }
        }
        break;
    }

    _pen = pen;
}


void SkiaContext::onSetBrush(const Gfx::Brush& brush)
{
    _skiaBrush = SkPaint();
    _skiaBrush.setAntiAlias(true);
    _skiaBrush.setStyle(SkPaint::kFill_Style);
    _skiaBrush.setColor(toSkia(brush.color()));

    switch( brush.fillStyle() )
    {
        case Gfx::Brush::Solid:
        {
        }
        break;

        case Gfx::Brush::Texture:
        {
        }
        break;

        case Gfx::Brush::Gradient:
        {
            switch (brush.gradient())
            {
            case Gfx::Brush::Linear:
            case Gfx::Brush::Horizontal:
            case Gfx::Brush::Vertical:
                break;

            case Gfx::Brush::Radial:

                break;
            }
        }
        break;

        default:
            break;
    }
}


void SkiaContext::onSetFont(const Gfx::Font& font)
{
    _font = font;
}


void SkiaContext::onSetClip(const Gfx::RectF* clip)
{
    _hasClip = clip != 0;

    if(clip)
        _clip = *clip;
    else
        _clip.clear();
}

} // namespace

} // namespace
