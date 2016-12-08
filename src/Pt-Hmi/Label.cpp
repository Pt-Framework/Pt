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
:  _textAlignment(MiddleCenter)
, _hasBackground(false)
, _customBackground(false)
, _hasFrame(false)
, _customFrame(false)
, _font()
, _hasFont(false)
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


Gfx::PointF Label::textPosition(const Gfx::Font& font) const
{
    Gfx::PointF pos(0, 0);
    Gfx::FontMetrics metric = Hmi::Painter::fontMetrics(font, _text);

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


void Label::setBackground(const Gfx::Brush& b)
{
    _background = b;
    _hasBackground = true;
    _customBackground = true;
    update();
}


void Label::setBackground(bool b)
{
    _hasBackground = b;

    if( ! _hasBackground )
    {
        if(_customBackground)
            _background = Gfx::Brush();
        
        _customBackground = false;
    }

    update();
}


void Label::setFrame(const Gfx::Color& color)
{
    _frameColor = color;
    _hasFrame = true;
    _customFrame = true;
    update();
}


void Label::setFrame(bool b)
{
    _hasFrame = b;

    if( ! _hasFrame )
    {       
        _customFrame = false;
    }

    update();
}


void Label::setTextColor(const Gfx::Color& color)
{
    _textColor = color;
    _hasTextColor = true;
    update();
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
        const Gfx::Brush& background = _customBackground ? _background
                                                         : options->background();

        renderer->renderBackground(*this, *options,
                                   painter, rect, background);
    }

    if( _hasFrame )
    {
        const Gfx::Color& frameColor = _customFrame ? _frameColor
                                                    : options->contourColor();

        renderer->renderFrame(*this, *options,
                              painter, rect, frameColor);
    }

    const Gfx::Font& font = _hasFont ? _font
                                     : options->font();

    const Gfx::Color& textColor = _hasTextColor ? _textColor
                                                : options->textColor(); 
    
    Gfx::PointF pos = textPosition(font);

    renderer->renderText(*this, *options,  painter, rect,
                         text(), pos, font, textColor);
}

} // namespace

} // namespace
