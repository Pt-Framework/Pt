/* Copyright (C) 2025 Marc Boris Duerner 
  
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

#ifndef PT_FORMS_CANVASLAYOUT_H
#define PT_FORMS_CANVASLAYOUT_H

#include <Pt/Forms/Layout.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <map>

namespace Pt {

namespace Forms {

/** @brief Arranges children at stored positions and sizes.

    Each child keeps the position and size given to %addItem(). The canvas
    adds its padding to those positions. The preferred size is the bounding
    box of the stored rectangles plus padding.

    @code
    +------------------+
    | A@(16,16)        |
    |        B@(80,40) |
    +------------------+
    @endcode

    @code
    Pt::Forms::CanvasLayout canvas;
    canvas.addItem(a, Gfx::PointF(16, 16), Gfx::SizeF(80, 24));
    canvas.addItem(b, Gfx::PointF(80, 40), Gfx::SizeF(64, 24));
    window.setContent(&canvas);
    @endcode

    @ingroup Pt-Forms-Layouts
*/
class PT_FORMS_API CanvasLayout : public Layout
{
    public:
        typedef Layout Base;

        /** @brief Position and size of a canvas child.
        */
        class LayoutParams
        {
            public:
                /** @brief Creates empty parameters.
                */
                LayoutParams()
                { }

                /** @brief Creates parameters for @a pos and @a size.
                */
                LayoutParams(const Gfx::PointF& pos,
                             const Gfx::SizeF& size)
                : _pos(pos)
                , _size(size)
                { }

                /** @brief Returns the stored position.
                */
                const Gfx::PointF& position() const
                { return _pos; }

                /** @brief Returns the stored size.
                */
                const Gfx::SizeF&  size() const
                { return _size; }

            private:
                Gfx::PointF _pos;
                Gfx::SizeF  _size;
        };

    public:
        /** @brief Creates an empty canvas layout.
        */
        CanvasLayout();

        /** @brief Destroys the layout. Attached controls are not destroyed.
        */
        virtual ~CanvasLayout();

        /** @brief Attaches @a control at the position and size in @a params.

            The layout does not take ownership.
        */
        void addItem(Control& control, const LayoutParams& params);

        /** @brief Attaches @a control at @a pos with @a size.

            The layout does not take ownership.
        */
        void addItem(Control& control, 
                     const Gfx::PointF& pos,
                     const Gfx::SizeF& size);

        /** @brief Detaches @a control without destroying it.
        */
        void removeItem(Control& control);

    protected:
        /** @brief Removes the stored parameters for @a control.
        */
        virtual void onRemoveControl(Control& control);

        /** @brief Measures the bounding box of stored child rectangles.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places each child at its stored position and size.
        */
        virtual void onLayout(const Gfx::RectF& rect);

    private:
        typedef std::map<Control*, LayoutParams> ItemMap;

        std::map<Control*, LayoutParams> _items;
};

} // namespace

} // namespace

#endif
