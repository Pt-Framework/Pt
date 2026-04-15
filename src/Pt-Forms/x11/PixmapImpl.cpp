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

#include "PixmapImpl.h"
#include "ApplicationImpl.h"

#include <Pt/Forms/Application.h>
#include <Pt/Forms/Pixmap.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Canvas.h>

#include <cassert>

namespace Pt {

namespace Forms {

const Gfx::ImageFormat& getScreenFormat()
{
    int depth = Application::instance().impl()->depth();

    switch(depth)
    {
        case 16:
            return Gfx::ImageFormat::rgb16();

        case 24:
        case 32:
            return Gfx::ImageFormat::rgb32();
    }

    return Gfx::ImageFormat::rgb32();
}


void PixmapImpl::drawPixmap(const Pt::Gfx::PointF& to,
                            const Pixmap& pixmap,
                            const Gfx::Paint& paint,
                            const Gfx::RectF* rect)
{
    const Gfx::Bitmap& bitmap = pixmap.impl()->_bitmap;
    _bitmap.drawBitmap(to, bitmap, paint, rect);
}


void PixmapImpl::drawPixmap(Gfx::Canvas& canvas,
                            const Pt::Gfx::PointF& to,
                            const Pixmap& pixmap,
                            const Gfx::RectF* rect)
{
    assert(_canvas == &canvas);

    if(_canvas == &canvas)
    {
        const Gfx::Image& image = pixmap.impl()->_bitmap.image();
        _canvas->drawImage(to, image, rect);
    }
}

} // namespace

} // namespace
