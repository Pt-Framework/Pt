/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

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
            virtual Math::Size minimumSize();

            //! @brief Always returns a size of (0, 0).
            //! @return Returns a size of (0, 0).
            virtual Math::Size preferredSize();

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
