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

#include <Pt/Forms/Label.h>
#include <Pt/Forms/LineEditor.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/TextMetrics.h>

namespace Pt {

namespace Forms {

Label::Label()
: _alignment(Alignment::Left)
, _iconInvalid(false)
, _hasRenderer(false)
, _fontOverride(0)
, _styleGeneration(0)
, _styleInvalid(true)
{
    _font = Application::instance().styleOptions().font();
    _styleGeneration = Application::instance().styleOptions().generation();
}


Label::~Label()
{
}


void Label::onConnect(Screen& screen)
{
    Base::onConnect(screen);
}


Alignment Label::alignment() const
{
    return _alignment;
}


void Label::setAlignment(Alignment a)
{
    _alignment = a;
    relayout();
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
    
    _pixmap.reset( Pt::Gfx::Image() );

    relayout();
}


void Label::setIcon(const Icon& icon, const Gfx::SizeF& iconSize)
{
    _icon = icon;
    _iconSize = iconSize;
    _iconInvalid = true;

    // ICON-UPDATE
    invalidate();
    ////relayout();
}


Pixmap& Label::getIconPixmap()
{
    // ICON-UPDATE
    //if(_iconInvalid)
    //{
    //    Gfx::SizeF scaledSize = scaling().toPhysical(_iconSize);
    //    const Pt::Gfx::Image& iconImage = _icon.getImage(scaledSize);
    //    _pixmap.set(iconImage);

    //    _iconInvalid = false;
    //}

    return _pixmap;
}


const Gfx::Brush* Label::background() const
{
    return _background ? _background.get() : 0;
}


void Label::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    repaint();
}


const Gfx::Pen* Label::contour() const
{
    return _contour ? _contour.get() : 0;
}


void Label::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );

    _styleInvalid = true;
    invalidate();
}


const Gfx::Color& Label::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void Label::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );

    _styleInvalid = true;
    invalidate();
}


const Gfx::Font& Label::font() const
{
    return _font;
}


void Label::setFont(const Gfx::Font& font)
{
    _customFont = font;
    _fontOverride = OverrideAll;

    _styleInvalid = true;
    invalidate();
}


Gfx::Font Label::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( _fontOverride == 0 )
        return base;

    if( _fontOverride == OverrideAll )
        return _customFont;

    std::size_t sz = (_fontOverride & OverrideSize) ? _customFont.size()
                                                    : base.size();
    Gfx::Font::Weight wt = (_fontOverride & OverrideWeight) ? _customFont.weight()
                                                            : base.weight();
    Gfx::Font::Slant sl = (_fontOverride & OverrideSlant) ? _customFont.slant() 
                                                          : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void Label::setFontSize(std::size_t size)
{
    _customFont = Gfx::Font(_customFont.family(), size,
                              _customFont.weight(), _customFont.slant(),
                              _customFont.stretch());
    _fontOverride |= OverrideSize;

    _styleInvalid = true;
    invalidate();
}


void Label::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                              weight, _customFont.slant(),
                              _customFont.stretch());
    _fontOverride |= OverrideWeight;

    _styleInvalid = true;
    invalidate();
}


void Label::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                              _customFont.weight(), slant,
                              _customFont.stretch());
    _fontOverride |= OverrideSlant;

    _styleInvalid = true;
    invalidate();
}


