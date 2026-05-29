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

namespace Pt {

namespace Forms {

StyleOptions::StyleOptions()
: _background(Gfx::Color(229, 229, 229))
, _foreground(Gfx::Color(210, 210, 210))
, _contour(Gfx::Color(150, 150, 150))
, _accentColor(20, 140, 250)
, _viewBackground(Gfx::Color(255, 255, 255))
, _highlightColor(190, 190, 190)
, _textBackground(Gfx::Color(255, 255, 255))
, _textColor(0, 0, 0)
, _highlightedTextColor(0, 0, 0)
, _font(Gfx::Font("", 10))
, _generation(0)
{
}


StyleOptions::StyleOptions(const StyleOptions& o)
: _background(o._background )
, _foreground(o._foreground)
, _contour(o._contour)
, _accentColor(o._accentColor)
, _viewBackground(o.viewBackground() )
, _highlightColor(o._highlightColor)
, _textBackground(o._textBackground )
, _textColor(o._textColor)
, _highlightedTextColor( o.highlightedTextColor() )
, _font(o._font)
, _generation(o._generation)
{
}


StyleOptions::~StyleOptions()
{
}


StyleOptions& StyleOptions::operator=(const StyleOptions& o)
{
    if(this == &o)
        return *this;

    _background = o._background;
    _foreground = o._foreground;
    _contour = o._contour;
    _accentColor = o._accentColor;
    _viewBackground = o._viewBackground;
    _textBackground = o._textBackground ;
    _highlightColor = o._highlightColor;
    _textColor = o._textColor;
    _highlightedTextColor = o._highlightedTextColor;
    _font = o._font;
    ++_generation;

    return *this;
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
