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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
	02110-1301 USA
*/

#ifndef Pt_Forms_FlowLayout_H
#define Pt_Forms_FlowLayout_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Direction.h>
#include <Pt/Forms/Layout.h>

namespace Pt {

namespace Forms {

/** @brief Arranges children in a single row or column.

    %Direction::Left and %Direction::Right place children on a horizontal
    axis. %Direction::Top and %Direction::Bottom place them on a vertical
    axis. Each child keeps its preferred size on the flow axis and stretches
    on the cross axis. %setCenter() packs the group in the remaining space.
    %setReverse() visits children in reverse order along the same direction.

    @code
    Direction::Left                 Direction::Top
    +----------------------+        +--------+
    | [A] [B] [C]          |        | [A]    |
    +----------------------+        | [B]    |
                                    | [C]    |
    Left, centered                  +--------+
    +----------------------+
    |      [A] [B] [C]     |
    +----------------------+
    @endcode

    @code
    Pt::Forms::FlowLayout row(Pt::Forms::Direction::Left);
    row.addItem(a);
    row.addItem(b);
    row.addItem(c);
    window.setContent(&row);
    @endcode

    @ingroup Pt-Forms-Layouts
*/
class PT_FORMS_API FlowLayout : public Layout
{
    typedef Layout Base;

    public:
        /** @brief Creates a flow layout in direction @a d.
        */
        explicit FlowLayout(Direction d = Direction::Left);

        /** @brief Destroys the layout. Attached controls are not destroyed.
        */
        virtual ~FlowLayout();

        /** @brief Sets the flow direction to @a d and requests relayout.
        */
        void setDirection(Direction d);

        /** @brief Sets whether items are packed as a centered group.
        */
        void setCenter(bool b);

        /** @brief Sets whether children are visited in reverse order.
        */
        void setReverse(bool b);

        /** @brief Attaches @a control as a flow item.

            The layout does not take ownership.
        */
        void addItem(Control& control);

        /** @brief Detaches @a control without destroying it.
        */
        void removeItem(Control& control);

    protected:
        /** @brief Measures children along the flow axis.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places children along the flow direction.
        */
        virtual void onLayout(const Gfx::RectF& rect);

    private:
        Gfx::SizeF onMeasureHorizontal(const SizePolicy& policy);

        Gfx::SizeF onMeasureVertical(const SizePolicy& policy);

        void onLayoutLeft(const Gfx::RectF& rect, bool center);

        void onLayoutRight(const Gfx::RectF& rect, bool center);

        void onLayoutTop(const Gfx::RectF& rect, bool center);

        void onLayoutBottom(const Gfx::RectF& rect, bool center);

    private:
        Direction _direction;
        bool      _center;
        bool      _reverse;
};

} // namespace

} // namespace

#endif // include guard