void Label::setRenderer(LabelRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    _styleInvalid = true;
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


Gfx::SizeF Label::onMeasure(PaintContext& ctx, const SizePolicy& policy)
{
    //std::clog << "label measure " << _text.narrow() << std::endl;

    double w = 0;
    double h = 0;

    const Gfx::Scaling& scaling = this->scaling();

    if( _icon.empty() )
    {
        Adjustment a = adjustment();

        if( ! _text.empty() )
        {
            TextBlock block;
            block.setAdjustment(a);

            // TODO: set max width if text wrap is enabled
            // NOTE: abbreviate text if text wrap is off and width is too small
            Painter _painter( ctx );
            _painter.setFont(_font);

            block.setMaxWidth(policy.size().width());
            block.setLineSpacing(scaling.align(_font.size() / 3));
            block.layout(_painter, _text);

            w = block.size().width() + scaling.toLogical(0.5);
            h = block.size().height() + scaling.toLogical(0.5);

            w = scaling.align(w);
            h = scaling.align(h);
        }
    }
    else
    {       
        Pixmap& picture = getIconPixmap();

        Gfx::SizeF pictureSize = scaling.toLogical( picture.size() );

        w = static_cast<double>( pictureSize.width() );
        h = static_cast<double>( pictureSize.height() );
    }

    return Gfx::SizeF( w + padding().leftRight(), 
                       h + padding().topBottom() );
}


void Label::layoutText(PaintContext& ctx)
{
    if( _text.empty() )
        return;

    Adjustment a = adjustment();

    Painter _painter( ctx );
    _painter.setFont(_font);

    const Gfx::Scaling& scaling = this->scaling();

    _textBlock.setMaxWidth( size().width() - padding().leftRight() );
    _textBlock.setAdjustment(a);
    _textBlock.setLineSpacing(scaling.align(_font.size() / 3.0));
    _textBlock.layout(_painter, _text);

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
            double y = (height - scaling.align( _textBlock.height())) / 2;
            pos.set(padding().left(), y + padding().top());
            break;
        }

        case Alignment::BottomLeft:
        case Alignment::Bottom:
        case Alignment::BottomRight:
        {
            double height = size().height() - padding().topBottom();
            double y = height - scaling.align(_textBlock.height());

            pos.set( padding().left(), padding().top() + y);
            break;
        }
    }

    _textBlock.setPosition(pos);
}


void Label::layoutImage()
{
    Pixmap& picture = getIconPixmap();
    Gfx::SizeF pictureSize = scaling().toLogical( picture.size() );

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


void Label::onLayout(PaintContext& ctx, const Gfx::RectF& rect)
{
    //std::clog  << " layout " << _text.narrow()<< this << std::endl;

    Base::onLayout(ctx, rect);
    
    if( _icon.empty() )
        layoutText(ctx);
    else
        layoutImage();

    repaint();
}


void Label::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);

    if( ! _icon.empty() )
    {
        _iconInvalid = true;
        relayout();
        
        ////invalidate();
    }
}


void Label::onResizeEvent(const ResizeEvent& ev)
{
    //std::clog << " resize " << _text.narrow() << this << std::endl;
    Base::onResizeEvent(ev);
}


void Label::onInvalidate()
{
    Base::onInvalidate();

    bool needsRelayout = false;

    std::size_t gen = Application::instance().styleOptions().generation();
    if(_styleGeneration != gen)
    {
        _styleGeneration = gen;
        _styleInvalid = true;
    }

    if(_styleInvalid)
    {
        _styleInvalid = false;

        const StyleOptions& options = Application::instance().styleOptions();
        const Style& style = Application::instance().style();

        _textPen = textColor();
        _font = getFont();

        const Gfx::Pen* pen = contour();
        if(pen)
        {
            _pen = *pen;
        }

        if( ! _hasRenderer )
            _renderer.reset( style.get<LabelRenderer>() );

        if( _renderer )
            _renderer->prepare(*this, options, _font, _pen, _textPen);

        needsRelayout = true;
    }

    // ICON-UPDATE
    if(_iconInvalid)
    {
        _iconInvalid = false;

        Gfx::SizeF scaledSize = scaling().toPhysical(_iconSize);
        const Pt::Gfx::Image& iconImage = _icon.getImage(scaledSize);
        _pixmap.reset(iconImage);

        needsRelayout = true;
    }

    if(needsRelayout)
        relayout();
}


void Label::onPaint(PaintContext& context, 
                    const Gfx::RectF& rect)
{
    //std::clog << " paint " << _text.narrow() << this << std::endl;

    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer)
        return;

    Forms::Painter painter(context);
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
        Pixmap& picture = getIconPixmap();

        painter.setCompositionMode(Gfx::CompositionMode::SourceOver);
        painter.drawPixmap(_iconPos, picture);
    }

    //if(pen)
    //{
    //    painter.beginPath();
    //    painter.moveTo( Gfx::PointF(0, 0) );
    //    painter.lineTo( Gfx::PointF(size().width(), size().height() - 1) );
    //    painter.lineTo( Gfx::PointF(0, size().height() - 1) );
    //    painter.lineTo( Gfx::PointF(0, 0) );
    //    painter.closePath();
    //    painter.fillPath();
    //    painter.drawPath();
    //}
}

} // namespace

} // namespace
