/* Copyright (C) 2006-2024 Laurentiu-Gheorghe Crisan
   Copyright (C) 2015-2024 Marc Boris Duerner

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

#ifndef PT_GFX_PAINT_H
#define PT_GFX_PAINT_H

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
#include <Pt/Gfx/Path.h>
#include <Pt/String.h>
#include <Pt/Types.h>
#include <cstddef>

namespace Pt {

namespace Gfx {

class Canvas;
class Painter;
class Scaling;
class PaintSurface;

class Line;
class Polyline;

/** @todo TODO:

    - distinguish between paint scaling and the physical/logical pixel ratio.
      If these are separate attributes, Paint objects can perform scaling, if
      needed for painting. The pixel ratio is reported by the implementation for
      alignment purposes. Paint scaling can be 1.0 if the underlying implementation
      works with logic pixels, even if the reported pixel ratio is higher.

    - PaintData -> Paint for attributes and PaintContext for implementation object
*/

/** @brief Paint context.
*/
class PT_GFX_API PaintData
{
    friend class Canvas;
    friend class Painter;

    public:
        virtual ~PaintData();

        void begin(Painter& painter);

        void finish();

        const ImageFormat& format() const;

        const Scaling& scaling() const;

        const RectF& region() const;

        const PointF& origin() const;

        void setRegion(const RectF& r);

    protected:
        virtual void onSetPainter(Painter* painter) = 0;

        virtual void onBeginPaint(Painter& painter) = 0;

    public:
        void setCompositionMode(const Gfx::CompositionMode& mode);

        void setPen(const Pen& pen);

        void setBrush(const Brush& brush);

        void setFont(const Gfx::Font& font);

        void setClip(const RectF& clip);

        void resetClip();

    public:
        void drawLine(const PointF& from, const PointF& to);

        void drawRect(const Gfx::RectF& rectangle);

        void fillRect(const Gfx::RectF& rectangle);

        void drawPolyline(const Gfx::PointF* ps, const size_t n);

        void fillPolygon(const Gfx::PointF* ps, const size_t n);

        void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

    public:
        FontMetrics fontMetrics(const Pt::String& text) const;

        void drawText(const PointF& to, const Pt::String& text);

        void drawText(const PointF& to, const Pt::String& text, const Transform& t);

    public:
        void drawImage(const Gfx::PointF& to, 
                       const Gfx::Image& image);

        void drawImage(const Gfx::PointF& to, 
                       const Gfx::Image& image, 
                       const Gfx::RectF& imgRect);

        void drawSurface(const Gfx::PointF& to, 
                         const Gfx::PaintSurface& surface);

        void drawSurface(const Gfx::PointF& to,
                         const Gfx::PaintSurface& surface,
                         const Gfx::RectF& rect);

    protected:
        virtual void onSetCompositionMode(const Gfx::CompositionMode& mode) = 0;

        virtual void onSetPen(const Pen& pen) = 0;

        virtual void onSetBrush(const Brush& pen) = 0;

        virtual void onSetFont(const Gfx::Font& font) = 0;

        virtual void onResetClip() = 0;

        virtual void onSetClip(const RectF& clip) = 0;

    protected:
        virtual void onDrawLine(const Line& line) = 0;

        virtual void onDrawPolyline(const Polyline& line) = 0;

        virtual void onFillPolygon(const Polyline& line) = 0;

        virtual void onDrawRect(const Gfx::RectF& rectangle) = 0;

        virtual void onFillRect(const Gfx::RectF& rectangle) = 0;

        virtual void onDrawEllipse(const Gfx::PointF& topLeft, 
                                   const Gfx::SizeF& size) = 0;

        virtual void onFillEllipse(const Gfx::PointF& topLeft, 
                                   const Gfx::SizeF& size) = 0;
        
    protected:
        virtual FontMetrics onGetFontMetrics(const Pt::String& text) const = 0;

        virtual void onDrawText(const Gfx::PointF& to, const Pt::String& text) = 0;

        virtual void onDrawText(const Gfx::PointF& to, const Pt::String& text, 
                                const Gfx::Transform& trans) = 0;

    protected:
        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image) = 0;

        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image, 
                                 const Gfx::RectF& imgRect) = 0;

        virtual void onDrawSurface(const Gfx::PointF& to, 
                                   const Gfx::PaintSurface& surface) = 0;

        virtual void onDrawSurface(const Gfx::PointF& to,
                                   const Gfx::PaintSurface& surface,
                                   const Gfx::RectF& rect) = 0;

    protected:
        PaintData();

    private:
        void attachCanvas(Canvas& canvas);

        void detachCanvas(Canvas& canvas);

    private:
        void onDetachPainter(Painter& painter);

        //void attachPainter(Painter& painter);

        //void detachPainter(Painter& painter);

    private:
        Painter*       _painter;
        Canvas*        _canvas;
        RectF          _region;
        Gfx::Scaling   _scaling;
        bool           _invalid;
};

/** @brief Line.
*/
class Line
{
    public:
        Line(PaintData& paint,
             const Gfx::PointF& from, 
             const Gfx::PointF& to)
        : _paint(paint)
        , _from(from)
        , _to(to)
        { }

        Gfx::PointF from() const
        {
            return _from + _paint.origin();
        }

        Gfx::PointF to() const
        {
            return _to + _paint.origin();
        }

    private:
        PaintData&         _paint;
        const Gfx::PointF& _from;
        const Gfx::PointF& _to;
};

/** @brief Polyline.
*/
class Polyline
{
    public:
        Polyline(PaintData& paint,
                 const Gfx::PointF* points, 
                 std::size_t n)
        : _paint(paint)
        , _points(points)
        , _n(n)
        { }

        Gfx::PointF at(std::size_t n) const
        {
            return _points[n] + _paint.origin();
        }

        std::size_t size() const
        {
            return _n;
        }

    private:
        PaintData&         _paint;
        const Gfx::PointF* _points;
        std::size_t        _n;
};

} // namespace

} // namespace

#endif
