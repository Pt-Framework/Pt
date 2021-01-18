/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 
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
 MA  02110-1301  USA
*/

#ifndef Pt_Gfx_PaintRegion_h
#define Pt_Gfx_PaintRegion_h

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/PaintSurface.h>

namespace Pt {
namespace Gfx {

/** @brief Drawing region on another surface.
*/
class PT_GFX_API PaintRegion : public virtual PaintSurface
{
    public:
        PaintRegion();

        PaintRegion(PaintSurface& surface, const Gfx::RectF& rect);

        virtual ~PaintRegion();

        void attach(PaintSurface& surface, const Gfx::RectF& rect);

        void detach();

        void reset(const Gfx::RectF& rect);

        virtual Image toImage(const Gfx::ImageFormat& format) const;

    protected:
        virtual const Gfx::ImageFormat& format() const;

        virtual void setClip( const Gfx::RectF& clip);

        virtual void resetClip();

        virtual void setCompositionMode(const Gfx::CompositionMode& mode);

        virtual void setPen(const Gfx::Pen& pen);

        virtual void setBrush(const Gfx::Brush& brush);

        virtual void setFont(const Gfx::Font& font);

        virtual Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

        virtual void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

        virtual void drawText(const Gfx::PointF& to, const Pt::String& text);

        virtual void drawText(const Gfx::PointF& to, const Pt::String& text, const Gfx::Transform& trans);

        virtual void drawRect(const Gfx::RectF& rectangle);

        virtual void fillRect(const Gfx::RectF& rectangle);

        virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void drawPolyline(const Gfx::PointF* points, size_t pointCount);

        virtual void fillPolygon(const Gfx::PointF* points, size_t pointCount);

        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& imgRect);

        virtual void drawPath(const Gfx::Path& path, float smoothness);

        virtual void fillPath(const Path& path, float smoothness);

        virtual void drawChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

        virtual void fillChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

        virtual void drawPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

        virtual void fillPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

        virtual void drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

        virtual void drawSurface(const Gfx::PointF& toF, const PaintSurface& surface);

        virtual void drawSurface(const Gfx::PointF& toF, const PaintSurface& pm, const Gfx::RectF& pmRect);

    protected:
        virtual const Gfx::SizeF& onSize() const;

        virtual double onScaleFactor() const
        {
            return _surface->onScaleFactor();
        }

        virtual void onBegin(Painter& painter);

        virtual void onFinish();

    public:
        const Gfx::RectF& area() const
        {
            return _area;
        }

    private:
        PaintSurface* _surface;
        Gfx::RectF _area;
};

} // namespace

} // namespace

#endif
