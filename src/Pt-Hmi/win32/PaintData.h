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

#ifndef Pt_Hmi_PaintData_h
#define Pt_Hmi_PaintData_h

#include "PixmapSurfaceImpl.h"

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/String.h>
#include <algorithm>
#include <cctype>

using std::max;
using std::min;
#include <Windows.h>
#include <Gdiplus.h>

//#define PT_HMI_GDIPLUS 1

namespace Pt {

namespace Hmi {

class PixmapSurfaceImpl;

#ifndef PT_HMI_WIN32_RASTER

class PaintData : public Gfx::PaintContext
{
    public:
        PaintData();

        ~PaintData();

        void reset(PixmapSurfaceImpl* canvas);

        HPEN pen() const;

        Gfx::Color penColor() const;

        HBRUSH brush() const;

        bool gradientBrush() const;

        HFONT font() const;

        HRGN clipRect() const;

    protected:
        virtual void onSetPaint(const Gfx::Paint* paint) override;

        virtual void onBeginPaint(const Gfx::Paint& paint) override;

    protected:
        virtual void onSetCompositionMode(const Gfx::CompositionMode& mode);

        virtual void onSetPen(const Gfx::Pen& pen);

        virtual void onSetBrush(const Gfx::Brush& brush);

        virtual void onSetFont(const Gfx::Font& font);

        virtual void onSetClip(const Gfx::RectF& rectF);

        virtual void onResetClip();

    protected:
        virtual void onDrawLine(const Gfx::Line& line) override;

        virtual void onDrawPolyline(const Gfx::Polyline& line) override;

        virtual void onFillPolygon(const Gfx::Polyline& line) override;

        virtual void onDrawRect(const Gfx::RectF& rectangle) override;

        virtual void onFillRect(const Gfx::RectF& rectangle) override;

        virtual void onDrawEllipse(const Gfx::PointF& topLeft, 
                                   const Gfx::SizeF& size) override;

        virtual void onFillEllipse(const Gfx::PointF& topLeft, 
                                   const Gfx::SizeF& size) override;

    protected:
        virtual Gfx::FontMetrics onGetFontMetrics(const Pt::String& text) const override;

        virtual void onDrawText(const Gfx::PointF& to, const Pt::String& text) override;

        virtual void onDrawText(const Gfx::PointF& to, const Pt::String& text, 
                                const Gfx::Transform& trans) override;

    protected:
        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image) override;

        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image, 
                                 const Gfx::RectF& imgRect) override;

        virtual void onDrawSurface(const Gfx::PointF& to, 
                                   const Gfx::PaintSurface& surface) override;

        virtual void onDrawSurface(const Gfx::PointF& to,
                                   const Gfx::PaintSurface& surface,
                                   const Gfx::RectF& rect) override;

    private:
        void updateMode(const Gfx::CompositionMode& mode);

        void updatePen(const Gfx::Pen& pen);

        void updateBrush(const Gfx::Brush& brush);

        void updateFont(const Gfx::Font& font);

        void updateClip(const Gfx::RectF* clip);

    private:
        PixmapSurfaceImpl*        _canvas;
        Gfx::CompositionMode      _compositionMode;
        HPEN                      _pen;
        Gfx::Color                _penColor;
        HBRUSH                    _brush;
        bool                      _gradientBrush;
        HRGN                      _clipRect;
        HFONT                     _font;
};

#endif

} // namespace

} // namespace

#endif
