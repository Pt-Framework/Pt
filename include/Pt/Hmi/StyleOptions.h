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
#include <Pt/Any.h>
#include <map>


namespace Pt {

namespace Hmi {

class PT_HMI_API StyleOptions : public Style::Facet
{
    public:
        StyleOptions(std::size_t refs = 0);

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
        
        // viewBackground
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

        const Gfx::Color& foreground() const
        {
          return _foreground;
        }

        void setForeground(const Gfx::Color& c)
        {
          _foreground = c;
        }

        const Gfx::Color& highlight() const
        {
          return _highlight;
        }

        void setHighlight(const Gfx::Color& c)
        {
          _highlight = c;
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
      Gfx::Color _foreground;
      Gfx::Color _highlight;
      Gfx::Color _textColor;
      Gfx::Font  _font;    
};

} // namespace

} // namespace

#endif
