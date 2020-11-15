/* Copyright (C) 2016 Marc Boris Duerner 
  
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

#ifndef Pt_Gfx_SkiaPaintData_h
#define Pt_Gfx_SkiaPaintData_h

#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>

#include <SkSurface.h>
#include <SkImage.h>
#include <SkCanvas.h>
#include <SkFont.h>
#include <SkPath.h>

namespace Pt {

namespace Gfx {

class SkiaPaintData : public PaintData
{
    public:
        SkiaPaintData()
        { }

        ~SkiaPaintData()
        { }

        void setPen(const Gfx::Pen& pen)
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
        }

        const SkPaint& pen() const
        {
            return _skiaPen;
        }

        void setBrush(const Gfx::Brush& brush)
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

        const SkPaint& brush() const
        {
            return _skiaBrush;
        }

        void setClip(const Gfx::RectF& r)
        {
            _clip = r;
        }

        const RectF& clipRect() const
        {
            return _clip;
        }

    private:
        static SkColor toSkia(const Gfx::Color& c)
        {
            return SkColorSetARGB((U8CPU)(c.alpha() / 257),
                (U8CPU)(c.red() / 257),
                (U8CPU)(c.green() / 257),
                (U8CPU)c.blue() / 257);
        }

        static SkPaint::Join toSkia(Gfx::Pen::JoinStyle s)
        {
            switch (s)
            {
            case Gfx::Pen::MiterJoin:
                return SkPaint::kMiter_Join;

            case Gfx::Pen::RoundJoin:
                return SkPaint::kRound_Join;

            case Gfx::Pen::BevelJoin:
                return SkPaint::kBevel_Join;
            }

            return SkPaint::kMiter_Join;
        }

        static SkPaint::Cap toSkia(Gfx::Pen::CapStyle s)
        {
            switch (s)
            {
            case Gfx::Pen::RoundCap:
                return SkPaint::kRound_Cap;

            case Gfx::Pen::FlatCap:
                return SkPaint::kButt_Cap;

            case Gfx::Pen::SquareCap:
                return SkPaint::kSquare_Cap;
            }

            return SkPaint::kSquare_Cap;
        }

    private:
        SkPaint   _skiaPen;
        SkPaint   _skiaBrush;
        RectF     _clip;
};

} // namespace

} // namespace

#endif
