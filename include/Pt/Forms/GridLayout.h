/* Copyright (C) 2017 Marc Boris Duerner 
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
	02110-1301  USA
*/

#ifndef Pt_Forms_GridLayout_H
#define Pt_Forms_GridLayout_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Layout.h>

namespace Pt {

namespace Forms {

/** @brief Arranges children in uniform wrapping cells.

    Cell size is the largest preferred child size. %Vertical fills columns
    then wraps to the next row. %Horizontal fills rows then wraps to the next
    column. The wrap count is the number of cells on that axis; `0` fits as
    many as the available size allows. Items keep their preferred size and
    are centered in their cells.

    @code
    Vertical, span 3
    +---+---+---+
    | A | B | C |
    +---+---+---+
    | D | E | F |
    +---+---+---+
    @endcode

    @code
    Pt::Forms::GridLayout grid(Pt::Forms::GridLayout::Vertical, 3);
    grid.addItem(a);
    grid.addItem(b);
    grid.addItem(c);
    grid.addItem(d);
    window.setContent(&grid);
    @endcode

    @ingroup Pt-Forms-Layouts
*/
class PT_FORMS_API GridLayout : public Layout
{
    public:
        /** @brief Wrapping axis of the grid.
        */
        enum Orientation
        {
            /** @brief Fills rows, then wraps to the next column.
            */
            Horizontal = 0,

            /** @brief Fills columns, then wraps to the next row.
            */
            Vertical = 1
        };

    public:
        /** @brief Creates a grid with orientation @a o and wrap count @a span.
        */
        explicit GridLayout(Orientation o = Vertical, std::size_t span = 0);

        /** @brief Destroys the layout. Attached controls are not destroyed.
        */
        virtual ~GridLayout();

        /** @brief Sets the orientation to @a o and the wrap count to @a span.
        */
        void setOrientation(Orientation o, std::size_t span = 0);

        /** @brief Attaches @a control as the next grid item.

            The layout does not take ownership.
        */
        void addItem(Control& control);

        /** @brief Detaches @a control without destroying it.
        */
        void removeItem(Control& control);

    protected:
        /** @brief Measures uniform cells from the largest preferred child.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places children in wrapping cells of uniform size.
        */
        virtual void onLayout(const Gfx::RectF& rect);

    private:
        Gfx::SizeF onMeasureVertical(const SizePolicy& policy);

        Gfx::SizeF onMeasureHorizontal(const SizePolicy& policy);

        void onLayoutVertical(const Gfx::SizeF& itemSize, const Gfx::RectF& rect);

        void onLayoutHorizontal(const Gfx::SizeF& itemSize, const Gfx::RectF& rect);

    private:
        Orientation _orientation;
        std::size_t _span;
};

} // namespace

} // namespace

#endif // include guard
