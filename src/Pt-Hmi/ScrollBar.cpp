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

#include <Pt/Hmi/ScrollBar.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/Brush.h>

namespace Pt {

namespace Hmi {

ScrollBar::ScrollBar(Orientation o)
: _orientation(o)
, _minPos(0)
, _maxPos(100)
, _pageStep(10)
, _scrollStep(1)
, _position(0)
, _dragging(false)
, _factorPixel(1)
, _offsetPixel(0)
, _factorPosition(1)
, _offsetPosition(0)
{
    setAcceptsFocus(false);
    setAcceptInput(true);
}


ScrollBar::~ScrollBar()
{
}


void ScrollBar::setPosition(int pos)
{
    if(_position == pos)
        return;

    _position = pos;

    double buttonLength = _orientation == Vertical ? size().width()
                                                   : size().height();
    double pixpos = positionToPixel(_position);

    Gfx::PointF pt = _orientation == Vertical ? Gfx::PointF(0, pixpos)
                                               : Gfx::PointF(pixpos, 0);
    
    Gfx::SizeF size(buttonLength, buttonLength);
    
    _handleRect.set(pt, size);

    update();

    _changed.send(*this, _position);
}


void ScrollBar::setRange(int minpos, int maxpos)
{
    _minPos = minpos;
    _maxPos = maxpos;
    updateScroll();
}


void ScrollBar::onMouseEvent(const MouseEvent& ev)
{
    Panel::onMouseEvent(ev);

    if( ev.isPress(MouseEvent::Left) )
    {
        if(_handleRect.contains( ev.position() ) )
        {
            this->grabMouse();
            _dragging = true;
        }
    }
    else if( ev.isRelease(MouseEvent::Left) )
    {
        _dragging = false;
        this->releaseMouse();
    }

    if(_dragging)
    {
        double pixPos =_orientation == Vertical ? ev.position().y() 
                                                : ev.position().x();
        int pos = pixelToPosition(pixPos);

        if( pos >= _minPos && pos <= _maxPos)
          setPosition( pos);
    }
}


void ScrollBar::updateScroll()
{
    double buttonLength = _orientation == Vertical ? size().width()
                                                   : size().height();

    double length = _orientation == Vertical ? size().height()
                                             : size().width(); 

    const double pixMin = buttonLength;
    const double pixMax = length - buttonLength;

    _factorPixel = (pixMax  - pixMin) / (_maxPos -_minPos );
    _offsetPixel = (pixMin * _maxPos - pixMax *_minPos) / (_maxPos - _minPos );
       
    _factorPosition = (_maxPos -_minPos) / (pixMax -pixMin );
    _offsetPosition = (_minPos * pixMax - _maxPos *pixMin) / (pixMax -pixMin );  

    double pixpos = positionToPixel(_position);

    Gfx::PointF pos = _orientation == Vertical ? Gfx::PointF(0, pixpos)
                                               : Gfx::PointF(pixpos, 0);
    
     Gfx::SizeF size(buttonLength, buttonLength);
    
    _handleRect.set( pos, size );
    
}


void ScrollBar::onResizeEvent(const ResizeEvent& ev)
{
    Panel::onResizeEvent(ev);

    updateScroll();
}


void ScrollBar::onPaint(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Panel::onPaint(surface, updateRect);

    Painter painter(surface);
    painter.setClip(updateRect);
    
    Gfx::Brush handleBrush( Gfx::Color(0.4f, 0.4f, 0.4f) );
    painter.setBrush(handleBrush);
    painter.fillRect(_handleRect);
}


int ScrollBar::pixelToPosition(double pix)
{    
    double pos = pix * _factorPosition + _offsetPosition;
    return static_cast<int>(pos);
}
        

double ScrollBar::positionToPixel(int pos)
{
  return pos * _factorPixel + _offsetPixel;
}

} // namespace

} // namespace
