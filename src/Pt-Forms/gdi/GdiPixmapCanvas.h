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

#ifndef PT_FORMS_PIXMAP_CANVAS_H
#define PT_FORMS_PIXMAP_CANVAS_H

#include <Pt/Forms/Api.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Path.h>
#include <Pt/Gfx/Canvas.h>

#include <Windows.h>

namespace Pt {

namespace Forms {

class Pixmap;
class GdiPixmapImpl;

class GdiPixmapCanvas : public Gfx::Canvas
{
    public:
        GdiPixmapCanvas();

        ~GdiPixmapCanvas();

        void setPixmap(GdiPixmapImpl& pixmap);

        void drawPixmap(const Gfx::PointF& to,
                        const Pixmap& pm,
                        const Gfx::RectF* rect);

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
        virtual Gfx::TextMetrics onGetTextMetrics(const Pt::String& text) const override;

        virtual const Gfx::FontMetrics& onGetFontMetrics() const override;

        virtual void onDrawText(const Gfx::PointF& to, 
                                const Pt::String& text, 
                                const Gfx::Transform* transform) override;

    protected:
        virtual void onDrawImage(const Gfx::PointF& toF, 
                                 const Gfx::Image& image,
                                 const Gfx::RectF* rect) override;

    protected:
        virtual void onSetPath(const Gfx::Path& path) override;

        virtual void onDrawPath() override;

        virtual void onFillPath() override;

        virtual void onDrawPath(const Gfx::Path& path) override;

        virtual void onFillPath(const Gfx::Path& path) override;

    private:
        POINT toContext(double x, double y);

        POINT toContext(const Gfx::PointF& p);

        void addPath(HDC dc, const Gfx::Path& path);

        Gfx::FontMetrics getFontMetrics() const;

    private:
        GdiPixmapImpl*               _pixmap;
        Gfx::CompositionMode      _compositionMode;
        double                    _lastScaleFactor;
        Gfx::Pen                  _logicalPen;
        HPEN                      _pen;
        DWORD                     _penSize;
        Gfx::Color                _penColor;
        HBRUSH                    _brush;
        bool                      _gradientBrush;
        Gfx::Brush::GradientStyle _gradient;
        Gfx::Color                _gradientStart;
        Gfx::Color                _gradientStop;
        std::wstring              _text;
        Gfx::RectF                _clip;
        bool                      _hasClip;
        HRGN                      _clipRect;
        HFONT                     _font;
        Gfx::FontMetrics          _fontMetrics;
        Gfx::Path                 _path;
};

} // namespace

} // namespace

#endif

