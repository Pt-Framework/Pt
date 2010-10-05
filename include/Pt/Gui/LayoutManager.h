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

#ifndef PTV_GUI_LAYOUT_H
#define PTV_GUI_LAYOUT_H

#include <Pt/Connectable.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gui/Api.h>


namespace Pt {

namespace Gui {

    class Widget;

    /**
     * @brief Abstract base class for all LayoutManagers.
     *
     * A LayoutManager is a class which knows how to lay out the child widgets inside a container
     * based on layout constraints (LayoutData) of widgets. This base class provides a minimal set
     * of operations which every LayoutManager has to provide.
     *
     * LayoutManagers are used to avoid the positioning of widgets on its container/parent using
     * pixel-coordinates. Rather a more or less complex rule-set is used to describe the desired
     * position of the widget, depending on certain criteria like for example the size of other
     * widgets in the same container, the container itself or some other criteria.
     *
     * Most LayoutManagers provide a specific LayoutData class, which derives from the basic
     * LayoutData class and stores all information which are necessary to layout one widget.
     *
     * To inform the LayoutManager about which LayoutData object is associated with which
     * LayoutData object each LayoutManager provides a specific method, which for consistency
     * reasons should have a signature similar to:
     * setLayoutData(Widget& widget, const SpecificLayoutData& layoutData).
     *
     * To remove a widget from the LayoutManager remove() can be used.
     *
     * A LayoutManager is only valid for one widget (=container) for which it does the layouting.
     * This container widget can be accessed using one of the widget() methods.
     *
     * A LayoutManager object can not be instantiated using its constructor, but provides
     * a factory method. For consistency reasons this method should have a signature similar to:
     * static SpecificLayout& create(Widget& widget, ...);
     *
     * The LayoutManager object is released only when the widget it is associated with, is
     * destroyed or as soon as another LayoutManager object is associated with this widget.
     *
     * To start the layout process update() can be called. The LayoutManager will layout all
     * child widgets of the associated container widget so the child widgets fit into the current
     * dimensions of the container widget considering the LayoutData constraints for all widgets.
     *
     * The LayoutManager class provides two methods which allow to determine the minimum and
     * preferred size of the container widget considering the minimum or preferred size of
     * the child widgets: minimumSize() and preferredSize().
     *
     * During the layout process only those widgets, for which a LayoutData object was
     * previously set, are be layouted. Widgets which don't have such an object are
     * ignored (and not moved or resized.)
     *
     * LayoutManagers can be nested by adding a container widget to a container widget. The
     * inner container widget can be assigned to another LayoutManager.
     */
    class PT_GUI_API Layout : public Connectable
    {
        public:
            //! Empty destructor for the label widget.
            virtual ~Layout()
            { }

            /**
             * @brief Returns the container widget this LayoutManager does the layouting for.
             *
             * Returns the container widget this LayoutManager does the layouting for. For every
             * widget there is only one LayoutManager. If a LayoutManager is created for a widget
             * which already has a LayoutManager, the older LayoutManager is destroyed.
             *
             * @return The container widget this LayoutManager does the layouting for.
             */
            Widget& widget()
            { return _widget; }

            /**
             * @brief Returns the container widget this LayoutManager does the layouting for.
             *
             * Returns the container widget this LayoutManager does the layouting for. For every
             * widget there is only one LayoutManager. If a LayoutManager is created for a widget
             * which already has a LayoutManager, the older LayoutManager is destroyed.
             *
             * @return The container widget this LayoutManager does the layouting for.
             */
            const Widget& widget() const
            { return _widget; }

            /**
             * @brief Removes the given widget from this LayoutManager. For future layout processing
             * this wid
             *
             * Removes the given widget (more precise it's LayoutData object) from this LayoutManager.
             * For future layout processing* this widget will not be layout, as no LayoutData is available
             * to base the layouting on.
             *
             * @param The widget to remove from this LayoutManager.
             */
            virtual void remove(Widget& widget) = 0;

            /**
             * @brief Layouts the container widget this LayoutManager was created for.
             *
             * The concrete layout process depends on the specific LayoutManager. The
             * LayoutManager will layout all child widgets of the associated container
             * widget so the child widgets fit into the current dimensions of the container
             * widget considering the LayoutData constraints for all widgets.
             *
             * Only those widgets, for which a LayoutData object was previously set are
             * layouted. Widgets which don't have such an object are ignored (and not
             * moved or resized.)
             *
             * See the classes description for a more precise description of the layouting
             * behaviour of this LayoutManager.
             */
            virtual void update() = 0;

            /**
             * @brief Calculates and returns the minimum size for the container widget's layout.
             *
             * To determine the minimum layout the LayoutManager simulates the layout process and
             * only uses the minimum size of each widget (see Widget::minimumSize()). The minimal
             * size which is needed to layout all widgets is returned.
             *
             * @return The minium layout size.
             */
            virtual Gfx::Size minimumSize() = 0;

            /**
             * @brief Calculates and returns the preferred size for the container widget's layout.
             *
             * To determine the preferred layout the LayoutManager simulates the layout process and
             * only uses the preferred size of each widget (see Widget::minimumSize()). The minimal
             * size which is needed to layout all widgets is returned.
             *
             * This information is used to "pack" a container widget. When packing a container the
             * preferred size of all child widgets is used to determine the optimal size of the layout.
             * Then the container widget is set to this optimal size.
             *
             * @return The preferred layout size.
             */
            virtual Gfx::Size preferredSize() = 0;

        protected:
            /**
             * @brief Private constructor. Use the "create()" method of one of the concrete LayoutManager-classes.
             *
             * This LayoutManager is set to the given widget by using its layout() method.
             * @see widget.setLayout()
             */
            Layout(Widget& widget);

        private:
            Widget& _widget;
    };

} // namespace Gui

} // namespace Pt

#endif
