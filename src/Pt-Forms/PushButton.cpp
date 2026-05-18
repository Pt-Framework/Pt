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
  MA 02110-1301 USA
*/

#include <Pt/Forms/PushButton.h>
#include <Pt/Forms/Style.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/StyleFlags.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Forms/Application.h>

#include <algorithm>
#include <cmath>

namespace Pt {

namespace Forms {

PushButton::PushButton()
: _isToggle(false)
, _isPressed(false)
, _isBeingToggled(false)
, _isFlat(false)
, _direction(Direction::Left)
, _customRenderer(false)
, _iconInvalid(false)
, _fontOverride(0)
{
}


PushButton::~PushButton()
{
}


bool PushButton::isToggle() const
{
    return _isToggle;
}


void PushButton::setToggle(bool toggle)
{
    _isToggle = toggle;
    invalidate();
}


void PushButton::setIcon(const Icon& icon, const Gfx::SizeF& iconSize)
{
    _icon = icon;
    _iconSize = iconSize;

    invalidate();
}


bool PushButton::isFlat() const
{
    return _isFlat;
}


void PushButton::setFlat(bool f)
{
    _isFlat = f;
    invalidate();
}


void PushButton::setLayout(Direction d)
{
    _direction = d;
    invalidate();
}


const Gfx::Brush& PushButton::foreground() const
{
    if( _renderer )
        return _renderer->foreground();

    return Application::instance().styleOptions().foreground();
}


void PushButton::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );

    if( ButtonRenderer* renderer = getRenderer() )
        renderer->setForeground(*_foreground);

    invalidate();
}


const Gfx::Pen& PushButton::contour() const
{
    if( _renderer )
        return _renderer->contour();

    return Application::instance().styleOptions().contour();
}


void PushButton::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );

    if( ButtonRenderer* renderer = getRenderer() )
        renderer->setContour(*_contour);

    invalidate();
}


const Gfx::Color& PushButton::accentColor() const
{
    if( _renderer )
        return _renderer->accentColor();

    return Application::instance().styleOptions().accentColor();
}


void PushButton::setAccentColor(const Gfx::Color& color)
{
    _accentColor.reset( new Gfx::Color(color) );

    if( ButtonRenderer* renderer = getRenderer() )
        renderer->setAccentColor(*_accentColor);

    invalidate();
}


const Gfx::Color& PushButton::highlightColor() const
{
    if( _renderer )
        return _renderer->highlightColor();

    return Application::instance().styleOptions().highlightColor();
}


void PushButton::setHighlightColor(const Gfx::Color& color)
{
    _highlightColor.reset( new Gfx::Color(color) );

    if( ButtonRenderer* renderer = getRenderer() )
        renderer->setHighlightColor(*_highlightColor);

    invalidate();
}


const Gfx::Color& PushButton::textColor() const
{
    if( _renderer )
        return _renderer->textColor();

    return Application::instance().styleOptions().textColor();
}


void PushButton::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );

    if( ButtonRenderer* renderer = getRenderer() )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    invalidate();
}


const Gfx::Font& PushButton::font() const
{
    if( _renderer )
        return _renderer->font();

    return Application::instance().styleOptions().font();
}


