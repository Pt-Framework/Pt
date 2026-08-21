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
#include <Pt/Any.h>
#include <Pt/NonCopyable.h>
#include <Pt/SmartPtr.h>

#include <cstddef>
#include <string>
#include <typeinfo>
#include <vector>

namespace Pt {

namespace Forms {

/** @brief Non-template base for a single named style option.

    Provides name- and type-based access via Pt::Any so that tooling can
    inspect and mutate options without knowing their concrete types.
*/
class PT_FORMS_API StyleOption
{
    public:
        virtual ~StyleOption()
        {};

        /** @brief Returns the option name. */
        const char* name() const
        {
            return _name.c_str();
        }

        /** @brief Returns the dynamic type of the stored value.
        */
        virtual const std::type_info& typeInfo() const = 0;

        /** @brief Returns the current value wrapped in an Any.
        */
        virtual const Pt::Any& get() const = 0;

        /** @brief Sets the value from an Any; throws std::bad_cast on type mismatch.
        */
        virtual void set(const Pt::Any& v) = 0;

    protected:
        explicit StyleOption(const char* name)
        : _name(name)
        {}

    private:
        std::string _name;
};


/** @brief Typed style option that stores a value of type T.

    Only mutated through StyleOptions setters, which bump the generation
    counter. Copy ctor and operator= preserve the value as-is.
*/
template<typename T>
class BasicStyleOption : public StyleOption
{
    public:
        BasicStyleOption(const char* name, const T& value)
        : StyleOption(name)
        , _value(value)
        {}

        /** @brief Returns the stored value. */
        const T& value() const
        {
            return any_cast<const T&>(_value);
        }

        /** @brief Sets the value (does not bump generation). */
        void setValue(const T& v)
        {
            _value = Pt::Any(v);
        }

        /** @brief Copies the value only. */
        BasicStyleOption& operator=(const BasicStyleOption& o)
        {
            _value = o._value;
            return *this;
        }

        virtual const std::type_info& typeInfo() const
        {
            return typeid(T);
        }

        virtual const Pt::Any& get() const
        {
            return _value;
        }

        virtual void set(const Pt::Any& v)
        {
            setValue(any_cast<T>(v));
        }

    private:
        Pt::Any _value;
};


/** @brief Stores the global theme option values shared across styles.

    %Application owns the live global %StyleOptions object. Widgets use the
    generation counter during invalidation to decide whether cached theme data
    must be refreshed. Theme-specific geometry and look details that are not
    part of a cross-style contract stay in the concrete style implementation
    instead of the global %StyleOptions object.

    TODO:
        // background
        // foreground
        // textColor
        // placeholderTextColor

        // highlightColor
        // hoverBackground
        // highlightedTextBackground
        // highlightedTextColor

        // accentColor
        // selectedColor
        // activeColor

        // textBackground
        // viewBackground
        // alternateTextBackground
        // alternateViewBackground

        // tooltipBackground / popupBackground
        // tooltipForeground / popupForeground
        // tooltipTextColor / popupTextColor

        // common standard-widget candidates:
        // selectionBackground
        // selectionTextColor
        // focusColor
        // disabledBackground
        // disabledTextColor
        // separatorColor
        // caretColor

*/
class PT_FORMS_API StyleOptions
{
    public:
        /** @brief Forward iterator over all StyleOption entries. */
        class Iterator
        {
            public:
                explicit Iterator(StyleOption* const* p)
                : _ptr(p)
                {}

                const StyleOption& operator*() const
                { return **_ptr; }

                const StyleOption* operator->() const
                { return *_ptr; }

                Iterator& operator++()
                {
                    ++_ptr;
                    return *this;
                }

                bool operator!=(const Iterator& o) const
                { return _ptr != o._ptr; }

                bool operator==(const Iterator& o) const
                { return _ptr == o._ptr; }

            private:
                StyleOption* const* _ptr;
        };

    public:
        explicit StyleOptions();

        StyleOptions(const StyleOptions& o);

        virtual ~StyleOptions();

        StyleOptions& operator=(const StyleOptions& o);

        /** @brief Returns the current change generation.
        */
        std::size_t generation() const;

        const Gfx::Brush& background() const;

