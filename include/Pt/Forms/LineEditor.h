/* Copyright (C) 2017 Marc Boris Duerner
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 As a special exception, you may use this file as part of a free
 software library without restriction. Specifically, if other files
 instantiate templates or use macros or inline functions from this
 file, or you compile this file and link it with other files to
 produce an executable, this file does not by itself cause the
 resulting executable to be covered by the GNU General Public
 License. This exception does not however invalidate any other
 reasons why the executable file might be covered by the GNU Library
 General Public License.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 MA 02110-1301 USA
*/

#ifndef Pt_Forms_LineEditor_H
#define Pt_Forms_LineEditor_H

#include <Pt/Forms/Adjustment.h>
#include <Pt/Forms/TextBlock.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/String.h>

namespace Pt {

namespace Forms {

/** @brief Single-line text, caret, and scroll helper.

    A %LineEditor stores the entered string, the caret index, and the
    scroll of one line. It is not a %Control. %LineEdit, %SpinBox, and
    %ComboBox use it to edit text. Applications do not place it in a
    layout.

    %text() is the entered string. %displayText() is that string, or a
    mask character per entered character when %isMasked() is true.
    %layout() writes a positioned %TextLine for painting.

    @ingroup Pt-Forms-Editors
*/
class LineEditor
{
    public:
        /** @brief Constructs an empty line editor.
        */
        LineEditor();
        
        /** @brief Destroys the line editor.
        */
        ~LineEditor();

        /** @brief Returns the top-left position of the line.
        */
        const Gfx::PointF& position() const;
        
        /** @brief Sets the top-left position of the line to @a p.
        */
        void setPosition(const Gfx::PointF& p);

        /** @brief Returns the size of the line.
        */
        const Gfx::SizeF& size() const;

        /** @brief Sets the size of the line to @a s.
        */
        void setSize(const Gfx::SizeF& s);

        /** @brief Returns the horizontal adjustment of the text.
        */
        Adjustment adjustment() const;

        /** @brief Sets the horizontal adjustment of the text to @a a.
        */
        void setAdjustment(Adjustment a);

        /** @brief Returns true if %displayText() shows a mask character per character.
        */
        bool isMasked() const;

        /** @brief Sets whether %displayText() shows a mask character per character.
        */
        void setMasked(bool m);

        /** @brief Returns the entered text.
        */
        const Pt::String& text() const;

        /** @brief Sets the entered text to @a s.
        */
        void setText(const Pt::String& s);

        /** @brief Returns the text shown for the line.

            This is the entered text, or a mask string when %isMasked()
            is true.
        */
        const Pt::String& displayText() const;

        /** @brief Returns the caret index in the entered text.
        */
        std::size_t cursorPosition() const;
        
        /** @brief Sets the caret index in the entered text to @a n.
        */
        void setCursorPosition(std::size_t n);

        /** @brief Returns true if the entered text is empty.
        */
        bool isEmpty() const;

        /** @brief Clears the entered text, caret, and scroll.
        */
        void clear();

        /** @brief Inserts @a ch at the caret and advances the caret.
        */
        void insert(Char ch);

        /** @brief Moves the caret one character to the left.
        */
        void left();

        /** @brief Moves the caret one character to the right.
        */
        void right();

        /** @brief Deletes the character at the caret.
        */
        void del();

        /** @brief Deletes the character before the caret.
        */
        void backspace();

        /** @brief Lays out %displayText() into @a line using @a painter.
        */
        void layout(const Painter& painter, TextLine& line);

        /** @brief Lays out @a text into @a line using @a painter.
        */
        void layout(const Painter& painter, const Pt::String& text, TextLine& line);

    private:
        Gfx::PointF _position;
        Gfx::SizeF  _size;
        Adjustment  _adjustment;
        bool        _isMasked;
        Pt::String  _text;
        Pt::String  _displayText;
        std::size_t _cursorPosition;
        double      _scrollOffset;
};

} // namespace

} // namespace

#endif
