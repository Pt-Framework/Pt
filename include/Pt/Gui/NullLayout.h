/*
 * Copyright (C) 2006 PTV AG
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PT_GUI_NULLLAYOUT_H
#define PT_GUI_NULLLAYOUT_H

#include <Pt/Gfx/Gfx.h>
#include <Pt/Gui/Api.h>
#include <Pt/Gui/LayoutManager.h>


namespace Pt {

namespace Gui {

    class Widget;

    /**
     * @brief The null layout manager, which does nothing for layouting.
     *
     * The null layout manager is not really a layout manager, as it does not layout
     * the component's of the widget. It does not even provide a method to add widgets
     * to be layouted.
     *
     * The null layout manager is used whenever the child widget inside a container
     * widget are supposed to be positioned and sized using exact pixel-values and not
     * by using a layout manager. When setting the null layout the position of the
     * widgets will not be changed by the layout manager after they were positioned
     * manually using pixel-coordinates.
     */
    class PT_GUI_API NullLayout : public Layout
    {
        public:
            //! @brief Does no layouting, as this is the null layout manager.
            virtual void update();

            //! @brief Does nothing, as no widgets can be added to this layout manager for layouting anyway.
            virtual void remove(Widget& widget);

            //! @brief Always returns a size of (0, 0).
            //! @return Returns a size of (0, 0).
            virtual Gfx::Size minimumSize();

            //! @brief Always returns a size of (0, 0).
            //! @return Returns a size of (0, 0).
            virtual Gfx::Size preferredSize();

            /**
             * @brief Creates the NullLayout for the given widget.
             *
             * The layout manager for the given widget is automatically set to this
             * layout manager. If the widget already has a layout manager, the old layout
             * manager is removed (and destroyed) and this layout manager is set as new
             * layout manager. Widgets that are supposed to be layouted by the new layout
             * manager have to be registered (again).
             *
             * @return A pointer to the object of this layout manager.
             */
            static NullLayout* createFor(Widget& widget);

        private:
            NullLayout(Widget& widget);

    };

} // namespace Gui

} // namespace Pt

#endif
