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
#include <Pt/Gfx/Path.h>
#include <Pt/String.h>
#include <Pt/Types.h>
#include <cstddef>

namespace Pt {

namespace Gfx {

class PaintSurface;

/** @brief Paint attributes.
*/
class PaintData
{
    public:
        virtual ~PaintData()
        {
        }

    protected:
        PaintData()
        {
        }
};

/** @brief 2D painter interface.
*/
class PT_GFX_API Painter
{
    friend class PaintSurface;

    public:
        Painter();

        Painter(PaintSurface& surface);

        //! @brief Destructor.
        virtual ~Painter();

        void begin(PaintSurface& surface);

        void finish();

        PaintData* paintData()
        {
            return _paintData;
        }

        void setPaintData(PaintData* data)
        {
            if(_paintData)
            {
                delete _paintData;
                _paintData = 0;
            }
            
            _paintData = data;
        }

        /** @brief Returns the painters native image format.
        */
        const ImageFormat& format() const;

        /** @brief Sets the composition mode.
        */
        void setCompositionMode(const CompositionMode& mode);

        /** @brief Returns the current composition mode.
        */
        const CompositionMode& compositionMode() const;

        /** @brief Returns the clipping rect.
        */
        const RectF& clip() const
        {
            return _clip;
        }

        /** @brief Sets the clipping rect.
        */
        void setClip(const RectF& clip);

        /** @brief Resets the clipping rect.
        */
        void resetClip();

        /** @brief Sets the pen used to stroke lines.
        */
        void setPen(const Pen& pen);

        /** @brief Returns the current pen.
        */
        const Pen& pen() const;

        /** @brief Sets the brush used to fill areas.
        */
        void setBrush(const Brush& brush);

        /** @brief Returns the current brush.
        */
        const Brush& brush() const;

        /** @brief Sets the font used to draw text.
        */
        void setFont(const Font& font);

        /** @brief Returns the current font.
        */
        const Font& font() const;

        /** @brief Measures the metrics of a text block.
        */
        FontMetrics fontMetrics(const Pt::String& text) const;

        /** @brief Draws a line between two points.
        */
        void drawLine(const PointF& from, const PointF& to);

        /** @brief Draws a polyline.
        */
        void drawPolyline(const PointF* points, const size_t pointCount);

        /** @brief Fills a polygon.
        */
        void fillPolygon(const PointF* points, const size_t pointCount);

        /** @brief Draws a text block.
        */
        void drawText(const PointF& to, const Pt::String& text);

        void drawText(const PointF& to, const Pt::String& text, const Transform& t);

        /** @brief Draws the outline of a rectangle.
        */
        void drawRect(const RectF& rect);

        /** @brief Fills a rectangular area.
        */
        void fillRect(const RectF& rect);

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
        void drawEllipse(const PointF& topLeft, const SizeF& size);

        /** @brief Fills an elliptical area.
        */
        void fillEllipse(const PointF& topLeft, const SizeF& size);

        void drawPath(const Path& path, float smoothness = 1.0f);
        
        void fillPath(const Path& path, float smoothness = 1.0f);
        /** @brief Draws an image.
        */
        void drawImage(const PointF& to, const Image& im);

        /** @brief Draws a part of an image.
        */
        void drawImage(const PointF& to, const Image& im, const RectF& rect);

        void drawArc(const PointF& topLeft, const SizeF& size,
                     float degBegin, float degEnd);

        void drawChord(const PointF& topLeft, const SizeF& size,
                       float degBegin, float degEnd);

        void drawPie(const PointF& topLeft, const SizeF& size,
                     float degBegin, float degEnd);

        void fillPie(const PointF& topLeft, const SizeF& size,
                     float degBegin, float degEnd);

        void fillChord(const PointF& topLeft, const SizeF& size,
                       float degBegin, float degEnd);

    //private:
    public:
        void drawSurface(const Gfx::PointF& toF, 
                         const PaintSurface& surface);

        void drawSurface(const Gfx::PointF& toF, 
                         const PaintSurface& pm, const Gfx::RectF& pmRect);

    public:
        double scaleFactor() const;

        double toPhysical(double n) const;

        Gfx::PointF toPhysical(const Gfx::PointF& p) const;

        Gfx::SizeF toPhysical(const Gfx::SizeF& s) const;

        Gfx::RectF toPhysical(const Gfx::RectF& r) const;

        double toLogical(double n) const;

        Gfx::PointF toLogical(const Gfx::PointF& p) const;

        Gfx::SizeF toLogical(const Gfx::SizeF& s) const;

        Gfx::RectF toLogical(const Gfx::RectF& r) const;

        double align(double n) const;

        double alignPixel(double n) const;

        double alignContour(size_t n) const;

        Gfx::PointF align(const Gfx::PointF& p) const;

        Gfx::SizeF align(const Gfx::SizeF& s) const;

        Gfx::RectF align(const Gfx::RectF& rect) const;

    protected:
        virtual void onFinish();

    private:
        PaintSurface*        _surface;
        PaintData*           _paintData;
        Gfx::Pen             _pen;
        Gfx::Brush           _brush;
        Gfx::Font            _font;
        Gfx::CompositionMode _compositionMode;
        Gfx::RectF           _clip;
};

} // namespace

} // namespace

#endif
