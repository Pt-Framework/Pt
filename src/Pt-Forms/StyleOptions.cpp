/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan

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
   MA  02110-1301  USA
*/

#include <Pt/Forms/StyleOptions.h>
#include <cstring>

namespace Pt {

namespace Forms {

StyleOptions::StyleOptions()
: _generation(0)
, _background("background", Gfx::Brush(Gfx::Color(229, 229, 229)))
, _foreground("foreground", Gfx::Brush(Gfx::Color(210, 210, 210)))
, _contour("contour", Gfx::Pen(Gfx::Color(150, 150, 150)))
, _accentColor("accentColor", Gfx::Color(20, 140, 250))
, _viewBackground("viewBackground", Gfx::Brush(Gfx::Color(255, 255, 255)))
, _highlightColor("highlightColor", Gfx::Color(190, 190, 190))
, _hoverBackground("hoverBackground", Gfx::Brush(Gfx::Color(190, 190, 190)))
, _textBackground("textBackground", Gfx::Brush(Gfx::Color(255, 255, 255)))
, _textColor("textColor", Gfx::Color(0, 0, 0))
, _placeholderTextColor("placeholderTextColor", Gfx::Color(150, 150, 150))
, _highlightedTextColor("highlightedTextColor", Gfx::Color(0, 0, 0))
, _alternateViewBackground("alternateViewBackground", Gfx::Brush(Gfx::Color(255, 255, 255)))
, _popupBackground("popupBackground", Gfx::Brush(Gfx::Color(229, 229, 229)))
, _popupTextColor("popupTextColor", Gfx::Color(0, 0, 0))
, _font("font", Gfx::Font("", 10))
{
    init();
}


StyleOptions::StyleOptions(const StyleOptions& o)
: _generation(o._generation)
, _background("background", o._background.value())
, _foreground("foreground", o._foreground.value())
, _contour("contour", o._contour.value())
, _accentColor("accentColor", o._accentColor.value())
, _viewBackground("viewBackground", o._viewBackground.value())
, _highlightColor("highlightColor", o._highlightColor.value())
, _hoverBackground("hoverBackground", o._hoverBackground.value())
, _textBackground("textBackground", o._textBackground.value())
, _textColor("textColor", o._textColor.value())
, _placeholderTextColor("placeholderTextColor", o._placeholderTextColor.value())
, _highlightedTextColor("highlightedTextColor", o._highlightedTextColor.value())
, _alternateViewBackground("alternateViewBackground", o._alternateViewBackground.value())
, _popupBackground("popupBackground", o._popupBackground.value())
, _popupTextColor("popupTextColor", o._popupTextColor.value())
, _font("font", o._font.value())
{
    init();
}


StyleOptions::~StyleOptions()
{
}


StyleOptions& StyleOptions::operator=(const StyleOptions& o)
{
    if(this == &o)
        return *this;

    _background              = o._background;
    _foreground              = o._foreground;
    _contour                 = o._contour;
    _accentColor             = o._accentColor;
    _viewBackground          = o._viewBackground;
    _highlightColor          = o._highlightColor;
    _hoverBackground         = o._hoverBackground;
    _textBackground          = o._textBackground;
    _textColor               = o._textColor;
    _placeholderTextColor    = o._placeholderTextColor;
    _highlightedTextColor    = o._highlightedTextColor;
    _alternateViewBackground = o._alternateViewBackground;
    _popupBackground         = o._popupBackground;
    _popupTextColor          = o._popupTextColor;
    _font                    = o._font;
    _generation              = o._generation;

    return *this;
}


void StyleOptions::init()
{
    _options.clear();
    registerOption(&_background);
    registerOption(&_foreground);
    registerOption(&_contour);
    registerOption(&_accentColor);
    registerOption(&_viewBackground);
    registerOption(&_highlightColor);
    registerOption(&_hoverBackground);
    registerOption(&_textBackground);
    registerOption(&_textColor);
    registerOption(&_placeholderTextColor);
    registerOption(&_highlightedTextColor);
    registerOption(&_alternateViewBackground);
    registerOption(&_popupBackground);
    registerOption(&_popupTextColor);
    registerOption(&_font);
}


void StyleOptions::registerOption(StyleOption* opt)
{
    _options.push_back(opt);
}


std::size_t StyleOptions::generation() const
{
    return _generation;
}


const Gfx::Brush& StyleOptions::background() const
{
    return _background.value();
}


void StyleOptions::setBackground(const Gfx::Brush& b)
{
    _background.setValue(b);
    ++_generation;
}


const Gfx::Brush& StyleOptions::foreground() const
{
    return _foreground.value();
}


void StyleOptions::setForeground(const Gfx::Brush& c)
{
    _foreground.setValue(c);
    ++_generation;
}


const Gfx::Pen& StyleOptions::contour() const
{
    return _contour.value();
}


void StyleOptions::setContour(const Gfx::Pen& p)
{
    _contour.setValue(p);
    ++_generation;
}


const Gfx::Color& StyleOptions::accentColor() const
{
    return _accentColor.value();
}


void StyleOptions::setAccentColor(const Gfx::Color& color)
{
    _accentColor.setValue(color);
    ++_generation;
}


const Gfx::Brush& StyleOptions::viewBackground() const
{
    return _viewBackground.value();
}


void StyleOptions::setViewBackground(const Gfx::Brush& b)
{
    _viewBackground.setValue(b);
    ++_generation;
}


const Gfx::Color& StyleOptions::highlightColor() const
{
    return _highlightColor.value();
}


void StyleOptions::setHighlightColor(const Gfx::Color& c)
{
    _highlightColor.setValue(c);
    ++_generation;
}


const Gfx::Brush& StyleOptions::hoverBackground() const
{
    return _hoverBackground.value();
}


void StyleOptions::setHoverBackground(const Gfx::Brush& b)
{
    _hoverBackground.setValue(b);
    ++_generation;
}


const Gfx::Brush& StyleOptions::textBackground() const
{
    return _textBackground.value();
}


void StyleOptions::setTextBackground(const Gfx::Brush& b)
{
    _textBackground.setValue(b);
    ++_generation;
}


const Gfx::Color& StyleOptions::textColor() const
{
    return _textColor.value();
}


void StyleOptions::setTextColor(const Gfx::Color& c)
{
    _textColor.setValue(c);
    ++_generation;
}


const Gfx::Color& StyleOptions::placeholderTextColor() const
{
    return _placeholderTextColor.value();
}


void StyleOptions::setPlaceholderTextColor(const Gfx::Color& c)
{
    _placeholderTextColor.setValue(c);
    ++_generation;
}


const Gfx::Color& StyleOptions::highlightedTextColor() const
{
    return _highlightedTextColor.value();
}


void StyleOptions::setHighlightedTextColor(const Gfx::Color& c)
{
    _highlightedTextColor.setValue(c);
    ++_generation;
}


const Gfx::Brush& StyleOptions::alternateViewBackground() const
{
    return _alternateViewBackground.value();
}


void StyleOptions::setAlternateViewBackground(const Gfx::Brush& b)
{
    _alternateViewBackground.setValue(b);
    ++_generation;
}


const Gfx::Brush& StyleOptions::popupBackground() const
{
    return _popupBackground.value();
}


void StyleOptions::setPopupBackground(const Gfx::Brush& b)
{
    _popupBackground.setValue(b);
    ++_generation;
}


const Gfx::Color& StyleOptions::popupTextColor() const
{
    return _popupTextColor.value();
}


void StyleOptions::setPopupTextColor(const Gfx::Color& c)
{
    _popupTextColor.setValue(c);
    ++_generation;
}


const Gfx::Font& StyleOptions::font() const
{
    return _font.value();
}


void StyleOptions::setFont(const Gfx::Font& f)
{
    _font.setValue(f);
    ++_generation;
}


StyleOptions::Iterator StyleOptions::begin() const
{
    return Iterator(_options.empty() ? 0 : &_options[0]);
}


StyleOptions::Iterator StyleOptions::end() const
{
    return Iterator(_options.empty() ? 0 : &_options[0] + _options.size());
}


const StyleOption* StyleOptions::find(const char* name) const
{
    for(const auto& opt : _options)
    {
        if( std::strcmp(opt->name(), name) == 0 )
            return opt;
    }

    return 0;
}


void StyleOptions::set(const char* name, const Pt::Any& value)
{
    for(auto opt : _options)
    {
        if( std::strcmp(opt->name(), name) == 0 )
        {
            opt->set(value);
            ++_generation;
            return;
        }
    }
}


FontOption::FontOption()
: _overrides(0)
{
}


bool FontOption::hasOverride() const
{
    return _overrides != 0;
}


const Gfx::Font* FontOption::font() const
{
    return _font.get();
}


void FontOption::setFont(const Gfx::Font& font)
{
    _font.reset( new Gfx::Font(font) );
    _overrides |= All;
}


void FontOption::setSize(std::size_t size)
{
    if( ! _font )
        _font.reset( new Gfx::Font );

    *_font = _font->withSize(size);
    _overrides |= Size;
}


void FontOption::setWeight(Gfx::Font::Weight weight)
{
    if( ! _font )
        _font.reset( new Gfx::Font );

    *_font = _font->withWeight(weight);
    _overrides |= Weight;
}


void FontOption::setSlant(Gfx::Font::Slant slant)
{
    if( ! _font )
        _font.reset( new Gfx::Font );

    *_font = _font->withSlant(slant);
    _overrides |= Slant;
}


Gfx::Font FontOption::getFont(const Gfx::Font& base) const
{
    if( ! _font )
        return base;

    if( _overrides & All )
        return *_font;

    Gfx::Font font(base);

    if( _overrides & Size )
        font = font.withSize(_font->size());

    if( _overrides & Weight )
        font = font.withWeight(_font->weight());

    if( _overrides & Slant )
        font = font.withSlant(_font->slant());

    return font;
}

} // namespace

} // namespace
