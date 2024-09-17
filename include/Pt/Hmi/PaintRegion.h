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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 02110-1301 USA
*/

#ifndef Pt_Hmi_PaintRegion_h
#define Pt_Hmi_PaintRegion_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Gfx/PaintRegion.h>

namespace Pt {

namespace Hmi {

class PixmapSurface;

/** @brief Drawing region on another surface.
*/
class PT_HMI_API PaintRegion : public Hmi::PaintSurface
{
    public:
        PaintRegion();

        PaintRegion(Hmi::PaintSurface& surface, const Gfx::RectF& rect);

        virtual ~PaintRegion();

        void attach(Hmi::PaintSurface& surface, const Gfx::RectF& rect);

        void detach();

        Hmi::PaintSurface* surface() const;

        const Gfx::PointF& position() const;

        void move(const Gfx::PointF& pos);

        void resize(const Gfx::SizeF& size);

    protected:
        virtual const Gfx::PaintInfo& onGetPaintInfo() const override;

        virtual Gfx::PaintContext* onGetPaint(Gfx::PaintContext* context) override;

    protected:
        virtual void onDraw(Gfx::PaintContext& paint,
                            const Gfx::PointF& to) const override;
        
        virtual void onDraw(Gfx::PaintContext& paint,
                            const Gfx::PointF& to, 
                            const Gfx::RectF& rect) const override;

    protected:
        virtual void onDrawPixmap(const Gfx::PointF& to, 
                                  const PixmapSurface& pixmap,
                                  const Gfx::CompositionMode& mode) override;

        virtual void onDrawPixmap(const Gfx::PointF& to,
                                  const PixmapSurface& pixmap, 
                                  const Gfx::RectF& rect,
                                  const Gfx::CompositionMode& mode) override;

    private:
        Gfx::PaintRegion _region;
};

} // namespace

} // namespace

#endif
