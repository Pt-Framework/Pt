/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/Hmi/Label.h>
#include <Pt/Gfx/FontMetrics.h>

namespace Pt {

namespace Hmi {

Label::Label()
:  _textAlignment(MiddleCenter)
, _font()
, _hasFont(false)
, _textColor( Gfx::Color::fromRgb8(0,0,0) )
, _hasTextColor(false)
{
}


Label::~Label()
{
}


const Pt::String& Label::text() const
{
    return _text;
}


void Label::setText(const Pt::String& text)
{
    _text = text;
    invalidate();
}


Label::Alignment Label::textAlignment() const
{
    return _textAlignment;
}


void Label::setTextAlignment(Alignment a)
{
    _textAlignment = a;
    update();
}


const Gfx::Color* Label::textColor() const
{
    return _hasTextColor ? &_textColor : 0;
}


void Label::setTextColor(const Gfx::Color& color)
{
    _textColor = color;
    _hasTextColor = true;
    update();
}


const Gfx::Font* Label::font() const
{ 
    return _hasFont ? &_font : 0;
}


void Label::setFont(const Gfx::Font& f)
{
    _font = f;
    _hasFont = true;
    invalidate();
}


Gfx::SizeF Label::onAutoSize() const
{
    Gfx::FontMetrics fm = Hmi::Painter::fontMetrics( _font, _text);

    return Gfx::SizeF( fm.width() + padding().leftRight(), 
                       fm.height() + padding().topBottom() );
}


void Label::onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Panel::onPaintBackground(surface, updateRect);

    const LabelRenderer* renderer = getFacet<LabelRenderer>();
    if(renderer)
        renderer->renderBackground(*this, surface, updateRect);
}


void Label::onPaintContent(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Panel::onPaintContent(surface, updateRect);

    const LabelRenderer* renderer = getFacet<LabelRenderer>();
    if(renderer)
        renderer->renderContent(*this, surface, updateRect);
}

} // namespace

} // namespace
