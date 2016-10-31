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
: _image()
, _imageAlign( MiddleLeft)
, _isPressed(false)
{
  setBackgroundBrush(Pt::Gfx::Brush(Gfx::Color::fromRgb8(245,245,245)));

  setContentAlignment(MiddleCenter);
  setAcceptsFocus(true);

  setBorderWidth(1);
  setBorderRound(true);
  setBorderStyle(Panel::Single);
}


Button::~Button()
{
}


void Button::onMnemonic()
{
    Label::onMnemonic();
    _clicked.send(*this);
}


void Button::onActionKey( const KeyEvent& kev )
{
    Label::onActionKey(kev);
    _clicked.send(*this);
}


void Button::onShortcut( const KeyEvent& kev )
{
    Label::onShortcut(kev);
    _clicked.send(*this);
}


void Button::onMouseEvent(const MouseEvent& ev)
{    
    Label::onMouseEvent(ev);

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
    Label::onTouchEvent(ev);

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
    Label::onEnterEvent(ev);
    update();
}


void Button::onLeaveEvent(const LeaveEvent& ev )
{
    _isPressed = false;
    Label::onLeaveEvent(ev);
    update();
}


void Button::onFocusEvent(const FocusEvent& ev)
{    
    Label::onFocusEvent(ev);
    update();
}


void Button::onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    if( ! isEnabled() )
    {
        Label::onPaintBackground(surface, updateRect);
        return;
    }

    // TODO: use enter/leave events
    bool mouseOver = Application::instance().pointerWidget() == this;
    
    Gfx::Brush bkgr = backgroundBrush();
    const Gfx::Color& bkgColor = bkgr.color();

    if( mouseOver )
    {
        setBackgroundBrush( Gfx::Color(bkgColor.red() * 0.9f ,
                                       bkgColor.green() * 0.9f ,
                                       bkgColor.blue() * 0.9f ), false );
    }

    if( _isPressed )
    {
        setBackgroundBrush( Gfx::Color(bkgColor.red() * 0.8f ,
                                       bkgColor.green() * 0.8f ,
                                       bkgColor.blue() * 0.8f ), false );
    }
    
    Label::onPaintBackground(surface, updateRect);
    
    setBackgroundBrush(bkgr, false);
}


void Button::onPaintContent(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Label::onPaintContent(surface, updateRect);
    
    if( hasFocus() )
    {
        Gfx::SizeF size = this->size();
        size.addHeight(-4);
        size.addWidth(-4);

        Gfx::Color armedColor(Gfx::Color::fromRgb8(176,176,176));
        Gfx::Pen pen(armedColor, 1, Gfx::Pen::Dash);
        
        Painter painter( surface );
        painter.setClip(updateRect);
        painter.setPen(pen);
        Gfx::RectF rect(Gfx::PointF(2,2), size);
        painter.drawRect(rect);
    }
}

} // namespace

} // namespace
