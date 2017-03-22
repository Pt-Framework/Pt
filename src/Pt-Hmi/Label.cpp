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
: _alignment(Alignment::Left)
, _hasImage(false)
, _hasRenderer(false)
{
}


Label::~Label()
{
}


Alignment Label::alignment() const
{
    return _alignment;
}


void Label::setAlignment(Alignment a)
{
    _alignment = a;
    invalidate();
}


const Pt::String& Label::text() const
{
    return _text;
}


void Label::setText(const Pt::String& text)
{
    _text = text;
    _hasImage = false;
    invalidate();
}


void Label::setImage(const Gfx::Image& image)
{
    _picture.set(image);
    _hasImage = true;
    invalidate();
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


Adjustment Label::adjustment() const
{
    Adjustment adjustment = Adjustment::Left;
    switch( _alignment )
    {
        default:
        case Alignment::TopLeft:
        case Alignment::Left:
        case Alignment::BottomLeft:
            adjustment = Adjustment::Left;
            break;

        case Alignment::Top:
        case Alignment::Center:
        case Alignment::Bottom:
            adjustment = Adjustment::Center;
            break;
        
        case Alignment::TopRight:
        case Alignment::Right:
        case Alignment::BottomRight:
            adjustment = Adjustment::Right;
            break;
    }

    return adjustment;
}


Gfx::SizeF Label::onAutoSize(const SizePolicy& policy) const
{
    double w = 0;
    double h = 0;

    if(_hasImage)
    {
        w = static_cast<double>( _picture.width() );
        h = static_cast<double>( _picture.height() );
    }
    else
    {
        Adjustment a = adjustment();

        TextBlock block;
        block.setAdjustment(a);
        
        if(policy.horizontal() == Pt::Hmi::SizePolicy::Fixed)
            block.setMaxWidth( policy.size().width() );

        block.layout(_text, _font);

        w = static_cast<double>( block.size().width() ); 
        h = static_cast<double>( block.size().height() );
    }

    return Gfx::SizeF( w + padding().leftRight(), 
                       h + padding().topBottom() );
}


void Label::layoutText()
{
    Adjustment a = adjustment();

    _textBlock.setMaxWidth( size().width() - padding().leftRight() );
    _textBlock.setAdjustment(a);
    _textBlock.layout(_text, _font);

    Gfx::PointF pos;

    switch( _alignment )
    {
        default:
        case Alignment::TopLeft:
        case Alignment::Top:
        case Alignment::TopRight:
        {
            pos.set(padding().left(), padding().top());
            break;
        }

        case Alignment::Left:
        case Alignment::Center:
        case Alignment::Right:
        {
            double height = size().height() - padding().topBottom();
            double y = (height - _textBlock.height()) / 2;
            pos.set(padding().left(), y + padding().top());
            break;
        }

        case Alignment::BottomLeft:
        case Alignment::Bottom:
        case Alignment::BottomRight:
        {
            double height = size().height() - padding().topBottom();
            double y = height - _textBlock.height();

            pos.set( padding().left(), padding().top() + y);
            break;
        }
    }

    _textBlock.setPosition(pos);
}


void Label::layoutImage()
{
    switch( _alignment )
    {
        default:
        case Alignment::TopLeft:
        {
            _imagePos.set(padding().left(), padding().top());
            break;
        }
        case Alignment::Top:
        {
            double width = size().width() - padding().leftRight();
            double x = (width - _picture.width()) / 2;

            _imagePos.set( padding().left() + x, padding().top() );
            break;
        }
        case Alignment::TopRight:
        {
            double width = size().width() - padding().leftRight();
            double x = width - _picture.width();

            _imagePos.set( padding().left() + x, padding().top() );
            break;
        }
        case Alignment::Left:
        {
            double height = size().height() - padding().topBottom();
            double y = (height - _picture.height()) / 2;

            _imagePos.set( padding().left(), padding().top() + y);
            break;
        }
        case Alignment::Center:
        {
            double width = size().width() - padding().leftRight();
            double x = (width - _picture.width()) / 2;
            
            double height = size().height() - padding().topBottom();
            double y = (height - _picture.height()) / 2;

            _imagePos.set( padding().left() + x, padding().top() + y);
            break;
        }
        case Alignment::Right:
        {
            double width = size().width() - padding().leftRight();
            double x = width - _picture.width();

            double height = size().height() - padding().topBottom();
            double y = (height - _picture.height()) / 2;

            _imagePos.set( padding().left() + x, padding().top() + y);
            break;
        }
        case Alignment::BottomLeft:
        {
            double height = size().height() - padding().topBottom();
            double y = height - _picture.height();

            _imagePos.set(padding().left(), padding().top() + y);
            break;
        }
        case Alignment::Bottom:
        {
            double width = size().width() - padding().leftRight();
            double x = (width - _picture.width()) / 2;

            double height = size().height() - padding().topBottom();
            double y = height - _picture.height();

            _imagePos.set( padding().left() + x, padding().top() + y);
            break;
        }
        case Alignment::BottomRight:
        {
            double width = size().width() - padding().leftRight();
            double x = width - _picture.width();

            double height = size().height() - padding().topBottom();
            double y = height - _picture.height();

            _imagePos.set( padding().left() + x, padding().top() + y);
            break;
        }
    }
}


void Label::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout();

    if(_hasImage)
        layoutImage();
    else
        layoutText();
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

    if(_hasImage)
        layoutImage();
    else
        layoutText();
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
    
    if(_hasImage)
    {
        painter.setCompositionMode(Gfx::CompositionMode::SourceOver);
        painter.drawPicture(_imagePos, _picture);
        painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);
    }
    else
    {
        TextBlock::ConstIterator it;
        for(it = _textBlock.begin(); it != _textBlock.end(); ++it)
        {
            const Pt::String& lineText = it->text();

            Gfx::PointF pos = _textBlock.position() + it->position();
            pos.addY( it->ascent() );

            _renderer->renderText(*this, options,  painter, rect,
                                  lineText, pos, _font, _textPen);
        }
    }
}

} // namespace

} // namespace
