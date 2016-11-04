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
#include <Pt/Hmi/StyleOptions.h>
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
    Control::onMouseEvent(ev);

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
    Control::onTouchEvent(ev);

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
: _isPressed(false)
, _isHover(false)
{
    setAcceptsFocus(true);
}


Button::~Button()
{
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


bool Button::isHovered() const
{
    return _isHover;
}


void Button::onMnemonic()
{
    ButtonBase::onMnemonic();
    clicked().send();
}


void Button::onActionKey( const KeyEvent& kev )
{
    ButtonBase::onActionKey(kev);
    clicked().send();
}


void Button::onShortcut( const KeyEvent& kev )
{
    ButtonBase::onShortcut(kev);
    clicked().send();
}


void Button::onMouseEvent(const MouseEvent& ev)
{    
    ButtonBase::onMouseEvent(ev);

    if( ev.isPressed() != _isPressed )
    {
        _isPressed = ev.isPressed();
        update();
    }
}


void Button::onTouchEvent(const TouchEvent& ev)
{    
    ButtonBase::onTouchEvent(ev);

    if( ev.isPressed() != _isPressed )
    {
        _isPressed = ev.isPressed();
        update();
    }
}


void Button::onEnterEvent(const EnterEvent& ev )
{
    ButtonBase::onEnterEvent(ev);
    _isHover = true;
    update();
}


void Button::onLeaveEvent(const LeaveEvent& ev )
{
    ButtonBase::onLeaveEvent(ev);

    _isHover = false;
    _isPressed = false;
    
    update();
}


void Button::onFocusEvent(const FocusEvent& ev)
{    
    ButtonBase::onFocusEvent(ev);
    update();
}


void Button::onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect)
{    
    Application& app = Application::instance();
    
    const Style& s = style() ? *style() 
                             : app.style();
    
    const StyleOptions& so = styleOptions() ? *styleOptions() 
                                            : app.styleOptions();

    const ButtonRenderer* renderer = s.get<ButtonRenderer>();
    if(renderer)
        renderer->renderBackground(*this, so, surface, updateRect);
}


void Button::onPaintContent(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Application& app = Application::instance();

    const Style& s = style() ? *style() 
                             : app.style();
    
    const StyleOptions& so = styleOptions() ? *styleOptions() 
                                            : app.styleOptions();

    const ButtonRenderer* renderer = s.get<ButtonRenderer>();
    if(renderer)
        renderer->renderContent(*this, so, surface, updateRect);
}

} // namespace

} // namespace
