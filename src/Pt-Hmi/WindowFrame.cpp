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
    float r = c.red() * factor;
    float g = c.green() * factor;
    float b = c.blue() * factor;

    r = r > 1.0f ? 1.0f : r;
    g = g > 1.0f ? 1.0f : g;
    b = b > 1.0f ? 1.0f : b;

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


void WindowButton::paintEvent(const PaintEvent& pev)
{
    PaintSurface& surface = _frame->window()->parent()->surface();
    Painter painter(surface);
   
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
    setColor( Gfx::Color(0.95f, 0.7f, 0.05f) );
}


MinimizeButton::~MinimizeButton()
{
}


void MinimizeButton::paintEvent(const PaintEvent& pev)
{
    WindowButton::paintEvent(pev);

    PaintSurface& surface = parent()->window()->parent()->surface();
    Painter painter(surface);

    //
    // draw symbol
    //
    Gfx::Pen pen(2, Gfx::Color(1, 1, 1), 
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
    setColor( Gfx::Color(0.35f, 0.65f, 0.25f) );
}


MaximizeButton::~MaximizeButton()
{
}


void MaximizeButton::paintEvent(const PaintEvent& pev)
{
    WindowButton::paintEvent(pev);

    PaintSurface& surface = parent()->window()->parent()->surface();
    Painter painter(surface);

    //
    // draw symbol
    //
    Pt::Gfx::Pen pen(2, Gfx::Color(1, 1, 1), 
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
    setColor( Gfx::Color(0.82f, 0.25f, 0.22f) );
}


CloseButton::~CloseButton()
{
}


void CloseButton::paintEvent(const PaintEvent& pev)
{
    WindowButton::paintEvent(pev);

    PaintSurface& surface = parent()->window()->parent()->surface();
    Painter painter(surface);

    //
    // draw symbol
    Pt::Gfx::Pen pen(2, Gfx::Color(1, 1, 1), 
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


void MenuButton::paintEvent(const PaintEvent& pev)
{
    PaintSurface& surface = parent()->window()->parent()->surface();
    Painter painter(surface);

    //
    // draw symbol
    //
    Gfx::PointF triangle[3];
    triangle[0] = geometry().topLeft() + Gfx::PointF(4, 4);
    triangle[1] = geometry().topRight() + Gfx::PointF(-2, 4);
    Gfx::PointF mid(geometry().width() / 2, geometry().height() / 2);
    mid.addY(1); 
    triangle[2] = geometry().topLeft() + mid;

    Gfx::Brush brush( Gfx::Color(1,1,1) );
    painter.setBrush(brush);
    painter.fillPolygon(triangle, 3);
}

//
// WindowFrame
//

WindowFrame::WindowFrame()
: _wm(0)
, _window(0)
{
}


WindowFrame::WindowFrame(WindowManager& wm, Window& window)
: _wm(&wm)
, _window(&window)
, _isClient(false)
, _isMoving(false)
, _isLeftResizing(false)
, _isRightResizing(false)
, _isTopResizing(false)
, _isBottomResizing(false)
{
    _maximizeButton.setParent(*this);
    _maximizeButton.clicked() += Pt::slot(*this, &WindowFrame::onMaximize);

    _minimizeButton.setParent(*this);
    _minimizeButton.clicked() += Pt::slot(*this, &WindowFrame::onMinimize);

    _closeButton.setParent(*this);
    _closeButton.clicked() += Pt::slot(*this, &WindowFrame::onClose);

    _menuButton.setParent(*this);
    _menuButton.clicked() += Pt::slot(*this, &WindowFrame::onMenu);
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


Gfx::RectF WindowFrame::clientRect() const
{
    return _clientRect;
}


Gfx::RectF WindowFrame::frameRect() const
{
    return _frameRect;
}


void WindowFrame::onMenu()
{
}


void WindowFrame::onMinimize()
{
}


void WindowFrame::onMaximize()
{
}


void WindowFrame::onClose()
{
    _window->close();
}


bool WindowFrame::isTitle(const Gfx::PointF& p) const
{            
    Gfx::PointF localPos = p - _frameRect.topLeft();

    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();

    return localPos.x() >= borderWidth && 
           localPos.x() < borderWidth + _clientRect.width() &&
           localPos.y() >= borderWidth &&
           localPos.y() < borderWidth + titleHeight;
}


bool WindowFrame::isLeftBorder(const Pt::Gfx::PointF& p) const
{        
    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();

    Gfx::PointF localPos = p - _frameRect.topLeft();

    return localPos.x() >= 0 &&
           localPos.x() < borderWidth &&
           localPos.y() >= 0 &&
           localPos.y() < _frameRect.height();
}


bool WindowFrame::isRightBorder(const Pt::Gfx::PointF& p) const
{        
    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();

    Gfx::PointF localPos = p - _frameRect.topLeft();

    return localPos.x() >= borderWidth + _clientRect.width() &&
           localPos.x() < 2 * borderWidth + _clientRect.width() &&
           localPos.y() >= 0 &&
           localPos.y() < _frameRect.height();
}


bool WindowFrame::isTopBorder(const Pt::Gfx::PointF& p) const
{        
    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();

    Gfx::PointF localPos = p - _frameRect.topLeft();

    return localPos.x() >= 0 &&
           localPos.x() < _frameRect.width() &&
           localPos.y() >= 0 &&
           localPos.y() < borderWidth;
}


bool WindowFrame::isBottomBorder(const Pt::Gfx::PointF& p) const
{        
    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();

    Gfx::PointF localPos = p - _frameRect.topLeft();

    return localPos.x() >= 0 &&
           localPos.x() < _frameRect.width() &&
           localPos.y() >= borderWidth + titleHeight + _clientRect.height() &&
           localPos.y() < _frameRect.height();
}


void WindowFrame::update()
{
    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();

    Gfx::PointF updatePos(0, 0);
    updatePos.subX(borderWidth);
    updatePos.subY(borderWidth +  titleHeight);

    Gfx::RectF updateRect(updatePos, _frameRect.size());
    _window->update(updateRect);
}


void WindowFrame::update(const Gfx::RectF& rect)
{
    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();

    Gfx::PointF updatePos = rect.topLeft() - _frameRect.topLeft();
    updatePos.subX(borderWidth);
    updatePos.subY(borderWidth +  titleHeight);

    Gfx::RectF updateRect(updatePos, rect.size());
    _window->update(updateRect);
}


void WindowFrame::moveEvent(const MoveEvent& mev)
{
    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();

    _frameRect.setOrigin( mev.position() );

    Gfx::PointF clientPos = mev.position();
    clientPos.addX(borderWidth);
    clientPos.addY(borderWidth + titleHeight);
    _clientRect.setOrigin(clientPos);

    onLayout();
}


void WindowFrame::resizeEvent(const ResizeEvent& rev)
{
    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();

    _clientRect.setSize( rev.size() );
    
    Gfx::SizeF frameSize = rev.size();
    frameSize.addWidth(2 * borderWidth);
    frameSize.addHeight(2 * borderWidth);
    frameSize.addHeight(titleHeight);
    _frameRect.setSize(frameSize);

    onLayout();
}


void WindowFrame::onLayout()
{
    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();
    double buttonWidth = titleHeight - borderWidth;

    Gfx::PointF menuPos(_frameRect.x() + borderWidth, _frameRect.y() + borderWidth);
    _menuButton.moveEvent( MoveEvent(0, menuPos ) );
    _menuButton.resizeEvent( ResizeEvent(0, Gfx::SizeF(buttonWidth, buttonWidth) ) );

    double buttonX = _frameRect.x() + _frameRect.width() - (borderWidth + buttonWidth);
    double buttonY = _frameRect.y() + borderWidth;

    _closeButton.moveEvent( MoveEvent(0, Gfx::PointF(buttonX, buttonY) ) );
    _closeButton.resizeEvent( ResizeEvent(0, Gfx::SizeF(buttonWidth, buttonWidth) ) );

    buttonX -= borderWidth + buttonWidth;
    _maximizeButton.moveEvent( MoveEvent(0, Gfx::PointF(buttonX, buttonY) ) );
    _maximizeButton.resizeEvent( ResizeEvent(0, Gfx::SizeF(buttonWidth, buttonWidth) ) );

    buttonX -= borderWidth + buttonWidth;
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

    if( _closeButton.geometry().contains(_lastPointer) )
        _closeButton.leaveEvent(lev);

    if( _minimizeButton.geometry().contains(_lastPointer) )
        _minimizeButton.leaveEvent(lev);

    if( _maximizeButton.geometry().contains(_lastPointer) )
        _maximizeButton.leaveEvent(lev);

    if( _menuButton.geometry().contains(_lastPointer) )
        _menuButton.leaveEvent(lev);
}


bool WindowFrame::mouseEvent(const MouseEvent& mev)
{
    bool r = onMouseEvent(mev);
    _lastPointer = mev.position();
    return r;
}


bool WindowFrame::onMouseEvent(const MouseEvent& mev)
{
    bool isResizing = _isLeftResizing || _isRightResizing || 
                      _isTopResizing || _isBottomResizing;
    
    if( ! _isMoving && ! isResizing )
    {
        if(_clientRect.contains( mev.position() ) )
        {        
            if( ! _isClient )
            {
                _isClient = true;
                EnterEvent eev(_window->vid());
                _window->processEvent(eev);
            }

            Gfx::PointF pos = mev.position() - _clientRect.topLeft();
            MouseEvent mev2 = mev;
            mev2.setPosition(pos);
            _window->processEvent(mev2);
            return false;
        }

        if(_isClient)
        {
            _isClient = false;
            LeaveEvent lev(_window->vid());
            _window->processEvent(lev);
        }

        if(_maximizeButton.geometry().contains( mev.position() ) )
        {
            _maximizeButton.mouseEvent(mev);
            return false;
        }
        else if(_maximizeButton.geometry().contains(_lastPointer) )
        {
            _maximizeButton.leaveEvent( LeaveEvent(0) );
        }

        if(_minimizeButton.geometry().contains( mev.position() ) )
        {
            _minimizeButton.mouseEvent(mev);
            return false;
        }
        else if(_minimizeButton.geometry().contains(_lastPointer) )
        {
            _minimizeButton.leaveEvent( LeaveEvent(0) );
        }

        if(_closeButton.geometry().contains( mev.position() ) )
        {
            _closeButton.mouseEvent(mev);
            return false;
        }
        else if(_closeButton.geometry().contains(_lastPointer) )
        {
            _closeButton.leaveEvent( LeaveEvent(0) );
        }

        if(_menuButton.geometry().contains( mev.position() ) )
        {
            _menuButton.mouseEvent(mev);
            return false;
        }
        else if(_menuButton.geometry().contains(_lastPointer) )
        {
            _menuButton.leaveEvent( LeaveEvent(0) );
        }
    }

    bool onTitle = _isMoving || isTitle( mev.position() );
    if( onTitle && ! isResizing )
    {
        Application::instance().setCursor( &Cursor::moveCursor() );
        _isMoving = (_isMoving || mev.isPress()) && ! mev.isReleased();
        
        if(_isMoving && mev.position() != _lastPointer)
        {
            Gfx::PointF to = _window->position() + mev.position() - _lastPointer;
            _window->move(to);
            _isMoving = true;
        }
        
        return _isMoving;
    }
 
    bool onLeftBorder = _isLeftResizing || isLeftBorder( mev.position() );
    bool onRightBorder = _isRightResizing || isRightBorder( mev.position() );
    bool onTopBorder = _isTopResizing || isTopBorder( mev.position() );
    bool onBottomBorder = _isBottomResizing || isBottomBorder( mev.position() );
    
    if(onLeftBorder || onRightBorder || onTopBorder || onBottomBorder)
    {
        if( onTopBorder && onRightBorder || onBottomBorder && onLeftBorder )
            Application::instance().setCursor( &Hmi::Cursor::sizeNESWCursor() );
        else if(onTopBorder && onLeftBorder || onBottomBorder && onRightBorder )
            Application::instance().setCursor( &Hmi::Cursor::sizeNWSECursor() );
        else if(onRightBorder || onLeftBorder)
            Application::instance().setCursor( &Hmi::Cursor::sizeWECursor() );
        else if(onTopBorder || onBottomBorder)
            Application::instance().setCursor( &Hmi::Cursor::sizeNSCursor() );     

        _isLeftResizing = _isLeftResizing || (onLeftBorder && mev.isPress());
        _isRightResizing = _isRightResizing || (onRightBorder&& mev.isPress());
        _isTopResizing = _isTopResizing || (onTopBorder && mev.isPress());
        _isBottomResizing =_isBottomResizing ||(onBottomBorder&& mev.isPress());

        if( ! mev.isPressed() )
        {
            _isLeftResizing = false;
            _isRightResizing = false;
            _isTopResizing = false;
            _isBottomResizing = false;
            return false;
        }

        Gfx::SizeF size = _window->size();
        Gfx::PointF pos = _window->position();
        Gfx::PointF delta = mev.position() - _lastPointer;

        if( _isLeftResizing )
        {
            pos.addX( delta.x() );
            size.subWidth(delta.x());
        }   

        if(_isRightResizing)
            size.addWidth( delta.x() );
            
        if(_isTopResizing)
        {
            pos.addY( delta.y() );
            size.subHeight(delta.y());
        }

        if(_isBottomResizing)
            size.addHeight( delta.y() );
            
        if( size != _window->size() )
            _window->resize(size);

        if( pos != _window->position() )
            _window->move(pos);

        return true;
    }

    Application::instance().setCursor( &Cursor::defaultCursor() ); 
    return false;
}


void WindowFrame::paintEvent(const PaintEvent& pev)
{
    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();

    PaintSurface& surface = _wm->parent()->surface();
    Painter painter(surface);

    Gfx::Color color = _window->isActive() ? _wm->activeColor() 
                                           : _wm->inactiveColor();
    //
    // frame background
    //
    Gfx::Brush brush(color);
    painter.setBrush(brush);

    Gfx::PointF pos = _window->position();

    Gfx::RectF leftBorder( pos.x(), 
                           pos.x() + borderWidth - 1,
                           pos.y() + borderWidth, 
                           pos.y() + _frameRect.height() - borderWidth - 1 );
    painter.fillRect(leftBorder);

    Gfx::RectF topBorder(pos.x(),
                         pos.x() + _frameRect.width() - 1,
                         pos.y(),
                         pos.y() + borderWidth - 1);
    painter.fillRect(topBorder);

    Gfx::RectF rightBorder(pos.x() + _frameRect.width() - borderWidth,
                           pos.x() + _frameRect.width() - 1,
                           pos.y() + borderWidth,
                           pos.y() + _frameRect.height() - borderWidth - 1 );
    painter.fillRect(rightBorder);

    Gfx::RectF bottomBorder(pos.x(),
                            pos.x() + _frameRect.width() - 1,
                            pos.y() + _frameRect.height() - borderWidth,
                            pos.y() + _frameRect.height() - 1);
    painter.fillRect(bottomBorder);

    Gfx::RectF titleArea( pos.x() + borderWidth,
                          pos.x() + _frameRect.width() - borderWidth - 1,
                          pos.y() + borderWidth,
                          pos.y() + borderWidth + titleHeight - 1);
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

    painter.drawLine( Gfx::PointF(_frameRect.topRight().x() - (borderWidth -1),
                                  _frameRect.topRight().y() + (borderWidth) + titleHeight),
                      Gfx::PointF(_frameRect.bottomRight().x() - (borderWidth -1),
                                  _frameRect.bottomRight().y() - (borderWidth-1)) );
    
    painter.drawLine( Gfx::PointF(_frameRect.bottomLeft().x() + (borderWidth),
                                  _frameRect.bottomLeft().y() - (borderWidth - 1)),
                      Gfx::PointF(_frameRect.bottomRight().x() - (borderWidth -2),
                                  _frameRect.bottomRight().y() - (borderWidth - 1)) );

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

    painter.drawLine( Gfx::PointF(_frameRect.topLeft().x() + (borderWidth-1),
                                  _frameRect.topLeft().y() + (borderWidth) + titleHeight),
                      Gfx::PointF(_frameRect.bottomLeft().x() + (borderWidth-1),
                                  _frameRect.bottomLeft().y() - (borderWidth - 2)) );

    painter.drawLine( Gfx::PointF(_frameRect.topLeft().x() + (borderWidth-1),
                                  _frameRect.topLeft().y() + (borderWidth-1) + titleHeight),
                      Gfx::PointF(_frameRect.topRight().x() - (borderWidth-2),
                                  _frameRect.topRight().y() + (borderWidth-1) + titleHeight) );

    //
    // title bar text
    //
    Pt::String title = _window->title().c_str();

    const Gfx::Font& font = _window->font();
    painter.setFont(font);
    Gfx::FontMetrics fm = painter.fontMetrics(title);

    Gfx::PointF textPos(pos.x() + borderWidth + titleHeight, 
                        pos.y() + titleHeight - borderWidth);

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


    Gfx::PointF gripStart( textPos.x() + fm.width() + borderWidth, 
                           pos.y() + borderWidth - 2); // approx.
    Gfx::PointF gripEnd(pos.x() + _frameRect.width() - borderWidth - 3*titleHeight,
                        pos.y() + borderWidth - 2); // approx.
    
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



    _maximizeButton.paintEvent( PaintEvent(0, _maximizeButton.geometry()) );
    _minimizeButton.paintEvent( PaintEvent(0, _maximizeButton.geometry()) );
    _closeButton.paintEvent( PaintEvent(0, _maximizeButton.geometry()) );
    _menuButton.paintEvent( PaintEvent(0, _maximizeButton.geometry()) );
}

} // namespace

} // namespace
