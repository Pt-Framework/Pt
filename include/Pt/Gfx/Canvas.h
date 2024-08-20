/* Copyright (C) 2020 Marc Boris Duerner

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

#ifndef Pt_Gfx_Canvas_h
#define Pt_Gfx_Canvas_h

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Scaling.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/Gfx/Path.h>

namespace Pt {

namespace Gfx {

class PaintSurface;
class PaintData;
class Line;
class Polyline;

/** @brief Paint canvas.
*/
class PT_GFX_API Canvas
{
    friend class PaintData;

    public:
        explicit Canvas(PaintSurface& surface);

        ~Canvas();

        const Gfx::ImageFormat& format() const;

        const Gfx::SizeF& size() const;

        const Scaling& scaling() const;

        PaintData* getPaint(PaintData* paint);

        void finishPaint();

    protected:
        //virtual const Gfx::ImageFormat& onGetFormat() const = 0;

        virtual const Gfx::SizeF& onGetSize() const = 0;

        virtual const Scaling& onGetScaling() const = 0;

        virtual PaintData* onGetPaint(PaintData* paint) = 0;

        virtual void onFinish() = 0;

    private:
#ifndef PT_HMI_CANVAS_PAINT
        void onDetachPaint(PaintData& paint);
#else
        void attachPaint(PaintData& paint);

        void detachPaint(PaintData& paint);
#endif
    private:
        PaintSurface* _surface;
        PaintData*    _paint;
};

} // namespace

} // namespace

#endif
