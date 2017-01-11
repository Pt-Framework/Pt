/* Copyright (C) 2016 Laurentiu-Gheorghe Crisan
   Copyright (C) 2016 Marc Boris Duerner
 
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

#ifndef Pt_Hmi_StyleOptions_h
#define Pt_Hmi_StyleOptions_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Style.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Color.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace Hmi {

template <typename T>
class Option : private NonCopyable
{
    public:
        Option()
        : _value(0)
        {}
        
        Option(const T& value)
        : _value( new T(value) )
        {}

        ~Option()
        {
            delete _value;
        }
        
        const T& get() const
        {
            return *_value;
        }

        void set(const T& value)
        {
            delete _value;
            _value = 0;
            _value = new T(value);
        }

        bool isValid() const
        {
            return _value != 0;
        }

        void reset()
        {
            delete _value;
            _value = 0;
        }

    private:
        T* _value;
};


// TODO: this could be a normal member of Style instead of a facet
class PT_HMI_API StyleOptions : public Style::Facet
{
    public:
        explicit StyleOptions(std::size_t refs = 0);

        StyleOptions(const StyleOptions& o, std::size_t refs = 0);

        virtual ~StyleOptions();

        // background
        // foreground
        // textColor

        // highlightColor
        // highlightedTextBackground
        // highlightedTextColor

        // accentColor
        // selectedColor
        // activeColor
        
        // textBackground
        // alternateViewBackground

        // tooltipBackground / popupBackground
        // tooltipForeground / popupForeground
        // tooltipTextColor / popupTextColor

        const Gfx::Brush& background() const
        {
            return _background;
        }

        void setBackground(const Gfx::Brush& b)
        {
            _background = b;
        }

        const Gfx::Brush& foreground() const
        {
            return _foreground;
        }

        void setForeground(const Gfx::Brush& c)
        {
            _foreground = c;
        }
        
        const Gfx::Pen& contour() const
        {
            return _contour;
        }
        
        void setContour(const Gfx::Pen& p)
        {
            _contour = p;
        }

        const Gfx::Brush& textBackground() const
        {
            return _textBackground;
        }

        void setTextBackground(const Gfx::Brush& b)
        {
            _textBackground = b;
        }

        const Gfx::Color& highlightColor() const
        {
            return _highlightColor;
        }

        void setHighlightColor(const Gfx::Color& c)
        {
            _highlightColor = c;
        }

        const Gfx::Color& textColor() const
        {
            return _textColor;
        }

        void setTextColor(const Gfx::Color& c)
        {
            _textColor = c;
        }

        const Gfx::Font& font() const
        {
            return _font;
        }

        void setFont(const Gfx::Font& c)
        {
            _font = c;
        }

    private:
      Gfx::Brush _background;
      Gfx::Brush _foreground;
      Gfx::Pen   _contour;
      Gfx::Brush _textBackground;
      Gfx::Color _highlightColor;
      Gfx::Color _textColor;
      Gfx::Font  _font;
};

} // namespace

} // namespace

#endif
