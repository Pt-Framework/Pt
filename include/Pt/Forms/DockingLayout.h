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
	02110-1301  USA
*/

#ifndef Pt_Forms_DockingLayout_H
#define Pt_Forms_DockingLayout_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Layout.h>
#include <map>

namespace Pt {

namespace Forms {

/** @brief Docks children to edges and fills leftover space.

    Children are placed in add order. %Left and %Right take preferred width
    and the remaining height. %Top and %Bottom take preferred height and the
    remaining width. Each dock shrinks the leftover rectangle. %Fill children
    share that leftover.

    @code
    +------------------------+
    | Top                    |
    +------+----------+------+
    | Left | Fill     | Right|
    +------+----------+------+
    | Bottom                 |
    +------------------------+
    @endcode

    @code
    Pt::Forms::DockingLayout content;
    content.addItem(toolbar, Pt::Forms::DockingLayout::Top);
    content.addItem(status, Pt::Forms::DockingLayout::Bottom);
    content.addItem(body, Pt::Forms::DockingLayout::Fill);
    window.setContent(&content);
    @endcode

    @ingroup Pt-Forms-Layouts
*/
class PT_FORMS_API DockingLayout : public Layout
{
    typedef Layout Base;

    public:
        /** @brief Edge or leftover region assigned to a child.
        */
        enum DockMode
        {
            /** @brief The child is not docked.
            */
            None = 0,

            /** @brief Docks to the left edge.
            */
            Left,

            /** @brief Docks to the top edge.
            */
            Top,

            /** @brief Docks to the right edge.
            */
            Right,

            /** @brief Docks to the bottom edge.
            */
            Bottom,

            /** @brief Fills leftover space after edge docks.
            */
            Fill
        };

    public:
        /** @brief Creates an empty docking layout.
        */
        explicit DockingLayout(DockMode ds = None);

        /** @brief Destroys the layout. Attached controls are not destroyed.
        */
        virtual ~DockingLayout();

        /** @brief Attaches @a control with dock mode @a ds.

            The layout does not take ownership.
        */
        void addItem(Control& control, DockMode ds);

        /** @brief Detaches @a control without destroying it.
        */
        void removeItem(Control& control);

        /** @brief Sets the dock mode of attached @a control to @a ds.
        */
        void setDockingStyle(Control& control, DockMode ds);

    protected:
        /** @brief Handles attachment of @a control.
        */
        virtual void onAddControl(Control& control);

        /** @brief Removes the stored dock mode for @a control.
        */
        virtual void onRemoveControl(Control& control);

        /** @brief Measures docked children and leftover Fill space.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Assigns edge docks, then leftover space to Fill children.
        */
        virtual void onLayout(const Gfx::RectF& rect);

    private:
        std::map<Control*, DockMode> _docking;
        DockMode                     _defaultDocking;
};

} // namespace

} // namespace

#endif // include guard
