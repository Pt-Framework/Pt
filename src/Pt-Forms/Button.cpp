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

#include <Pt/Forms/Button.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/Style.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

Button::Button()
: _onClickBegin(false)
, _isHovered(false)
{
    setFocusPolicy(Control::AcceptFocus);
}

  
Button::~Button()
{
}


const Pt::String& Button::text() const
{
    return _text;
}


void Button::setText(const Pt::String& text)
{
    _text = Control::setMnemonic(text);
    relayout();
}


bool Button::isHovered() const
{
    return _isHovered;
}


void Button::click()
{
    onPressed();
    onReleased();
}


Signal<>& Button::clicked()
{
    return _clicked;
}


void Button::onPressed()
{
}


void Button::onReleased()
{
}


void Button::onCanceled()
{
}


void Button::onMnemonic(Pt::Char m)
{
    Base::onMnemonic(m);
    
    onPressed();
    onReleased();
}


void Button::onActionKey( const KeyEvent& kev )
{
    Base::onActionKey(kev);

    if( kev.isPress() )
        onPressed();
    else if( kev.isRelease() )
        onReleased();
}


void Button::onShortcut(const Key& key)
{
    Base::onShortcut(key);

    onPressed();
    onReleased();
}


bool Button::onEnterEvent(const EnterEvent& ev)
{
    Base::onEnterEvent(ev);

    _isHovered = true;
    
    invalidate();
    return true;
}


bool Button::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);

    _isHovered = false;

    invalidate();
    return true;
}


void Button::onFocusEvent(const FocusEvent& ev)
{    
    Base::onFocusEvent(ev);
}


bool Button::onMouseEvent(const MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    if( ev.isPress() )
    {
        _onClickBegin = true;
        //grabPointer();
        onPressed();
    }
    else if( ev.isRelease() )
    {
        const Gfx::PointF& pos = ev.position();

        bool inside = pos.x() >= 0 && pos.x() <= size().width() &&
                      pos.y() >= 0 && pos.y() <= size().height();

        bool isClick = _onClickBegin && inside;

        if(_onClickBegin)
        {
            _onClickBegin = false;
            //releasePointer();
        }
        
        if(isClick)
        {
            onReleased();
        }
        else
        {
            onCanceled();
        }
    }

    return true;
}


bool Button::onTouchEvent(const TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    if( ev.isPress() )
    {
        _onClickBegin = true;
        //grabPointer();
        onPressed();
    }
    else if( ev.isRelease() )
    {
        const Gfx::PointF& pos = ev.position();

        bool inside = pos.x() > 0 && pos.x() <= size().width() &&
                      pos.y() > 0 && pos.y() <= size().height();

        bool isClick = _onClickBegin && inside;
        
        if(_onClickBegin)
        {
            _onClickBegin = false;
            //releasePointer();
        }

        if(isClick)
        {
            onReleased();
        }
        else
        {
            onCanceled();
        }
    }
    return true;
}


bool Button::onScrollEvent(const ScrollEvent& ev)
{    
    // cancel click detection
    if(_onClickBegin)
    {
        _onClickBegin = false;
        //releasePointer();
        onCanceled();
    }

    return Base::onScrollEvent(ev);
}

} // namespace

} // namespace
