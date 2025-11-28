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

class Painter;
class PaintSurface;

/** @brief Paint context.
*/
class PT_GFX_API PaintContext : private NonCopyable
{
    friend class Painter;

    public:
        PaintContext(PaintSurface& surface);

        ~PaintContext();

        /** @brief Returns the image format.
        */
        const Gfx::ImageFormat& format() const;

        /** @brief Returns the size in physical pixels.
        */
        const Gfx::SizeF& size() const;

        /** @brief Returns the scaling from logical to physical pixels.
        */
        const Scaling& scaling() const;

        /** @brief Gets a Canvas.
        */
        Canvas* getCanvas(Canvas* canvas);

        void sync();

        void finish();

    private:
        PaintSurface* surface();

        void attachPainter(Painter& painter);

        void detachPainter(Painter& painter);

    private:
        PaintSurface* _surface;
        Painter*      _painter;
};

} // namespace

} // namespace

#endif
