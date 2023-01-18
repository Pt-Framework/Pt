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

#ifndef Pt_Hmi_LineEditor_H
#define Pt_Hmi_LineEditor_H

#include <Pt/Hmi/Adjustment.h>
#include <Pt/Hmi/TextBlock.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/String.h>

namespace Pt {

namespace Hmi {

class LineEditor
{
    public:
        LineEditor();
        
        ~LineEditor();

        const Gfx::PointF& position() const;
        
        void setPosition(const Gfx::PointF& p);

        const Gfx::SizeF& size() const;

        void setSize(const Gfx::SizeF& s);

        Adjustment adjustment() const;

        void setAdjustment(Adjustment a);

        bool isMasked() const;

        void setMasked(bool m);

        const Pt::String& text() const;

        void setText(const Pt::String& s);

        const Pt::String& displayText() const;

        std::size_t cursorPosition() const;
        
        void setCursorPosition(std::size_t n);

        bool isEmpty() const;

        void clear();

        void insert(Char ch);

        void left();

        void right();

        void del();

        void backspace();

        void layout(Gfx::Painter& painter, TextLine& line);

        void layout(Gfx::Painter& painter, const Pt::String& text, TextLine& line);

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
