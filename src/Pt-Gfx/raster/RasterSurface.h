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

#ifndef PT_GFX_RasterSurface_H
#define PT_GFX_RasterSurface_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Paint.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/System/Path.h>
#include <Pt/NonCopyable.h>

#include <string>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Gfx {

class RasterContext;

/** @brief Image drawing surface.
*/
class PT_GFX_API RasterSurface : private NonCopyable
{
  public:
    RasterSurface();

    RasterSurface(const Gfx::SizeF& size, std::size_t stride = 0);

    virtual ~RasterSurface();

    const Gfx::Image& image() const;

    void reset(const Gfx::Image& image);

    void reset(const Gfx::SizeF&, std::size_t stride = 0);

    const SizeF& physicalSize() const;

    const SizeF& logicalSize() const;

    void setScaleFactor(double scaleFactor);

    const Gfx::ImageFormat& format() const;

    const Gfx::SizeF& size() const;

    const Scaling& scaling() const;

    virtual Gfx::PaintContext* createContext(Gfx::PaintContext* context);

    virtual void releaseContext();

  private:
    Image          _image;
    Gfx::SizeF     _physicalSize;
    Gfx::SizeF     _logicalSize;
    Gfx::Scaling   _scaling;
    RasterContext* _context;
};

} // namespace

} // namespace

#endif

