/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2010-2017 Aloysius Indrayanto

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

#ifndef PT_GFX_PAINTER_H
#define PT_GFX_PAINTER_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/TextMetrics.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Scaling.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/Gfx/Path.h>
#include <Pt/Gfx/Paint.h>

#include <Pt/Gfx/FontMetrics.h>

#include <Pt/String.h>
#include <Pt/Types.h>

#include <cstddef>

namespace Pt {

namespace Gfx {

class PaintContext;
class PaintSurface;
class PaintLayer;

/** @brief 2D painter interface.
*/
class PT_GFX_API Painter
{
    friend class PaintSurface;

    public:
        /** @brief @brief Default constructor.
        */
        Painter();

        /** @brief @brief Constructs using a paint surface.
        */
        explicit Painter(PaintSurface& surface);
        
        /** @brief @brief Constructs using a paint layer.
        */
        explicit Painter(PaintLayer& layer);

        /** @brief @brief Destructor.
        */
        virtual ~Painter();

        /** @brief @brief Begins painting to a paint surface.
        */
        void begin(PaintSurface& surface);
        
        /** @brief @brief Begins painting to a paint layer.
        */
        void begin(PaintLayer& layer);

        /** @brief @brief Ends painting.
        */
        void finish();

        /** @brief Returns the image format.
        */
        const ImageFormat& format() const;

        /** @brief Returns the paint scaling.
        */
        const Scaling& scaling() const;

    public:
        /** @brief Returns the paint attributes.
        */
        const Paint& paint() const;

        /** @brief Returns the current composition mode.
        */
        const CompositionMode& compositionMode() const;

        /** @brief Sets the composition mode.
        */
        void setCompositionMode(const CompositionMode& mode);

        /** @brief Returns the current pen.
        */
        const Pen& pen() const;

        /** @brief Sets the pen used to stroke lines.
        */
        void setPen(const Pen& pen);

        /** @brief Returns the current brush.
        */
        const Brush& brush() const;

        /** @brief Sets the brush used to fill areas.
        */
        void setBrush(const Brush& brush);

        /** @brief Returns the current font.
        */
        const Font& font() const;

        /** @brief Sets the font used to draw text.
        */
        void setFont(const Font& font);

        /** @brief Returns the clipping rect or null if none.
        */
        const RectF* clip() const;

        /** @brief Sets the clipping rect.
        */
        void setClip(const RectF& clip);

        /** @brief Resets the clipping rect.
        */
        void resetClip();

    public:
        /** @brief Draws a line between two points.
        */
        void drawLine(const PointF& from, const PointF& to);

        /** @brief Draws a polyline.
        */
        void drawPolyline(const PointF* points, const size_t pointCount);

        /** @brief Fills a polygon.
        */
        void fillPolygon(const PointF* points, const size_t pointCount);

        /** @brief Draws the outline of a rectangle.
        */
        void drawRect(const RectF& rect);

        /** @brief Fills a rectangular area.
        */
        void fillRect(const RectF& rect);

        /** @brief Draws the outline of a circle.
          */
        void drawCircle(const PointF& topLeft, double diameter);

        /** @brief Fills a circular area.
        */
        void fillCircle(const PointF& topLeft, double diameter);

        /** @brief Draws the outline of an ellipse.
        */
        void drawEllipse(const PointF& topLeft, const SizeF& size);

        /** @brief Fills an elliptical area.
        */
        void fillEllipse(const PointF& topLeft, const SizeF& size);

        /** @internal TODO.
        */
        void drawArc(const PointF& topLeft, const SizeF& size,
                     float degBegin, float degEnd);
        
        /** @internal TODO.
        */
        void fillChord(const PointF& topLeft, const SizeF& size,
                       float degBegin, float degEnd);
        
        /** @internal TODO.
        */
        void fillPie(const PointF& topLeft, const SizeF& size,
                     float degBegin, float degEnd);

    public:
        /** @brief Returns the current path.
        */
        const Gfx::Path& path() const;

        void beginPath();

        void moveTo(const PointF& to);

        void lineTo(const PointF& to);

        void curveTo(const PointF& cp, const PointF& to);

        void curveTo(const PointF& cp1, const PointF& cp2, const PointF& to);

        void closePath();

        /** @brief Sets the current path.
        */
        void setPath(const Path& path);

        /** @brief Draws the current path.
        */
        void drawPath();
        
        /** @brief Fills the current path.
        */
        void fillPath();

    public:
        /** @brief Returns the metrics of a line of text.
        */
        TextMetrics textMetrics(const Pt::String& text) const;

        /** @internal TODO.
        */
        FontMetrics fontMetrics(const Pt::String& text) const
        {
            return textMetrics(text);
        }

        /** @brief Draws a line of text.
        */
        void drawText(const PointF& to, const Pt::String& text);

        /** @brief Draws a line of text.
        */
        void drawText(const PointF& to, const Pt::String& text, const Transform& t);

    public:
        /** @brief Draws an image.
        */
        void drawImage(const PointF& to, const Image& im);

        /** @brief Draws a part of an image.
        */
        void drawImage(const PointF& to, const Image& im, const RectF& rect);

        /** @brief Draws a layer.
        */
        void drawLayer(const Gfx::PointF& to, const PaintLayer& layer);

        /** @brief Draws a part of a layer.
        */
        void drawLayer(const Gfx::PointF& to, const PaintLayer& layer, 
                       const Gfx::RectF& layerRect);

    private:
        void onDetachSurface(PaintSurface& surface);

    private:
        PaintSurface*        _surface;
        PaintContext*        _paintContext;
        Scaling              _scaling;
        Paint                _paint;
        Path                 _path;
};

} // namespace

} // namespace

#endif
