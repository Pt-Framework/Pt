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

#ifndef PTV_GUI_LAYOUTDATA_H
#define PTV_GUI_LAYOUTDATA_H

#include <Pt/Gui/Api.h>
#include <Pt/Gui/Margin.h>


namespace Pt {

namespace Gui {

    /**
     * @brief The base class for all LayoutManager-specific LayoutData objects.
     *
     * A single LayoutData object describes the layout constraints for a single widget
     * which is going to be layouted using a specific LayoutManager. It contains specific
     * information about the layout data for a widget. In the case of this base class, only
     * a margin can be specified. For complex LayoutManager other information, like position,
     * stretching, orientation of widgets to each others and others hints may be adjusted
     * with the layout data object.
     * 
     * Usually every LayoutManager does provide its specific LayoutData object, which contains
     * all necessary information to layout a widget in its container context. Only very basic
     * LayoutManagers may use this base class.
     *
     * @see Layout
     */
    class PT_GUI_API LayoutData
    {
        public:
            /**
             * @brief Constructs a new LayoutData object using the optional margins.
             *
             * Constructs a new LayoutData object using the optional margins. If no margins are
             * specified, a 0-margin (0, 0, 0, 0) is used.
             *
             * @param margin The margin to use for this LayoutData object.
             */
            LayoutData(const Margin& margin = Margin(0, 0, 0, 0))
            : _margin(margin)
            {
            }

            /**
             * @brief Sets the margin for this LayoutData object.
             *
             * !Information
             * Setting the margin does not automatically trigger a re-layout of the
             * container this LayoutData's widget is contained in. Use Widget::updateLayout()
             * to manually start a re-layout.
             *
             * @param margin The new margin for this LayoutData object.
             */
            void setMargin(const Margin& margin)
            {
                _margin = margin;
            }

            /**
             * @brief Returns the margin of this LayoutData object.
             * @return The margin of this LayoutData object.
             */
            const Margin& margin() const
            {
                return _margin;
            }

        private:
            Margin _margin;
    };

} // namespace Gui

} // namespace Pt

#endif
