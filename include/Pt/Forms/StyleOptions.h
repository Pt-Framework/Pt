/* Copyright (C) 2016-2026 Laurentiu-Gheorghe Crisan
   Copyright (C) 2016-2026 Marc Boris Duerner

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
#include <Pt/SmartPtr.h>

#include <cstddef>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

namespace Pt {

namespace Forms {

/** @brief Non-template base for a typed style option.

    Each concrete option type implements %clone() and %typeId().
    The lookup key is the option class, not the stored value type.
*/
class PT_FORMS_API StyleOption
{
    public:
        virtual ~StyleOption();

        virtual StyleOption* clone() const = 0;

        virtual const std::type_info& typeId() const = 0;
};


/** @brief Background brush option.
*/
class BackgroundOption : public StyleOption
{
    public:
        explicit BackgroundOption(const Gfx::Color& color)
        : _value(color)
        {}

        explicit BackgroundOption(const Gfx::Brush& brush)
        : _value(brush)
        {}

        const Gfx::Brush& value() const
        { return _value; }

        virtual StyleOption* clone() const
        { return new BackgroundOption(*this); }

        virtual const std::type_info& typeId() const
        { return typeid(BackgroundOption); }

        static const char* name()
        { return "background"; }

    private:
        Gfx::Brush _value;
};


/** @brief Foreground brush option.
*/
class ForegroundOption : public StyleOption
{
    public:
        explicit ForegroundOption(const Gfx::Color& color)
        : _value(color)
        {}

        explicit ForegroundOption(const Gfx::Brush& brush)
        : _value(brush)
        {}

        const Gfx::Brush& value() const
        { return _value; }

        virtual StyleOption* clone() const
        { return new ForegroundOption(*this); }

        virtual const std::type_info& typeId() const
        { return typeid(ForegroundOption); }

        static const char* name()
        { return "foreground"; }

    private:
        Gfx::Brush _value;
};


/** @brief Contour pen option.
*/
class ContourOption : public StyleOption
{
    public:
        explicit ContourOption(const Gfx::Color& color)
        : _value(color)
        {}

        explicit ContourOption(const Gfx::Pen& pen)
        : _value(pen)
        {}

        const Gfx::Pen& value() const
        { return _value; }

        virtual StyleOption* clone() const
        { return new ContourOption(*this); }

        virtual const std::type_info& typeId() const
        { return typeid(ContourOption); }

        static const char* name()
        { return "contour"; }

    private:
        Gfx::Pen _value;
};


/** @brief Accent color option.
*/
class AccentColorOption : public StyleOption
{
    public:
        explicit AccentColorOption(const Gfx::Color& color)
        : _value(color)
        {}

        const Gfx::Color& value() const
        { return _value; }

        virtual StyleOption* clone() const
        { return new AccentColorOption(*this); }

        virtual const std::type_info& typeId() const
        { return typeid(AccentColorOption); }

        static const char* name()
        { return "accentColor"; }

    private:
        Gfx::Color _value;
};


/** @brief View background brush option.
*/
class ViewBackgroundOption : public StyleOption
{
    public:
        explicit ViewBackgroundOption(const Gfx::Color& color)
        : _value(color)
        {}

        explicit ViewBackgroundOption(const Gfx::Brush& brush)
        : _value(brush)
        {}

        const Gfx::Brush& value() const
        { return _value; }

        virtual StyleOption* clone() const
        { return new ViewBackgroundOption(*this); }

        virtual const std::type_info& typeId() const
        { return typeid(ViewBackgroundOption); }

        static const char* name()
        { return "viewBackground"; }

    private:
        Gfx::Brush _value;
};


/** @brief Highlight color option.
*/
class HighlightColorOption : public StyleOption
{
    public:
        explicit HighlightColorOption(const Gfx::Color& color)
        : _value(color)
        {}

        const Gfx::Color& value() const
        { return _value; }

        virtual StyleOption* clone() const
        { return new HighlightColorOption(*this); }

        virtual const std::type_info& typeId() const
        { return typeid(HighlightColorOption); }

        static const char* name()
        { return "highlightColor"; }

    private:
        Gfx::Color _value;
};


/** @brief Hover background brush option.
*/
class HoverBackgroundOption : public StyleOption
{
    public:
        explicit HoverBackgroundOption(const Gfx::Color& color)
        : _value(color)
        {}

        explicit HoverBackgroundOption(const Gfx::Brush& brush)
        : _value(brush)
        {}

        const Gfx::Brush& value() const
        { return _value; }

        virtual StyleOption* clone() const
        { return new HoverBackgroundOption(*this); }

        virtual const std::type_info& typeId() const
        { return typeid(HoverBackgroundOption); }

