/* Copyright (C) 2015 Marc Boris Duerner

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

#ifndef Pt_Gfx_PaintContext_h
#define Pt_Gfx_PaintContext_h

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Scaling.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace Gfx {

class PainterBase;
class PaintSurface;

/** @brief Active painting session for a surface.
    @ingroup Drawing

    %PaintContext represents a prepared drawing session on a %PaintSurface. It
    exposes target properties, provides access to a backend canvas and keeps
    painter and surface lifetime in sync while painting is active.
*/
class PT_GFX_API PaintContext : private NonCopyable
{
    friend class PainterBase;
    friend class PaintSurface;

    public:
        /** @brief Constructs a context using the paint surface.
        */
        explicit PaintContext(PaintSurface& surface);

        /** @brief Destructor.
        */
        virtual ~PaintContext();

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

        /** @brief Synchronizes pending drawing operations.
        */
        void sync();

        /** @brief Finishes painting on the surface.
        */
        void finish();

    protected:
        /** @brief Handles detachment from the surface.
        */
        virtual void onDetachSurface(PaintSurface& surface);

    private:
        PaintSurface* surface();

        void attachPainter(PainterBase& painter);

        void detachPainter(PainterBase& painter);

    private:
        PaintSurface* _surface;
        PainterBase*  _painter;
};

} // namespace

} // namespace

#endif
