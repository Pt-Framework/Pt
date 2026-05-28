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

#ifndef Pt_Forms_StyleOptions_h
#define Pt_Forms_StyleOptions_h

#include <Pt/Forms/Api.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Color.h>
#include <Pt/NonCopyable.h>
#include <Pt/SmartPtr.h>

#include <cstddef>

namespace Pt {

namespace Forms {

/** @brief Stores the global theme option values.

    %Application owns the live global %StyleOptions object. Widgets use the
    generation counter during invalidation to decide whether cached theme data
    must be refreshed.
*/
class PT_FORMS_API StyleOptions
{
    public:
        explicit StyleOptions();

        StyleOptions(const StyleOptions& o);

        virtual ~StyleOptions();

        StyleOptions& operator=(const StyleOptions& o);

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
        // alternateTextBackground

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
            ++_generation;
        }

        const Gfx::Brush& foreground() const
        {
            return _foreground;
        }

        void setForeground(const Gfx::Brush& c)
        {
            _foreground = c;
            ++_generation;
        }
        
        const Gfx::Pen& contour() const
        {
            return _contour;
        }
        
        void setContour(const Gfx::Pen& p)
        {
            _contour = p;
            ++_generation;
        }

        const Gfx::Color& accentColor() const
        {
            return _accentColor;
        }
        
        void setAccentColor(const Gfx::Color& color)
        {
            _accentColor = color;
            ++_generation;
        }

        const Gfx::Brush& viewBackground() const
        {
            return _viewBackground;
        }

        void setViewBackground(const Gfx::Brush& b)
        {
            _viewBackground = b;
            ++_generation;
        }

        const Gfx::Color& highlightColor() const
        {
            return _highlightColor;
        }

        void setHighlightColor(const Gfx::Color& c)
        {
            _highlightColor = c;
            ++_generation;
        }

        const Gfx::Brush& textBackground() const
        {
            return _textBackground;
        }

        void setTextBackground(const Gfx::Brush& b)
        {
            _textBackground = b;
            ++_generation;
        }

        const Gfx::Color& textColor() const
        {
            return _textColor;
        }

        void setTextColor(const Gfx::Color& c)
        {
            _textColor = c;
            ++_generation;
        }

        const Gfx::Color& highlightedTextColor() const
        {
            return _highlightedTextColor;
        }

        void setHighlightedTextColor(const Gfx::Color& c)
        {
            _highlightedTextColor = c;
            ++_generation;
        }

        const Gfx::Font& font() const
        {
            return _font;
        }

        void setFont(const Gfx::Font& c)
        {
            _font = c;
            ++_generation;
        }

        /** @brief Returns the current change generation.
        */
        std::size_t generation() const
        {
            return _generation;
        }

        double cornerRadius() const
        {
            return _cornerRadius;
        }

        void setCornerRadius(double r)
        {
            _cornerRadius = r;
            ++_generation;
        }

        double borderWidth() const
        {
            return _borderWidth;
        }

        void setBorderWidth(double w)
        {
            _borderWidth = w;
            ++_generation;
        }

        double focusWidth() const
        {
            return _focusWidth;
        }

        void setFocusWidth(double w)
        {
            _focusWidth = w;
            ++_generation;
        }

    private:
      Gfx::Brush _background;
      Gfx::Brush _foreground;
      Gfx::Pen   _contour;
      Gfx::Color _accentColor;
      Gfx::Brush _viewBackground;
      Gfx::Color _highlightColor;
      Gfx::Brush _textBackground;
      Gfx::Color _textColor;
      Gfx::Color _highlightedTextColor;
      Gfx::Font  _font;
      double _cornerRadius;
      double _borderWidth;
      double _focusWidth;
      std::size_t _generation;
};

/** @brief Composable font override slice for widget-local style options.

    Stores an optional local font plus four partial-override bits (%All,
    %Size, %Weight, %Slant). Enclosing widget-local options classes
    delegate their font setters and resolver to this helper and bump
    their own override bit after each mutation.
*/
class PT_FORMS_API FontOption
{
    public:
        /** @brief Constructs an empty font option.
        */
        FontOption();

        /** @brief Returns true if any font override is set.
        */
        bool hasOverride() const;

        /** @brief Returns the local font override or 0 if none is set.
        */
        const Gfx::Font* font() const;

        /** @brief Replaces the complete local font override.
        */
        void setFont(const Gfx::Font& font);

        /** @brief Sets the local font size override.
        */
        void setSize(std::size_t size);

        /** @brief Sets the local font weight override.
        */
        void setWeight(Gfx::Font::Weight weight);

        /** @brief Sets the local font slant override.
        */
        void setSlant(Gfx::Font::Slant slant);

        /** @brief Resolves the effective font against the given base font.

            Returns %base unchanged when no override is set. A full font
            override replaces %base entirely; partial overrides for size,
            weight, and slant are merged into %base.
        */
        Gfx::Font getFont(const Gfx::Font& base) const;

    private:
        enum Override
        {
            All    = 0x1,
            Size   = 0x2,
            Weight = 0x4,
            Slant  = 0x8
        };

    private:
        AutoPtr<Gfx::Font> _font;
        unsigned           _overrides;
};

} // namespace

} // namespace

#endif
