/* Copyright (C) 2020 Marc Boris Duerner

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

#ifndef Pt_Gfx_Canvas_h
#define Pt_Gfx_Canvas_h

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Scaling.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/Gfx/ImageFormat.h>

namespace Pt {

namespace Gfx {

class PaintSurface;
class PaintContext;
class Paint;
class Line;
class Polyline;
class Path;

/** @brief Paint canvas.
*/
class PT_GFX_API Canvas
{
    friend class PaintContext;

    public:
        explicit Canvas(PaintSurface& surface);

        ~Canvas();

        const Gfx::ImageFormat& format() const;

        const Gfx::SizeF& size() const;

        const Scaling& scaling() const;

        Image toImage() const;

        Gfx::PaintContext* beginPaint(Gfx::PaintContext* context);

    protected:
        virtual const Scaling& onGetScaling() const = 0;

        virtual bool onBeginPaint(PaintContext* context) = 0;

        virtual PaintContext* onBeginPaint() = 0;

        virtual void onReleasePaint() = 0;

    protected:
        virtual void setCompositionMode(const Gfx::CompositionMode& mode) = 0;

        virtual void setPen(const Pen& pen) = 0;

        virtual void setBrush(const Brush& brush) = 0;

        virtual void setFont(const Gfx::Font& font) = 0;

        virtual void setClip(const RectF& clip) = 0;

        virtual void resetClip() = 0;

    protected:
        void drawLine(const PointF& from, const PointF& to);

        void drawPolyline(const Gfx::Polyline& line);

        void fillPolygon(const Gfx::Polyline& line);

        void drawRect(const Gfx::RectF& rect);

        void fillRect(const Gfx::RectF& rect);

        void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

    protected:
        FontMetrics fontMetrics(const Pt::String& text) const;

        void drawText(const PointF& to, const Pt::String& text, 
                      const Transform* t = 0);

    protected:
        void drawImage(const Gfx::PointF& to, 
                       const Gfx::Image& image);

        void drawImage(const Gfx::PointF& to, 
                       const Gfx::Image& image, 
                       const Gfx::RectF& imgRect);

        void drawCanvas(const Gfx::PointF& to, 
                        const Gfx::Canvas& canvas);

        void drawCanvas(const Gfx::PointF& to,
                        const Gfx::Canvas& canvas,
                        const Gfx::RectF& rect);
    
    protected:
        virtual void onDrawLine(const PointF& from, const PointF& to) = 0;

        virtual void onDrawPolyline(const Gfx::Polyline& line) = 0;

        virtual void onFillPolygon(const Gfx::Polyline& line) = 0;

        virtual void onDrawRect(const Gfx::RectF& rect) = 0;

        virtual void onFillRect(const Gfx::RectF& rect) = 0;

        virtual void onDrawEllipse(const Gfx::PointF& topLeft, 
                                   const Gfx::SizeF& size) = 0;

        virtual void onFillEllipse(const Gfx::PointF& topLeft, 
                                   const Gfx::SizeF& size) = 0;

        virtual void onDrawArc(const Gfx::PointF& topLeft, const Gfx::SizeF& size, 
                               float degBegin, float degEnd) = 0;

        virtual void onFillChord(const Gfx::PointF& topLeft, const Gfx::SizeF& size, 
                                 float degBegin, float degEnd) = 0;

        virtual void onFillPie(const Gfx::PointF& topLeft, const Gfx::SizeF& size, 
                               float degBegin, float degEnd) = 0;

        virtual void onDrawPath(const Gfx::Path& path, float smoothness) = 0;

        virtual void onFillPath(const Gfx::Path& path, float smoothness) = 0;

    protected:
        virtual Gfx::FontMetrics onGetFontMetrics(const Pt::String& text) const = 0;

        virtual void onDrawText(const PointF& to, 
                                const Pt::String& text,
                                const Transform* transform = 0) = 0;

    protected:
        virtual Gfx::Image onGetImage() const = 0;

        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image) = 0;

        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image, 
                                 const Gfx::RectF& imgRect) = 0;

        virtual void onDrawCanvas(const Gfx::PointF& to, 
                                   const Gfx::Canvas& canvas) = 0;

        virtual void onDrawCanvas(const Gfx::PointF& to,
                                   const Gfx::Canvas& canvas,
                                   const Gfx::RectF& rect) = 0;

    private:
        void onDetachPaint(PaintContext& paint);
    
    private:
        PaintSurface*     _surface;
        PaintContext*     _paint;
};

} // namespace

} // namespace

#endif
