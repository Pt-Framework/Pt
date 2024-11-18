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

#ifndef PT_GFX_PAINTCONTEXT_H
#define PT_GFX_PAINTCONTEXT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Scaling.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/Gfx/Path.h>

#include <Pt/String.h>
#include <Pt/Types.h>

#include <cstddef>

namespace Pt {

namespace Gfx {

class Canvas;
class Polyline;

/** @brief Paint context.
*/
class PT_GFX_API PaintContext
{
    friend class Canvas;
    friend class PaintLayer;

    protected:
        PaintContext();

    public:
        virtual ~PaintContext();

        const PointF& origin() const;

        const RectF& region() const;

        void setRegion(const RectF& r);

        const Scaling& scaling() const;

        void beginPaint();

        void finishPaint();

        bool isActive() const;

        void reset();

    public:
        void setCompositionMode(const Gfx::CompositionMode& mode);

        void setPen(const Pen& pen);

        void setBrush(const Brush& brush);

        void setFont(const Gfx::Font& font);

        void setClip(const RectF& clip);

        void resetClip();

    public:
        void drawLine(const PointF& from, const PointF& to);

        void drawPolyline(const Gfx::PointF* ps, const size_t n);

        void fillPolygon(const Gfx::PointF* ps, const size_t n);

        void drawRect(const Gfx::RectF& rectangle);

        void fillRect(const Gfx::RectF& rectangle);

        void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

    public:
        FontMetrics fontMetrics(const Pt::String& text) const;

        void drawText(const PointF& to, const Pt::String& text, 
                      const Transform* t = 0);

    public:
        void drawImage(const Gfx::PointF& to, 
                       const Gfx::Image& image, 
                       const Gfx::RectF* rect = 0);
        
        bool drawLayer(const Gfx::PointF& to,
                       const Gfx::PaintLayer& layer,
                       const Gfx::RectF* rect = 0);

    protected:
        virtual void onSetCompositionMode(const Gfx::CompositionMode& mode) = 0;

        virtual void onSetPen(const Pen& pen) = 0;

        virtual void onSetBrush(const Brush& pen) = 0;

        virtual void onSetFont(const Gfx::Font& font) = 0;

        virtual void onSetClip(const Gfx::RectF* clip) = 0;

        virtual void onReleasePaint() {}

    private:
        void attachCanvas(Canvas& canvas);

        void detachCanvas(Canvas& canvas);

    private:
        Canvas*        _canvas;
        Canvas*        _active;
        RectF          _region;
        Gfx::Scaling   _scaling;
        RectF          _clip;
        bool           _hasClip;
};

/** @brief Polyline.
*/
class Polyline
{
    public:
        Polyline(PaintContext& paint,
                 const Gfx::PointF* points, 
                 std::size_t n)
        : _paint(paint)
        , _points(points)
        , _n(n)
        { }

        Gfx::PointF at(std::size_t n) const
        {
            Gfx::PointF p = _points[n] + _paint.origin();
            return _paint.scaling().toPhysical(p);
        }

        std::size_t size() const
        {
            return _n;
        }

    private:
        PaintContext&      _paint;
        const Gfx::PointF* _points;
        std::size_t        _n;
};

} // namespace

} // namespace

#endif
