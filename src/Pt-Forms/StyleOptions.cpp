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

#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

namespace {

const std::size_t invalidGeneration = 0;

}

///////////////////////////////////////////////////////////////////////
// StyleOption
///////////////////////////////////////////////////////////////////////

StyleOption::~StyleOption()
{
}

///////////////////////////////////////////////////////////////////////
// FontOption
///////////////////////////////////////////////////////////////////////

FontOption::FontOption()
: _size(0)
, _weight(Gfx::Font::Weight::Normal)
, _slant(Gfx::Font::Slant::Normal)
, _resolvedFont()
, _overrides(0)
{
}


FontOption::FontOption(const FontOption& o)
: _size(o._size)
, _weight(o._weight)
, _slant(o._slant)
, _resolvedFont()
, _overrides(o._overrides)
{
    if( o._font )
        _font.reset( new Gfx::Font(*o._font) );

    bind(0);
}


FontOption& FontOption::operator=(const FontOption& o)
{
    if(this == &o)
        return *this;

    _size = o._size;
    _weight = o._weight;
    _slant = o._slant;
    _overrides = o._overrides;

    if( o._font )
        _font.reset( new Gfx::Font(*o._font) );
    else
        _font.reset();

    _resolvedFont = o._resolvedFont;

    return *this;
}


StyleOption* FontOption::clone() const
{
    return new FontOption(*this);
}


const std::type_info& FontOption::typeId() const
{
    return typeid(FontOption);
}


bool FontOption::isSet() const
{
    return _overrides != 0;
}


const Gfx::Font& FontOption::value() const
{
    return _resolvedFont;
}


void FontOption::setFont(const Gfx::Font& font)
{
    _font.reset( new Gfx::Font(font) );
    _overrides |= All;
    _resolvedFont = getFont(_resolvedFont);
}


void FontOption::setSize(std::size_t size)
{
    _size = size;
    _overrides |= Size;
    _resolvedFont = _resolvedFont.withSize(size);
}


void FontOption::setWeight(Gfx::Font::Weight weight)
{
    _weight = weight;
    _overrides |= Weight;
    _resolvedFont = _resolvedFont.withWeight(weight);
}


void FontOption::setSlant(Gfx::Font::Slant slant)
{
    _slant = slant;
    _overrides |= Slant;
    _resolvedFont = _resolvedFont.withSlant(slant);
}


void FontOption::bind(const StyleOptions* inherited)
{
    const FontOption* base = inherited ? inherited->find<FontOption>() : 0;
    _resolvedFont = base ? base->value() : Gfx::Font();

    if( _overrides & All )
        _resolvedFont = *_font;

    if( _overrides & Size )
        _resolvedFont = _resolvedFont.withSize(_size);

    if( _overrides & Weight )
        _resolvedFont = _resolvedFont.withWeight(_weight);

    if( _overrides & Slant )
        _resolvedFont = _resolvedFont.withSlant(_slant);
}


Gfx::Font FontOption::getFont(const Gfx::Font& baseFont) const
{
    Gfx::Font font = baseFont;

    if( _overrides & All )
        font = *_font;

    if( _overrides & Size )
        font = font.withSize(_size);

    if( _overrides & Weight )
        font = font.withWeight(_weight);

    if( _overrides & Slant )
        font = font.withSlant(_slant);

    return font;
}

///////////////////////////////////////////////////////////////////////
// StyleOptions
///////////////////////////////////////////////////////////////////////

StyleOptions::StyleOptions()
: _generation(1)
, _parent(0)
, _boundGeneration(invalidGeneration)
{
}


StyleOptions::StyleOptions(const StyleOptions& o)
: _generation(o._generation)
, _parent(o._parent)
, _boundGeneration(invalidGeneration)
{
    _options.reserve( o._options.size() );
    for(std::size_t n = 0; n < o._options.size(); ++n)
        _options.push_back( o._options[n]->clone() );
}


StyleOptions::~StyleOptions()
{
    clearOptions();
}


StyleOptions& StyleOptions::operator=(const StyleOptions& o)
{
    if(this == &o)
        return *this;

    clearOptions();
    ++_generation;

    _parent = o._parent;
    _boundGeneration = invalidGeneration;

    _options.reserve(  o._options.size() );
    for(std::size_t n = 0; n < o._options.size(); ++n)
        _options.push_back( o._options[n]->clone() );

    return *this;
}


StyleOptions StyleOptions::defaults()
{
    StyleOptions options;

    BackgroundOption background(Gfx::Color(229, 229, 229));
    options.set(background);

    ForegroundOption foreground(Gfx::Color(210, 210, 210));
    options.set(foreground);

    ContourOption contour(Gfx::Color(150, 150, 150));
    options.set(contour);

    AccentColorOption accentColor(Gfx::Color(20, 140, 250));
    options.set(accentColor);

    ViewBackgroundOption viewBackground(Gfx::Color(255, 255, 255));
    options.set(viewBackground);

    HighlightColorOption highlightColor(Gfx::Color(190, 190, 190));
    options.set(highlightColor);

    HoverBackgroundOption hoverBackground(Gfx::Color(190, 190, 190));
    options.set(hoverBackground);

    TextBackgroundOption textBackground(Gfx::Color(255, 255, 255));
    options.set(textBackground);

    TextColorOption textColor(Gfx::Color(0, 0, 0));
    options.set(textColor);

    PlaceholderTextColorOption placeholderTextColor(Gfx::Color(150, 150, 150));
    options.set(placeholderTextColor);

    HighlightedTextColorOption highlightedTextColor(Gfx::Color(0, 0, 0));
    options.set(highlightedTextColor);

    AlternateViewBackgroundOption alternateViewBackground(Gfx::Color(255, 255, 255));
    options.set(alternateViewBackground);

    PopupBackgroundOption popupBackground(Gfx::Color(229, 229, 229));
    options.set(popupBackground);

    PopupTextColorOption popupTextColor(Gfx::Color(0, 0, 0));
    options.set(popupTextColor);

    FontOption font;
    font.setFont(Gfx::Font("", 10));
    options.set(font);

    return options;
}


std::size_t StyleOptions::generation() const
{
    std::size_t gen = _generation;
    if( _parent )
        gen += _parent->generation();
    return gen;
}


bool StyleOptions::hasOptions() const
{
    return ! _options.empty();
}


void StyleOptions::bind(const StyleOptions* base)
{
    const std::size_t baseGeneration = base ? base->generation() : 0;

    if( _parent == base && _boundGeneration == baseGeneration )
        return;

    if( _parent != base )
        ++_generation;

    _parent = base;

    for(std::size_t n = 0; n < _options.size(); ++n)
        _options[n]->bind(base);

    _boundGeneration = baseGeneration;
}


const StyleOptions* StyleOptions::parent() const
{
    return _parent;
}


StyleOption* StyleOptions::findOption(const std::type_info& ti) const
{
    for(std::size_t n = 0; n < _options.size(); ++n)
    {
        if( _options[n]->typeId() == ti )
            return _options[n];
    }

    return 0;
}


void StyleOptions::replaceOption(StyleOption* option)
{
    const std::type_info& ti = option->typeId();
    for(std::size_t n = 0; n < _options.size(); ++n)
    {
        if( _options[n]->typeId() == ti )
        {
            delete _options[n];
            _options[n] = option;
            return;
        }
    }

    ++_generation;
    _options.push_back(option);
}


void StyleOptions::removeOption(const std::type_info& ti)
{
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


void StyleOptions::clearOptions()
{
    for(std::size_t n = 0; n < _options.size(); ++n)
        delete _options[n];

    _options.clear();
}

} // namespace

} // namespace
