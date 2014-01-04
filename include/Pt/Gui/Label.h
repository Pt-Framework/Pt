/*
 * Copyright (C) 2006 Tobias Mueller
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
#ifndef PT_GUI_LABEL_H
#define PT_GUI_LABEL_H

#include <Pt/Gfx/Gfx.h>
#include <Pt/Gui/Api.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/Painter.h>


namespace Pt {

namespace Gui {

    /**
     * @brief A widget to show a single line of text.
     *
     * A label widget shows a single line of read-only text, which can not be edited directly
     * by the user, but can be changed programmatically using the setText() method.
     *
     * The label uses the default font of the platform to draw the text.
     *
     * As for any widget the background color can be set explicitely to draw the background of
     * the label. This is everything that is not text. The foreground color is used to as color
     * for the text of the label.
     */
    class PT_GUI_API Label : public Widget
    {
        public:
            /**
             * Constructs a label widget.
             *
             * A label widget is created. The given parent is set as parent of this label and
             * the label is added to the parent's children list. The label is positioned at the
             * given location using the given size. An optional text/label can be passed as an
             * argument. This text will be shown as read-only text of the label. If no text is
             * given, no text is shown.
             *
             * @param parent The parent widget for this label. The label will become the child of
             * this parent and be shown inside of it. To create a top-level widget 0 can be passed
             * as an argument.
             * @param at The position of this label inside its parent relative to the parent's top-left border.
             * @param size The size of this label. The size must be >0 for width and height.
             * @param text The (optional) text of this label.
             * @see setText(const Pt::String)
             */
            Label(Widget& parent,
                  const Gfx::Point& at = Gfx::Point(0, 0),
                  const Gfx::Size& size = Gfx::Size(0, 0),
                  const Pt::String& text = Pt::String());

            //! Empty destructor for the label widget.
            ~Label();

            /**
             * @brief Sets the label's text which is displayed in the top-left border of the label.
             *
             * When calling this method, the presentation of the label is updated automatically.
             * No re-layout is done, though.
             *
             * @param text The new text for this label.
             */
            void setText(const Pt::String& text);

            /**
             * @brief Returns the current text of this label.
             *
             * @return The current text of this label.
             */
            const Pt::String& text() const;

            /**
             * @brief Updates the presentation of this label.
             *
             * It does a complete repaint including the background and the text of the label.
             */
            virtual void update();

            // Automatically inherits the documentation of its base class.
            virtual Gfx::Size minimumSize();

            // Automatically inherits the documentation of its base class.
            virtual Gfx::Size preferredSize();

        public:
            /**
             * @brief A signal that notifies the registered listeners when this label was clicked by the user.
             */
            Signal<> clicked;

        protected:
            //! Internal resize event handle method.
            virtual void _resizeEvent(const ResizeEvent& event);

            //! Internal mouse event handle method.
            virtual void _mouseEvent(const MouseEvent& event);

            //! Internal paint event handle method.
            virtual void _paintEvent(const PaintEvent& event);

        private:
            std::auto_ptr<Pixmap> _backbuffer;
            Pt::String _text;
    };

} // namespace Gui

} // namespace Pt

#endif
