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

#ifndef PT_GUI_INSETS_H
#define PT_GUI_INSETS_H

#include <Pt/Types.h>
#include <Pt/Gui/Api.h>


namespace Pt {

namespace Gui {

    /**
     * @brief Insets represent the inner borders of a (possibly containing) widget.
     *
     * The insets specify the space that a widget leaves between its edges and its content.
     * In case of a widget which does contain child widgets, this is the minimum space between
     * any child and one of the four widget's edges.
     *
     * In case of a widget which does not contain child widgets, the precise meaning of the
     * inset may depend on the widget. For widget that have a textual description, for example
     * a button, a textfield or a lable, it may be the spacing between the edges and the shown
     * text. For other widgets it may have no meaning at all.
     *
     * This class is immutable, its inset values can not be changed after their initialization.
     */
    class Insets
    {
        friend bool operator!=(const Insets& a, const Insets& b);

        public:
            /**
             * @brief Constructs and initializes a new Insets object with the specified top, left,
             * bottom, and right insets.
             *
             * All values are optional. Their default value is 0.
             */
            Insets(ssize_t top = 0, ssize_t left = 0, ssize_t bottom = 0, ssize_t right = 0)
            : _top(top), _left(left), _bottom(bottom), _right(right)
            {
            }

            /**
             * @brief Returns the top inset.
             * @return The top inset.
             */
            ssize_t top() const
            {
                return _top;
            }

            /**
             * @brief Returns the left inset.
             * @return The left inset.
             */
            ssize_t left() const
            {
                return _left;
            }

            /**
             * @brief Returns the bottom inset.
             * @return The bottom inset.
             */
            ssize_t bottom() const
            {
                return _bottom;
            }

            /**
             * @brief Returns the right inset.
             * @return The right inset.
             */
            ssize_t right() const
            {
                return _right;
            }

        private:
            ssize_t _top;
            ssize_t _left;
            ssize_t _bottom;
            ssize_t _right;
    };

    /**
     * @brief Compares the given Insets for inequality.
     *
     * Compares both Insets by comparing each value for top, left, bottom, right of the
     * Insets with each other.
     *
     * @param a Insets a to compare to Insets b.
     * @param b Insets b to compare to Insets a.
     * @return <code>true</code> if the Insets are inequal; <code>false</code> if they are equal.
     */
    inline bool operator!=(const Insets& a, const Insets& b)
    {
        return
               a._top    != b._top
            || a._left   != b._left
            || a._bottom != b._bottom
            || a._right  != b._right;
    }

} // namespace Gui

} // namespace Pt

#endif
