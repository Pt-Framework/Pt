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

namespace Pt {

namespace Gfx {

/** @brief Paint surface.
*/
class PT_GFX_API PaintSurface : private NonCopyable
{
    friend class Painter;
    friend class PaintContext;

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

        /** @brief Returns the size in physical pixel.
        */
        const Gfx::SizeF& size() const;

        /** @brief Returns the scaling fro logical to physical pixels.
        */
        const Scaling& scaling() const;

        /** @brief Get a PaintContext.
        */
        PaintContext* getContext(PaintContext* context);

        /** @brief Resets the currently active paint context.
        */
        void invalidate();

        /** @brief Synchronizes pending operations to the surface.
        */
        void sync();

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
        /** @brief Get a PaintContext.
        */
        virtual Gfx::PaintContext* onGetContext(Gfx::PaintContext* context);

        /** @brief Creates a PaintContext.
        */
        virtual Gfx::PaintContext* onCreateContext(Gfx::PaintContext* context);
        
        /** @brief Releases the current PaintContext.
        */
        virtual void onReleaseContext() = 0;

        /** @brief Synchronizes pending operations to the surface.
        */
        virtual void onSync() = 0;

    private:
        //! @internal
        void attachPainter(Painter& painter);

        //! @internal
        void detachPainter(Painter& painter);

    private:
        //! @internal
        void onDetachContext(PaintContext& context);

    private:
        PaintContext*  _context;
        Painter*       _painter;
};

/** @brief Paint canvas.
*/
class PT_GFX_API Canvas : private NonCopyable
{
    friend class Painter;

    public:
        Canvas(PaintSurface& surface);

        ~Canvas();

        PaintSurface* surface();

        void sync();

    private:
        void attachPainter(Painter& painter);

        void detachPainter(Painter& painter);

    private:
        PaintSurface* _surface;
        Painter*      _painter;
};

} // namespace

} // namespace

#endif
