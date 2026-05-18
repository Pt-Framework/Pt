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
#include <Pt/Forms/TextBlock.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/TextMetrics.h>

namespace Pt {

namespace Forms {

Label::Label()
: _alignment(Alignment::Left)
, _iconInvalid(false)
, _customRenderer(false)
, _hasBackground(false)
, _hasFrame(false)
, _fontOverride(0)
, _styleGeneration(0)
, _styleInvalid(false)
{
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

    invalidate();
}


const Gfx::Brush* Label::background() const
{
    if( ! _hasBackground )
        return 0;

    if( _renderer )
        return _renderer->background();

    return &Application::instance().styleOptions().background();
}


void Label::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _hasBackground = true;

    if( PanelRenderer* renderer = getRenderer() )
        renderer->setBackground(*_background);

    invalidate();
}


void Label::setBackground(bool b)
{
    _hasBackground = b;
    invalidate();
}


const Gfx::Pen* Label::contour() const
{
    if( ! _hasFrame )
        return 0;

    if( _renderer )
        return _renderer->contour();

    return &Application::instance().styleOptions().contour();
}


void Label::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _hasFrame = true;

    if( PanelRenderer* renderer = getRenderer() )
        renderer->setContour(*_contour);

    invalidate();
}


void Label::setFrame(bool b)
{
    _hasFrame = b;
    invalidate();
}


const Gfx::Color& Label::textColor() const
{
    if( _renderer )
        return _renderer->textColor();

    return Application::instance().styleOptions().textColor();
}


void Label::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );

    if( PanelRenderer* renderer = getRenderer() )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    invalidate();
}


const Gfx::Font& Label::font() const
{
    if( _renderer )
        return _renderer->font();

    return Application::instance().styleOptions().font();
}


