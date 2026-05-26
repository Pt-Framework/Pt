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
, _styleGeneration(0)
, _iconInvalid(false)
, _overrideFlags(0)
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
    _overrideFlags |= OverrideForeground;

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
    _overrideFlags |= OverrideContour;

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
    _overrideFlags |= OverrideAccentColor;

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
    _overrideFlags |= OverrideHighlightColor;

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
    _overrideFlags |= OverrideTextColor;

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
    _overrideFlags |= OverrideFontAll;

    if( ButtonRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


Gfx::Font PushButton::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( ! (_overrideFlags & OverrideFontAny) )
        return base;

    if( _overrideFlags & OverrideFontAll )
        return _customFont;

    std::size_t sz = (_overrideFlags & OverrideFontSize) ? _customFont.size()
                                                        : base.size();
    Gfx::Font::Weight wt = (_overrideFlags & OverrideFontWeight) ? _customFont.weight()
                                                                 : base.weight();
    Gfx::Font::Slant sl = (_overrideFlags & OverrideFontSlant) ? _customFont.slant()
                                                               : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void PushButton::setFontSize(std::size_t size)
{
    _customFont = _customFont.withSize(size);
    _overrideFlags |= OverrideFontSize;

    if( ButtonRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void PushButton::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = _customFont.withWeight(weight);
    _overrideFlags |= OverrideFontWeight;

    if( ButtonRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void PushButton::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = _customFont.withSlant(slant);
    _overrideFlags |= OverrideFontSlant;

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
    if( ! _renderer )
    {
        const Style& style = Application::instance().style();
        ButtonRenderer* proto = style.get<ButtonRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );
    }

    return _renderer.get();
}


void PushButton::applyRenderer(ButtonRenderer* renderer)
{
    if( _overrideFlags & OverrideForeground )
        renderer->setForeground( *_foreground );

    if( _overrideFlags & OverrideContour )
        renderer->setContour( *_contour );

    if( _overrideFlags & OverrideAccentColor )
        renderer->setAccentColor( *_accentColor );

    if( _overrideFlags & OverrideHighlightColor )
        renderer->setHighlightColor( *_highlightColor );

    if( _overrideFlags & OverrideTextColor )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    if( _overrideFlags & OverrideFontAny )
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


void PushButton::onInvalidate()
{
    std::size_t gen = Application::instance().styleOptions().generation();
    if( _styleGeneration != gen )
    {
        _styleGeneration = gen;
        if( ! _customRenderer )
            _renderer.reset();
    }

    if( ! _renderer )
    {
        bool hasOverride = (_overrideFlags != 0);
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

    Gfx::SizeF itemsSize = _renderer->measureContent(surface(), _direction,
                                                      _measuredIconSize, textSize);

    Gfx::SizeF contentSize(itemsSize.width() + padding().leftRight(),
                           itemsSize.height() + padding().topBottom());

    return _renderer->measureFrame( surface(), contentSize );
}


void PushButton::onLayout(const Gfx::RectF& rect)
{    
    Base::onLayout(rect);

    if( ! _renderer )
        return;

    const Gfx::Scaling& scaling = surface().scaling();

    Gfx::SizeF textSize(_textMetrics.advance(), _fontMetrics.height());

    _contentRect = _renderer->layoutFrame( surface(),
                                              Gfx::RectF(size()) );

    _renderer->layoutContent(surface(), _contentRect, _direction,
                             _measuredIconSize, textSize,
                             _iconRect, _textRect);

    if( ! _picture.empty() )
    {
        double pw = _measuredIconSize.width();
        double ph = _measuredIconSize.height();
        double x = _iconRect.left() + (_iconRect.width() - pw) / 2;
        double y = _iconRect.top() + (_iconRect.height() - ph) / 2;

        Gfx::SizeF pictureSize = scaling.toLogical( _picture.size() );
        double xOff = (pw - pictureSize.width()) / 2;
        double yOff = (ph - pictureSize.height()) / 2;

        _iconPos = scaling.align( Gfx::PointF(x + xOff, y + yOff) );
    }

    if( ! text().empty() )
    {
        double tx = _textRect.left()
                    + (_textRect.width() - _textMetrics.advance()) / 2;
        double ty = _textRect.top()
                    + ((_textRect.height() - _fontMetrics.height()) / 2)
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

    onPaintBackground(context);
    onPaintFrame(context);
    onPaintIcon(context);
    onPaintText(context);
    onPaintMnemonic(context);
}


void PushButton::onPaintBackground(PaintContext& context)
{
    if( ! _renderer )
        return;

    Gfx::RectF widgetRect( size() );
    ButtonStyleFlags state = buttonStyleFlags();

    _renderer->renderBackground(context, widgetRect, state);
}


void PushButton::onPaintFrame(PaintContext& context)
{
    if( ! _renderer || _isFlat )
        return;

    Gfx::RectF widgetRect( size() );
    ButtonStyleFlags state = buttonStyleFlags();

    _renderer->renderFrame(context, widgetRect, state);
}


void PushButton::onPaintIcon(PaintContext& context)
{
    if( ! _renderer || _picture.empty() )
        return;

    ButtonStyleFlags state = buttonStyleFlags();

    _renderer->renderIcon(context, _contentRect, _picture, _iconPos, state);
}


void PushButton::onPaintText(PaintContext& context)
{
    if( ! _renderer )
        return;

    const String& buttonText = text();
    if( buttonText.empty() )
        return;

    ButtonStyleFlags state = buttonStyleFlags();

    _renderer->renderText(context, _contentRect, buttonText, _textPos, state);
}


void PushButton::onPaintMnemonic(PaintContext& context)
{
    if( ! _renderer || text().empty() )
        return;

    Gfx::RectF widgetRect( size() );
    ButtonStyleFlags state = buttonStyleFlags();

    _renderer->renderMnemonic(context, widgetRect, _mnemonicRect, state);
}

} // namespace

} // namespace
