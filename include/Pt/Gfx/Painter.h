/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2010-2017 Aloysius Indrayanto

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

#ifndef PT_GFX_PAINTER_H
#define PT_GFX_PAINTER_H

#include <Pt/Gfx/PainterBase.h>

namespace Pt {

namespace Gfx {

/** @brief 2D painter for Gfx paint surfaces.
*/
class PT_GFX_API Painter : public PainterBase
{
    public:
        /** @brief Default constructor.
        */
        Painter();

        /** @brief Constructs using a paint surface.
        */
        explicit Painter(PaintSurface& surface);

        /** @brief Constructs using a paint context.
        */
        explicit Painter(PaintContext& context);

        /** @brief Destructor.
        */
        virtual ~Painter();

        /** @brief Begins painting to a paint surface.
        */
        void begin(PaintSurface& surface);

        /** @brief Begins painting to a paint context.
        */
        void begin(PaintContext& context);
};

} // namespace

} // namespace

#endif