        static const char* name()
        { return "hoverBackground"; }

    private:
        Gfx::Brush _value;
};


/** @brief Text background brush option.
*/
class TextBackgroundOption : public StyleOption
{
    public:
        explicit TextBackgroundOption(const Gfx::Color& color)
        : _value(color)
        {}

        explicit TextBackgroundOption(const Gfx::Brush& brush)
        : _value(brush)
        {}

        const Gfx::Brush& value() const
        { return _value; }

        virtual StyleOption* clone() const
        { return new TextBackgroundOption(*this); }

        virtual const std::type_info& typeId() const
        { return typeid(TextBackgroundOption); }

        static const char* name()
        { return "textBackground"; }

    private:
        Gfx::Brush _value;
};


/** @brief Text color option.
*/
class TextColorOption : public StyleOption
{
    public:
        explicit TextColorOption(const Gfx::Color& color)
        : _value(color)
        {}

        const Gfx::Color& value() const
        { return _value; }

        virtual StyleOption* clone() const
        { return new TextColorOption(*this); }

        virtual const std::type_info& typeId() const
        { return typeid(TextColorOption); }

        static const char* name()
        { return "textColor"; }

    private:
        Gfx::Color _value;
};


/** @brief Placeholder text color option.
*/
class PlaceholderTextColorOption : public StyleOption
{
    public:
        explicit PlaceholderTextColorOption(const Gfx::Color& color)
        : _value(color)
        {}

        const Gfx::Color& value() const
        { return _value; }

        virtual StyleOption* clone() const
        { return new PlaceholderTextColorOption(*this); }

        virtual const std::type_info& typeId() const
        { return typeid(PlaceholderTextColorOption); }

        static const char* name()
        { return "placeholderTextColor"; }

    private:
        Gfx::Color _value;
};


/** @brief Highlighted text color option.
*/
class HighlightedTextColorOption : public StyleOption
{
    public:
        explicit HighlightedTextColorOption(const Gfx::Color& color)
        : _value(color)
        {}

        const Gfx::Color& value() const
        { return _value; }

        virtual StyleOption* clone() const
        { return new HighlightedTextColorOption(*this); }

        virtual const std::type_info& typeId() const
        { return typeid(HighlightedTextColorOption); }

        static const char* name()
        { return "highlightedTextColor"; }

    private:
        Gfx::Color _value;
};


/** @brief Alternate view background brush option.
*/
class AlternateViewBackgroundOption : public StyleOption
{
    public:
        explicit AlternateViewBackgroundOption(const Gfx::Color& color)
        : _value(color)
        {}

        explicit AlternateViewBackgroundOption(const Gfx::Brush& brush)
        : _value(brush)
        {}

        const Gfx::Brush& value() const
        { return _value; }

        virtual StyleOption* clone() const
        { return new AlternateViewBackgroundOption(*this); }

        virtual const std::type_info& typeId() const
        { return typeid(AlternateViewBackgroundOption); }

        static const char* name()
        { return "alternateViewBackground"; }

    private:
        Gfx::Brush _value;
};


/** @brief Popup background brush option.
*/
class PopupBackgroundOption : public StyleOption
{
    public:
        explicit PopupBackgroundOption(const Gfx::Color& color)
        : _value(color)
        {}

        explicit PopupBackgroundOption(const Gfx::Brush& brush)
        : _value(brush)
        {}

        const Gfx::Brush& value() const
        { return _value; }

        virtual StyleOption* clone() const
        { return new PopupBackgroundOption(*this); }

        virtual const std::type_info& typeId() const
        { return typeid(PopupBackgroundOption); }

        static const char* name()
        { return "popupBackground"; }

    private:
        Gfx::Brush _value;
};


/** @brief Popup text color option.
*/
class PopupTextColorOption : public StyleOption
{
    public:
        explicit PopupTextColorOption(const Gfx::Color& color)
        : _value(color)
        {}

        const Gfx::Color& value() const
        { return _value; }

        virtual StyleOption* clone() const
        { return new PopupTextColorOption(*this); }

        virtual const std::type_info& typeId() const
        { return typeid(PopupTextColorOption); }

        static const char* name()
        { return "popupTextColor"; }

    private:
        Gfx::Color _value;
};


/** @brief Font option with full and partial overrides.

    A complete font replaces the base font. Size, weight, and slant
    overrides are merged into a base font by %getFont(). %isSet() is
    true when any override bit is set on this value. Bag presence is
    separate and is reported by %StyleOptions::find().
*/
class PT_FORMS_API FontOption : public StyleOption
{
    public:
        /** @brief Constructs an empty partial font option.
        */
        FontOption();

