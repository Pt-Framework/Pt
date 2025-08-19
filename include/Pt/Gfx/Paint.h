/* Copyright (C) 2015-2024 Marc Boris Duerner

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

#ifndef PT_GFX_PAINT_H
#define PT_GFX_PAINT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Path.h>

namespace Pt {

namespace Gfx {

/** @todo TODO:

    - distinguish between paint scaling and the physical/logical pixel ratio.
      If these are separate attributes, Paint objects can perform scaling, if
      needed for painting. The pixel ratio is reported by the implementation for
      alignment purposes. Paint scaling can be 1.0 if the underlying implementation
      works with logic pixels, even if the reported pixel ratio is higher.

    - Apply paint scaling in PaintContext and not in every impl tha needs it
*/

//
// TODO: Paint objects shared/refcounted so PaintContext can recognise
//       when paint attributes change
//

//
// TODO: active flag to defer paint updates
//
// if not active set only invalid flag otherwise apply
// so changes are visible immediately during painting
//

//
// TODO: PaintDevice is returned from Canvas::beginPaint
//
// PaintDevice is a movable type with the drawing ops and the region
//
// rename PaintContxt -> PaintDevice


/*
  TODO:

  drawLine in derived canvas should apply the necessary attribute
  for the current begin/finsh paint session. then Canvas needs no
  public apply* methods, but the NVI drawLine of canvas applies
  the ccorrect attributes

*/

/** @brief Paint context.
*/
class PT_GFX_API Paint
{
    public:
        Paint();

        ~Paint();

        /** @brief Returns the current composition mode.
        */
        const CompositionMode& compositionMode() const;

        /** @brief Sets the composition mode.
        */
        void setCompositionMode(const CompositionMode& mode);

        /** @brief Returns the clipping rect.
        */
        const RectF* clip() const;

        /** @brief Sets the clipping rect.
        */
        void setClip(const RectF& clip);

        /** @brief Resets the clipping rect.
        */
        void resetClip();

        /** @brief Returns the current pen.
        */
        const Pen& pen() const;

        /** @brief Sets the pen used to stroke lines.
        */
        void setPen(const Pen& pen);

        /** @brief Returns the current brush.
        */
        const Brush& brush() const;

        /** @brief Sets the brush used to fill areas.
        */
        void setBrush(const Brush& brush);

        /** @brief Returns the current font.
        */
        const Font& font() const;

        /** @brief Sets the font used to draw text.
        */
        void setFont(const Font& font);

        /** @brief Returns the current path.
        */
        const Gfx::Path& path() const;

        /** @brief Sets the current path.
        */
        void setPath(const Path& path);

    private:
        Gfx::CompositionMode _compositionMode;
        Gfx::RectF           _clip;
        bool                 _hasClip;
        Gfx::Pen             _pen;
        Gfx::Brush           _brush;
        Gfx::Font            _font;
        Gfx::Path            _path;
};

} // namespace

} // namespace

#endif
