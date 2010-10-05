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

#ifndef Ptv_Gui_ImageButton_h
#define Ptv_Gui_ImageButton_h

#include <Pt/String.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gui/Api.h>
#include <Pt/Gui/Pixmap.h>
#include <Pt/Gui/Widget.h>

#include <memory>


// TODO It would probably be more reasonable to derive ImageButton (and Button) from a super-class
// AbstractButton, that has all common code in it.



namespace Pt {
namespace Gui {

    class Pixmap;

    /**
     * @brief The implementation for a Button widget.
     *
     * A button (sometimes known as a command button or push button) is a widget that provides
     * the user a simple way to trigger an event by clicking said button. When a user clicks
     * the button this usually triggers some event dispatch mechanism which will call handle
     * code which reacts to the button click like confirming a dialog or executing a search
     * request.
     *
     * This Button class provides the 'clicked'-Signal for that matter. A Slot can be connected
     * to this Signal and will be executed whenever the button was clicked.
     *
     * A typical button's presentation is a rectangle, wider than it is tall, with a descriptive
     * Text in its center. As this button is an ImageButton, which uses 3 images to represent
     * its state, the text usually is incorporated in the image; or there is no text at all.
     *
     * A button usually has three visual states: pressed, not pressed and deactivated. For each
     * of these states an image can be set using the constructor. The image is then shown for
     * the appropriate state.
     *
     * @see Button
     */
    class PT_GUI_API ImageButton : public Widget
    {
        public:
            /**
             * @brief Constructor for the Button widget.
             *
             * A button widget is created. The given parent is set as parent of this button and
             * the button is added to the parent's children list. The button is positioned at the
             * given location using the given size. At least one image has to be given; this image
             * will be shown for the normal state (and for all other states if there is no explicit
             * image set for the other states).
             *
             * Beside the image for the normal state, there are images for the pressed state and
             * the disabled state. If no image for the pressed state is given, the normal state's
             * image is shown, but drawn with an offset of some pixels so it appears as if the button
             * is actually pressed. If no image for the disabled state is given, the normal state's
             * image is also shown.
             *
             * @param parent The parent widget for this button. The button will become the child of
             * this parent and be shown inside of it. To create a top-level widget 0 can be passed
             * as an argument.
             * @param at The position of this button inside its parent relative to the parent's top-left corner.
             * @param size The size of this button. The size must be >0 for width and height.
             * @param normalState The image for the normal state of this button.
             * @param pressedState The image for the pressed state of this button. (Optional)
             * @param disabledState The image for the disabled state of this button. (Optional)
             */
            ImageButton(Widget& parent,
                        const Pt::Gfx::Point& at,
                        const Pt::Gfx::Size& size,
                        const Pt::Gfx::ARgbImage* normalState,
                        const Pt::Gfx::ARgbImage* pressedState = 0,
                        const Pt::Gfx::ARgbImage* disabledState = 0);

            //! @brief Empty destructor for the button widget.
            ~ImageButton();

            /**
             * @brief Updates the presentation of this button.
             *
             * It does a complete repaint including the background, the border and the Text of the button.
             */
            virtual void update();

            // Automatically inherits the documentation of its base class.
            virtual Pt::Gfx::Size minimumSize();

            // Automatically inherits the documentation of its base class.
            virtual Pt::Gfx::Size preferredSize();

        public:
            /**
             * @brief A signal that notifies the registered slots when this button was clicked by the user.
             */
            Signal<> clicked;

        protected:
            //! Internal resize event handle method.
            virtual void _resizeEvent(const ResizeEvent& event);

            //! Internal mouse event handle method.
            virtual void _mouseEvent(const MouseEvent& event);

            //! Internal repaint event handle method.
            virtual void _paintEvent(const PaintEvent& event);

            //! Internal move event handle method.
            virtual void _mouseMoveEvent(const MouseMoveEvent& event);

        protected:
            bool _pressed;

        private:

            /**
             * @brief Draws the buttons background, given by the parameter 'image' to the given
             * painter object.
             *
             * The given image is drawn to the painter at a centered position. If an offset is
             * specified, the image is positioned using this offset in x and y direction.
             *
             * @param painter The given image is drawn to this painter.
             * @param image This image is drawn to the painter. The pointer has to be valid and
             * not 0!
             * @param offset If this optional parameter is given, the image is painted with an
             * offset of this amount in x and y direction.
             */
            void drawBackground(Painter& painter, const Pt::Gfx::ARgbImage* image, const Pt::ssize_t offset = 0);

        private:
            std::auto_ptr<Pixmap> _backbuffer;
            const Pt::Gfx::ARgbImage* _normalStateImage;
            const Pt::Gfx::ARgbImage* _pressedStateImage;
            const Pt::Gfx::ARgbImage* _disabledStateImage;

    };

} // namespace Gui
} // namespace Pt

#endif
