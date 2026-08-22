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
, _hasBackground(false)
, _hasFrame(false)
{
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

    if( const BackgroundOption* background = _panelStyleOptions.get<BackgroundOption>() )
        return &background->value();

    const StyleOptions& options = Application::instance().styleOptions();
    return &options.get<BackgroundOption>()->value();
}


void Label::setBackground(const Gfx::Brush& b)
{
    BackgroundOption background(b);
    _panelStyleOptions.set(background);
    _hasBackground = true;

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

    if( const ContourOption* contour = _panelStyleOptions.get<ContourOption>() )
        return &contour->value();

    const StyleOptions& options = Application::instance().styleOptions();
    return &options.get<ContourOption>()->value();
}


void Label::setContour(const Gfx::Pen& p)
{
    ContourOption contour(p);
    _panelStyleOptions.set(contour);
    _hasFrame = true;

    invalidate();
}


void Label::setFrame(bool b)
{
    _hasFrame = b;
    invalidate();
}


const Gfx::Color& Label::textColor() const
{
    if( const TextColorOption* textColor = _panelStyleOptions.get<TextColorOption>() )
        return textColor->value();

    const StyleOptions& options = Application::instance().styleOptions();
    return options.get<TextColorOption>()->value();
}


void Label::setTextColor(const Gfx::Color& color)
{
    TextColorOption textColor(color);
    _panelStyleOptions.set(textColor);

    invalidate();
}


Gfx::Font Label::font() const
{
    const StyleOptions& options = Application::instance().styleOptions();
    const Gfx::Font& baseFont = options.get<FontOption>()->value();
    const FontOption* localFont = _panelStyleOptions.get<FontOption>();
    return localFont ? localFont->getFont(baseFont) : baseFont;
}


void Label::setFont(const Gfx::Font& font)
{
    FontOption fontOption;
    fontOption.setFont(font);
    _panelStyleOptions.set(fontOption);

    invalidate();
}


void Label::setFontSize(std::size_t size)
{
    const FontOption* localFont = _panelStyleOptions.get<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setSize(size);
    _panelStyleOptions.set(font);

    invalidate();
}


void Label::setFontWeight(Gfx::Font::Weight weight)
{
    const FontOption* localFont = _panelStyleOptions.get<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setWeight(weight);
    _panelStyleOptions.set(font);

    invalidate();
}


void Label::setFontSlant(Gfx::Font::Slant slant)
{
    const FontOption* localFont = _panelStyleOptions.get<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setSlant(slant);
    _panelStyleOptions.set(font);

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


void Label::setRenderer(PanelRenderer* renderer)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( renderer )
        _panelStyle.bind(*renderer, options, _panelStyleOptions);
    else
        _panelStyle.bind(Application::instance().style(), options,
                         _panelStyleOptions);

    invalidate();
}


void Label::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    PanelRenderer* renderer = _panelStyle.rebind(style, options,
                                                 _panelStyleOptions);
    if( ! renderer )
        return;

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
    }

    relayout();
}


Gfx::SizeF Label::onMeasure(const SizePolicy& policy)
{
    PanelRenderer* renderer = _panelStyle.renderer();
    if( ! renderer )
        return Gfx::SizeF(0, 0);

    Gfx::SizeF contentSize;

    if( _text.empty() )
    {
        if( _iconSize.isEmpty() )
            _measuredIconSize = surface().scaling().toLogical( _pixmap.size() );
        else
            _measuredIconSize = _iconSize;

        contentSize = _measuredIconSize;
    }
    else
    {
        const Gfx::Scaling& scaling = surface().scaling();

        const Painter& painter = renderer->textPainter( surface() );
        const Gfx::FontMetrics fontMetrics = painter.fontMetrics();

        TextBlock block;
        block.setAdjustment( adjustment() );
        block.setMaxWidth( policy.size().width() );
        block.setLineSpacing( scaling.align( fontMetrics.height() / 3.0 ) );
        block.layout(painter, _text);

        double w = block.size().width() + scaling.toLogical(0.5);
        double h = block.size().height() + scaling.toLogical(0.5);

        _measuredIconSize = Gfx::SizeF();
        contentSize = Gfx::SizeF( scaling.align(w), scaling.align(h) );
    }

    Gfx::SizeF paddedSize( contentSize.width() + padding().leftRight(),
                           contentSize.height() + padding().topBottom() );

    return renderer->measureFrame( surface(), paddedSize );
}


void Label::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    PanelRenderer* renderer = _panelStyle.renderer();
    if( ! renderer )
        return;

    _contentRect = renderer->layoutFrame( surface(), Gfx::RectF(size()) );

    double left = _contentRect.left() + padding().left();
    double top = _contentRect.top() + padding().top();
    double contentWidth = _contentRect.width() - padding().leftRight();
    double contentHeight = _contentRect.height() - padding().topBottom();

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

        const Painter& painter = renderer->textPainter( surface() );
        const Gfx::FontMetrics fontMetrics = painter.fontMetrics();

        _textBlock.setMaxWidth( innerRect.width() );
        _textBlock.setAdjustment( adjustment() );
        _textBlock.setLineSpacing( scaling.align( fontMetrics.height() / 3.0 ) );
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
    if( ! _panelStyle.renderer() )
        return;

    Gfx::RectF widgetRect( size() );
    PanelState state = panelState();

    onPaintBackground(context, widgetRect, state);
    onPaintFrame(context, widgetRect, state);
    onPaintIcon(context, _contentRect, state);
    onPaintText(context, _contentRect, state);
}

void Label::onPaintBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              const PanelState& state)
{
    PanelRenderer* renderer = _panelStyle.renderer();
    if( ! renderer || ! _hasBackground )
        return;

    renderer->renderBackground(context, rect, state);
}

void Label::onPaintFrame(PaintContext& context,
                         const Gfx::RectF& rect,
                         const PanelState& state)
{
    PanelRenderer* renderer = _panelStyle.renderer();
    if( ! renderer || ! _hasFrame )
        return;

    renderer->renderFrame(context, rect, state);
}

void Label::onPaintIcon(PaintContext& context,
                        const Gfx::RectF& contentRect,
                        const PanelState& state)
{
    PanelRenderer* renderer = _panelStyle.renderer();
    if( ! renderer || _icon.empty() || _pixmap.empty() )
        return;

    renderer->renderIcon(context, contentRect, _pixmap, _iconPos,
                         state);
}


void Label::onPaintText(PaintContext& context,
                        const Gfx::RectF& contentRect,
                        const PanelState& state)
{
    PanelRenderer* renderer = _panelStyle.renderer();
    if( ! renderer || ! _icon.empty() )
        return;

    TextBlock::ConstIterator it;
    for(it = _textBlock.begin(); it != _textBlock.end(); ++it)
    {
        const Pt::String& lineText = it->text();
        double ascent = it->ascent();

        Gfx::PointF pos = _textBlock.position() + it->position();
        pos.addY(ascent);

        renderer->renderText(context, contentRect, lineText, pos,
                             state);
    }
}


PanelState Label::panelState() const
{
    PanelState state;
    state.setEnabled( isEnabled() );
    state.setFocused( hasFocus() );
    return state;
}

} // namespace

} // namespace
