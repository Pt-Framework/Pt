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

#ifndef PT_GFX_RASTER_CONTEXT_H
#define PT_GFX_RASTER_CONTEXT_H

#include <Pt/Gfx/PaintContext.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Rect.h>

#include <SkSurface.h>
#include <SkImage.h>
#include <SkCanvas.h>
#include <SkFont.h>
#include <SkPath.h>
#include <SkDashPathEffect.h>

namespace Pt {

namespace Gfx {

class SkiaContext : public PaintContext
{
    public:
        SkiaContext();

        ~SkiaContext();

        const Gfx::CompositionMode& compositionMode() const
        {
            return _compositionMode;
        }

        const Pen& pen() const
        {
            return _pen;
        }

        const SkPaint& skPen() const
        {
            return _skiaPen;
        }

        const SkPaint& brush() const
        {
            return _skiaBrush;
        }

        const Font& font() const
        {
            return _font;
        }

        const RectF* clip() const
        {
            return _hasClip ? &_clip : 0;
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

    protected:
        void onSetCompositionMode(const Gfx::CompositionMode& mode) override;

        virtual void onSetPen(const Gfx::Pen& pen) override;

        virtual void onSetBrush(const Gfx::Brush& brush) override;

        virtual void onSetFont(const Gfx::Font& font) override;

        virtual void onSetClip(const Gfx::RectF* clip) override;

    private:
        CompositionMode _compositionMode;
        Pen             _pen;
        SkPaint         _skiaPen;
        SkPaint         _skiaBrush;
        Font            _font;
        RectF           _clip;
        bool            _hasClip;
};

} //namespace

} //namespace

#endif
