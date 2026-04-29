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
    _background = o._background;
    _foreground = o._foreground;
    _contour = o._contour;
    _textBackground = o._textBackground ;
    _highlightColor = o._highlightColor;
    _textColor = o._textColor;
    _font = o._font;
    ++_generation;
    
    return *this;
}

} // namespace

} // namespace
