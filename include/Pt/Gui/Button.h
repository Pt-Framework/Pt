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

#ifndef Ptv_Gui_Button_h
#define Ptv_Gui_Button_h

#include <Pt/String.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gui/Api.h>
#include <Pt/Gui/Pixmap.h>
#include <Pt/Gui/Widget.h>

#include <memory>


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
     * text in its center. This text can be set using the constructor of this class or by calling
     * setTex().
     *
     * A button usually has three visual states: pressed, not pressed and disabled.
     *
     * A foreground and a background color can be set for a button. The foreground color is
     * used as color for the label's text. The background color is used for the button's interior
     * except for the border drawing. The default background color is a gray shade.
     *
     * \image html Button-default_cancel_button.png "A default cancel button"
     */
    class PT_GUI_API Button : public Widget
    {
        public:
            /**
             * @brief Constructor for the Button widget.
             *
             * A button widget is created. The given parent is set as parent of this button and
             * the button is added to the parent's children list. The button is positioned at the
             * given location using the given size. An optional text can be passed as an argument.
             * This text will be shown as button text. If no text is given, no text is shown.
             *
             * @param parent The parent widget for this button. The button will become the child of
             * this parent and be shown inside of it. To create a top-level widget 0 can be passed
             * as an argument.
             * @param at The position of this button inside its parent relative to the parent's top-left corner.
             * @param size The size of this button. The size must be >0 for width and height.
             * @param text The (optional) text of this button.
             * @see setText(const std::string)
             */
            Button(Widget& parent,
                   const Gfx::Point& at = Gfx::Point(0, 0),
                   const Gfx::Size& size = Gfx::Size(0, 0),
                   const Pt::String& text = Pt::String());

            //! @brief Empty destructor for the button widget.
            ~Button();

            /**
             * @brief Sets the button's text which is displayed centered inside the button.
             *
             * When calling this method, the presentation of the button is updated automatically.
             * No re-layout is done, though.
             *
             * @param text The new text for this button.
             */
            void setText(const Pt::String& text);

            /**
             * @brief Returns the current text of this button.
             *
             * @return The current text of this button.
             */
            const Pt::String& text() const;

            /**
             * @brief Updates the presentation of this button.
             *
             * It does a complete repaint including the background, the border and the text of the button.
             */
            virtual void update();

            // Automatically inherits the documentation of its base class.
            virtual Gfx::Size minimumSize();

            // Automatically inherits the documentation of its base class.
            virtual Gfx::Size preferredSize();

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
             * @brief Draws the button in pressed state using the given painter.
             */
            void drawPressed(Painter& painter);

            /**
             * @brief Draws the button in pressed state using the given painter.
             */
            void drawNormal(Painter& painter, bool focused);

            void drawText(Painter& painter, const Pt::ssize_t offset) const;

        private:
            std::auto_ptr<Pixmap> _backbuffer;
            Pt::String _text;
    };

} // namespace Gui

} // namespace Pt

#endif
