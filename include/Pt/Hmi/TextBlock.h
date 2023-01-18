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
#include <Pt/Gfx/PaintSurface.h>
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

        const Gfx::PointF& position() const;
        
        void setPosition(const Gfx::PointF& p);

        void setPosition(double x, double y);

        double width() const;

        double height() const;

        double maxHeight() const;

        double ascent() const;

        double descent() const;

        const Pt::String& text() const;

        //void setText(const Pt::String& text, const Gfx::Font& font);

        void setText(const Pt::String& text, const Gfx::FontMetrics& tm);

        double cursorToX(const Gfx::Painter& painter, std::size_t n) const;

        std::size_t xToCursor(const Gfx::Painter& painter, double x) const;

    private:
        Gfx::PointF      _position;
        Pt::String       _text;
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

        const Gfx::PointF& position() const;
        
        void setPosition(const Gfx::PointF& p);

        const Gfx::SizeF& size() const;

        double width() const;

        double height() const;

        double maxWidth() const;

        void setMaxWidth(double w);

        void setAdjustment(Adjustment a);

        Adjustment adjustment() const;

        void setLineSpacing(double v)
        {
            _lineSpacing = v;
        }

        double lineSpacing() const
        {
            return _lineSpacing;
        }

        Iterator begin();

        Iterator end();

        ConstIterator begin() const;

        ConstIterator end() const;

        void layout(const Gfx::Painter& painter, const Pt::String& text);

    private:
        void addLine(const Pt::String& line, const Gfx::FontMetrics& tm);

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
