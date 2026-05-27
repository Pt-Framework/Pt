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

#ifndef PT_GFX_CANVAS_H
#define PT_GFX_CANVAS_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Scaling.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/TextMetrics.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/Gfx/Path.h>

#include <Pt/String.h>
#include <Pt/Types.h>

#include <cstddef>

namespace Pt {

namespace Gfx {

class Bitmap;

/** @brief Backend canvas for drawing commands.
    @ingroup Pt-Gfx-Drawing

    %Canvas is implemented by paint backends and executes the drawing commands
    emitted by %PainterBase. It manages the active drawing state for a target,
    exposes target geometry and scaling, and translates high-level operations
    such as path, text and bitmap drawing into backend-specific primitives.
*/
class PT_GFX_API Canvas
{
    friend class PaintSurface;

    protected:
        Canvas();

        enum DirtyFlags : unsigned
        {
            DirtyTransform   = 0x01,
            DirtyComposition = 0x02,
            DirtyPen         = 0x04,
            DirtyBrush       = 0x08,
            DirtyFont        = 0x10,
            DirtyClip        = 0x20,
            DirtyAll         = 0x3F
        };

    public:
        /** @brief Destroys the canvas.
        */
        virtual ~Canvas();

        /** @brief Returns the logical origin of the current region.
        */
        const PointF& origin() const;

        /** @brief Returns the current paint region.
        */
        const RectF& region() const;

        /** @brief Sets the paint region.
        */
        void setRegion(const RectF& r);

        /** @brief Returns the logical-to-physical scaling.
        */
        const Scaling& scaling() const;

        /** @brief Sets the logical-to-physical scaling.
        */
        void setScaling(const Scaling& scaling);

        /** @brief Returns the pixel format of the target.
        */
        const Gfx::ImageFormat& format() const;

        /** @brief Returns the current transform.
        */
        const Gfx::Transform& transform() const;

        /** @brief Sets the current transform.
        */
        void setTransform(const Gfx::Transform& tx);

        /** @brief Resets the transform to the identity matrix.
        */
        void resetTransform();

    public:
        /** @brief Begins painting with the given paint state.
        */
        void beginPaint(const Gfx::Paint& paint);

        /** @brief Ends the current painting operation.
        */
        void finishPaint();

        /** @brief Returns true if painting is currently active.
        */
        bool isActive() const;

    public:
        /** @brief Sets the current composition mode.
        */
        void setCompositionMode(const Gfx::CompositionMode& mode);

        /** @brief Sets the current pen.
        */
        void setPen(const Pen& pen);

        /** @brief Sets the current brush.
        */
        void setBrush(const Brush& brush);

        /** @brief Sets the current font.
        */
        void setFont(const Gfx::Font& font);

        /** @brief Sets the clip rectangle.
        */
        void setClip(const RectF& clip);

        /** @brief Clears the clip rectangle.
        */
        void resetClip();

    public:
        /** @brief Draws a line between two points.
        */
        void drawLine(const PointF& from, const PointF& to);

        /** @brief Draws a connected sequence of line segments.
        */
        void drawPolyline(const Gfx::PointF* ps, const size_t n);

        /** @brief Fills a polygon.
        */
        void fillPolygon(const Gfx::PointF* ps, const size_t n);

        /** @brief Draws the outline of a rectangle.
        */
        void drawRect(const Gfx::RectF& rectangle);

        /** @brief Fills a rectangle.
        */
        void fillRect(const Gfx::RectF& rectangle);

        /** @brief Draws the outline of an ellipse.
        */
        void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        /** @brief Fills an ellipse.
        */
        void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

    public:
        /** @brief Sets the current path.
        */
        void setPath(const Path& path);

        /** @brief Draws the current path.
        */
        void drawPath();

        /** @brief Fills the current path.
        */
        void fillPath();

        /** @brief Draws the given path.
        */
        void drawPath(const Path& path);

        /** @brief Fills the given path.
        */
        void fillPath(const Path& path);

    public:
        /** @brief Returns metrics for the current font.
        */
        const FontMetrics& fontMetrics() const;

        /** @brief Measures a line of text.
        */
        TextMetrics textMetrics(const Pt::String& text) const;

        /** @brief Draws a line of text.
        */
        void drawText(const PointF& to, const Pt::String& text, 
                      const Transform* tform = 0);

    public:
        /** @brief Draws an image.
        */
        void drawImage(const Gfx::PointF& to, 
                       const Gfx::Image& image, 
                       const Gfx::RectF* rect = 0);

        /** @brief Draws a bitmap.
        */
        void drawBitmap(const Gfx::PointF& to, 
                        const Gfx::Bitmap& bitmap, 
                        const Gfx::RectF* rect = 0);

    protected:
        virtual void onBeginPaint(const Gfx::Paint& paint) = 0;
       
        virtual void onFinishPaint() = 0;

    protected:
        virtual void onSetTransform(const Gfx::Transform& tx) = 0;

        virtual void onApplyTransform() = 0;

        virtual void onSetCompositionMode(const Gfx::CompositionMode& mode) = 0;

        virtual void onApplyCompositionMode() = 0;

        virtual void onSetPen(const Pen& pen) = 0;

        virtual void onApplyPen() = 0;

        virtual void onSetBrush(const Brush& pen) = 0;

        virtual void onApplyBrush() = 0;

        virtual void onSetFont(const Gfx::Font& font) = 0;

        virtual void onApplyFont() = 0;

        virtual void onSetClip(const Gfx::RectF* clip) = 0;

        virtual void onApplyClip() = 0;

    protected:
        void invalidate(unsigned flags);

    protected:
        virtual void onDrawLine(const PointF& from, const PointF& to) = 0;

        virtual void onDrawPolyline(const Gfx::PointF* pts, const size_t n) = 0;

        virtual void onFillPolygon(const Gfx::PointF* ps, const size_t n) = 0;

        virtual void onDrawRect(const Gfx::RectF& rectangle) = 0;

        virtual void onFillRect(const Gfx::RectF& rectangle) = 0;

        virtual void onDrawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size) = 0;

        virtual void onFillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size) = 0;

    protected:
        virtual void onSetPath(const Path& path) = 0;

        virtual void onDrawPath() = 0;

        virtual void onFillPath() = 0;

        virtual void onDrawPath(const Path& path) = 0;

        virtual void onFillPath(const Path& path) = 0;

    protected:
        virtual const Gfx::FontMetrics& onGetFontMetrics() const = 0;

        virtual Gfx::TextMetrics onGetTextMetrics(const Pt::String& text) const = 0;

        virtual void onDrawText(const Gfx::PointF& to, 
                                const Pt::String& text, 
                                const Gfx::Transform* transform) = 0;

    protected:
        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image, 
                                 const Gfx::RectF* rect = 0) = 0;

    protected:
        void prepare();

    private:
        void attachSurface(PaintSurface& surface);

        void detachSurface(PaintSurface& surface);

        void updateTransform();

    private:
        PaintSurface*  _surface;
        PaintSurface*  _active;
        RectF          _region;
        Gfx::Scaling   _scaling;
        Transform      _viewTx;
        Transform      _userTx;
        Transform      _tx;
        unsigned       _dirty;
};

} // namespace

} // namespace

#endif
