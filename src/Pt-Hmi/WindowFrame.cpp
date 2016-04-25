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

namespace Pt {

namespace Hmi {

WindowFrame::WindowFrame()
: _wm(0)
, _window(0)
{
}


WindowFrame::WindowFrame(WindowManager& wm, Window& window)
: _wm(&wm)
, _window(&window)
, _isMoving(false)
, _isLeftResizing(false)
, _isRightResizing(false)
, _isTopResizing(false)
, _isBottomResizing(false)
, _closeButton(0, 10, 0, 10)
, _maximizeButton(0, 10, 0, 10)
, _minimizeButton(0, 10, 0, 10)
{
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


Pt::uint8_t WindowFrame::isResize(const Pt::Gfx::PointF& p) const
{        
    const Window* w = _window;
    const double borderWidth = _wm->borderWidth();
    const double titleHeight = _wm->titleHeight();

    const Gfx::SizeF  wsize = _clientRect.size();
    const Gfx::PointF wpos  = _frameRect.topLeft();
    
    if( p.x() < wpos.x() ||
        p.x() > wpos.x() + 2*borderWidth + wsize.width() ||
        p.y() < wpos.y() ||
        p.y() >= wpos.y() + 2*borderWidth + titleHeight + wsize.height() )
        return None;

    const bool left   = p.x() < (wpos.x() + borderWidth);
    const bool right  = p.x() >= wpos.x() + borderWidth + wsize.width();
    const bool top    = p.y() < wpos.y() + borderWidth;
    const bool bottom = p.y() >= wpos.y() + borderWidth + titleHeight + wsize.height();
    Pt::uint8_t direction = 0;

    if(left)                
        direction |= West;

    if(right)
            direction |= East;
    if(top)
        direction |= North;

    if(bottom)
        direction |= South;            

    return direction;            
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

    double buttonX = _frameRect.x() + _frameRect.width() - (borderWidth + buttonWidth);
    double buttonY = _frameRect.y() + borderWidth;
    
    Gfx::PointF menuPos(_frameRect.x() + borderWidth, _frameRect.y() + borderWidth);
    _menuButton.setOrigin(menuPos);
    _menuButton.setSize( Gfx::SizeF(buttonWidth, buttonWidth) );

    _closeButton.setOrigin( Gfx::PointF(buttonX, buttonY) );
    _closeButton.setSize( Gfx::SizeF(buttonWidth, buttonWidth) );

    buttonX -= borderWidth + buttonWidth;
    
    _maximizeButton.setOrigin( Gfx::PointF(buttonX, buttonY) );
    _maximizeButton.setSize( Gfx::SizeF(buttonWidth, buttonWidth) );

    buttonX -= borderWidth + buttonWidth;
    
    _minimizeButton.setOrigin( Gfx::PointF(buttonX, buttonY) );
    _minimizeButton.setSize( Gfx::SizeF(buttonWidth, buttonWidth) );
}


void WindowFrame::enterEvent(const EnterEvent& eev)
{
  std::clog << "enter " << _window->title() << std::endl;
}


void WindowFrame::leaveEvent(const LeaveEvent& lev)
{
  std::clog << "leave " << _window->title() << std::endl;
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
            Gfx::PointF pos = mev.position() - _clientRect.topLeft();
            MouseEvent mev2 = mev;
            mev2.setPosition(pos);
            _window->processEvent(mev2);
            return false;
        }

        if( _closeButton.contains( mev.position() ) )
        {
            Application::instance().setCursor( &Cursor::defaultCursor() ); 

            if( mev.isRelease() )
                _wm->onClosing(*_window);

            return false;
        }
    }

