/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PTV_GUI_MARGIN_H
#define PTV_GUI_MARGIN_H

#include <Pt/Types.h>
#include <Pt/Gui/Api.h>

namespace Pt {

namespace Gui {

    /**
     * @brief A Margin represents the (outer) margin of some object, for example a widget.
     *
     * A Margin object may be used to describe the spacing of one widget to other widgets in
     * layout which is layouted using a LayoutManager. The top, left, bottom and right spacing
     * can be specified independently.
     *
     * This class is immutable, its margin values can not be changed after their initialization.
     */
    class Margin
    {
        public:
            /**
             * @brief Constructs a Margin object.
             *
             * Constructs a Margin object by setting the top, left, bottom and right margin
             * value (=spacing) in this order. All values are optional and default to 0. Calling
             * the empty constructor will lead to a Margin object with 0 spacing for all sides.
             *
             * This class is immutable, so setting the values after initialization is not possible.
             *
             * @param top Specifies the top spacing of this margin. The default value is 0.
             * @param left Specifies the left spacing of this margin. The default value is 0.
             * @param bottom Specifies the bottom spacing of this margin. The default value is 0.
             * @param right Specifies the right spacing of this margin. The default value is 0.
             */
            Margin(ssize_t top = 0, ssize_t left = 0, ssize_t bottom = 0, ssize_t right = 0)
            : _top(top), _left(left), _bottom(bottom), _right(right)
            {
            }

            /**
             * @brief Returns the top spacing of this margin.
             * @return The top spacing of this margin.
             */
            ssize_t top() const
            {
                return _top;
            }

            /**
             * @brief Returns the left spacing of this margin.
             * @return The left spacing of this margin.
             */
            ssize_t left() const
            {
                return _left;
            }

            /**
             * @brief Returns the bottomspacing of this margin.
             * @return The bottom spacing of this margin.
             */
            ssize_t bottom() const
            {
                return _bottom;
            }

            /**
             * @brief Returns the right spacing of this margin.
             * @return The right spacing of this margin.
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

} // namespace Gui

} // namespace Pt

#endif
