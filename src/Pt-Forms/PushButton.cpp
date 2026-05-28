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
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Forms/Application.h>

#include <algorithm>
#include <cmath>

namespace Pt {

namespace Forms {

PushButton::PushButton()
: _isToggle(false)
, _isBeingToggled(false)
, _direction(Direction::Left)
, _iconInvalid(false)
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
    _iconInvalid = true;

    invalidate();
}


bool PushButton::isFlat() const
{
    return _buttonState.isFlat();
}


void PushButton::setFlat(bool f)
{
    _buttonState.setFlat(f);
    _iconInvalid = true;
    invalidate();
}


void PushButton::setLayout(Direction d)
{
    _direction = d;
    invalidate();
}


const Gfx::Brush& PushButton::foreground() const
{
    if( const Gfx::Brush* foreground = _buttonStyleOptions.foreground() )
        return *foreground;

    return Application::instance().styleOptions().foreground();
}


void PushButton::setForeground(const Gfx::Brush& b)
{
    _buttonStyleOptions.setForeground(b);

    invalidate();
}


const Gfx::Pen& PushButton::contour() const
{
    if( const Gfx::Pen* contour = _buttonStyleOptions.contour() )
        return *contour;

    return Application::instance().styleOptions().contour();
}


void PushButton::setContour(const Gfx::Pen& p)
{
    _buttonStyleOptions.setContour(p);

    invalidate();
}


const Gfx::Color& PushButton::accentColor() const
{
    if( const Gfx::Color* accentColor = _buttonStyleOptions.accentColor() )
        return *accentColor;

    return Application::instance().styleOptions().accentColor();
}


void PushButton::setAccentColor(const Gfx::Color& color)
{
    _buttonStyleOptions.setAccentColor(color);

    invalidate();
}


const Gfx::Color& PushButton::highlightColor() const
{
    if( const Gfx::Color* highlightColor = _buttonStyleOptions.highlightColor() )
        return *highlightColor;

    return Application::instance().styleOptions().highlightColor();
}


void PushButton::setHighlightColor(const Gfx::Color& color)
{
    _buttonStyleOptions.setHighlightColor(color);

    invalidate();
}


const Gfx::Color& PushButton::textColor() const
{
    if( const Gfx::Color* textColor = _buttonStyleOptions.textColor() )
        return *textColor;

    return Application::instance().styleOptions().textColor();
}


void PushButton::setTextColor(const Gfx::Color& color)
{
    _buttonStyleOptions.setTextColor(color);

    invalidate();
}


Gfx::Font PushButton::font() const
{
    const StyleOptions& options = Application::instance().styleOptions();
    return _buttonStyleOptions.getFont(options.font());
}


void PushButton::setFont(const Gfx::Font& font)
{
    _buttonStyleOptions.setFont(font);

    invalidate();
}


void PushButton::setFontSize(std::size_t size)
{
    _buttonStyleOptions.setFontSize(size);

    invalidate();
}


void PushButton::setFontWeight(Gfx::Font::Weight weight)
{
    _buttonStyleOptions.setFontWeight(weight);

    invalidate();
}


void PushButton::setFontSlant(Gfx::Font::Slant slant)
{
    _buttonStyleOptions.setFontSlant(slant);

    invalidate();
}


const ButtonState& PushButton::buttonState()
{
    _buttonState.setEnabled( isEnabled() );
    _buttonState.setHovered( isHovered() );
    _buttonState.setFocused( hasFocus() );

    return _buttonState;
}


bool PushButton::isPressed() const
{
    return _buttonState.isPressed();
}


void PushButton::setPressed(bool pressed)
{
    _buttonState.setPressed(pressed);
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


void PushButton::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);

    if( ! _icon.empty() )
    {
        _iconInvalid = true;
        relayout();
    }
}


void PushButton::setRenderer(ButtonRenderer* renderer)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( renderer )
        _buttonStyle.bind(*renderer, options, _buttonStyleOptions);
    else
        _buttonStyle.bind(Application::instance().style(), options, _buttonStyleOptions);

    invalidate();
}


