/*
 * Copyright (C) 2006 Marc Boris D�rner
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

#ifndef PT_GUI_PANEL_H
#define PT_GUI_PANEL_H

#include <Pt/Gfx/Gfx.h>
#include <Pt/Gui/Api.h>
#include <Pt/Gui/Widget.h>


namespace Pt {

namespace Gui {

    class Painter;

    /**
     * @brief A panel widget which is usually only used as a container widget.
     *
     * A panel widget has no graphical representation, except for the background
     * color. It is usually only used as a container widget, for example to group
     * widgets inside a common parent or to nest layout managers by adding a panel
     * to a layout to use another layout manager for this panel.
     *
     * Usually a panel should be completely transparent. In the current implementation
     * a panel's background is always painted, though.
     */
    class PT_GUI_API Panel : public Widget
    {
        public:
            /**
             * @brief Constructs a panel widget.
             *
             * A panel widget is created. The given parent is set as parent of this panel and
             * the panel is added to the parent's children list. The panel is positioned at the
             * given location using the given size.
             *
             * @param parent The parent widget for this panel. The oabel will become the child of
             * this parent and be shown inside of it. To create a top-level widget 0 can be passed
             * as an argument.
             * @param at The position of this panel inside its parent relative to the parent's top-left border.
             * @param size The size of this lanel. The size must be >0 for width and height.
             */
            Panel(Widget& parent, const Gfx::Point& at, const Gfx::Size& size);

            //! @brief Empty destructor.
            ~Panel();

            /**
             * @brief Updates the presentation of this panel.
             *
             * It only does a complete repaint of the background.
             */
            virtual void update();

            // inherit doc
            // As this is only a container with no "own size" it just returns the minimum
            // size of the currently set layout manager.
            virtual Gfx::Size minimumSize();

            // inherit doc
            // As this is only a container with no "own size" it just returns the preferred
            // size of the currently set layout manager.
            virtual Gfx::Size preferredSize();

        protected:
            //! @brief Does a repaint of the widget.
            virtual void _resizeEvent(const ResizeEvent& event);

            //! @brief Does a repaint of the widget.
            virtual void _paintEvent(const PaintEvent& event);
    };

} // namespace Gui

} // namespace Pt

#endif
