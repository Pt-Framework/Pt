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
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/String.h>
#include <Pt/Types.h>
#include <cstddef>

namespace Pt {

namespace Gfx {

/** @brief 2D painter interface.
  */
class PT_GFX_API Painter
{
    public:
        //! @brief Destructor.
        virtual ~Painter()
        {}

        /** @brief Returns the painters native image format.
        */
        virtual const ImageFormat& format() const  = 0;

        /** @brief Sets the composition mode.
        */
        virtual void setCompositionMode(const CompositionMode& mode) = 0;

        /** @brief Returns the current composition mode.
        */
        virtual const CompositionMode& compositionMode() const = 0;

        /** @brief Sets the clipping rect.
        */
        virtual void setClip(const RectF& clip) = 0;

        /** @brief Resets the clipping rect.
        */
        virtual void resetClip() = 0;

        /** @brief Sets the pen used to stroke lines.
        */
        virtual void setPen(const Pen& pen) = 0;

        /** @brief Returns the current pen.
        */
        virtual const Pen& pen() const = 0;

        /** @brief Sets the brush used to fill areas.
        */
        virtual void setBrush(const Brush& brush) = 0;

        /** @brief Returns the current brush.
        */
        virtual const Brush& brush() const = 0;

        /** @brief Sets the font used to draw text.
        */
        virtual void setFont(const Font& font) = 0;

        /** @brief Returns the current font.
        */
        virtual const Font& font() const = 0;

        /** @brief Measures the metrics of a text block.
        */
        virtual FontMetrics fontMetrics(const Pt::String& text) const = 0;

        /** @brief Draws a line between two points.
        */
        virtual void drawLine(const PointF& from, const PointF& to) = 0;

        /** @brief Draws a polyline.
        */
        virtual void drawPolyline(const PointF* points, const size_t pointCount) = 0;

        /** @brief Fills a polygon.
        */
        virtual void fillPolygon(const PointF* points, const size_t pointCount) = 0;

        /** @brief Draws a text block.
        */
        virtual void drawText(const PointF& to, const Pt::String& text) = 0;

        virtual void drawText(const PointF& to, const Pt::String& text, const Transform& t) = 0;

        /** @brief Draws the outline of a rectangle.
        */
        virtual void drawRect(const RectF& rect) = 0;

        /** @brief Fills a rectangular area.
        */
        virtual void fillRect(const RectF& rect) = 0;

        /** @brief Draws the outline of a circle.
          */
        void drawCircle(const PointF& topLeft, double diameter)
        {
            drawEllipse(topLeft, SizeF(diameter, diameter));
        }

        /** @brief Fills a circular area.
        */
        inline void fillCircle(const PointF& topLeft, double diameter)
        {
            fillEllipse(topLeft, SizeF(diameter, diameter));
        }

        /** @brief Draws the outline of an ellipse.
        */
        virtual void drawEllipse(const PointF& topLeft, const SizeF& size) = 0;

        /** @brief Fills an elliptical area.
        */
        virtual void fillEllipse(const PointF& topLeft, const SizeF& size) = 0;

        /** @brief Draws an image.
        */
        virtual void drawImage(const PointF& to, const Image& im) = 0;

        /** @brief Draws a part of an image.
        */
        virtual void drawImage(const PointF& to, const Image& im, const RectF& rect) = 0;
};

} // namespace

} // namespace

#endif
