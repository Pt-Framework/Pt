/* Copyright (C) 2013 Marc Boris Duerner 
   Copyright (C) 2013 Laurentiu-Gheorghe Crisan
  
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

#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Style.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/TouchEvent.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Point.h>

namespace Pt {

namespace Hmi {

ButtonBase::ButtonBase()
{
}

  
ButtonBase::~ButtonBase()
{
}


void ButtonBase::onPressed()
{
    _pressed.send();
}


void ButtonBase::onReleased()
{
    _released.send();
}


void ButtonBase::onClicked()
{
    _clicked.send();
}

    
void ButtonBase::onMouseEvent(const MouseEvent& ev)
{    
    Panel::onMouseEvent(ev);

    if( ev.isPress() )
    {
        onPressed();
    }
    
    if( ev.isRelease() )
    {
        onReleased();
    }
    
    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( ! rect.contains( ev.position() ) )
        return;
    
    if( ev.isRelease() && hasFocus() )
    {
        onClicked();
    }
}


void ButtonBase::onTouchEvent(const TouchEvent& ev)
{    
    Panel::onTouchEvent(ev);

    if( ev.isPress() )
    {
        onPressed();
    }

    if( ev.isRelease() )
    {
        onReleased();
    }
    
    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( ! rect.contains( ev.position() ) )
        return;
    
    if( ev.isRelease() && hasFocus() )
    {
        onClicked();
    }
}




Button::Button()
: _style(0)
, _styleOptions(0)
, _isPressed(false)
{
    setAcceptsFocus(true);
}


Button::~Button()
{
    delete _style;
    delete _styleOptions;
}


void Button::setText(const Pt::String& text)
{
    _text = Widget::setMnemonic(text);
    invalidate();
}


const Pt::String& Button::text() const
{
    return _text;
}


bool Button::isPressed() const
{
    return _isPressed;
}


void Button::setStyle(const Style& style)
{
    delete _style;
    _style = 0;
    _style = new Style(style);
}    


void Button::setStyleOptions(const StyleOptions& opts)
{
    delete _styleOptions;
    _styleOptions = 0;
    _styleOptions = new StyleOptions(opts);
}


void Button::onMnemonic()
{
    Control::onMnemonic();
    _clicked.send(*this);
}


void Button::onActionKey( const KeyEvent& kev )
{
    Control::onActionKey(kev);
    _clicked.send(*this);
}


void Button::onShortcut( const KeyEvent& kev )
{
    Control::onShortcut(kev);
    _clicked.send(*this);
}


void Button::onMouseEvent(const MouseEvent& ev)
{    
    Control::onMouseEvent(ev);

    if( ev.isPressed() != _isPressed )
    {
        _isPressed = ev.isPressed();
        update();
    }

    if( ev.isPress() )
    {
        _pressed.send(*this);
    }

    if( ev.isRelease() )
    {
        _released.send(*this);
    }

    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( ! rect.contains( ev.position() ) )
        return;

    if( ev.isRelease() && hasFocus() )
    {
        _clicked.send(*this);
    }
}


void Button::onTouchEvent(const TouchEvent& ev)
{    
    Control::onTouchEvent(ev);

    if( ev.isPressed() != _isPressed )
    {
        _isPressed = ev.isPressed();
        update();
    }

    if( ev.isPress() )
    {
        _pressed.send(*this);
    }

    if( ev.isRelease() )
    {
        _released.send(*this);
    }

    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( ! rect.contains( ev.position() ) )
        return;

    if( ev.isRelease() && hasFocus() )
    {
        _clicked.send(*this);
    }
}


void Button::onEnterEvent(const EnterEvent& ev )
{
    Control::onEnterEvent(ev);
    update();
}


void Button::onLeaveEvent(const LeaveEvent& ev )
{
    _isPressed = false;
    Control::onLeaveEvent(ev);
    update();
}


void Button::onFocusEvent(const FocusEvent& ev)
{    
    Control::onFocusEvent(ev);
    update();
}


void Button::onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect)
{    
    Application& app = Application::instance();
    
    const Style& style = _style ? *_style 
                                : app.style();
    
    const StyleOptions& styleOptions = _styleOptions ? *_styleOptions 
                                                     : app.styleOptions();

    const ButtonRenderer* renderer = style.get<ButtonRenderer>();
    if(renderer)
        renderer->renderBackground(*this, styleOptions, surface, updateRect);
}


void Button::onPaintContent(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Application& app = Application::instance();

    const Style& style = _style ? *_style 
                                : app.style();
    
    const StyleOptions& styleOptions = _styleOptions ? *_styleOptions 
                                                     : app.styleOptions();

    const ButtonRenderer* renderer = style.get<ButtonRenderer>();
    if(renderer)
        renderer->renderContent(*this, styleOptions, surface, updateRect);
}

} // namespace

} // namespace
