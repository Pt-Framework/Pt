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
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Scaling.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/Path.h>

#include <vector>

namespace Pt {

namespace Gfx {

class PaintContext;

/** @brief Abstract target for drawing operations.
    @ingroup Pt-Gfx-Drawing

    %PaintSurface supplies the backend-specific resources needed for painting.
    It reports target format, size and scaling, creates a %Canvas on demand and
    coordinates %PainterBase and %PaintContext while drawing is active.
*/
class PT_GFX_API PaintSurface : private NonCopyable
{
    friend class PainterBase;
    friend class PaintContext;
    friend class Canvas;

    protected:
        /** @brief Default constructor.
        */
        PaintSurface();

    public:
        /** @brief Destructor.
        */
        virtual ~PaintSurface();
        
        /** @brief Returns the image format.
        */
        const Gfx::ImageFormat& format() const;

        /** @brief Returns the size in physical pixels.
        */
        const Gfx::SizeF& size() const;

        /** @brief Returns the scaling from logical to physical pixels.
        */
        const Scaling& scaling() const;

        /** @brief Returns a canvas for backend drawing.
        */
        Canvas* getCanvas(Canvas* canvas);

        /** @brief Synchronizes pending operations to the surface.
        */
        void sync();

        /** @brief Finishes painting to the surface.
        */
        void finish();

    protected:
        /** @brief Invalidates the currently active paint canvas.
        */
        void invalidate();

    protected:
        /** @brief Returns the image format.
        */
        virtual const Gfx::ImageFormat& onGetFormat() const = 0;

        /** @brief Returns the size in physical pixel.
        */
        virtual const Gfx::SizeF& onGetSize() const = 0;

        /** @brief Returns the scaling fro logical to physical pixels.
        */
        virtual const Scaling& onGetScaling() const = 0;

    protected:
        /** @brief Returns a reusable canvas instance.
        */
        virtual Gfx::Canvas* onGetCanvas(Gfx::Canvas* reuse);

        /** @brief Creates a Canvas.
        */
        virtual Gfx::Canvas* onCreateCanvas(Gfx::Canvas* reuse) = 0;
        
        /** @brief Releases the current Canvas.
        */
        virtual void onReleaseCanvas() = 0;

        /** @brief Synchronizes pending operations to the surface.
        */
        virtual void onSync() = 0;

        /** @brief Finishes painting to the surface.
        */
        virtual void onFinish() = 0;

    private:
        /** @internal
        */
        void attachPainter(PainterBase& painter);

        /** @internal
        */
        void detachPainter(PainterBase& painter);

        /** @internal
        */
        void attachContext(PaintContext& context);

        /** @internal
        */
        void detachContext(PaintContext& context);

    private:
        /** @internal
        */
        void onDetachCanvas(Canvas& canvas);

    private:
        Canvas*       _canvas;
        PainterBase*  _painter;
        PaintContext* _context;
        void*     _reserved;
};

} // namespace

} // namespace

#endif
