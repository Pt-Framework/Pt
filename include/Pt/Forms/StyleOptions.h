/*
  Copyright (C) 2016-2026 Laurentiu-Gheorghe Crisan
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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
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

class StyleOptions;

/** @brief Non-template base for a typed style option.

    Each concrete option type implements %clone(), %typeId(), and %name().
    The lookup key is the option class, not the stored value type.
*/
class PT_FORMS_API StyleOption
{
    public:
        virtual ~StyleOption();

        virtual StyleOption* clone() const = 0;

        virtual const std::type_info& typeId() const = 0;

        virtual const char* name() const = 0;

        virtual void bind(const StyleOptions* /*inherited*/)
        {}
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

        virtual const char* name() const override
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

        virtual const char* name() const override
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

        virtual const char* name() const override
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

        virtual const char* name() const override
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

        virtual const char* name() const override
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

        virtual const char* name() const override
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

        virtual const char* name() const override
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

        virtual const char* name() const override
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

        virtual const char* name() const override
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

        virtual const char* name() const override
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

        virtual const char* name() const override
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

        virtual const char* name() const override
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

        virtual const char* name() const override
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

        virtual const char* name() const override
        { return "popupTextColor"; }

    private:
        Gfx::Color _value;
};

/** @brief Font option.
*/
class PT_FORMS_API FontOption : public StyleOption
{
    public:
        /** @brief Constructs an empty font option.
        */
        FontOption();

        FontOption(const FontOption& o);

        FontOption& operator=(const FontOption& o);

        virtual StyleOption* clone() const;

        virtual const std::type_info& typeId() const;

        virtual const char* name() const override
        { return "font"; }

        /** @brief Returns true if any font override is set.
        */
        bool isSet() const;

        /** @brief Returns the effective font.
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

        virtual void bind(const StyleOptions* inherited);

        /** @brief Returns the local font overrides merged with @a baseFont.
        */
        Gfx::Font getFont(const Gfx::Font& baseFont) const;

    private:
        enum Override
        {
            All    = 0x1,
            Size   = 0x2,
            Weight = 0x4,
            Slant  = 0x8
        };

    private:
        AutoPtr<Gfx::Font>    _font;
        std::size_t           _size;
        Gfx::Font::Weight     _weight;
        Gfx::Font::Slant      _slant;
        Gfx::Font             _resolvedFont;
        unsigned              _overrides;
};

/** @brief Style options container.

    Contains local style options such as %ForegroundOption or %FontOption.
    The default constructor creates an empty container. %defaults() returns
    a container populated with the built-in default options.

    %bind() associates an inherited container and materializes local partial
    options against its effective values. %find() and %get() consider both local and
    inherited options. %findLocal() and %hasOptions() only consider
    locally stored options. %set() and %reset() modify the local options.
*/
class PT_FORMS_API StyleOptions
{
    public:
        static const std::size_t InvalidGeneration = 0;

    public:
        /** @brief Constructs an empty container.
        */
        StyleOptions();

        /** @brief Copy Constructor.
        */
        StyleOptions(const StyleOptions& o);

        /** @brief Destructor.
        */
        ~StyleOptions();

        /** @brief Assigns style options.
        */
        StyleOptions& operator=(const StyleOptions& o);

        /** @brief Returns a complete container with the built-in default options.
        */
        static StyleOptions defaults();

        /** @brief Returns the current change generation.
        */
        std::size_t generation() const;

        /** @brief Returns true if the container contains any local option.

            TODO: remove this in favour of isDefault
        */
        bool hasOptions() const;

        /** @brief Returns true if no options override @a base.
        */
        bool isDefault(const StyleOptions& base) const;

        /** @brief Binds local options to @a inherited and materializes effective values.

            Rebinding with unchanged local and inherited generations performs no option
            traversal. Passing 0 removes the inherited container.
        */
        void bind(const StyleOptions* inherited);

        /** @brief Returns the parent container that provides inherited options, or 0.
        */
        const StyleOptions* parent() const;

        /** @brief Returns the local option of type T, or 0 if absent.
        */
        template <typename T>
        const T* findLocal() const;

        /** @brief Returns the local or inherited option of type T, or 0 if absent.
        */
        template <typename T>
        const T* find() const;

        /** @brief Returns the local or inherited option of type T.
        */
        template <typename T>
        const T& get() const;

        /** @brief Sets or replaces the local option of type T.
        */
        template <typename T>
        void set(const T& option);

        /** @brief Removes the local option of type T if present.
        */
        template <typename T>
        void reset();

    private:
        bool hasLocalOptions() const;

        StyleOption* findOption(const std::type_info& ti) const;

        void replaceOption(StyleOption* option);

        void removeOption(const std::type_info& ti);

        void clearOptions();

    private:
        std::size_t               _generation;
        std::size_t               _boundGeneration;
        const StyleOptions*       _parent;
        std::vector<StyleOption*> _options;
};


template <typename T>
const T* StyleOptions::findLocal() const
{
    return static_cast<const T*>( findOption(typeid(T)) );
}


template <typename T>
const T* StyleOptions::find() const
{
    const T* option = findLocal<T>();
    if(option)
        return option;

    if( _parent )
        return _parent->find<T>();

    return 0;
}


template <typename T>
const T& StyleOptions::get() const
{
    const T* option = find<T>();
    if(option)
        return *option;

    throw std::logic_error("invalid style option");
}


template <typename T>
void StyleOptions::set(const T& option)
{
    StyleOption* localOption = option.clone();
    localOption->bind(_parent);
    replaceOption(localOption);
}


template <typename T>
void StyleOptions::reset()
{
    removeOption(typeid(T));
}

} // namespace

} // namespace

#endif
