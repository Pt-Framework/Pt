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
#include <Pt/Hmi/LineEditor.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/FontMetrics.h>

namespace Pt {

namespace Hmi {

Label::Label()
: _textAlignment(MiddleCenter)
, _hasRenderer(false)
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


const Gfx::Brush* Label::background() const
{
    return _background ? _background.get() : 0;
}


void Label::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    update();
}


const Gfx::Pen* Label::contour() const
{
    return _contour ? _contour.get() : 0;
}


void Label::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    update();
}


const Gfx::Color& Label::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void Label::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    invalidate();
}


const std::string& Label::font() const
{
    return _fontName ? *_fontName
                     : Application::instance().styleOptions().font().name();
}


void Label::setFont(const std::string& fontName)
{
    _fontName.reset( new std::string(fontName) );
    invalidate();
}


std::size_t Label::fontSize() const
{

    return _fontSize ? *_fontSize
                     : Application::instance().styleOptions().font().size();
}


void Label::setFontSize(const std::size_t s)
{
    _fontSize.reset( new std::size_t(s) );
    invalidate();
}


Gfx::Font::Style Label::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void Label::setFontStyle(Gfx::Font::Style style)
{
    _fontStyle.reset( new Gfx::Font::Style(style) );
    invalidate();
}


void Label::setRenderer(LabelRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


Gfx::SizeF Label::onAutoSize() const
{
    Gfx::FontMetrics fm = Hmi::Painter::fontMetrics(_font, _text);

    return Gfx::SizeF( fm.width() + padding().leftRight(), 
                       fm.height() + padding().topBottom() );
}


void Label::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _textPen = textColor();
    _font = Gfx::Font(font(), fontSize(), fontStyle());

    if( ! _hasRenderer )
        _renderer.reset( style.get<LabelRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepare(*this, options, _font, _textPen);
}


void Label::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer)
        return;

    Painter painter(surface);
    painter.setClip(rect);

    const Gfx::Brush* brush = background();
    if(brush)
    {
        _renderer->renderBackground(*this, options,
                                    painter, rect, *brush);
    }

    const Gfx::Pen* pen = contour();
    if( pen )
    {
        _renderer->renderFrame(*this, options,
                               painter, rect, *pen);
    }
    
    //Gfx::PointF pos = textPosition();
    //_renderer->renderText(*this, options,  painter, rect,
    //                      _text, pos, _font, _textPen);

    TextBlock block;
    block.setMaxWidth( size().width() );
    block.setText(_text, _font);

    std::vector<TextLine>::const_iterator it;
    for(it = block.lines().begin(); it != block.lines().end(); ++it)
    {
        const Pt::String& lineText = it->text();

        Gfx::PointF pos = it->position();
        pos.addY( it->ascent() );

        _renderer->renderText(*this, options,  painter, rect,
                              lineText, pos, _font, _textPen);
    }


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
