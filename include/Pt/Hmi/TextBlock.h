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

#ifndef Pt_Hmi_TextBlock_H
#define Pt_Hmi_TextBlock_H

#include <Pt/Hmi/Adjustment.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Point.h>
#include <Pt/String.h>
#include <vector>

namespace Pt {

namespace Hmi {

class PT_HMI_API TextLine
{
    public:
        TextLine();

        ~TextLine();

        const Gfx::Point& position() const;
        
        void setPosition(const Gfx::Point& p);

        void setPosition(Pt::ssize_t x, Pt::ssize_t y);

        Pt::ssize_t width() const;

        Pt::ssize_t height() const;

        Pt::ssize_t maxHeight() const;

        Pt::ssize_t ascent() const;

        Pt::ssize_t descent() const;

        const Pt::String& text() const;

        void setText(const Pt::String& text, const Gfx::Font& font);

        void setText(const Pt::String& text, const Gfx::Font& font,
                     const Gfx::FontMetrics& tm);

        Pt::ssize_t cursorToX(std::size_t n) const;

        std::size_t xToCursor(Pt::ssize_t x) const;

    private:
        Gfx::Point      _position;
        Pt::String       _text;
        Gfx::Font        _font;
        Gfx::FontMetrics _textMetrics;
};

class PT_HMI_API TextBlock
{
    public:
        typedef TextLine* Iterator;
        typedef const TextLine* ConstIterator;

    public:
        TextBlock();

        ~TextBlock();

        const Gfx::Point& position() const;
        
        void setPosition(const Gfx::Point& p);

        const Gfx::Size& size() const;

        Pt::ssize_t width() const;

        Pt::ssize_t height() const;

        Pt::ssize_t maxWidth() const;

        void setMaxWidth(Pt::ssize_t w);

        void setAdjustment(Adjustment a);

        Adjustment adjustment() const;

        Iterator begin();

        Iterator end();

        ConstIterator begin() const;

        ConstIterator end() const;

        void layout(const Pt::String& text, const Gfx::Font& font);

    private:
        void addLine(const Pt::String& line, 
                     const Gfx::Font& font, 
                     const Gfx::FontMetrics& tm);

    private:
        Gfx::Point           _position;
        Gfx::Size            _size;
        Pt::ssize_t                _maxWidth;
        Adjustment            _adjustment;
        std::vector<TextLine> _lines;
};

} // namespace

} // namespace

#endif