void PushButton::setFont(const Gfx::Font& font)
{
    _customFont = font;
    _fontOverride = OverrideAll;

    if( ButtonRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


Gfx::Font PushButton::getFont() const
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


void PushButton::setFontSize(std::size_t size)
{
    _customFont = Gfx::Font(_customFont.family(), size,
                            _customFont.weight(), _customFont.slant(),
                            _customFont.stretch());
    _fontOverride |= OverrideSize;

    if( ButtonRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void PushButton::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                            weight, _customFont.slant(),
                            _customFont.stretch());
    _fontOverride |= OverrideWeight;

    if( ButtonRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void PushButton::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                            _customFont.weight(), slant,
                            _customFont.stretch());
    _fontOverride |= OverrideSlant;

    if( ButtonRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void PushButton::setRenderer(ButtonRenderer* renderer)
{
    _renderer.reset(renderer);
    _customRenderer = renderer != 0;

    if( renderer )
        applyRenderer(renderer);

    invalidate();
}


ButtonRenderer* PushButton::getRenderer()
{
    if( ! _customRenderer )
    {
        const Style& style = Application::instance().style();
        ButtonRenderer* proto = style.get<ButtonRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );
        _customRenderer = true;
    }

    return _renderer.get();
}


void PushButton::applyRenderer(ButtonRenderer* renderer)
{
    if( _foreground )
        renderer->setForeground( *_foreground );

    if( _contour )
        renderer->setContour( *_contour );

    if( _accentColor )
        renderer->setAccentColor( *_accentColor );

    if( _highlightColor )
        renderer->setHighlightColor( *_highlightColor );

    if( _textColor )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    if( _fontOverride )
        renderer->setFont( getFont() );
}


ButtonStyleFlags PushButton::buttonStyleFlags() const
{
    StyleFlags common;

    if( isEnabled() )
        common.set(StyleFlags::Enabled);
    else
        common.set(StyleFlags::Disabled);

    if( isHighlighted() )
        common.set(StyleFlags::Highlighted);

    if( hasFocus() )
        common.set(StyleFlags::Focused);

    ButtonStyleFlags state(common);

    if( _isPressed )
        state.set(ButtonStyleFlags::Pressed);

    if( _isFlat )
        state.set(ButtonStyleFlags::Flat);

    return state;
}


bool PushButton::isPressed() const
{
    return _isPressed;
}


void PushButton::setPressed(bool pressed)
{
    _isPressed = pressed;
    _isBeingToggled = false;
    _iconInvalid = true;
    invalidate();
}


void PushButton::onPressed()
{
    Base::onPressed();

    if( isToggle() )
    {
        setPressed( ! isPressed() );
        _isBeingToggled = true;
    }
    else
        setPressed(true);
}


void PushButton::onReleased()
{
    Base::onReleased();

    if(_isBeingToggled)
    {
        _isBeingToggled = false;
        clicked().send();
    }
    else
    {
        setPressed(false);
        clicked().send();
    }
}


void PushButton::onCanceled()
{
    Base::onCanceled();

    if( _isBeingToggled )
    { 
        _isBeingToggled = false;
        setPressed( ! isPressed() );
    }
    else
        setPressed(false);
}


void PushButton::onSetStyleOptions(const StyleOptions& o)
{
    _foreground.reset( new Gfx::Brush(o.foreground()) );
    _contour.reset( new Gfx::Pen( o.contour() ) );
    _textColor.reset( new Gfx::Color( o.textColor() ) );
    _accentColor.reset( new Gfx::Color( o.accentColor() ) );
    _highlightColor.reset( new Gfx::Color( o.highlightColor() ) );
    _customFont = o.font();
    _fontOverride = OverrideAll;
}


void PushButton::onInvalidate()
{
    if( ! _renderer )
    {
        bool hasOverride = _foreground || _contour || _accentColor ||
                           _highlightColor || _textColor || _fontOverride;
        if(hasOverride)
        {
            if( ButtonRenderer* renderer = getRenderer() )
                applyRenderer(renderer);
        }
        else
        {
            _renderer.reset( Application::instance().style().get<ButtonRenderer>() );
        }
    }

    if( ! _renderer )
        return;

    if( _iconInvalid )
    {
        _iconInvalid = false;

        ButtonStyleFlags state = buttonStyleFlags();

        if( ! _icon.empty() )
        {
            const Gfx::SizeF scaledSize = scaling().toPhysical(_iconSize);
            const Pt::Gfx::Image& iconImage = _icon.getImage(scaledSize);
            _renderer->prepareIcon(iconImage, _picture, state);
        }
        else
        {
            _picture.reset();
        }
    }

    Base::onInvalidate();

    relayout();
}


void PushButton::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);

    if( ! _icon.empty() )
    {
        _iconInvalid = true;
        relayout();
    }
}


Gfx::SizeF PushButton::onMeasure(const SizePolicy& policy)
{
    if( ! _renderer )
        return Gfx::SizeF(0, 0);

    const Painter& painter = _renderer->textPainter( surface() );
    _textMetrics = painter.textMetrics( text() );
    _fontMetrics = painter.fontMetrics();
    Gfx::SizeF textSize(_textMetrics.advance(), _fontMetrics.height());

    if( _picture.empty() )
        _measuredIconSize = Gfx::SizeF();
    else if( _iconSize.isNull() )
        _measuredIconSize = surface().scaling().toLogical( _picture.size() );
    else
        _measuredIconSize = _iconSize;

    double spacing = _picture.empty() || text().empty()
                   ? 0 : std::ceil(_fontMetrics.height() / 2.0);

    double itemsWidth = 0;
    double itemsHeight = 0;

    switch(_direction)
    {
        default:
        case Direction::Left:
        case Direction::Right:
            itemsWidth = textSize.width() + spacing + _measuredIconSize.width();
            itemsHeight = std::max(textSize.height(), _measuredIconSize.height());
            break;

        case Direction::Top:
        case Direction::Bottom:
            itemsWidth = std::max(textSize.width(), _measuredIconSize.width());
            itemsHeight = textSize.height() + spacing + _measuredIconSize.height();
            break;
    }

    Gfx::SizeF contentSize(itemsWidth + padding().leftRight(),
                           itemsHeight + padding().topBottom());

    return _renderer->measureSurface( surface(), contentSize );
}


void PushButton::onLayout(const Gfx::RectF& rect)
{    
    Base::onLayout(rect);

    if( ! _renderer )
        return;

    const Gfx::Scaling& scaling = surface().scaling();

    Gfx::RectF contentRect = _renderer->layoutSurface( surface(),
                                                       Gfx::RectF(size()) );

    Gfx::RectF iconRect = contentRect;
    Gfx::RectF textRect = contentRect;

    if( ! _picture.empty() && ! text().empty() )
    {
        double spacing = std::ceil(_fontMetrics.height() / 2.0);
        double pw = _measuredIconSize.width();
        double ph = _measuredIconSize.height();
        Gfx::SizeF textSize(_textMetrics.advance(), _fontMetrics.height());

        bool horizontal = (_direction == Direction::Left || _direction == Direction::Right);
        bool iconFirst  = (_direction == Direction::Left || _direction == Direction::Top);

        if( horizontal )
        {
            double groupW = pw + spacing + textSize.width();
            double gx = contentRect.left() + (contentRect.width() - groupW) / 2;
            double y = contentRect.top();
            double h = contentRect.height();

            double iconX = iconFirst ? gx : gx + textSize.width() + spacing;
            double textX = iconFirst ? gx + pw + spacing : gx;

            iconRect.set( Gfx::PointF(iconX, y), Gfx::SizeF(pw, h) );
            textRect.set( Gfx::PointF(textX, y), Gfx::SizeF(textSize.width(), h) );
        }
        else
        {
            double groupH = ph + spacing + textSize.height();
            double x = contentRect.left();
            double gy = contentRect.top() + (contentRect.height() - groupH) / 2;
            double w = contentRect.width();

            double iconY = iconFirst ? gy : gy + textSize.height() + spacing;
            double textY = iconFirst ? gy + ph + spacing : gy;

            iconRect.set( Gfx::PointF(x, iconY), Gfx::SizeF(w, ph) );
            textRect.set( Gfx::PointF(x, textY), Gfx::SizeF(w, textSize.height()) );
        }
    }

    if( ! _picture.empty() )
    {
        double pw = _measuredIconSize.width();
        double ph = _measuredIconSize.height();
        double x = iconRect.left() + (iconRect.width() - pw) / 2;
        double y = iconRect.top() + (iconRect.height() - ph) / 2;

        Gfx::SizeF pictureSize = scaling.toLogical( _picture.size() );
        double xOff = (pw - pictureSize.width()) / 2;
        double yOff = (ph - pictureSize.height()) / 2;

        _iconPos = scaling.align( Gfx::PointF(x + xOff, y + yOff) );
    }

    if( ! text().empty() )
    {
        double tx = textRect.left()
                    + (textRect.width() - _textMetrics.advance()) / 2;
        double ty = textRect.top()
                    + ((textRect.height() - _fontMetrics.height()) / 2)
                    + _fontMetrics.ascent();

        _textPos = scaling.align( Gfx::PointF(tx, ty) );

        String::size_type mnIdx = String::npos;
        const Char* m = mnemonic();
        if(m)
            mnIdx = text().find(*m);

        _mnemonicRect = _renderer->layoutMnemonic(surface(), text(), _textPos,
                                                  _fontMetrics, mnIdx);
    }

    repaint();
}


void PushButton::onPaint(PaintContext& context, const Gfx::RectF& /*rect*/)
{
    if( ! _renderer )
        return;

    Gfx::RectF widgetRect( size() );
    ButtonStyleFlags state = buttonStyleFlags();

    if( ! _isFlat )
    {
        _renderer->renderSurface(context, widgetRect, state);
    }

    if( ! _picture.empty() )
    {
        _renderer->renderIcon(context, widgetRect, _picture, _iconPos, state);
    }

    _renderer->renderText(context, widgetRect, text(), _textPos, state);
    _renderer->renderMnemonic(context, widgetRect, _mnemonicRect, state);
}

} // namespace

} // namespace
