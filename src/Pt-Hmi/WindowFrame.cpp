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
#include <Pt/Hmi/Window.h>

namespace Pt{
namespace Hmi{

WindowFrame::WindowFrame()
: _wm(0)
, _window(0)
{
}


WindowFrame::WindowFrame(WindowManager& wm, Window& window)
: _wm(&wm)
, _window(&window)
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
    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();

    return p.x() >= borderWidth && 
           p.x() < borderWidth + _clientRect.width() &&
           p.y() >= borderWidth &&
           p.y() < borderWidth + titleHeight;
}


bool WindowFrame::isBorder(const Gfx::PointF& p) const
{            
    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();

    return p.x() >= borderWidth && 
           p.x() < borderWidth + _clientRect.width() &&
           p.y() >= borderWidth &&
           p.y() < borderWidth + titleHeight;
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


bool WindowFrame::mouseEvent(const MouseEvent& mev)
{
    if( _closeButton.contains( mev.position() ) )
    {
        if( mev.isRelease() )
             _wm->onClosing(*_window);
        
        return true;
    }

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

    const Gfx::Font& font = _window->font();
    painter.setFont(font);

    Gfx::Pen pen(1, _wm->textColor());
    painter.setPen(pen);

    Gfx::FontMetrics fm = painter.fontMetrics( font, Pt::String("A") );

    double textMargin = (titleHeight - fm.height()) / 2;
    Gfx::PointF textPos(pos.x() + borderWidth + titleHeight, 
                        pos.y() + titleHeight - textMargin);

    painter.drawText(textPos, Pt::String( _window->title().c_str()) );

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
