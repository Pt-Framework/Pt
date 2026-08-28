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

#include <Pt/Forms/ScrollBar.h>
#include <Pt/Forms/ScrollBarStyler.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/PaintContext.h>

namespace Pt {

namespace Forms {

ScrollBar::ScrollBar(Orientation o)
: _orientation(o)
, _minPos(0)
, _maxPos(100)
, _pageStep(10)
, _scrollStep(1)
, _position(0)
, _dragging(false)
, _hoveredZone(NoZone)
, _pressedZone(NoZone)
{
}


ScrollBar::~ScrollBar()
{
}


void ScrollBar::setRange(double minpos, double maxpos)
{
    _minPos = minpos;
    _maxPos = maxpos;

    if(_position < minpos)
        scroll(minpos);

    if(_position > maxpos)
        scroll(maxpos);

    repaint();
}


void ScrollBar::setStepping(double scroll, double page)
{
    _scrollStep = scroll;
    _pageStep = page;
    repaint();
}


double ScrollBar::minimumPosition() const
{
    return _minPos;
}


double ScrollBar::maximumPosition() const
{
    return _maxPos;
}


double ScrollBar::position() const
{
    return _position;
}


void ScrollBar::setPosition(double pos)
{
    if(_position == pos)
        return;

    if(pos < _minPos)
        pos = _minPos;
    else if(pos > _maxPos)
        pos = _maxPos;

    _position = pos;
    repaint();
}


void ScrollBar::scroll(double pos)
{
    setPosition(pos);
    _changed.send(_position);
}


const Gfx::Brush& ScrollBar::background() const
{
    return _styler.background();
}


void ScrollBar::setBackground(const Gfx::Brush& b)
{
    _styler.setBackground(b);
    invalidate();
}


const Gfx::Brush& ScrollBar::foreground() const
{
    return _styler.foreground();
}


void ScrollBar::setForeground(const Gfx::Brush& b)
{
    _styler.setForeground(b);
    invalidate();
}


const Gfx::Pen& ScrollBar::contour() const
{
    return _styler.contour();
}


void ScrollBar::setContour(const Gfx::Pen& p)
{
    _styler.setContour(p);
    invalidate();
}


void ScrollBar::setRenderer(ScrollBarRenderer* renderer)
{
    const Style& style = Application::instance().style();
    const StyleOptions& options = Application::instance().styleOptions();

    _styler.setRenderer(renderer);
    _styler.bind(style, options);

    _trackRect = Gfx::RectF();
    _decreaseRect = Gfx::RectF();
    _increaseRect = Gfx::RectF();

    invalidate();
}


ScrollBarState ScrollBar::scrollBarState() const
{
    ScrollBarState state;
    state.setEnabled( isEnabled() );
    state.setFocused( hasFocus() );
    state.setHandleHovered( _hoveredZone == HandleZone );
    state.setHandlePressed( _pressedZone == HandleZone );
    state.setDecreaseHovered( _hoveredZone == DecreaseZone );
    state.setDecreasePressed( _pressedZone == DecreaseZone );
    state.setIncreaseHovered( _hoveredZone == IncreaseZone );
    state.setIncreasePressed( _pressedZone == IncreaseZone );
    return state;
}


Direction ScrollBar::direction() const
{
    if( _orientation == Vertical )
        return Direction(Direction::Top);

    return Direction(Direction::Left);
}


float ScrollBar::fraction() const
{
    if( _maxPos <= _minPos )
        return 0.0f;

    float f = static_cast<float>( (_position - _minPos) / (_maxPos - _minPos) );

    if( f < 0.0f ) f = 0.0f;
    if( f > 1.0f ) f = 1.0f;

    return f;
}


float ScrollBar::viewProportion() const
{
    if( _maxPos <= _minPos )
        return 1.0f;

    double range = _maxPos - _minPos;
    float vp = static_cast<float>( _pageStep / (range + _pageStep) );

    if( vp < 0.0f ) vp = 0.0f;
    if( vp > 1.0f ) vp = 1.0f;

    return vp;
}


void ScrollBar::onInvalidate()
{
    Base::onInvalidate();

    const Style& style = Application::instance().style();
    const StyleOptions& options = Application::instance().styleOptions();

    if( _styler.bind(style, options) )
    {
        _trackRect = Gfx::RectF();
        _decreaseRect = Gfx::RectF();
        _increaseRect = Gfx::RectF();
    }

    relayout();
}


void ScrollBar::onPaint(PaintContext& context, const Gfx::RectF& /*updateRect*/)
{
    Direction dir = direction();
    Gfx::RectF widgetRect( Gfx::PointF(0, 0), size() );

    Gfx::RectF handleRect = currentHandleRect();

    ScrollBarState state = scrollBarState();

    onPaintChrome(context, widgetRect, dir, _trackRect, handleRect,
                  _decreaseRect, _increaseRect, state);
}


void ScrollBar::onPaintChrome(PaintContext& context,
                              const Gfx::RectF& rect,
                              Direction direction,
                              const Gfx::RectF& trackRect,
                              const Gfx::RectF& handleRect,
                              const Gfx::RectF& decreaseRect,
                              const Gfx::RectF& increaseRect,
                              const ScrollBarState& state)
{
    _styler.renderChrome(context, rect, direction, trackRect, handleRect,
                         decreaseRect, increaseRect, state);
}


Gfx::SizeF ScrollBar::onMeasure(const SizePolicy& policy)
{
    Direction dir = direction();
    Gfx::SizeF contentSize = policy.size();
    return _styler.measureFrame(surface(), contentSize, dir);
}


bool ScrollBar::onMouseEvent(const MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    HotZone zone = hitTest( ev.position() );

    if( ev.isPress(MouseEvent::Left) )
    {
        _pressedZone = zone;

        if( zone == HandleZone )
        {
            _dragging = true;
        }
        else if( zone == DecreaseZone )
        {
            scroll(_position - _scrollStep);
        }
        else if( zone == IncreaseZone )
        {
            scroll(_position + _scrollStep);
        }
        else if( zone == TrackZone )
        {
            Gfx::RectF handleRect = currentHandleRect();
            bool vertical = (_orientation == Vertical);
            double clickPos = vertical ? ev.position().y() : ev.position().x();
            double handleCenter = vertical ? (handleRect.y() + handleRect.height() / 2)
                                           : (handleRect.x() + handleRect.width() / 2);

            if( clickPos < handleCenter )
                scroll(_position - _pageStep);
            else
                scroll(_position + _pageStep);
        }

        repaint();
    }
    else if( ev.isRelease(MouseEvent::Left) )
    {
        _dragging = false;
        _pressedZone = NoZone;
        repaint();
    }
    else
    {
        HotZone prevHover = _hoveredZone;
        _hoveredZone = zone;

        if( prevHover != _hoveredZone )
            repaint();
    }

    if( _dragging )
    {
        bool vertical = (_orientation == Vertical);
        double trackStart = vertical ? _trackRect.y() : _trackRect.x();
        double trackLen = vertical ? _trackRect.height() : _trackRect.width();

        Gfx::RectF handleRect = currentHandleRect();
        double handleLen = vertical ? handleRect.height() : handleRect.width();
        double travel = trackLen - handleLen;

        if( travel > 0.0 )
        {
            double pixPos = vertical ? ev.position().y() : ev.position().x();
            double rel = (pixPos - trackStart - handleLen / 2) / travel;

            if( rel < 0.0 ) rel = 0.0;
            if( rel > 1.0 ) rel = 1.0;

            double pos = _minPos + rel * (_maxPos - _minPos);
            scroll(pos);
        }
    }

    return true;
}


bool ScrollBar::onTouchEvent(const TouchEvent& tev)
{
    Base::onTouchEvent(tev);

    HotZone zone = hitTest( tev.position() );

    if( tev.isPress() )
    {
        _pressedZone = zone;

        if( zone == HandleZone )
        {
            _dragging = true;
        }
        else if( zone == DecreaseZone )
        {
            scroll(_position - _scrollStep);
        }
        else if( zone == IncreaseZone )
        {
            scroll(_position + _scrollStep);
        }
        else if( zone == TrackZone )
        {
            Gfx::RectF handleRect = currentHandleRect();
            bool vertical = (_orientation == Vertical);
            double tapPos = vertical ? tev.position().y() : tev.position().x();
            double handleCenter = vertical ? (handleRect.y() + handleRect.height() / 2)
                                           : (handleRect.x() + handleRect.width() / 2);

            if( tapPos < handleCenter )
                scroll(_position - _pageStep);
            else
                scroll(_position + _pageStep);
        }

        repaint();
    }
    else if( tev.isRelease() )
    {
        _dragging = false;
        _pressedZone = NoZone;
        repaint();
    }

    if( _dragging )
    {
        bool vertical = (_orientation == Vertical);
        double trackStart = vertical ? _trackRect.y() : _trackRect.x();
        double trackLen = vertical ? _trackRect.height() : _trackRect.width();

        Gfx::RectF handleRect = currentHandleRect();
        double handleLen = vertical ? handleRect.height() : handleRect.width();
        double travel = trackLen - handleLen;

        if( travel > 0.0 )
        {
            double pixPos = vertical ? tev.position().y() : tev.position().x();
            double rel = (pixPos - trackStart - handleLen / 2) / travel;

            if( rel < 0.0 ) rel = 0.0;
            if( rel > 1.0 ) rel = 1.0;

            double pos = _minPos + rel * (_maxPos - _minPos);
            scroll(pos);
        }
    }

    return true;
}


void ScrollBar::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    Direction dir = direction();
    Gfx::RectF widgetRect( Gfx::PointF(0, 0), size() );