        FontOption(const FontOption& o);

        FontOption& operator=(const FontOption& o);

        virtual StyleOption* clone() const;

        virtual const std::type_info& typeId() const;

        static const char* name()
        { return "font"; }

        /** @brief Returns true if any font override is set.
        */
        bool isSet() const;

        /** @brief Returns the stored font.

            Valid when a complete font override is set.
        */
        const Gfx::Font& value() const;

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

            Returns @a base unchanged when no override is set. A full font
            override replaces @a base entirely; partial overrides for size,
            weight, and slant are merged into @a base.
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


/** @brief Stores global style tokens or a sparse widget-local overlay.

    One class is used for both the complete global options and a sparse
    widget overlay. The default constructor is an empty bag.
    %StyleOptions::defaults() inserts the built-in contract tokens.
    Presence is bag membership via %find(), %set, and %reset(). Each
    token is a concrete option type (%ForegroundOption, %FontOption,
    and the other contract types). Overlay callers use %find(); it
    may be 0. Complete bags use %get(), which returns a reference.
    %get(overlay) selects the overlay token when present and otherwise
    %get(). Font merge stays on %FontOption::getFont().

    A parent can be set with %setParent(). When set, %get() and
    %generation() recurse to the parent after consulting the local
    bag. This allows a sparse widget overlay to fall back to the
    global theme automatically. %find() and %hasOverrides() remain
    local-only and report only the contents of this bag. Cycles in
    the parent chain must be avoided.

    %Application owns the live global instance constructed from
    defaults(). Widgets or their styler hold a second instance as the
    overlay. %generation() increments on a successful %set, %reset,
    or %setParent call that changes the parent. The vocabulary is
    closed: only the built-in option types are stored.
*/
class PT_FORMS_API StyleOptions
{
    public:
        /** @brief Constructs an empty option bag.
        */
        StyleOptions();

        StyleOptions(const StyleOptions& o);

        ~StyleOptions();

        StyleOptions& operator=(const StyleOptions& o);

        /** @brief Returns a complete bag with the built-in contract tokens.
        */
        static StyleOptions defaults();

        /** @brief Returns the current change generation.
        */
        std::size_t generation() const;

        /** @brief Returns true if the bag contains any option.
        */
        bool hasOverrides() const;

        /** @brief Sets a parent bag for fallback lookups.

            Changing the parent bumps the generation. Passing 0 clears
            the parent.
        */
        void setParent(const StyleOptions* parent);

        /** @brief Returns the parent bag or 0.
        */
        const StyleOptions* parent() const;

        /** @brief Returns the option of type T, or 0 if absent.

            Use for overlays and bags of unknown completeness.
        */
        template <typename T>
        const T* find() const;

        /** @brief Returns the option of type T.

            Use for complete bags such as %defaults() and
            %Application::styleOptions(). Throws std::logic_error if T
            is absent.
        */
        template <typename T>
        const T& get() const;

        /** @brief Returns the overlay option of type T, or this bag's option.

            Prefers @a overlay when T is present there. Otherwise %get().
            Throws std::logic_error when T is absent from both bags.
        */
        template <typename T>
        const T& get(const StyleOptions& overlay) const;

        /** @brief Replaces the option of type T and bumps generation.
        */
        template <typename T>
        void set(const T& option);

        /** @brief Removes the option of type T if present.

            Generation is unchanged when T is absent.
        */
        template <typename T>
        void reset();

    private:
        StyleOption* lookup(const std::type_info& ti) const;

        void replace(StyleOption* option);

        void clear();

    private:
        std::size_t               _generation;
        const StyleOptions*       _parent;
        std::vector<StyleOption*> _options;
};


template <typename T>
const T* StyleOptions::find() const
{
    return static_cast<const T*>( lookup(typeid(T)) );
}


template <typename T>
const T& StyleOptions::get() const
{
    const T* option = find<T>();
    if( option )
        return *option;

    if( _parent )
        return _parent->get<T>();

    throw std::logic_error(std::string("style option not set: ") + T::name());
}


template <typename T>
const T& StyleOptions::get(const StyleOptions& overlay) const
{
    const T* local = overlay.find<T>();
    return local ? *local : get<T>();
}


template <typename T>
void StyleOptions::set(const T& option)
{
    replace(option.clone());
    ++_generation;
}


template <typename T>
void StyleOptions::reset()
{
    const std::type_info& ti = typeid(T);
    for(std::size_t n = 0; n < _options.size(); ++n)
    {
        if( _options[n]->typeId() == ti )
        {
            delete _options[n];
            _options.erase(_options.begin() + n);
            ++_generation;
            return;
        }
    }
}

} // namespace

} // namespace

#endif
