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

#ifndef Pt_Forms_TextBlock_H
#define Pt_Forms_TextBlock_H

#include <Pt/Forms/Adjustment.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/TextMetrics.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Point.h>
#include <Pt/String.h>
#include <vector>

namespace Pt {

namespace Forms {

class Painter;

/** @brief Represents one laid-out line in a %TextBlock.

    %TextBlock::layout() produces %TextLine objects. Application code does
    not construct them as standalone objects. A line stores its text, its
    position in the block, and the text and font metrics used to lay it out.
    %cursorToX() and %xToCursor() map caret indices for editors.

    @ingroup Pt-Forms-Icons
*/
class PT_FORMS_API TextLine
{
    public:
        /** @brief Creates an empty line.
        */
        TextLine();

        /** @brief Destructor.
        */
        ~TextLine();

        /** @brief Returns the position relative to the text block origin.
        */
        const Gfx::PointF& position() const;

        /** @brief Sets the position relative to the text block origin.
        */
        void setPosition(const Gfx::PointF& p);

        /** @brief Sets the position relative to the text block origin.
        */
        void setPosition(double x, double y);

        /** @brief Returns the advance width of the line text.
        */
        double width() const;

        /** @brief Returns the font height of the line.
        */
        double height() const;

        /** @brief Returns the sum of ascent and descent.
        */
        double maxHeight() const;

        /** @brief Returns the font ascent of the line.
        */
        double ascent() const;

        /** @brief Returns the font descent of the line.
        */
        double descent() const;

        /** @brief Returns the line text.
        */
        const Pt::String& text() const;

        //void setText(const Pt::String& text, const Gfx::Font& font);

        /** @brief Assigns @a text and the metrics used to lay it out.
        */
        void setText(const Pt::String& text, const Gfx::TextMetrics& tm,
                    const Gfx::FontMetrics& fm);

        /** @brief Returns the x position of caret index @a n.
        */
        double cursorToX(const Painter& painter, std::size_t n) const;

        /** @brief Returns the caret index nearest to @a x.
        */
        std::size_t xToCursor(const Painter& painter, double x) const;

    private:
        Gfx::PointF       _position;
        Pt::String        _text;
        Gfx::TextMetrics  _textMetrics;
        Gfx::FontMetrics  _fontMetrics;
};


/** @brief Represents wrapped text as positioned lines.

    A %TextBlock is not a %Widget and does not own a painter or control.
    %Label and %LineEditor use it. A custom control sets the maximum width,
    adjustment, and line spacing, calls %layout(), then iterates the lines to
    measure or paint the text.

    %layout() uses the painter's font metrics to wrap a string into
    %TextLine objects. It replaces the previous lines and updates the block
    size. Wrapping is word-based: whitespace separates words, and explicit
    newlines are not hard breaks. %Adjustment::Left, %Adjustment::Right, and
    %Adjustment::Center align each line within the maximum width.
    %Adjustment::Justify is treated as left. The default adjustment is Center.

    %position() is the block origin and each line position is relative to it.
    %size() returns the laid-out width and height. Line spacing adds extra
    space between consecutive lines.

    @ingroup Pt-Forms-Icons
*/
class PT_FORMS_API TextBlock
{
    public:
        /** @brief Iterator over the laid-out lines.
        */
        typedef TextLine* Iterator;

        /** @brief Const iterator over the laid-out lines.
        */
        typedef const TextLine* ConstIterator;

    public:
        /** @brief Creates an empty text block.

            The default maximum width is large. The default adjustment is
            Center.
        */
        TextBlock();

        /** @brief Destructor.
        */
        ~TextBlock();

        /** @brief Returns the origin of the block.
        */
        const Gfx::PointF& position() const;

        /** @brief Sets the origin of the block.
        */
        void setPosition(const Gfx::PointF& p);

        /** @brief Returns the laid-out width and height.
        */
        const Gfx::SizeF& size() const;

        /** @brief Returns the laid-out width.
        */
        double width() const;

        /** @brief Returns the laid-out height.
        */
        double height() const;

        /** @brief Returns the maximum width used to wrap lines.
        */
        double maxWidth() const;

        /** @brief Sets the maximum width used to wrap lines.
        */
        void setMaxWidth(double w);

        /** @brief Sets the horizontal alignment of each line.
        */
        void setAdjustment(Adjustment a);

        /** @brief Returns the horizontal alignment of each line.
        */
        Adjustment adjustment() const;

        /** @brief Sets extra space between consecutive lines.
        */
        void setLineSpacing(double v)
        {
            _lineSpacing = v;
        }

        /** @brief Returns extra space between consecutive lines.
        */
        double lineSpacing() const
        {
            return _lineSpacing;
        }

        /** @brief Returns a pointer to the first line, or 0 when empty.
        */
        Iterator begin();

        /** @brief Returns a pointer past the last line, or 0 when empty.
        */
        Iterator end();

        /** @brief Returns a pointer to the first line, or 0 when empty.
        */
        ConstIterator begin() const;

        /** @brief Returns a pointer past the last line, or 0 when empty.
        */
        ConstIterator end() const;

        /** @brief Wraps @a text into lines using @a painter's font metrics.

            Replaces the previous lines and updates %size().
        */
        void layout(const Painter& painter, const Pt::String& text);

    private:
        void addLine(const Pt::String& line, const Gfx::TextMetrics& tm,
                    const Gfx::FontMetrics& fm);

        double align(double v) const
        {
            return v;
        }

    private:
        Gfx::PointF           _position;
        Gfx::SizeF            _size;
        double                _maxWidth;
        Adjustment            _adjustment;
        std::vector<TextLine> _lines;
        double                _lineSpacing;
};

} // namespace

} // namespace

#endif
