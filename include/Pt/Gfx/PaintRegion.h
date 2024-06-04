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
class PT_GFX_API PaintRegion : public PaintSurface
{
    public:
        PaintRegion();

        PaintRegion(PaintSurface& surface, const Gfx::RectF& rect);

        virtual ~PaintRegion();

        void attach(PaintSurface& surface, const Gfx::RectF& rect);

        void detach();

        PaintSurface* surface() const;

        const Gfx::RectF& area() const;

        void move(const Gfx::PointF& size);

        void resize(const Gfx::SizeF& size);

        virtual Image toImage() const;

    protected:
        virtual const Gfx::ImageFormat& onGetFormat() const;

        virtual const Gfx::SizeF& onSize() const;

        virtual const Gfx::Scaling& onGetScaling() const;

        virtual double onScaleFactor() const;

        virtual PaintData* onGetPaint(PaintData*);

        virtual Canvas* onGetCanvas();

        virtual RectF onGetRegion() const;

        virtual void onDestroy(PaintSurface* region);

        virtual void onReset();       

    private:
        PaintSurface* _surface;
        Gfx::RectF    _area;
};

} // namespace

} // namespace

#endif
