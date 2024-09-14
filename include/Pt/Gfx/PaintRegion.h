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
#include <Pt/Gfx/PaintSurface.h>

namespace Pt {

namespace Gfx {

class PaintRegion;

/** @internal
*/
class PaintRegionInfo : public PaintInfo
{
    public:
        PaintRegionInfo();

        virtual ~PaintRegionInfo();

        void reset();

        void reset(PaintSurface& surface, const Gfx::RectF* rect = 0);

        const Gfx::PointF& position() const;

        void setPosition(const Gfx::PointF& pos);

        const Gfx::SizeF& size() const;

        void setSize(const Gfx::SizeF& size);

        const RectF* area() const;

    protected:
        virtual const Gfx::ImageFormat& onGetFormat() const;

        virtual const Gfx::SizeF& onGetSize() const;

        virtual const Scaling& onGetScaling() const;

    private:
        PaintSurface* _surface;
        Gfx::Scaling  _scaling;
        Gfx::RectF    _area;
        bool          _hasArea;
};

/** @brief Drawing region on another surface.
*/
class PT_GFX_API PaintRegion : public PaintSurface
{
    friend class PaintSurface;

    public:
        PaintRegion();

        PaintRegion(PaintSurface& surface, const Gfx::RectF& rect);

        virtual ~PaintRegion();

        void attach(PaintSurface& surface);

        void attach(PaintSurface& surface, const Gfx::RectF& rect);

        void detach();

        PaintSurface* surface() const;

        const PointF& position() const;

        void move(const Gfx::PointF& pos);

        void resize(const Gfx::SizeF& size);

    protected:
        virtual const PaintInfo& onGetPaintInfo() const override;

        virtual PaintContext* onGetPaint(PaintContext* context) override;

        virtual void onReset() override;

    protected:
        virtual void onDraw(PaintContext& paint,
                            const Gfx::PointF& to) const override;
        
        virtual void onDraw(PaintContext& paint,
                            const Gfx::PointF& to, 
                            const Gfx::RectF& rect) const override;

    private:
        virtual void onDetachSurface(PaintSurface* region);
    
    private:
        PaintRegionInfo _info;
        PaintSurface*   _surface;
};

} // namespace

} // namespace

#endif