void Label::setFont(const Gfx::Font& font)
{
    _customFont = font;
    _fontOverride = OverrideAll;

    if( PanelRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

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

    if( PanelRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void Label::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                              weight, _customFont.slant(),
                              _customFont.stretch());
    _fontOverride |= OverrideWeight;

    if( PanelRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void Label::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                              _customFont.weight(), slant,
                              _customFont.stretch());
    _fontOverride |= OverrideSlant;

    if( PanelRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void Label::setRenderer(PanelRenderer* renderer)
{
    _renderer.reset(renderer);
    _customRenderer = renderer != 0;

    if( renderer )
        applyRenderer(renderer);

    invalidate();
}


PanelRenderer* Label::getRenderer()
{
    if( ! _customRenderer )
    {
        const Style& style = Application::instance().style();
        PanelRenderer* proto = style.get<PanelRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );
        _customRenderer = true;
    }

    return _renderer.get();
}


void Label::applyRenderer(PanelRenderer* renderer)
{
    if( _background )
        renderer->setBackground( *_background );

    if( _contour )
        renderer->setContour( *_contour );

    if( _textColor )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    if( _fontOverride )
        renderer->setFont( getFont() );
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


void Label::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);

    if( ! _icon.empty() )
    {
        _iconInvalid = true;
        relayout();
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

    if( ! _renderer )
    {
        bool hasOverride = _background || _contour || _textColor || _fontOverride;
        if(hasOverride)
        {
            if( PanelRenderer* renderer = getRenderer() )
                applyRenderer(renderer);
        }
        else
        {
            _renderer.reset( Application::instance().style().get<PanelRenderer>() );
        }
    }

    std::size_t gen = Application::instance().styleOptions().generation();
    _styleGeneration = gen;

    bool needsRelayout = _styleGeneration != gen;

    if(_iconInvalid)
    {
        _iconInvalid = false;

        if( ! _icon.empty() )
        {
            Gfx::SizeF scaledSize = scaling().toPhysical(_iconSize);
            const Pt::Gfx::Image& iconImage = _icon.getImage(scaledSize);
            _pixmap.reset(iconImage);
        }
        else
        {
            _pixmap.reset( Pt::Gfx::Image() );
        }

        needsRelayout = true;
    }

    if(needsRelayout)
        relayout();
}


Gfx::SizeF Label::onMeasure(const SizePolicy& policy)
{
    if( ! _renderer )
        return Gfx::SizeF(0, 0);

    Gfx::SizeF contentSize;

    if( _text.empty() )
    {
        if( _iconSize.isNull() )
            _measuredIconSize = surface().scaling().toLogical( _pixmap.size() );
        else
            _measuredIconSize = _iconSize;

        contentSize = _measuredIconSize;
    }
    else
    {
        const Gfx::Scaling& scaling = surface().scaling();

        const Painter& painter = _renderer->textPainter( surface() );

        TextBlock block;
        block.setAdjustment( adjustment() );
        block.setMaxWidth( policy.size().width() );
        block.setLineSpacing( scaling.align( _renderer->font().size() / 3.0 ) );
        block.layout(painter, _text);

        double w = block.size().width() + scaling.toLogical(0.5);
        double h = block.size().height() + scaling.toLogical(0.5);

        _measuredIconSize = Gfx::SizeF();
        contentSize = Gfx::SizeF( scaling.align(w), scaling.align(h) );
    }

    Gfx::SizeF paddedSize( contentSize.width() + padding().leftRight(),
                           contentSize.height() + padding().topBottom() );

    return _renderer->measureFrame( surface(), paddedSize );
}


void Label::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    if( ! _renderer )
        return;

    Gfx::RectF contentRect = _renderer->layoutFrame( surface(),
                                                     Gfx::RectF(size()) );

    double left = contentRect.left() + padding().left();
    double top = contentRect.top() + padding().top();
    double contentWidth = contentRect.width() - padding().leftRight();
    double contentHeight = contentRect.height() - padding().topBottom();

    Gfx::RectF innerRect( Gfx::PointF(left, top),
                          Gfx::SizeF(contentWidth, contentHeight) );

    if( _text.empty() )
    {
        double picW = _measuredIconSize.width();
        double picH = _measuredIconSize.height();
        double x = 0;
        double y = 0;

        switch( static_cast<Pt::uint32_t>(_alignment) )
        {
            default:
            case Alignment::TopLeft:
                x = 0; y = 0;
                break;
            case Alignment::Top:
                x = (innerRect.width() - picW) / 2; y = 0;
                break;
            case Alignment::TopRight:
                x = innerRect.width() - picW; y = 0;
                break;
            case Alignment::Left:
                x = 0; y = (innerRect.height() - picH) / 2;
                break;
            case Alignment::Center:
                x = (innerRect.width() - picW) / 2;
                y = (innerRect.height() - picH) / 2;
                break;
            case Alignment::Right:
                x = innerRect.width() - picW;
                y = (innerRect.height() - picH) / 2;
                break;
            case Alignment::BottomLeft:
                x = 0; y = innerRect.height() - picH;
                break;
            case Alignment::Bottom:
                x = (innerRect.width() - picW) / 2;
                y = innerRect.height() - picH;
                break;
            case Alignment::BottomRight:
                x = innerRect.width() - picW;
                y = innerRect.height() - picH;
                break;
        }

        _iconPos = surface().scaling().align( Gfx::PointF(innerRect.left() + x, 
                                                          innerRect.top() + y) );
    }
    else
    {
        const Gfx::Scaling& scaling = surface().scaling();

        const Painter& painter = _renderer->textPainter( surface() );

        _textBlock.setMaxWidth( innerRect.width() );
        _textBlock.setAdjustment( adjustment() );
        _textBlock.setLineSpacing( scaling.align(
            _renderer->font().size() / 3.0 ) );
        _textBlock.layout(painter, _text);

        double blockH = scaling.align( _textBlock.height() );
        double y = 0;

        switch( static_cast<Pt::uint32_t>(_alignment) )
        {
            default:
            case Alignment::TopLeft:
            case Alignment::Top:
            case Alignment::TopRight:
                y = 0;
                break;

            case Alignment::Left:
            case Alignment::Center:
            case Alignment::Right:
                y = (innerRect.height() - blockH) / 2;
                break;

            case Alignment::BottomLeft:
            case Alignment::Bottom:
            case Alignment::BottomRight:
                y = innerRect.height() - blockH;
                break;
        }

        Gfx::PointF pos( innerRect.left(), innerRect.top() + y );
        _textBlock.setPosition(pos);
    }

    repaint();
}


void Label::onPaint(PaintContext& context, 
                    const Gfx::RectF& /*rect*/)
{
    if( ! _renderer )
        return;

    Gfx::RectF widgetRect( size() );

    if( _hasBackground )
        _renderer->renderBackground(context, widgetRect, styleFlags());

    if( _hasFrame )
        _renderer->renderFrame(context, widgetRect, styleFlags());

    if( _icon.empty() )
    {
        TextBlock::ConstIterator it;
        for(it = _textBlock.begin(); it != _textBlock.end(); ++it)
        {
            const Pt::String& lineText = it->text();
            double ascent = it->ascent();

            Gfx::PointF pos = _textBlock.position() + it->position();
            pos.addY(ascent);

            _renderer->renderText(context, widgetRect, lineText, pos,
                                  styleFlags());
        }
    }
    else
    {
        _renderer->renderIcon(context, widgetRect, _pixmap, _iconPos,
                              styleFlags());
    }
}

} // namespace

} // namespace
