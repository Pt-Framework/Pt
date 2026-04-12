/* Copyright (C) 2015-2024 Marc Boris Duerner

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

#ifndef PT_GFX_SKIA_BITMAP_CANVAS_H
#define PT_GFX_SKIA_BITMAP_CANVAS_H

#include "BitmapSurface.h"
#include "../freetype/FreeTypeRenderer.h"

#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Rgb32.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Polygon.h>

#include <SkCanvas.h>
#include <SkPaint.h>
#include <SkPath.h>
#include <SkDashPathEffect.h>

namespace Pt {

namespace Gfx {

class BitmapCanvas : public Canvas
{
    public:
        BitmapCanvas();

        ~BitmapCanvas();

        void init(BitmapSurface& surface);

    protected:
        virtual void onBeginPaint(const Gfx::Paint& paint) override;

        virtual void onFinishPaint() override;

    protected:
        virtual void onSetTransform(const Gfx::Transform& tx) override;

        virtual void onApplyTransform() override;

        virtual void onSetCompositionMode(const Gfx::CompositionMode& mode) override;

        virtual void onApplyCompositionMode() override;

        virtual void onSetPen(const Gfx::Pen& pen) override;

        virtual void onApplyPen() override;

        virtual void onSetBrush(const Gfx::Brush& brush) override;

        virtual void onApplyBrush() override;

        virtual void onSetFont(const Gfx::Font& font) override;

        virtual void onApplyFont() override;

        virtual void onSetClip(const Gfx::RectF* clip) override;

        virtual void onApplyClip() override;

    protected:
        virtual void onDrawLine(const Gfx::PointF& from, const Gfx::PointF& to) override;

        virtual void onDrawPolyline(const Gfx::PointF* pts, const size_t n) override;

        virtual void onFillPolygon(const Gfx::PointF* ps, const size_t n) override;

        virtual void onDrawRect(const Gfx::RectF& rectangle) override;

        virtual void onFillRect(const Gfx::RectF& rectangle) override;

        virtual void onDrawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size) override;

        virtual void onFillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size) override;

    protected:
        virtual void onSetPath(const Gfx::Path& path) override;

        virtual void onDrawPath() override;

        virtual void onFillPath() override;

        virtual void onDrawPath(const Gfx::Path& path) override;

        virtual void onFillPath(const Gfx::Path& path) override;

    protected:
        virtual const Gfx::FontMetrics& onGetFontMetrics() const override;

        virtual Gfx::TextMetrics onGetTextMetrics(const Pt::String& text) const override;

        virtual void onDrawText(const Gfx::PointF& to,
                                const Pt::String& text,
                                const Gfx::Transform* transform) override;

    protected:
        virtual void onDrawImage(const Gfx::PointF& to,
                                 const Gfx::Image& image,
                                 const Gfx::RectF* rect = 0) override;

    private:
        static SkPath toSkPath(const Gfx::Path& path);

        static SkColor toSkColor(const Gfx::ColorF& c);

        static SkPaint::Join toSkJoin(Gfx::Pen::JoinStyle s);

        static SkPaint::Cap toSkCap(Gfx::Pen::CapStyle s);

    private:
        BitmapSurface*          _surface;
        SkCanvas*               _canvas;
        Rgb32Image*             _image;
        CompositionMode         _compositionMode;
        RectI                   _currentClip;
        bool                    _hasClip;
        RectF                   _clip;
        Gfx::Pen                _pen;
        Gfx::Brush              _brush;
        Gfx::Font               _font;
        Gfx::FontMetrics        _fontMetrics;
        SkPaint                 _skPen;
        SkPaint                 _skBrush;
        Path                    _ptPath;
        SkPath                  _skPath;
        FreeTypeRenderer        _fontRenderer;
};

} // namespace

} // namespace

#endif
