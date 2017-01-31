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

#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Style.h>
#include <Pt/Hmi/StyleOptions.h>

namespace Pt {

namespace Hmi {

Button::Button()
: _isPressed(false)
, _onClickBegin(false)
{
    setFocusPolicy(Widget::NormalFocus);
}

  
Button::~Button()
{
}


bool Button::isPressed() const
{
    return _isPressed;
}


void Button::setPressed(bool pressed)
{
    _isPressed = pressed;
    invalidate();
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


Signal<>& Button::clicked()
{
    return _clicked;
}


void Button::onPressed(const Gfx::PointF& pos)
{
}


void Button::onReleased(const Gfx::PointF& pos)
{
}


void Button::onMnemonic()
{
    Base::onMnemonic();
    
    onPressed( Gfx::PointF(0,0) );
    onReleased( Gfx::PointF(0,0) );
}


void Button::onActionKey( const KeyEvent& kev )
{
    Base::onActionKey(kev);

    if( kev.isPress() )
        onPressed( Gfx::PointF(0,0) );
    else if( kev.isRelease() )
        onReleased( Gfx::PointF(0,0) );
}


void Button::onShortcut( const KeyEvent& kev )
{
    Base::onShortcut(kev);

    if( kev.isPress() )
        onPressed( Gfx::PointF(0,0) );
    else if( kev.isRelease() )
        onReleased( Gfx::PointF(0,0) );
}


void Button::onEnterEvent(const EnterEvent& ev)
{
    Base::onEnterEvent(ev);
    update();
}


void Button::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);
    update();
}


void Button::onFocusEvent(const FocusEvent& ev)
{    
    Base::onFocusEvent(ev);
    update();
}


void Button::onMouseEvent(const MouseEvent& ev)
{    
    Base::onMouseEvent(ev);

    if( ev.isPress() )
    {
        _onClickBegin = true;
        grabPointer();
        onPressed( ev.position() );
    }
    else if( ev.isRelease() )
    {
        if(_onClickBegin)
        {
            _onClickBegin = false;
            releasePointer();
            onReleased( ev.position() );
        }
    }   
}


void Button::onTouchEvent(const TouchEvent& ev)
{    
    Base::onTouchEvent(ev);

    if( ev.isPress() )
    {
        _onClickBegin = true;
        grabPointer();
        onPressed( ev.position() );
    }
    else if( ev.isRelease() )
    {
        if(_onClickBegin)
        {
            _onClickBegin = false;
            releasePointer();
            onReleased( ev.position() );
        }
    }   
}

} // namespace

} // namespace