void PushButton::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    ButtonRenderer* renderer = _buttonStyle.rebind(style, options,
                                                   _buttonStyleOptions);
    if( ! renderer )
        return;

    if( _iconInvalid )
    {
        _iconInvalid = false;
        const ButtonState& state = buttonState();

        if( ! _icon.empty() )
        {
            const Gfx::SizeF scaledSize = scaling().toPhysical(_iconSize);
            const Pt::Gfx::Image& iconImage = _icon.getImage(scaledSize);
            renderer->prepareIcon(iconImage, _picture, state);
        }
        else
        {
            _picture.reset();
        }
    }

    relayout();
}


Gfx::SizeF PushButton::onMeasure(const SizePolicy& policy)
{
    ButtonRenderer* renderer = _buttonStyle.renderer();
    if( ! renderer )
        return Gfx::SizeF(0, 0);

    const Painter& painter = renderer->textPainter( surface() );
    _textMetrics = painter.textMetrics( text() );
    _fontMetrics = painter.fontMetrics();
    Gfx::SizeF textSize(_textMetrics.advance(), _fontMetrics.height());

    if( _picture.empty() )
        _measuredIconSize = Gfx::SizeF();
    else if( _iconSize.isNull() )
        _measuredIconSize = surface().scaling().toLogical( _picture.size() );
    else
        _measuredIconSize = _iconSize;

    Gfx::SizeF itemsSize = renderer->measureContent(surface(),
                                                    _direction,
                                                    _measuredIconSize,
                                                    textSize);

    Gfx::SizeF contentSize(itemsSize.width() + padding().leftRight(),
                           itemsSize.height() + padding().topBottom());

    return renderer->measureFrame( surface(), contentSize );
}


void PushButton::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    ButtonRenderer* renderer = _buttonStyle.renderer();
    if( ! renderer )
        return;

    const Gfx::Scaling& scaling = surface().scaling();

    Gfx::SizeF textSize(_textMetrics.advance(), _fontMetrics.height());

    _contentRect = renderer->layoutFrame( surface(), Gfx::RectF(size()) );

    renderer->layoutContent(surface(),
                            _contentRect,
                            _direction,
                            _measuredIconSize,
                            textSize,
                            _iconRect,
                            _textRect);

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

        _mnemonicRect = renderer->layoutMnemonic(surface(),
                             text(),
                             _textPos,
                             _fontMetrics,
                             mnIdx);
    }

    repaint();
}


void PushButton::onPaint(PaintContext& context, const Gfx::RectF& /*rect*/)
{
    if( ! _buttonStyle.renderer() )
        return;

    onPaintBackground(context);
    onPaintFrame(context);
    onPaintIcon(context);
    onPaintText(context);
    onPaintMnemonic(context);
}


void PushButton::onPaintBackground(PaintContext& context)
{
    ButtonRenderer* renderer = _buttonStyle.renderer();
    if( ! renderer )
        return;

    Gfx::RectF widgetRect( size() );
    const ButtonState& state = buttonState();

    renderer->renderBackground(context, widgetRect, state);
}


void PushButton::onPaintFrame(PaintContext& context)
{
    ButtonRenderer* renderer = _buttonStyle.renderer();
    if( ! renderer || _buttonState.isFlat() )
        return;

    Gfx::RectF widgetRect( size() );
    const ButtonState& state = buttonState();

    renderer->renderChrome(context, widgetRect, state);
}


void PushButton::onPaintIcon(PaintContext& context)
{
    ButtonRenderer* renderer = _buttonStyle.renderer();
    if( ! renderer || _picture.empty() )
        return;

    const ButtonState& state = buttonState();

    renderer->renderIcon(context, _contentRect, _picture, _iconPos, state);
}


void PushButton::onPaintText(PaintContext& context)
{
    ButtonRenderer* renderer = _buttonStyle.renderer();
    if( ! renderer )
        return;

    const String& buttonText = text();
    if( buttonText.empty() )
        return;

    const ButtonState& state = buttonState();

    renderer->renderText(context, _contentRect, buttonText, _textPos, state);
}


void PushButton::onPaintMnemonic(PaintContext& context)
{
    ButtonRenderer* renderer = _buttonStyle.renderer();
    if( ! renderer || text().empty() )
        return;

    Gfx::RectF widgetRect( size() );
    const ButtonState& state = buttonState();

    renderer->renderMnemonic(context, widgetRect, _mnemonicRect, state);
}

} // namespace

} // namespace
