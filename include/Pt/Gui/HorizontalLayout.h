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

#ifndef PT_GUI_HorizontalLayout_H
#define PT_GUI_HorizontalLayout_H

#include <Pt/Connectable.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gui/Api.h>
#include <Pt/Gui/LayoutManager.h>
#include <Pt/Gui/LayoutData.h>

#include <map>


namespace Pt {

namespace Gui {

    /**
     * @brief The LayoutData class for the HorizontalLayout manager.
     *
     * An object of this LayoutData class stores layout information of the orientation
     * and margin for the layouting of one widget. These informations can be accessed
     * with the methods orientation() and margin().
     *
     * The orientation specifies how the widget should be layouted if there is more
     * vertical space available than the preferred height of the widget needs:
     *
     * - Top - The widget is positioned at the top using the preferred height of the widget.
     * - Bottom - The widget is positioned at the bottom using the preferred height of the widget.
     * - Center - The widget is positioned at the vertical center using the preferred
     * height of the widget.
     * - Grab - The widget uses the complete vertical space; its height it set to the
     * available vertical space.
     *
     * !!Attention
     * LayoutManager are only inofficially supported by now. Use them at your own risk.
     * The documentation is not completed yet.
     *
     * @see HorizontalLayout
     */
    class PT_GUI_API HorizontalLayoutData : public LayoutData
    {
        public:
            //! @brief The orienation of the widget if there is more vertical space available than needed.
            enum Orientation {
                Top,     //! @brief Positions the widget at the top, using its preferred height.
                Bottom,  //! @brief Positions the widget at the bottom, using its preferred height.
                Center,  //! @brief Positions the widget in the vertical center, using its preferred height.
                Grab     //! @brief Resizes the widget so it uses all available vertical space.
            };

        public:
            /**
             * @brief Constructs a new HorizontalLayoutData object with the given orientation and margin.
             *
             * @param orientation Optional parameter to specifiy the orientation for the widget. (Default is Top.)
             * @param margin The margin (for all 4 sides) for the widget. The default is 0 for all sides.
             */
            HorizontalLayoutData(Orientation orientation = Top, const Margin& margin = Margin(0, 0, 0, 0));

            /**
             * @brief Clones this HorizontalLayoutData object.
             * @return A pointer to the cloned HorizontalLayoutData object (on the heap).
             */
            //virtual HorizontalLayoutData* clone() const;

            /**
             * @brief Sets the orientation property of this object.
             *
             * No automatic re-layout is done after this property was changed.
             *
             * @orientation The new orientation value.
             */
            void setOrientation(Orientation orientation);

            /**
             * @brief Returns the orientation property of this object.
             * @return The orientation of this object.
             */
            Orientation orientation() const;

        protected:
            Orientation _orientation;
    };


    /**
     * @brief The HorizontalLayout Manager positions the widget horizontally.
     *
     * This LayoutManager positions the widget horizontally in the order they
     * where added to their parent's widget (which this layout manager is
     * associated with).
     *
     * The widgets' vertical position can be specified in the HorizontalLayoutData
     * object. Possible vertical positions are top, bottom, center and grab for the
     * positioning at the top, bottom or center of the available vertical space.
     * Grab does change the height in of the widget to the full available vertical
     * space. For all other cases the widget's preferred height is used.
     *
     * The behaviour of the widget's width can be controlled by the widthBehaviour-
     * parameter of the constructor (WidthBehaviour):
     * - UniformWidth will set the width of all widgets to the preferred width of
     * the widest widget.
     * - VaryingWidth will set the width of each widget to its respective preferred
     * width.
     *
     * A gap/spacing can be specified using the constructor. When set to a non-zero value
     * this set amount of pixels is left as spacing between the horizontally positioned
     * widgets. A negative value will lead to overlapping widgets.
     *
     * This LayoutManager does not wrap the widgets if there is not enough horizontal
     * space in the parent widget available. The widgets will be positioned beyond
     * the parent's borders instead.
     *
     * !!Attention
     * LayoutManager are only inofficially supported by now. Use them at your own risk.
     * The documentation is not completed yet.
     *
     * @see HorizontalLayoutData
     */
    class PT_GUI_API HorizontalLayout : public Layout
    {
        public:
            /**
             * @brief Enumeration for setting the way the widths of the layout widgets is set.
             *
             * When setting the width behaviour to UniformWidth all widget will be set to
             * the same width. The width of the widest widget is used.
             *
             * When setting the width behaviour to VaryingWidth every widget's width will
             * set to its preferred width.
             *
             * @see Widget::preferredSize()
             */
            enum WidthBehaviour {
                UniformWidth,  //! All widgets have the same width; that of the widest widget.
                VaryingWidth   //! Each widget's width will be set to its preferred width.
            };

        public:
            /**
             * @brief Set the layout data for the given widget.
             *
             * Only widgets for which a layout data object is set will be layouted by this
             * LayoutManager. To remove the layout data information for a widget, use the
             * method remove().
             *
             * If the layout data for the given widget was already set, it will be replaced.
             * For a more precise description of HorizontalLayoutData see its class commentary.
             *
             * Setting or changing the layout data of a widget will not commence an immediate
             * re-layout. You have to call update() instead.
             *
             * @param widget The widget for which this layout data object is set.
             * @param layoutData The layout data object for the widget.
             */
            void setLayoutData(Widget& widget, const HorizontalLayoutData& layoutData);

            // inherit doc
            virtual void remove(Widget& widget);

            // inerhit doc
            virtual void update();

            // inerhit doc
            virtual Gfx::Size minimumSize();

            // inerhit doc
            virtual Gfx::Size preferredSize();

            /**
             * @brief Creates a new HorizontalLayout for the given widget.
             *
             * The newly created HorizontalLayout is set as LayoutManager for the given widget.
             *
             * The way the width of the widgets is set can be specified using the parameter
             * 'widthBehaviour'. This parameter is optional. The default behaviour is
             * VaryingWidth.
             *
             * When setting the width behaviour to UniformWidth all widget will be set to
             * the same width. The width of the widest widget is used.
             *
             * When setting the width behaviour to VaryingWidth every widget's width will
             * set to its preferred width.
             *
             * Specifying a gap using the parameter 'gap' will inform the LayoutManager to
             * add a spacing between the horizontally positioned widgets. A negative value
             * will make the widgets to overlap.
             *
             * This parameter is optinal. The default gap size is 0.
             */
            static HorizontalLayout& create(Widget& widget, WidthBehaviour widthBehaviour = VaryingWidth, ssize_t gap = 0);


        private:
            //! @brief Private constructor. See create() for a description of its parameters.
            HorizontalLayout(Widget& widget, WidthBehaviour widthBehaviour, ssize_t gap);

            /**
             * @brief Calculates the preferred or minimum size for the given parent widget.
             *
             * This is done using the basically same algorithm as is used to calculate the
             * actual position and size of the widget. Depending on if the preferred or
             * minimum size of the layout is calculated, the preferred or minimum size of
             * each widget is used for the calculation.
             *
             * @param parent The calculation of the preferred or minimum size of this widget
             * is used.
             * @param forPreferredSize When set to $true$ the preferred size is calculate; when
             * set to $false$ the minimum size is calculated.
             */
            Gfx::Size calculateSize(Widget& parent, bool forPreferredSize);

        private:
            WidthBehaviour _widthBehaviour;
            ssize_t        _gap;

            //! @brief Association between widget and its HorizontalLayoutData object.
            std::map<Widget*, HorizontalLayoutData> _widget2LayoutData;
    };

} // namespace Gui

} // namespace Pt

#endif
