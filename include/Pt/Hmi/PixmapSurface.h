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
#include <Pt/Hmi/PaintSurface.h>

#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
//#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Image.h>

#include <Pt/System/Path.h>

namespace Pt {

namespace Hmi {

class PixmapSurfaceImpl;

/** @brief Back buffer drawing surface.
*/
class PT_HMI_API PixmapSurface : public Hmi::PaintSurface
{
    public:
        PixmapSurface();

        virtual ~PixmapSurface();

        void set(const Gfx::Image& image);

        bool empty() const;

        /** @brief Returns the size in device pixels. 
        */
        const Gfx::SizeF& size() const;

        /** @brief Resizes to a size in device pixels. 
        */
        void resize(const Gfx::SizeF& size);

        void setScaleFactor(double v);

        void clear( const Gfx::Color& color = Gfx::Color( 1, 1, 1 ) );

    protected:
        virtual void onDrawPixmap(const Gfx::PointF& to, 
                                  const PixmapSurface& pixmap,
                                  const Gfx::CompositionMode& mode) override;;

        virtual void onDrawPixmap(const Gfx::PointF& to,
                                  const PixmapSurface& pixmap, 
                                  const Gfx::RectF& rect,
                                  const Gfx::CompositionMode& mode) override;;

    protected:
        virtual const Gfx::PaintInfo& onGetPaintInfo() const override;

        virtual Gfx::PaintContext* onGetPaint(Gfx::PaintContext* context) override;

    public:
        static void setFontDir(const System::Path& path);

        static std::string defaultFont();

        static void setDefaultFont(const std::string& name);

        static std::vector<std::string> fontNames();

    public:
        PixmapSurfaceImpl* impl()
        {
            return _impl;
        }

        const PixmapSurfaceImpl* impl() const
        {
            return _impl;
        }
    
    private:
        PixmapSurfaceImpl* _impl;
};

} // namespace

} // namespace

#endif
