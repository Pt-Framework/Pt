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
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/FontMetrics.h>

namespace Pt {

namespace Hmi {

Label::Label()
: _alignment(Alignment::Left)
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
    _icon.clear();
    _iconSize.set(0, 0);
    invalidate();
}


void Label::setIcon(const Icon& icon, const Gfx::SizeF& iconSize)
{
    _icon     = icon;
    _iconSize = iconSize;
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


const std::string& Label::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void Label::setFontStyle(const std::string& style)
{
    _fontStyle.reset( new std::string(style) );
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


Gfx::SizeF Label::onMeasure(const SizePolicy& policy)
{
    //std::clog << _text.narrow() << " measure " << this << std::endl;

    double w = 0;
    double h = 0;

    if(_icon.empty())
    {
        Adjustment a = adjustment();

        TextBlock block;
        block.setAdjustment(a);

        // TODO: set max width if text wrap is enabled
        // NOTE: abbreviate text if text wrap is off and width is too small

        block.setMaxWidth(policy.size().width());
        block.setLineSpacing(align(_font.size() / 3));
        block.layout(_text, _font);

        w = align(block.size().width());
        h = align(block.size().height());
    }
    else
    {
        Gfx::SizeF pictureSize = toLogical(Gfx::SizeF(_picture.width(), _picture.height()));

        w = static_cast<double>( pictureSize.width() );
        h = static_cast<double>( pictureSize.height() );
    }

    return Gfx::SizeF( w + padding().leftRight(), 
                       h + padding().topBottom() );
}


void Label::layoutText()
{
    Adjustment a = adjustment();

    _textBlock.setMaxWidth( size().width() - padding().leftRight() );
    _textBlock.setAdjustment(a);
    _textBlock.setLineSpacing(align(_font.size() / 3.0));
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
            double y = (height -align( _textBlock.height())) / 2;
            pos.set(padding().left(), y + padding().top());
            break;
        }

        case Alignment::BottomLeft:
        case Alignment::Bottom:
        case Alignment::BottomRight:
        {
            double height = size().height() - padding().topBottom();
            double y = height - align(_textBlock.height());

            pos.set( padding().left(), padding().top() + y);
            break;
        }
    }

    _textBlock.setPosition(pos);
}


void Label::layoutImage()
{
    Gfx::SizeF pictureSize = toLogical(Gfx::SizeF(_picture.width(), _picture.height()));

    switch( _alignment )
    {
        default:
        case Alignment::TopLeft:
        {
            _iconPos.set(padding().left(), padding().top());
            break;
        }
        case Alignment::Top:
        {
            double width = size().width() - padding().leftRight();
            double x = (width - pictureSize.width()) / 2;

            _iconPos.set( padding().left() + x, padding().top() );
            break;
        }
        case Alignment::TopRight:
        {
            double width = size().width() - padding().leftRight();
            double x = width - pictureSize.width();

            _iconPos.set( padding().left() + x, padding().top() );
            break;
        }
        case Alignment::Left:
        {
            double height = size().height() - padding().topBottom();
            double y = (height - pictureSize.height()) / 2;

            _iconPos.set( padding().left(), padding().top() + y);
            break;
        }
        case Alignment::Center:
        {
            double width = size().width() - padding().leftRight();
            double x = (width - pictureSize.width()) / 2;
            
            double height = size().height() - padding().topBottom();
            double y = (height - pictureSize.height()) / 2;

            _iconPos.set( padding().left() + x, padding().top() + y);
            break;
        }
        case Alignment::Right:
        {
            double width = size().width() - padding().leftRight();
            double x = width - pictureSize.width();

            double height = size().height() - padding().topBottom();
            double y = (height - pictureSize.height()) / 2;

            _iconPos.set( padding().left() + x, padding().top() + y);
            break;
        }
        case Alignment::BottomLeft:
        {
            double height = size().height() - padding().topBottom();
            double y = height - pictureSize.height();

            _iconPos.set(padding().left(), padding().top() + y);
            break;
        }
        case Alignment::Bottom:
        {
            double width = size().width() - padding().leftRight();
            double x = (width - pictureSize.width()) / 2;

            double height = size().height() - padding().topBottom();
            double y = height - pictureSize.height();

            _iconPos.set( padding().left() + x, padding().top() + y);
            break;
        }
        case Alignment::BottomRight:
        {
            double width = size().width() - padding().leftRight();
            double x = width - pictureSize.width();

            double height = size().height() - padding().topBottom();
            double y = height - pictureSize.height();

            _iconPos.set( padding().left() + x, padding().top() + y);
            break;
        }
    }
}


void Label::onLayout(const Gfx::RectF& rect)
{
    //std::clog << _text.narrow() << " layout " << this << std::endl;

    Base::onLayout(rect);
    
    if( _icon.empty() )
        layoutText();
    else
        layoutImage();
}


void Label::onResizeEvent(const ResizeEvent& ev)
{
    //std::clog << _text.narrow() << " resize " << this << std::endl;
    Base::onResizeEvent(ev);
}


void Label::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _textPen = textColor();
    _font = Gfx::Font(font(), fontSize(), fontStyle());

    const Gfx::Pen* pen = contour();
    if(pen)
    {
        _pen = *pen;
    }

    if( ! _hasRenderer )
        _renderer.reset( style.get<LabelRenderer>() );

    if( ! _renderer )
        return;

    _renderer->prepare(*this, options, _font, _pen, _textPen);

    if( _icon.empty() )
    {
        _picture.set( Pt::Gfx::Image() );
        //layoutText();
    }
    else
    {
        const Gfx::SizeF scaledSize = toPhysical(_iconSize);
        const Pt::Gfx::Image& iconImage = _icon.getImage(scaledSize);
        _picture.set(iconImage);

        //layoutImage();
    }
}


void Label::onPaint(Gfx::PaintSurface& surface, 
                    const Gfx::RectF& rect)
{
    //std::clog << _text.narrow() << " paint " << this << std::endl;

    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer)
        return;

    Gfx::Painter painter(surface);
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
                               painter, rect, _pen);
    }
    
    if(_icon.empty())
    {
        TextBlock::ConstIterator it;
        for (it = _textBlock.begin(); it != _textBlock.end(); ++it)
        {
            const Pt::String& lineText = it->text();
            double ascent = it->ascent();

            Gfx::PointF pos = _textBlock.position() + it->position();
            pos.addY(ascent);

            _renderer->renderText(*this, options, painter, rect,
                lineText, pos, _font, _textPen);
        }
    }
    else
    {
        painter.setCompositionMode(Gfx::CompositionMode::SourceOver);
        painter.drawSurface(_iconPos, _picture);
        painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);
    }
}

} // namespace

} // namespace