    bool onTitle = _isMoving || isTitle( mev.position() );
    if( onTitle && ! isResizing )
    {
        Application::instance().setCursor( &Cursor::moveCursor() );
        _isMoving = (_isMoving || mev.isPress()) && ! mev.isReleased();
        
        if(_isMoving)
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

    PaintSurface& surface = _wm->surface();
    Painter painter(surface);

    Gfx::SizeF frameSize = _window->size();
    frameSize.addWidth(borderWidth * 2);
    frameSize.addHeight(borderWidth * 2 + titleHeight);

    Gfx::Color color = _window->isActive() ? _wm->activeColor() 
                                           : _wm->inactiveColor();  
    
    //
    // frame background
    //

    Gfx::Brush brush(color);
    painter.setBrush(brush);

    Gfx::PointF pos = _window->position();

    Gfx::RectF leftBorder( pos.x(), 
                           pos.x() + borderWidth,
                           pos.y() + borderWidth, 
                           pos.y() + frameSize.height() - borderWidth - 1 );
    painter.fillRect(leftBorder);

    Gfx::RectF topBorder(pos.x(),
                         pos.x() + frameSize.width() - 1,
                         pos.y(),
                         pos.y() + borderWidth);
    painter.fillRect(topBorder);

    Gfx::RectF rightBorder(pos.x() + frameSize.width() - borderWidth,
                           pos.x() + frameSize.width() - 1,
                           pos.y() + borderWidth,
                           pos.y() + frameSize.height() - borderWidth - 1 );
    painter.fillRect(rightBorder);

    Gfx::RectF bottomBorder(pos.x(),
                            pos.x() + frameSize.width() - 1,
                            pos.y() + frameSize.height() - borderWidth,
                            pos.y() + frameSize.height() - 1);
    painter.fillRect(bottomBorder);

    Gfx::RectF titleArea( pos.x() + borderWidth,
                          pos.x() + frameSize.width() - borderWidth - 1,
                          pos.y() + borderWidth,
                          pos.y() + borderWidth + titleHeight - 1);
    painter.fillRect(titleArea);

    //
    // light outer and inner border border contour
    //
    Gfx::Color borderLight( color.red() * 1.25f, 
                            color.green() * 1.25f, 
                            color.blue() * 1.25f );
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
    // dark outer and inner border border contour
    //
    Gfx::Color borderDark( color.red() * 0.75f, 
                           color.green() * 0.75f, 
                           color.blue() * 0.75f );
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

    Pt::String title = _window->title().c_str();
    Gfx::FontMetrics fm = painter.fontMetrics(title);

    Gfx::PointF textPos(pos.x() + borderWidth + titleHeight, 
                        pos.y() + fm.height() + 1);

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

    //
    // title bar text
    //
    const Gfx::Font& font = _window->font();
    painter.setFont(font);

    Gfx::Color textColor = _window->isActive() ? _wm->textColor() 
                                               : _wm->inactiveTextColor(); 
    Gfx::Pen pen(1, textColor);
    painter.setPen(pen);

    painter.drawText(textPos, title);

    //
    // window buttons
    //
    pen = Gfx::Pen(1, Gfx::Color(0, 0, 0) );
    painter.setPen(pen);

    brush = Gfx::Color(0.82f, 0.25f, 0.22f);
    painter.setBrush(brush);
    painter.fillRect(_closeButton);
    painter.drawRect(_closeButton);

    brush = Gfx::Color(0.35f, 0.65f, 0.25f);
    painter.setBrush(brush);
    painter.fillRect(_maximizeButton);
    painter.drawRect(_maximizeButton);

    brush = Gfx::Color(0.95f, 0.7f, 0.05f);
    painter.setBrush(brush);
    painter.fillRect(_minimizeButton);
    painter.drawRect(_minimizeButton);

    pen = Gfx::Pen(2, Gfx::Color(1, 1, 1), 
                   Gfx::Pen::SolidStyle, Gfx::Pen::RoundCap);
    painter.setPen(pen);

    Gfx::PointF tl = _closeButton.topLeft() + Gfx::PointF(4, 4);
    Gfx::PointF br = _closeButton.bottomRight() - Gfx::PointF(4, 4);
    Gfx::PointF tr = _closeButton.topRight() + Gfx::PointF(-4, 4);
    Gfx::PointF bl = _closeButton.bottomLeft() - Gfx::PointF(-4, 4);
    painter.drawLine(tl, br);
    painter.drawLine(tr, bl);

    pen = Gfx::Pen(2, Gfx::Color(1, 1, 1), 
                   Gfx::Pen::SolidStyle, Gfx::Pen::FlatCap);
    painter.setPen(pen);

    tl = _maximizeButton.topLeft() + Gfx::PointF(5, 5);
    tr = _maximizeButton.topRight() + Gfx::PointF(-4, 4);
    br = _maximizeButton.bottomRight() - Gfx::PointF(4, 4);
    bl = _maximizeButton.bottomLeft() - Gfx::PointF(-4, 4);
    painter.drawLine(tl, tr);
    painter.drawLine(bl, tr);
    painter.drawLine(br, tr);

    tl = _minimizeButton.topLeft() + Gfx::PointF(5, 5);
    tr = _minimizeButton.topRight() + Gfx::PointF(-4, 4);
    br = _minimizeButton.bottomRight() - Gfx::PointF(4, 4);
    bl = _minimizeButton.bottomLeft() - Gfx::PointF(-4, 4);
    painter.drawLine(bl, br);
    painter.drawLine(tl, bl);
    painter.drawLine(tr, bl);

    //
    // menu button
    //
    Gfx::PointF triangle[3];
    triangle[0] = _menuButton.topLeft() + Gfx::PointF(4, 3);
    triangle[1] = _menuButton.topRight() + Gfx::PointF(-2, 3);
    Gfx::PointF mid(_menuButton.width() / 2, _menuButton.height() / 2);
    triangle[2] = _menuButton.topLeft() + mid;

    brush = Gfx::Color(1,1,1);
    painter.setBrush(brush);
    painter.fillPolygon(triangle, 3);
}

} // namespace

} // namespace
