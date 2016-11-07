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

#include <Pt/Hmi/PushButton.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Style.h>
#include <Pt/Hmi/StyleOptions.h>

namespace Pt {

namespace Hmi {

PushButton::PushButton()
: _isPressed(false)
, _isHover(false)
{
    setAcceptsFocus(true);
}


PushButton::~PushButton()
{
}


void PushButton::setText(const Pt::String& text)
{
    _text = Widget::setMnemonic(text);
    invalidate();
}


const Pt::String& PushButton::text() const
{
    return _text;
}


bool PushButton::isPressed() const
{
    return _isPressed;
}


bool PushButton::isHovered() const
{
    return _isHover;
}


void PushButton::onMnemonic()
{
    Button::onMnemonic();
    clicked().send(*this);
}


void PushButton::onActionKey( const KeyEvent& kev )
{
    Button::onActionKey(kev);
    clicked().send(*this);
}


void PushButton::onShortcut( const KeyEvent& kev )
{
    Button::onShortcut(kev);
    clicked().send(*this);
}


void PushButton::onMouseEvent(const MouseEvent& ev)
{    
    Button::onMouseEvent(ev);

    if( ev.isPressed() != _isPressed )
    {
        _isPressed = ev.isPressed();
        update();
    }
}


void PushButton::onTouchEvent(const TouchEvent& ev)
{    
    Button::onTouchEvent(ev);

    if( ev.isPressed() != _isPressed )
    {
        _isPressed = ev.isPressed();
        update();
    }
}


void PushButton::onEnterEvent(const EnterEvent& ev )
{
    Button::onEnterEvent(ev);
    _isHover = true;
    update();
}


void PushButton::onLeaveEvent(const LeaveEvent& ev )
{
    Button::onLeaveEvent(ev);

    _isHover = false;
    _isPressed = false;
    
    update();
}


void PushButton::onFocusEvent(const FocusEvent& ev)
{    
    Button::onFocusEvent(ev);
    update();
}


void PushButton::onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect)
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


void PushButton::onPaintContent(PaintSurface& surface, const Gfx::RectF& updateRect)
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
