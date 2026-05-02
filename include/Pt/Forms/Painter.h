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

#ifndef Pt_Forms_Painter_h
#define Pt_Forms_Painter_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/PaintSurface.h>
#include <Pt/Gfx/PainterBase.h>

namespace Pt {

namespace Forms {

class PaintContext;
class Pixmap;

/** @brief 2D painter for Forms paint surfaces.
*/
class PT_FORMS_API Painter : public Gfx::PainterBase
{
    public:
        /** @brief Default constructor.
        */
        Painter();

        /** @brief Constructs using a Forms paint surface.
        */
        //explicit Painter(PaintSurface& surface);

        /** @brief Constructs using a Forms paint context.
        */
        explicit Painter(PaintContext& context);

        /** @brief Destructor.
        */
        virtual ~Painter();

        /** @brief Begins painting to a Forms paint surface.
        */
        //void begin(PaintSurface& surface);

        /** @brief Begins painting to a Forms paint context.
        */
        void begin(PaintContext& context);

        /** @brief Draws a pixmap.
        */
        void drawPixmap(const Gfx::PointF& to, const Pixmap& pixmap);

        /** @brief Draws a part of a pixmap.
        */
        void drawPixmap(const Gfx::PointF& to, const Pixmap& pixmap,
                        const Gfx::RectF& rect);

    protected:
        virtual void onDetachSurface(Gfx::PaintSurface& surface);

    private:
        PaintSurface*  _formsSurface;
};

} // namespace

} // namespace

#endif
