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

        Gfx::PaintContext* beginPaint(const Gfx::Paint& paint,
                                      Gfx::PaintContext* context);

    protected:
        virtual const Scaling& onGetScaling() const = 0;

        virtual Gfx::Image onGetImage() const = 0;

        virtual bool onBeginPaint(const Gfx::Paint& paint, PaintContext* context) = 0;

        virtual PaintContext* onBeginPaint(const Gfx::Paint& paint) = 0;

        virtual void onReleasePaint() = 0;

    protected:
        virtual void setCompositionMode(const Gfx::CompositionMode& mode) = 0;

        virtual void setPen(const Pen& pen) = 0;

        virtual void setBrush(const Brush& brush) = 0;

        virtual void setFont(const Gfx::Font& font) = 0;

        virtual void setClip(const RectF& clip) = 0;

        virtual void resetClip() = 0;

    protected:
        virtual void drawLine(const Line& line) = 0;

        virtual void drawRect(const Gfx::RectF& rect) = 0;

        virtual void fillRect(const Gfx::RectF& rect) = 0;

        virtual void drawPolyline(const Gfx::Polyline& line) = 0;

        virtual void fillPolygon(const Gfx::Polyline& line) = 0;
        
        virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size) = 0;

        virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size) = 0;

    protected:
        virtual Gfx::FontMetrics fontMetrics(const Pt::String& text) const = 0;

        virtual void drawText(const PointF& to, const Pt::String& text) = 0;

        virtual void drawText(const PointF& to, const Pt::String& text, const Transform& t) = 0;

    protected:
        virtual void drawImage(const Gfx::PointF& to, 
                               const Gfx::Image& image) = 0;

        virtual void drawImage(const Gfx::PointF& to, 
                               const Gfx::Image& image, 
                               const Gfx::RectF& imgRect) = 0;

        virtual void drawSurface(const Gfx::PointF& to, 
                                 const Gfx::PaintSurface& surface) = 0;

        virtual void drawSurface(const Gfx::PointF& to,
                                 const Gfx::PaintSurface& surface,
                                 const Gfx::RectF& rect) = 0;

    private:
        void attachPaint(PaintContext& paint);

        void detachPaint(PaintContext& paint);
    
    private:
        PaintSurface*     _surface;
        PaintContext*     _paint;
};

} // namespace

} // namespace

#endif
