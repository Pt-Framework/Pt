/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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

#ifndef Pt_Gfx_PaintSurface_h
#define Pt_Gfx_PaintSurface_h

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Scaling.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Paint.h>

#include <vector>

namespace Pt {

namespace Gfx {

class Canvas;
class Painter;
class PaintContext;
class PaintRegion;

/** @brief Paint surface.
*/
class PT_GFX_API PaintSurface
{
    friend class Painter;
    friend class PaintRegion;

    protected:
        PaintSurface();

    public:
        virtual ~PaintSurface();
        
        const PaintInfo& info() const;

        const Scaling& scaling() const;

        const ImageFormat& format() const;

        PaintContext* getPaint(PaintContext* context);

        Canvas* canvas();

        const Canvas* canvas() const;

    protected:
        void setCanvas(Canvas* canvas);

    protected:      
        virtual const PaintInfo& onGetPaintInfo() const = 0;

        virtual PaintContext* onGetPaint(PaintContext* context);

        virtual void onReset();

    private:
        void attachRegion(PaintRegion& region);
        
        void detachRegion(PaintRegion& region);

    private:
        void attachPainter(Painter& painter);

        void detachPainter(Painter& painter);

    private:
        Canvas*                    _canvas;
        std::vector<PaintRegion*>  _regions;
        Painter*                   _painter;
};

/** @brief Paint layer surface.
*/
class PT_GFX_API PaintLayer : public PaintSurface
{
    public:
        virtual ~PaintLayer()
        {}

        Image toImage() const
        {
            return onGetImage();
        }

        void draw(PaintContext& paint,
                  const Gfx::PointF& to) const
        {
            onDraw(paint, to);
        }
        
        void draw(PaintContext& paint,
                  const Gfx::PointF& to, 
                  const Gfx::RectF& rect) const
        {
            onDraw(paint, to, rect);
        }

    protected:
        PaintLayer()
        { }

        virtual Image onGetImage() const = 0;

        virtual void onDraw(PaintContext& paint,
                            const Gfx::PointF& to) const;
        
        virtual void onDraw(PaintContext& paint,
                            const Gfx::PointF& to, 
                            const Gfx::RectF& rect) const;
};

} // namespace

} // namespace

#endif
