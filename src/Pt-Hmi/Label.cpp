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
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/FontMetrics.h>

namespace Pt {

namespace Hmi {

Label::Label()
: _textAlignment(MiddleCenter)
, _hasBackground(false)
, _hasFrame(false)
, _hasFontName(false)
, _hasFontSize(false)
, _hasFontStyle(false)
, _hasTextPen(false)
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


void Label::setBackground(const Gfx::Brush& b)
{
    _background = b;
    _hasBackground = true;
    update();
}


void Label::setFrame(const Gfx::Color& color)
{
    _frameColor = color;
    _hasFrame = true;
    update();
}


void Label::setTextColor(const Gfx::Color& color)
{
    _textPen = color;
    _hasTextPen = true;
    update();
}


void Label::setFont(const Gfx::Font& f)
{
    _font = f;
    _hasFontName = true;
    _hasFontSize = true;
    _hasFontStyle = true;
    
    invalidate();
}


void Label::setFontSize(const std::size_t s)
{
    _font = Gfx::Font( _font.name(), s, _font.style() );
    _hasFontSize = true;
    
    invalidate();
}


void Label::setFontStyle(Gfx::Font::Style style)
{
    _font = Gfx::Font( _font.name(), _font.style(), style );
    _hasFontStyle = true;
    
    invalidate();
}


Gfx::SizeF Label::onAutoSize() const
{
    Gfx::FontMetrics fm = Hmi::Painter::fontMetrics( _font, _text);

    return Gfx::SizeF( fm.width() + padding().leftRight(), 
                       fm.height() + padding().topBottom() );
}


void Label::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions* options = getFacet<StyleOptions>();
    if( ! options )
      return;

    const LabelRenderer* renderer = getFacet<LabelRenderer>();
    if( ! renderer )
        return;

    Gfx::Font font;
    Gfx::Pen textPen;
    renderer->prepare(*this, *options, font, textPen);

    const std::string& fontName = _hasFontName ? _font.name()
                                               : font.name();

    Gfx::Font::Style fontStyle = _hasFontStyle ? _font.style()
                                               : font.style();

    std::size_t fontSize = _hasFontSize ? _font.size()
                                        : font.size();

    _font = Gfx::Font(fontName, fontSize, fontStyle);

    if( ! _hasTextPen )
        _textPen = textPen;
}


void Label::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions* options = getFacet<StyleOptions>();
    if( ! options )
        return;

    const LabelRenderer* renderer = getFacet<LabelRenderer>();
    if( ! renderer)
        return;

    Painter painter(surface);
    painter.setClip(rect);

    if( _hasBackground )
    {
        renderer->renderBackground(*this, *options,
                                   painter, rect, _background);
    }

    if( _hasFrame )
    {
        renderer->renderFrame(*this, *options,
                              painter, rect, _frameColor);
    }
    
    Gfx::PointF pos = textPosition();

    renderer->renderText(*this, *options,  painter, rect,
                         text(), pos, _font, _textPen);
}


Gfx::PointF Label::textPosition() const
{
    Gfx::PointF pos(0, 0);
    Gfx::FontMetrics metric = Hmi::Painter::fontMetrics(_font, _text);

    switch( _textAlignment )
    {
        case Label::TopLeft:
        {
            pos = Gfx::PointF(0, metric.ascent());
            break;
        }
        
        case Label::TopCenter:
        {
            const double widthHalf     = size().width() / 2;
            const double textWidthHalf = metric.width() / 2;
            pos = Gfx::PointF(widthHalf - textWidthHalf, metric.ascent());
            break;
        }
        break;

        case Label::TopRight:
        {
            const double width     = size().width();
            const double textWidth = metric.width();
            pos = Gfx::PointF(width - textWidth, metric.ascent());
            break;
        }
        break;

        case Label::MiddleLeft:
        {
            const double heightHalf     = size().height() / 2;
            const double textHeightHalf = metric.height() / 2;
            pos = Gfx::PointF(0, (heightHalf - textHeightHalf) + metric.ascent());
            break;
        }

        default:
        case Label::MiddleCenter:
        {            
            const double widthHalf      = size().width() / 2;
            const double heightHalf     = size().height() / 2;
            const double textWidthHalf  = metric.width() / 2;
            const double textHeightHalf = metric.height() / 2;
            pos = Gfx::PointF(widthHalf - textWidthHalf, 
                              heightHalf - textHeightHalf + metric.ascent());
            break;
        }

        case Label::MiddleRight:
        {
            const double width          = size().width();
            const double textWidth      = metric.width();
            const double heightHalf     = size().height()/2;
            const double textHeightHalf = metric.height()/2;
            pos = Gfx::PointF(width - textWidth, 
                              heightHalf - textHeightHalf + metric.ascent());
            break;
        }

        case Label::BottomLeft:
        {
            const double height     = size().height();
            const double textHeight = metric.height();
            pos = Gfx::PointF(0, height- textHeight + metric.ascent());
            break;
        }

        case Label::BottomCenter:
        {
            const double widthHalf     = size().width() / 2;
            const double textWidthHalf = metric.width() / 2;
            const double height        = size().height();
            const double textHeight    = metric.height();
            pos = Gfx::PointF(widthHalf - textWidthHalf, 
                              height - textHeight + metric.ascent());
            break;
        }

        case Label::BottomRight:
        {
            const double width      = size().width();
            const double textWidth  = metric.width();
            const double height     = size().height();
            const double textHeight = metric.height();
            pos = Gfx::PointF(width - textWidth, 
                              height- textHeight + metric.ascent());
            break;
        }
    }

    return pos;
}

} // namespace

} // namespace
