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

#ifndef PT_GUI_VERTICALLAYOUT_H
#define PT_GUI_VERTICALLAYOUT_H

#include <Pt/Connectable.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gui/Api.h>
#include <Pt/Gui/LayoutManager.h>
#include <Pt/Gui/LayoutData.h>

#include <map>

namespace Pt {

namespace Gui {


    /**
     * @brief The VerticalLayout Manager positions the widget vertically.
     *
     * This LayoutManager positions the widget vertically in the order they
     * where added to their parent's widget (which this layout manager is
     * associated with).
     *
     * The widgets' horizontal position can be specified when setting the layout
     * data of a widget. Possibal horizontal positions are top, bottom, center and grab for the
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
    class PT_GUI_API VerticalLayout : public Layout
    {
        public:
            enum Mode {
                UniformHeight,
                VaryingHeight
            };

            public:
                enum Orientation {
                    Left,
                    Right,
                    Center,
                    Grab
            };

    /**
     * @brief The LayoutData class for the VerticalLayout manager.
     *
     * An object of this LayoutData class stores layout information of the orientation
     * and margin for the layouting of one widget. These informations can be accessed
     * with the methods orientation() and margin().
     *
     * The orientation specifies how the widget should be layouted if there is more
     * horizontal space available than the preferred width of the widget needs:
     *
     * - Left - The widget is positioned at the left using the preferred width of the widget.
     * - Right - The widget is positioned at the right using the preferred width of the widget.
     * - Center - The widget is positioned at the horizontal center using the preferred
     * width of the widget.
     * - Grab - The widget uses the complete horizontal space; its width it set to the
     * available horizontal space.
     *
     * @see VerticalLayout
     */
     class PT_GUI_API LayoutData : public Gui::LayoutData
            {
                public:
                    LayoutData( Orientation orientation = Left, const Margin& margin = Margin(0, 0, 0, 0) )
                    : Gui::LayoutData(margin)
                    , _orientation(orientation)
                    {}

                    //virtual LayoutData* clone() const
                    //{ return new LayoutData(*this); }

                    void setOrientation(Orientation orientation)
                    { _orientation = orientation; }

                    Orientation orientation() const
                    { return _orientation; }

                protected:
                    Orientation _orientation;
            };

        public:
            static VerticalLayout& create(Widget& widget, Mode mode = VaryingHeight, size_t gap = 0);

            void set(Widget& widget, Orientation orientation = Left, const Margin& margin = Margin(0, 0, 0, 0));

            void remove(Widget& widget);

            ssize_t maximumHeight() const;

            virtual void update();

            virtual Gfx::Size minimumSize();

            virtual Gfx::Size preferredSize();

        protected:
            VerticalLayout(Widget& widget, Mode mode, size_t gap);

            Gfx::Size calculateSize(Widget& parent, bool forPreferredSize);

        private:
            Mode _mode;
            size_t _gap;

            typedef std::map<Widget*, LayoutData> WidgetMap;
            WidgetMap _widgets;
    };

} // namespace Gui

} // namespace Pt

#endif
