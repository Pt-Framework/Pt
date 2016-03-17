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
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/TouchEvent.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Point.h>

namespace Pt {

namespace Hmi {

Button::Button()
: _image(Gfx::Image(0,0))
, _imageAlign( MiddleLeft)
, _isPressed(false)
{
  setBackgroundColor(Gfx::Color::fromRgb8(245,245,245));

  setAutoSize(true);
  setContentAlignment(MiddleCenter);
  setAcceptFocus(true);

  setBorderWidth(1);
  setBorderRound(true);
  setBorderStyle(Panel::Single);
}


Button::~Button()
{
}


void Button::onClicked(const Gfx::PointF& pos)
{
    Label::onClicked(pos);
    _clicked.send(*this);
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


void Button::onPointerEvent(const MouseEvent& ev)
{    
    Label::onPointerEvent(ev);

    if( ev.isPressed(MouseEvent::Left) != _isPressed )
    {
        _isPressed = ev.isPressed(MouseEvent::Left);
        repaint();
    }
}


void Button::onPointerEnter()
{
    Label::onPointerEnter();
    repaint();
}


void Button::onPointerLeave()
{
    _isPressed = false;
    Label::onPointerLeave();
    repaint();
}


void Button::onTouchEvent(const TouchEvent& ev)
{    
    Label::onTouchEvent(ev);

    if( ev.isPress() != _isPressed )
    {
        _isPressed = ev.isPress();
        repaint();
    }
}


void Button::onFocus(bool hasFocus)
{    
    Label::onFocus(hasFocus);
    repaint();
}


void Button::onPaint(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    bool mouseOver = window()->pointerWidget() == this;
    Gfx::Color bkgColor = backgroundColor();

    if( ! enabled() )
    {
        Label::onPaint(surface, updateRect);
        return;
    }

    /*
    if( mouseOver )
    {
        setBackgroundColor( Gfx::Color(bkgColor.red() * 0.9f ,
                                       bkgColor.green() * 0.9f ,
                                       bkgColor.blue() * 0.9f ) );
    }

    if( _isPressed )
    {
        setBackgroundColor( Gfx::Color(bkgColor.red() * 0.8f ,
                                       bkgColor.green() * 0.8f ,
                                       bkgColor.blue() * 0.8f ) );
    }
    */
    Label::onPaint(surface, updateRect);
    
    //setBackgroundColor(bkgColor);

    if( hasFocus() )
    {
        Gfx::SizeF size = this->size();
        size.addHeight(-4);
        size.addWidth(-4);

        Gfx::Color armedColor(1, 0, 0);
        Gfx::Pen pen(1, armedColor, Gfx::Pen::DashStyle);
        
        Painter painter( surface );
        painter.setPen(pen);
        Gfx::RectF rect(Gfx::PointF(2,2), size);
        painter.drawRect(rect);
    }
}

} // namespace

} // namespace