        void setBackground(const Gfx::Brush& b);

        const Gfx::Brush& foreground() const;

        void setForeground(const Gfx::Brush& c);

        const Gfx::Pen& contour() const;

        void setContour(const Gfx::Pen& p);

        const Gfx::Color& accentColor() const;

        void setAccentColor(const Gfx::Color& color);

        const Gfx::Brush& viewBackground() const;

        void setViewBackground(const Gfx::Brush& b);

        const Gfx::Color& highlightColor() const;

        void setHighlightColor(const Gfx::Color& c);

        const Gfx::Brush& hoverBackground() const;

        void setHoverBackground(const Gfx::Brush& b);

        const Gfx::Brush& textBackground() const;

        void setTextBackground(const Gfx::Brush& b);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& c);

        const Gfx::Color& placeholderTextColor() const;

        void setPlaceholderTextColor(const Gfx::Color& c);

        const Gfx::Color& highlightedTextColor() const;

        void setHighlightedTextColor(const Gfx::Color& c);

        const Gfx::Brush& alternateViewBackground() const;

        void setAlternateViewBackground(const Gfx::Brush& b);

        const Gfx::Brush& popupBackground() const;

        void setPopupBackground(const Gfx::Brush& b);

        const Gfx::Color& popupTextColor() const;

        void setPopupTextColor(const Gfx::Color& c);

        const Gfx::Font& font() const;

        void setFont(const Gfx::Font& f);

        /** @brief Returns an iterator to the first option.
        */
        Iterator begin() const;

        /** @brief Returns an iterator past the last option.
        */
        Iterator end() const;

        /** @brief Finds an option by name, or returns 0 if not found.
        */
        const StyleOption* find(const char* name) const;

        /** @brief Sets an option by name from an Any value; no-op if name is unknown.
        */
        void set(const char* name, const Pt::Any& value);

    protected:
        /** @brief Registers a StyleOption into the polymorphic vector for iteration.
        */
        void registerOption(StyleOption* opt);

    private:
        void init();

    private:
        std::size_t                      _generation;
        std::vector<StyleOption*>        _options;

        BasicStyleOption<Gfx::Brush>     _background;
        BasicStyleOption<Gfx::Brush>     _foreground;
        BasicStyleOption<Gfx::Pen>       _contour;
        BasicStyleOption<Gfx::Color>     _accentColor;
        BasicStyleOption<Gfx::Brush>     _viewBackground;
        BasicStyleOption<Gfx::Color>     _highlightColor;
        BasicStyleOption<Gfx::Brush>     _hoverBackground;
        BasicStyleOption<Gfx::Brush>     _textBackground;
        BasicStyleOption<Gfx::Color>     _textColor;
        BasicStyleOption<Gfx::Color>     _placeholderTextColor;
        BasicStyleOption<Gfx::Color>     _highlightedTextColor;
        BasicStyleOption<Gfx::Brush>     _alternateViewBackground;
        BasicStyleOption<Gfx::Brush>     _popupBackground;
        BasicStyleOption<Gfx::Color>     _popupTextColor;
        BasicStyleOption<Gfx::Font>      _font;
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


/** @brief Common base for widget-local style override tokens.

    Tracks which local overrides are present and a generation counter
    that changes whenever an override is set. Slice-specific options
    classes derive from this type and store only their token values.
*/
class PT_FORMS_API StyleOptionsBase
{
    public:
        /** @brief Constructs empty local style options.
        */
        StyleOptionsBase();

        /** @brief Destructor.
        */
        virtual ~StyleOptionsBase();

        /** @brief Returns true if any local style override is present.
        */
        bool hasOverrides() const;

        /** @brief Returns the current local override generation.

            The generation changes whenever any local override token is
            modified and can be used to detect when renderer preparation
            must be refreshed.
        */
        std::size_t generation() const;

    protected:
        /** @brief Returns true if the given override bit is set.
        */
        bool hasOverride(unsigned mask) const;

        /** @brief Marks the given override bit and bumps the generation.
        */
        void setOverride(unsigned mask);

        /** @brief Increments the local override generation.
        */
        void bumpGeneration();

    private:
        std::size_t _generation;
        unsigned    _overrides;
};

} // namespace

} // namespace

#endif
