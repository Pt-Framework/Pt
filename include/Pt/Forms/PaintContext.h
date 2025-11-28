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
  MA 02110-1301 USA
*/

#ifndef Pt_Forms_PaintContext_h
#define Pt_Forms_PaintContext_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/PaintSurface.h>
#include <Pt/Gfx/PaintContext.h>

namespace Pt {

namespace Forms {

/** @brief Paint context.
*/
class PT_FORMS_API PaintContext : public Gfx::PaintContext
{
    public:
        PaintContext(PaintSurface& surface);

        ~PaintContext();

        void drawPixmap(const Gfx::PointF& to,
                        const Pixmap& pm,
                        const Gfx::Paint& paint,
                        const Gfx::RectF* rect = 0) const;

    private:
        PaintSurface&  _surface;
};


inline PaintContext::PaintContext(PaintSurface& surface)
: Gfx::PaintContext( surface )
, _surface(surface)
{
}


inline PaintContext::~PaintContext()
{
}


inline void PaintContext::drawPixmap(const Gfx::PointF& to,
                               const Pixmap& pixmap,
                               const Gfx::Paint& paint,
                               const Gfx::RectF* rect) const
{
    _surface.drawPixmap(to, pixmap, paint, rect);
}

} // namespace

} // namespace

#endif