    Gfx::SizeF buttonSize = _styler.measureButton(surface(), dir);

    _styler.layoutChrome(surface(), widgetRect, dir, buttonSize,
                         _trackRect, _decreaseRect, _increaseRect);
}


Gfx::RectF ScrollBar::currentHandleRect()
{
    Direction dir = direction();
    Gfx::RectF handleRect;
    _styler.layoutHandle(surface(), _trackRect, dir,
                         fraction(), viewProportion(), handleRect);

    return handleRect;
}


ScrollBar::HotZone ScrollBar::hitTest(const Gfx::PointF& pos)
{
    Gfx::RectF handleRect = currentHandleRect();

    if( handleRect.contains(pos) )
        return HandleZone;

    if( _decreaseRect.contains(pos) )
        return DecreaseZone;

    if( _increaseRect.contains(pos) )
        return IncreaseZone;

    if( _trackRect.contains(pos) )
        return TrackZone;

    return NoZone;
}


bool ScrollBar::onEnterEvent(const EnterEvent& ev)
{
    Base::onEnterEvent(ev);
    return true;
}


bool ScrollBar::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);

    if( _hoveredZone != NoZone )
    {
        _hoveredZone = NoZone;
        repaint();
    }

    return true;
}

} // namespace Forms

} // namespace Pt
