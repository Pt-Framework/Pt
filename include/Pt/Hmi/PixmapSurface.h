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

#ifndef Pt_Hmi_PixmapSurface_h
#define Pt_Hmi_PixmapSurface_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Hmi/PaintSurface.h>

namespace Pt {

namespace Hmi {

class Picture;
class PixmapSurfaceImpl;

/** @brief A back buffer drawing surface.
*/
class PT_HMI_API PixmapSurface : public PaintSurface
{
    public:
        PixmapSurface();

        virtual ~PixmapSurface();

        void resize(const Gfx::SizeF& size);

        void clear( const Gfx::Color& color = Gfx::Color( 1, 1, 1 ) );

        PixmapSurfaceImpl* pixmapImpl() const;

        void setScaleFactor(double v)
        {
            _scaleFactor = v;
        }

    protected:
        virtual const Gfx::ImageFormat& format() const;

        virtual void setCompositionMode(const Gfx::CompositionMode& mode);

        virtual void setClip( const Gfx::RectF& clip);

        virtual void resetClip();

        virtual void setPen(const Gfx::Pen& pen);

        virtual void setBrush(const Gfx::Brush& brush);

        virtual void setFont(const Gfx::Font& font);

        virtual Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

        virtual void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

        virtual void drawText(const Gfx::PointF& to, const Pt::String& Text);

        virtual void drawText(const Gfx::PointF& to, const Pt::String& Text, const Gfx::Transform& trans);

        virtual void drawRect(const Gfx::RectF& rectangle);

        virtual void fillRect(const Gfx::RectF& rectangle);

        virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void drawPolyline(const Gfx::PointF* points, size_t pointCount);

        virtual void fillPolygon(const Gfx::PointF* points, size_t pointCount);

        virtual void drawPath(const Gfx::Path& path, float smoothness);

        virtual void drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface);

        virtual void drawSurface(const Gfx::PointF& toF, const PixmapSurface& pm, const Gfx::RectF& pmRect);

        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& imgRect);

        virtual void drawPicture(const Gfx::PointF& to, const Picture& pic);

    protected:
        virtual const Gfx::SizeF& onSize() const;

        virtual double onScaleFactor() const
        {
            return _scaleFactor;
        }

        virtual void onBegin(Painter& painter);

        virtual void onFinish();

    private:
        PixmapSurfaceImpl* _impl;
        Gfx::SizeF _logicSize;
        double _scaleFactor;
};

} // namespace

} // namespace

#endif
