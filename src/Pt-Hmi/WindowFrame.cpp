/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2015 Marc Boris Duerner
  
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

#include "WindowFrame.h"
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/MoveEvent.h>

namespace {

Pt::Gfx::Color brighten(const Pt::Gfx::Color& c, float factor)
{    
    float rf = c.red() * factor;
    float gf = c.green() * factor;
    float bf = c.blue() * factor;

    Pt::uint16_t r = rf > 65535 ? 65535 : (Pt::uint16_t) rf ;
    Pt::uint16_t g = gf > 65535 ? 65535 : (Pt::uint16_t) gf ;
    Pt::uint16_t b = bf > 65535 ? 65535 : (Pt::uint16_t) bf ;

    return Pt::Gfx::Color(c.alpha(), r, g, b);
}

}

namespace Pt {

namespace Hmi {

//
// WindowButton
//

WindowButton::WindowButton()
: _frame(0)
, _isPressed(false)
{
}


WindowButton::~WindowButton()
{
}


void WindowButton::update()
{
    if(_frame)
        _frame->update( _geometry);
}


void WindowButton::moveEvent(const MoveEvent& mev)
{
    _geometry.setOrigin( mev.position() );
}


void WindowButton::resizeEvent(const ResizeEvent& rev)
{
    _geometry.setSize( rev.size() );
}


void WindowButton::enterEvent(const EnterEvent& eev)
{
}


void WindowButton::leaveEvent(const LeaveEvent& lev)
{
    if(_isPressed)
    {
        _isPressed = false;
        update();
    }
}


void WindowButton::mouseEvent(const MouseEvent& mev)
{
    Application::instance().setCursor( &Cursor::defaultCursor() ); 

    bool isPressed = mev.isPress() || (_isPressed && mev.isPressed());

    if(_isPressed != isPressed)
        update();

    bool wasPressed = _isPressed;
    _isPressed = isPressed;
    
    if( wasPressed && mev.isRelease() )
        _clicked.send();
}


void WindowButton::touchEvent(const TouchEvent& tev)
{
    bool isPressed = tev.isPress() || tev.isMove();

    if(_isPressed != isPressed)
        update();

    bool wasPressed = _isPressed;
    _isPressed = isPressed;
    
    if( wasPressed && tev.isRelease() )
        _clicked.send();
}


void WindowButton::paint(PaintSurface& surface, const Gfx::RectF& rect)
{
    Painter painter(surface);
    painter.setClip(rect);
   
    Gfx::Color light = brighten(color(), 1.25f);
    Gfx::Color dark = brighten(color(), 0.75f);

    Gfx::Color backgroundColor = color();
    Gfx::Color borderTopLeftColor = light;
    Gfx::Color borderBottomRightColor = dark;
    if(_isPressed)
    {
        backgroundColor = brighten(color(), 0.9f);
        borderTopLeftColor = dark;
        borderBottomRightColor = light;
    }

    //
    // fill background
    //
    Gfx::Brush brush = backgroundColor;
    painter.setBrush(brush);
    painter.fillRect(_geometry);

    //
    // bottom right border
    //
    painter.setPen( Gfx::Pen(1, borderBottomRightColor) );
    painter.drawLine(Gfx::PointF(_geometry.topRight().x(),
                                 _geometry.topRight().y() +1), 
                     Gfx::PointF(_geometry.bottomRight().x(),
                                 _geometry.bottomRight().y() + 1) );
    painter.drawLine(Gfx::PointF(_geometry.bottomLeft().x() + 1,
                                 _geometry.bottomLeft().y() ),
                     Gfx::PointF(_geometry.bottomRight().x() + 1,
                                 _geometry.bottomRight().y() ) );

    //
    // top left border
    //
    painter.setPen( Gfx::Pen(1, borderTopLeftColor) );
    painter.drawLine(_geometry.topLeft(), _geometry.topRight() );
    painter.drawLine(_geometry.topLeft(), _geometry.bottomLeft() );
}

//
// MaximizeButton
//

MinimizeButton::MinimizeButton()
{
    setColor( Gfx::Color(62258, 45874, 3276 ));
}


MinimizeButton::~MinimizeButton()
{
}


void MinimizeButton::paint(PaintSurface& surface, const Gfx::RectF& rect)
{
    WindowButton::paint(surface, rect);

    Painter painter(surface);
    painter.setClip(rect);

    //
    // draw symbol
    //
    Gfx::Pen pen(2, Gfx::Color(65535, 65535, 65535), 
                 Gfx::Pen::SolidStyle, Gfx::Pen::FlatCap);
    painter.setPen(pen);

    Gfx::PointF tl = geometry().topLeft() + Gfx::PointF(5, 5);
    Gfx::PointF tr = geometry().topRight() + Gfx::PointF(-4, 4);
    Gfx::PointF br = geometry().bottomRight() - Gfx::PointF(4, 4);
    Gfx::PointF bl = geometry().bottomLeft() - Gfx::PointF(-4, 4);
    painter.drawLine(bl, br);
    painter.drawLine(tl, bl);
    painter.drawLine(tr, bl);
}

//
// MaximizeButton
//

MaximizeButton::MaximizeButton()
{
    setColor( Gfx::Color(22937, 42597, 16383) );
}


MaximizeButton::~MaximizeButton()
{
}


void MaximizeButton::paint(PaintSurface& surface, const Gfx::RectF& rect)
{
    WindowButton::paint(surface, rect);

    Painter painter(surface);
    painter.setClip(rect);

    //
    // draw symbol
    //
    Pt::Gfx::Pen pen(2, Gfx::Color(65535, 65535, 65535), 
                     Gfx::Pen::SolidStyle, Gfx::Pen::FlatCap);
    painter.setPen(pen);

    Gfx::PointF tl = geometry().topLeft() + Gfx::PointF(5, 5);
    Gfx::PointF tr = geometry().topRight() + Gfx::PointF(-4, 4);
    Gfx::PointF br = geometry().bottomRight() - Gfx::PointF(4, 4);
    Gfx::PointF bl = geometry().bottomLeft() - Gfx::PointF(-4, 4);
    painter.drawLine(tl, tr);
    painter.drawLine(bl, tr);
    painter.drawLine(br, tr);
}

//
// CloseButton
//

CloseButton::CloseButton()
{
    setColor( Gfx::Color(53738, 16383, 14417) );
}


CloseButton::~CloseButton()
{
}


void CloseButton::paint(PaintSurface& surface, const Gfx::RectF& rect)
{
    WindowButton::paint(surface, rect);

    Painter painter(surface);
    painter.setClip(rect);

    //
    // draw symbol
    Pt::Gfx::Pen pen(2, Gfx::Color(65535, 65535, 65535), 
                     Gfx::Pen::SolidStyle, Gfx::Pen::RoundCap);
    painter.setPen(pen);

    Gfx::PointF tl = geometry().topLeft() + Gfx::PointF(4, 4);
    Gfx::PointF br = geometry().bottomRight() - Gfx::PointF(4, 4);
    Gfx::PointF tr = geometry().topRight() + Gfx::PointF(-4, 4);
    Gfx::PointF bl = geometry().bottomLeft() - Gfx::PointF(-4, 4);
    painter.drawLine(tl, br);
    painter.drawLine(tr, bl);
}

//
// MenuButton
//

MenuButton::MenuButton()
{
}


MenuButton::~MenuButton()
{
}


void MenuButton::paint(PaintSurface& surface, const Gfx::RectF& rect)
{
    Painter painter(surface);
    painter.setClip(rect);

    //
    // draw symbol
    //
    Gfx::PointF triangle[3];
    triangle[0] = geometry().topLeft() + Gfx::PointF(4, 4);
    triangle[1] = geometry().topRight() + Gfx::PointF(-2, 4);
    Gfx::PointF mid(geometry().width() / 2, geometry().height() / 2);
    mid.addY(1); 
    triangle[2] = geometry().topLeft() + mid;

    Gfx::Brush brush( Gfx::Color(65535,65535,65535) );
    painter.setBrush(brush);
    painter.fillPolygon(triangle, 3);
}

//
// WindowFrame
//

WindowFrame::WindowFrame()
: _wm(0)
, _window(0)
, _borderWidth(0)
, _titleHeight(0)
, _state(Window::Normal)
{
}


WindowFrame::WindowFrame(WindowManager& wm, Window& window)
: _wm(&wm)
, _window(&window)
, _borderWidth(0)
, _titleHeight(0)
, _state( window.state() )
, _isClient(false)
, _isMoving(false)
, _isLeftResizing(false)
, _isRightResizing(false)
, _isTopResizing(false)
, _isBottomResizing(false)
{
    _maximizeButton.setParent(*this);
    _maximizeButton.clicked() += Pt::slot(*this, &WindowFrame::onMaximize);
    _buttons.push_back(&_maximizeButton);

    _minimizeButton.setParent(*this);
    _minimizeButton.clicked() += Pt::slot(*this, &WindowFrame::onMinimize);
    _buttons.push_back(&_minimizeButton);

    _closeButton.setParent(*this);
    _closeButton.clicked() += Pt::slot(*this, &WindowFrame::onClose);
    _buttons.push_back(&_closeButton);

    _menuButton.setParent(*this);
    _menuButton.clicked() += Pt::slot(*this, &WindowFrame::onMenu);
    _buttons.push_back(&_menuButton);
}


WindowFrame::~WindowFrame()
{
}


Window* WindowFrame::window()
{
    return _window;
}


const Window* WindowFrame::window() const
{
    return _window;
}


Window::State WindowFrame::state() const
{
    return _state;
}


void WindowFrame::setState(Window::State state)
{
    _state = state;
}


const Gfx::PointF& WindowFrame::restorePosition() const
{
    return _restorePos;
}


const Gfx::SizeF& WindowFrame::restoreSize() const
{
    return _restoreSize;
}

      
void WindowFrame::setRestore(const Gfx::PointF& pos, const Gfx::SizeF& size)
{
    _restorePos = _window->position();
    _restoreSize = _window->size();
}


const Gfx::RectF& WindowFrame::clientRect() const
{
    return _clientRect;
}


const Gfx::RectF& WindowFrame::frameRect() const
{
    return _frameRect;
}


void WindowFrame::setFrame(double bw, double th)
{ 
    _borderWidth = bw;
    _titleHeight = th;
}


Gfx::PointF WindowFrame::toFrame(const Gfx::PointF& pos) const
{
    double offY = _borderWidth + _titleHeight;
    double offX = _borderWidth;

    return pos + Gfx::PointF(offX, offY);
}


Gfx::PointF WindowFrame::fromFrame(const Gfx::PointF& pos) const
{
    double offY = _borderWidth + _titleHeight;
    double offX = _borderWidth;

    return pos - Gfx::PointF(offX, offY);
}


Gfx::SizeF WindowFrame::fromFrame(const Gfx::SizeF& size) const
{
    double offY = (2*_borderWidth) + _titleHeight;
    double offX = 2*_borderWidth;

    return Gfx::SizeF(size.width() - offX, size.height() - offY);
}


void WindowFrame::onMenu()
{
}


void WindowFrame::onMinimize()
{
    if(_state == Window::Minimized)
        _window->setState(Window::Normal);
    else 
        _window->setState(Window::Minimized);
}


void WindowFrame::onMaximize()
{
    if(_state == Window::Maximized)
        _window->setState(Window::Normal);
    else 
        _window->setState(Window::Maximized);
}


void WindowFrame::onClose()
{
    _window->close();
}


void WindowFrame::update()
{
    update(_frameRect);
}


void WindowFrame::update(const Gfx::RectF& rect)
{
    Gfx::PointF updatePos = _window->fromParent( rect.topLeft() );
    Gfx::RectF updateRect(updatePos, rect.size());
    
    _window->update(updateRect);
}


void WindowFrame::moveEvent(const MoveEvent& mev)
{
    _frameRect.setOrigin( mev.position() );

    Gfx::PointF clientPos = mev.position();
    clientPos.addX(_borderWidth);
    clientPos.addY(_borderWidth + _titleHeight);
    _clientRect.setOrigin(clientPos);

    onLayout();
}


void WindowFrame::resizeEvent(const ResizeEvent& rev)
{
    _clientRect.setSize( rev.size() );
    
    Gfx::SizeF frameSize = rev.size();
    frameSize.addWidth(2 * _borderWidth);
    frameSize.addHeight(2 * _borderWidth);
    frameSize.addHeight(_titleHeight);
    _frameRect.setSize(frameSize);

    onLayout();
}


void WindowFrame::onLayout()
{
    double buttonWidth = _titleHeight - _borderWidth;

    Gfx::PointF menuPos(_frameRect.x() + _borderWidth, _frameRect.y() + _borderWidth);
    _menuButton.moveEvent( MoveEvent(0, menuPos ) );
    _menuButton.resizeEvent( ResizeEvent(0, Gfx::SizeF(buttonWidth, buttonWidth) ) );

    double buttonX = _frameRect.x() + _frameRect.width() - (_borderWidth + buttonWidth);
    double buttonY = _frameRect.y() + _borderWidth;

    _closeButton.moveEvent( MoveEvent(0, Gfx::PointF(buttonX, buttonY) ) );
    _closeButton.resizeEvent( ResizeEvent(0, Gfx::SizeF(buttonWidth, buttonWidth) ) );

    buttonX -= _borderWidth + buttonWidth;
    _maximizeButton.moveEvent( MoveEvent(0, Gfx::PointF(buttonX, buttonY) ) );
    _maximizeButton.resizeEvent( ResizeEvent(0, Gfx::SizeF(buttonWidth, buttonWidth) ) );

    buttonX -= _borderWidth + buttonWidth;
    _minimizeButton.moveEvent( MoveEvent(0, Gfx::PointF(buttonX, buttonY) ) );
    _minimizeButton.resizeEvent( ResizeEvent(0, Gfx::SizeF(buttonWidth, buttonWidth) ) );
}


void WindowFrame::enterEvent(const EnterEvent& eev)
{
}


void WindowFrame::leaveEvent(const LeaveEvent& lev)
{   
    if(_isClient)
    {
        _isClient = false;
        LeaveEvent lev(_window->vid());
        _window->processEvent(lev);
    }

    std::vector<WindowButton*>::iterator it;
    for(it = _buttons.begin(); it != _buttons.end(); ++it)
    {
        WindowButton* button = *it;
        
        if( button->geometry().contains(_lastPointer) )
            button->leaveEvent(lev);
    }
}


bool WindowFrame::mouseEvent(const MouseEvent& mev)
{
    bool r = onMouseEvent(mev);
    _lastPointer = mev.position();
    return r;
}


bool WindowFrame::touchEvent(const TouchEvent& tev)
{
    bool r = onTouchEvent(tev);
    _lastPointer = tev.position();
    return r;
}


bool WindowFrame::onMouseEvent(const MouseEvent& mev)
{
    Window* window = checkWindow( mev.position() );
    if(window)
    {
        if( window->isEnabled() )
        {
            Gfx::PointF pos = mev.position() - _clientRect.topLeft();
            MouseEvent mev2 = mev;
            mev2.setPosition(pos);
            window->processEvent(mev2);
        }
        
        return false;
    }

    WindowButton* button = checkButton( mev.position() );
    if(button)
    {
        button->mouseEvent(mev);
        return false;
    }

    if( isTitle( mev.position() ) )
    {
        Application::instance().setCursor( &Cursor::moveCursor() );
        
        return checkMove(mev.position(), mev.isPressed(), mev.isPress() );
    }

    bool onLeftBorder = isLeftBorder( mev.position() );
    bool onRightBorder = isRightBorder( mev.position() );
    bool onTopBorder = isTopBorder( mev.position() );
    bool onBottomBorder = isBottomBorder( mev.position() );
    
    if(onLeftBorder || onRightBorder || onTopBorder || onBottomBorder)
    {
        if( (onTopBorder && onRightBorder) || (onBottomBorder && onLeftBorder) )
            Application::instance().setCursor( &Hmi::Cursor::sizeNESWCursor() );
        else if((onTopBorder && onLeftBorder) || (onBottomBorder && onRightBorder) )
            Application::instance().setCursor( &Hmi::Cursor::sizeNWSECursor() );
        else if(onRightBorder || onLeftBorder)
            Application::instance().setCursor( &Hmi::Cursor::sizeWECursor() );
        else if(onTopBorder || onBottomBorder)
            Application::instance().setCursor( &Hmi::Cursor::sizeNSCursor() );

        return checkResize(mev.position(), mev.isPressed(), mev.isPress());
    }

    Application::instance().setCursor( &Cursor::defaultCursor() );
    return false;
}


bool WindowFrame::onTouchEvent(const TouchEvent& tev)
{
    Window* window = checkWindow( tev.position() );
    if(window)
    {
        if( window->isEnabled() )
        {
            Gfx::PointF pos = tev.position() - _clientRect.topLeft();
            TouchEvent tev2 = tev;
            tev2.setPosition(pos);
            window->processEvent(tev2);
        }
        
        return false;
    }

    WindowButton* button = checkButton( tev.position() );
    if(button)
    {
        button->touchEvent(tev);
        return false;
    }

    bool isDrag = tev.isPress() || tev.isMove();
    bool isPress = tev.isPress();

    bool isMove = checkMove(tev.position(), isDrag, isPress);
    if(isMove)
    {
        return true;
    }

    return checkResize(tev.position(), isDrag, isPress);
}


bool WindowFrame::isTitle(const Gfx::PointF& p) const
{            
    bool isResizing = _isLeftResizing || _isRightResizing || 
                      _isTopResizing || _isBottomResizing;


    Gfx::PointF localPos = p - _frameRect.topLeft();

    bool overTitle = localPos.x() >= _borderWidth && 
                     localPos.x() < _borderWidth + _clientRect.width() &&
                     localPos.y() >= _borderWidth &&
                     localPos.y() < _borderWidth + _titleHeight;

    return (_isMoving || overTitle) && ! isResizing;
}


bool WindowFrame::isLeftBorder(const Pt::Gfx::PointF& p) const
{        
    Gfx::PointF localPos = p - _frameRect.topLeft();

    bool r =  localPos.x() >= 0 &&
              localPos.x() < _borderWidth &&
              localPos.y() >= 0 &&
              localPos.y() < _frameRect.height();

    return _isLeftResizing || r;
}


bool WindowFrame::isRightBorder(const Pt::Gfx::PointF& p) const
{        
    Gfx::PointF localPos = p - _frameRect.topLeft();

    bool r =   localPos.x() >= _borderWidth + _clientRect.width() &&
               localPos.x() < 2 * _borderWidth + _clientRect.width() &&
               localPos.y() >= 0 &&
               localPos.y() < _frameRect.height();

    return _isRightResizing || r;
}


bool WindowFrame::isTopBorder(const Pt::Gfx::PointF& p) const
{        
    Gfx::PointF localPos = p - _frameRect.topLeft();

    bool r =   localPos.x() >= 0 &&
               localPos.x() < _frameRect.width() &&
               localPos.y() >= 0 &&
               localPos.y() < _borderWidth;
    
    return _isTopResizing || r;
}


bool WindowFrame::isBottomBorder(const Pt::Gfx::PointF& p) const
{        
    Gfx::PointF localPos = p - _frameRect.topLeft();

    double minY = _clientRect.height() + _borderWidth + _titleHeight;

    bool r =   localPos.x() >= 0 &&
               localPos.x() < _frameRect.width() &&
               localPos.y() >= minY &&
               localPos.y() < _frameRect.height();
    
    return _isBottomResizing || r;
}


Window* WindowFrame::checkWindow(const Gfx::PointF& pos)
{
    bool isResizing = _isLeftResizing || _isRightResizing || 
                      _isTopResizing || _isBottomResizing;

    if( ! _isMoving && ! isResizing )
    {
        if(_clientRect.contains( pos ) )
        {        
            if( ! _isClient )
            {
                _isClient = true;
                EnterEvent eev(_window->vid());
                _window->processEvent(eev);
            }

            return _window;
        }

        if(_isClient)
        {
            _isClient = false;
            LeaveEvent lev(_window->vid());
            _window->processEvent(lev);
        }
    }

    return 0;
}


WindowButton* WindowFrame::checkButton(const Gfx::PointF& pos)
{
    bool isResizing = _isLeftResizing || _isRightResizing || 
                      _isTopResizing || _isBottomResizing;
    
    if( ! _isMoving && ! isResizing )
    {
        std::vector<WindowButton*>::iterator it;
        for(it = _buttons.begin(); it != _buttons.end(); ++it)
        {
            WindowButton* button = *it;
        
            if(button->geometry().contains(pos) )
            {
                return button;
            }
            else if(button->geometry().contains(_lastPointer) )
            {
                button->leaveEvent( LeaveEvent(0) );
            }
        }
    }

    return 0;
}


bool WindowFrame::checkMove(const Gfx::PointF& pos, bool isDrag, bool isPress)
{
    if( isTitle(pos) )
    {       
        _isMoving = (_isMoving && isDrag) || isPress;

        if(_isMoving && ! isPress)
        {
            Gfx::PointF to = _window->position() + pos - _lastPointer;
            _window->move(to);
        }
        
        return _isMoving;
    }

    return false;
}
 

bool WindowFrame::checkResize(const Gfx::PointF& pos, bool isDrag, bool isPress)
{
    bool onLeftBorder = isLeftBorder(pos);
    bool onRightBorder = isRightBorder(pos);
    bool onTopBorder = isTopBorder(pos);
    bool onBottomBorder = isBottomBorder(pos);
    
    if(onLeftBorder || onRightBorder || onTopBorder || onBottomBorder)
    {
        _isLeftResizing   = onLeftBorder && ((_isLeftResizing && isDrag) || isPress);
        _isRightResizing  = onRightBorder && ((_isRightResizing && isDrag) || isPress);
        _isTopResizing    = onTopBorder && ((_isTopResizing && isDrag) || isPress);
        _isBottomResizing = onBottomBorder && ((_isBottomResizing && isDrag) || isPress);

        bool isResizing = _isLeftResizing || _isRightResizing || 
                          _isTopResizing || _isBottomResizing;

        if(isResizing && ! isPress)
        {
            Gfx::SizeF size = _window->size();
            Gfx::PointF winpos = _window->position();
            Gfx::PointF delta = pos - _lastPointer;

            if( _isLeftResizing )
            {
                winpos.addX( delta.x() );
                size.subWidth(delta.x());
            }   

            if(_isRightResizing)
                size.addWidth( delta.x() );
            
            if(_isTopResizing)
            {
                winpos.addY( delta.y() );
                size.subHeight(delta.y());
            }

            if(_isBottomResizing)
                size.addHeight( delta.y() );
            
            if( size != _window->size() )
                _window->resize(size);

            if( winpos != _window->position() )
                _window->move(winpos);
        }

        return isResizing;
    }

    return false;
}


void WindowFrame::paint(PaintSurface& surface, const Gfx::RectF& rect)
{
    if( _borderWidth < 0.1 && _titleHeight < 0.1  )
        return;

    // TODO: clipping region
    Painter painter(surface); 
    painter.setClip(rect);   

    Gfx::Color color = _window->isActive() ? _wm->activeColor() 
                                           : _wm->inactiveColor();
    //
    // frame background
    //
    
    Gfx::Brush brush(color);
    painter.setBrush(brush);

    Gfx::PointF pos = _window->position();

    Gfx::RectF leftBorder( pos.x(), 
                           pos.x() + _borderWidth - 1,
                           pos.y() + _borderWidth, 
                           pos.y() + _frameRect.height() - _borderWidth - 1 );
    painter.fillRect(leftBorder);

    Gfx::RectF topBorder(pos.x(),
                         pos.x() + _frameRect.width() - 1,
                         pos.y(),
                         pos.y() + _borderWidth - 1);
    painter.fillRect(topBorder);

    Gfx::RectF rightBorder(pos.x() + _frameRect.width() - _borderWidth,
                           pos.x() + _frameRect.width() - 1,
                           pos.y() + _borderWidth,
                           pos.y() + _frameRect.height() - _borderWidth - 1 );
    painter.fillRect(rightBorder);

    Gfx::RectF bottomBorder(pos.x(),
                            pos.x() + _frameRect.width() - 1,
                            pos.y() + _frameRect.height() - _borderWidth,
                            pos.y() + _frameRect.height() - 1);
    painter.fillRect(bottomBorder);

    Gfx::RectF titleArea( pos.x() + _borderWidth,
                          pos.x() + _frameRect.width() - _borderWidth - 1,
                          pos.y() + _borderWidth,
                          pos.y() + _borderWidth + _titleHeight - 1);
    painter.fillRect(titleArea);
 
    //
    // light outer and inner border contour
    //
    Gfx::Color borderLight = brighten(color, 1.25f);
    Gfx::Pen borderPenLight(1, borderLight);

    painter.setPen(borderPenLight);
    painter.drawLine(_frameRect.topLeft(), 
                     Gfx::PointF(_frameRect.topRight().x() + 1,
                                 _frameRect.topRight().y()) );
    
    painter.drawLine(_frameRect.topLeft(), 
                     Gfx::PointF(_frameRect.bottomLeft().x(),
                                 _frameRect.bottomLeft().y() + 1) );

    painter.drawLine( Gfx::PointF(_frameRect.topRight().x() - (_borderWidth -1),
                                  _frameRect.topRight().y() + (_borderWidth) + _titleHeight),
                      Gfx::PointF(_frameRect.bottomRight().x() - (_borderWidth -1),
                                  _frameRect.bottomRight().y() - (_borderWidth-1)) );
    
    painter.drawLine( Gfx::PointF(_frameRect.bottomLeft().x() + (_borderWidth),
                                  _frameRect.bottomLeft().y() - (_borderWidth - 1)),
                      Gfx::PointF(_frameRect.bottomRight().x() - (_borderWidth -2),
                                  _frameRect.bottomRight().y() - (_borderWidth - 1)) );

    //
    // dark outer and inner border contour
    //
    Gfx::Color borderDark = brighten(color, 0.75f);
    Gfx::Pen borderPenDark(1, borderDark);

    painter.setPen(borderPenDark);
    painter.drawLine( Gfx::PointF(_frameRect.bottomLeft().x() + 1,
                                  _frameRect.bottomLeft().y()), 
                      _frameRect.bottomRight() );

    painter.drawLine(Gfx::PointF( _frameRect.topRight().x(),
                                  _frameRect.topRight().y() + 1 ), 
                     Gfx::PointF(_frameRect.bottomRight().x(),
                                 _frameRect.bottomRight().y() + 1) );

    painter.drawLine( Gfx::PointF(_frameRect.topLeft().x() + (_borderWidth-1),
                                  _frameRect.topLeft().y() + (_borderWidth) + _titleHeight),
                      Gfx::PointF(_frameRect.bottomLeft().x() + (_borderWidth-1),
                                  _frameRect.bottomLeft().y() - (_borderWidth - 2)) );

    painter.drawLine( Gfx::PointF(_frameRect.topLeft().x() + (_borderWidth-1),
                                  _frameRect.topLeft().y() + (_borderWidth-1) + _titleHeight),
                      Gfx::PointF(_frameRect.topRight().x() - (_borderWidth-2),
                                 _frameRect.topRight().y() + (_borderWidth-1) + _titleHeight) );



    //
    // title bar text
    //
    Pt::String title = _window->title().c_str();

    const Gfx::Font& font = _wm->font();
    painter.setFont(font);
    Gfx::FontMetrics fm = painter.fontMetrics(title);

    Gfx::PointF textPos(pos.x() + _borderWidth + _titleHeight, 
                        pos.y() + _titleHeight - _borderWidth);

    Gfx::Color textColor = _window->isActive() ? _wm->textColor() 
                                               : _wm->inactiveTextColor(); 
    Gfx::Pen pen(1, textColor);
    painter.setPen(pen);
    painter.drawText(textPos, title);

    //
    // grip area on title bar
    //
    Gfx::Color gripColorLight( color.red() * 1.1f, 
                               color.green() * 1.1f, 
                               color.blue() * 1.1f );
    Gfx::Pen gripPenLight(1, gripColorLight);

    Gfx::Color gripColorDark( color.red() * 0.9f, 
                              color.green() * 0.9f, 
                              color.blue() * 0.9f );
    Gfx::Pen gripPenDark(1, gripColorDark);


    Gfx::PointF gripStart( textPos.x() + fm.width() + _borderWidth, 
                           pos.y() + _borderWidth - 2); // approx.
    Gfx::PointF gripEnd(pos.x() + _frameRect.width() - _borderWidth - 3*_titleHeight,
                        pos.y() + _borderWidth - 2); // approx.
    
    for(int n = 0; n < 4; ++n)
    {
        gripStart.setY(gripStart.y() + 3);
        gripEnd.setY(gripEnd.y() + 3);

        painter.setPen(gripPenLight);
        painter.drawLine(gripStart, gripEnd);

        gripStart.setY(gripStart.y() + 1);
        gripEnd.setY(gripEnd.y() + 1);

        painter.setPen(gripPenDark);
        painter.drawLine(gripStart, gripEnd);
    }

    std::vector<WindowButton*>::iterator it;
    for(it = _buttons.begin(); it != _buttons.end(); ++it)
    {
        WindowButton* button = *it;

        Gfx::RectF buttonUpdateRect = button->geometry().intersect(rect);
        if( buttonUpdateRect.isNull() )
            continue;

        button->paint(surface, buttonUpdateRect);
    }           
}

} // namespace

} // namespace
