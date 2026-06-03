/* Copyright (C) 2015 Marc Boris Duerner

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

#include "WorkspaceFrame.h"

#include <Pt/Forms/WindowManager.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/WorkspaceManager.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Forms/WindowStateEvent.h>
#include <Pt/Forms/ResizeEvent.h>
#include <Pt/Forms/MoveEvent.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Point.h>
#include <algorithm>

namespace {

Pt::Gfx::Color brighten(const Pt::Gfx::Color& c, float factor)
{
    float rf = c.red() * factor;
    float gf = c.green() * factor;
    float bf = c.blue() * factor;

    Pt::uint8_t r = rf > 255 ? 255 : (Pt::uint8_t) rf ;
    Pt::uint8_t g = gf > 255 ? 255 : (Pt::uint8_t) gf ;
    Pt::uint8_t b = bf > 255 ? 255 : (Pt::uint8_t) bf ;

    return Pt::Gfx::Color(c.alpha(), r, g, b);
}

}

namespace Pt {

namespace Forms {

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
        _frame->repaint(_geometry);
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


void WindowButton::paint(PaintContext& ctx, const Gfx::RectF& rect)
{
    Painter painter(ctx);
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

    const size_t penSize = 1;
    const double spacing = penSize / 2.0;

    Gfx::RectF borderRect(_geometry.left() + spacing,
                          _geometry.right() - spacing,
                          _geometry.top() + spacing,
                          _geometry.bottom() - spacing);
    double offset = 0;

    Gfx::PointF topLeft(borderRect.topLeft().x() + offset, 
                        borderRect.topLeft().y() + offset);
    Gfx::PointF bottomLeft(borderRect.bottomLeft().x() + offset, 
                           borderRect.bottomLeft().y() - offset);
    Gfx::PointF bottomRight(borderRect.bottomRight().x() - offset, 
                            borderRect.bottomRight().y() - offset);
    Gfx::PointF topRight(borderRect.topRight().x() - offset, 
                         borderRect.topRight().y() + offset);
    
    //
    // fill background
    //
    Gfx::Brush brush = backgroundColor;
    painter.setBrush(brush);
    painter.fillRect(borderRect);
  
    //
    // bottom right border
    //
    Gfx::PointF points[3] = { bottomLeft, bottomRight, topRight };

    painter.setPen( Gfx::Pen(borderBottomRightColor, penSize, 
                             Gfx::Pen::Solid, Gfx::Pen::FlatCap, Gfx::Pen::MiterJoin) );

    painter.drawPolyline(points, 3);

    //
    // top left border
    //    
    points[0] = topRight;
    points[1] = topLeft;
    points[2] = bottomLeft;

    painter.setPen(Gfx::Pen(borderTopLeftColor, penSize, 
                            Gfx::Pen::Solid, Gfx::Pen::FlatCap, Gfx::Pen::MiterJoin));

    painter.drawPolyline(points, 3);
}

//
// MaximizeButton
//

MinimizeButton::MinimizeButton()
{
    setColor( Gfx::Color(242, 178, 12));
}


MinimizeButton::~MinimizeButton()
{
}


void MinimizeButton::paint(PaintContext& ctx, const Gfx::RectF& rect)
{
    WindowButton::paint(ctx, rect);

    Painter painter(ctx);
    painter.setClip(rect);

    const Gfx::Scaling& scaling = painter.scaling();

    double inset = scaling.align(3.0);
    double height = scaling.align(2.0);

    Gfx::RectF frameSymbol( geometry().left() + inset,
                            geometry().right() - inset,
                            geometry().bottom() - inset - height,
                            geometry().bottom() - inset);

    painter.setBrush( Gfx::Color(255, 255, 255) );
    painter.fillRect(frameSymbol);
}

//
// MaximizeButton
//

MaximizeButton::MaximizeButton()
{
    setColor( Gfx::Color(89, 165, 63) );
}


MaximizeButton::~MaximizeButton()
{
}


void MaximizeButton::paint(PaintContext& ctx, const Gfx::RectF& rect)
{
    WindowButton::paint(ctx, rect);

    Painter painter(ctx);
    painter.setClip(rect);

    const Gfx::Scaling& scaling = painter.scaling();

    double inset = scaling.align(3.0) + scaling.toLogical(0.5);

    Gfx::RectF frameSymbol( geometry().left() + inset,
                            geometry().right() - inset,
                            geometry().top() + inset,
                            geometry().bottom() - inset);
     
    Pt::Gfx::Pen pen(Gfx::Color(255, 255, 255), 
                     1, Gfx::Pen::Solid, Gfx::Pen::SquareCap, Gfx::Pen::MiterJoin);
    painter.setPen(pen);
    painter.drawRect(frameSymbol);

    frameSymbol.setHeight( scaling.align(2.0) );
    
    painter.setBrush( Gfx::Color(255, 255, 255) );
    painter.fillRect(frameSymbol);
}

//
// CloseButton
//

CloseButton::CloseButton()
{
    setColor( Gfx::Color(209, 63, 56) );
}


CloseButton::~CloseButton()
{
}


void CloseButton::paint(PaintContext& ctx, const Gfx::RectF& rect)
{
    WindowButton::paint(ctx, rect);

    Painter painter(ctx);
    painter.setClip(rect);

    const Gfx::Scaling& scaling = painter.scaling();

    double margin = scaling.align(3.0);
    double offset = scaling.align(1.0);
    double inset = scaling.toLogical(0.5);

    const Gfx::RectF& buttonRect = geometry();

    Gfx::PointF line1[] = { Gfx::PointF( buttonRect.topLeft().x() + margin + inset,
                                         buttonRect.topLeft().y() + offset + margin + inset),
                            
                            Gfx::PointF( buttonRect.topLeft().x() + offset + margin + inset,
                                         buttonRect.topLeft().y() + margin + inset ),
                            
                            Gfx::PointF( buttonRect.bottomRight().x() - margin - inset,
                                         buttonRect.bottomRight().y() - offset - margin - inset),
                            
                            Gfx::PointF( buttonRect.bottomRight().x() - offset - margin - inset,
                                         buttonRect.bottomRight().y() - margin - inset) };

    Gfx::PointF line2[] = { Gfx::PointF( buttonRect.topRight().x() - offset - margin - inset,
                                         buttonRect.topRight().y() + margin + inset ),
                            
                            Gfx::PointF( buttonRect.topRight().x() - margin - inset,
                                         buttonRect.topRight().y() + offset + margin + inset ),
                            
                            Gfx::PointF( buttonRect.bottomLeft().x() + offset + margin + inset,
                                         buttonRect.bottomLeft().y() - margin - inset),
                            
                            Gfx::PointF( buttonRect.bottomLeft().x() + margin + inset,
                                         buttonRect.bottomLeft().y() - offset - margin - inset) };

    painter.setBrush( Gfx::Color(255, 255, 255) );
    painter.fillPolygon(line1, 4);
    painter.fillPolygon(line2, 4);
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


void MenuButton::paint(PaintContext& ctx, const Gfx::RectF& rect)
{
    Painter painter(ctx);
    painter.setClip(rect);

    const Gfx::Scaling& scaling = painter.scaling();

    double pixelWidth = scaling.toLogical(1.0);

    double triangleWidth = geometry().height() / 2.0;
    triangleWidth = scaling.align(triangleWidth);

    // even number of pixels
    int pixelsPerWidth = Pt::lround(triangleWidth / pixelWidth);
    if(pixelsPerWidth % 2 != 0)
      triangleWidth += pixelWidth;

    double triangleHeight = triangleWidth / 2.0;
    triangleHeight = scaling.align(triangleHeight);

    double x = (geometry().width() - triangleWidth) / 2.0;
    x = geometry().x() + scaling.align(x);
    
    double y = (geometry().height() - triangleHeight) / 2.0;
    y = geometry().y() + scaling.align(y - 1);

    Gfx::PointF triangle[4];
    triangle[0] = Gfx::PointF(x, y);
    triangle[1] = Gfx::PointF(x + triangleWidth, y);
    triangle[2] = Gfx::PointF(x + triangleHeight, y + triangleHeight);
    triangle[3] = Gfx::PointF(x, y);

    Gfx::Brush brush( Gfx::Color(255, 255, 255) );
    painter.setBrush(brush);
    painter.fillPolygon(triangle, 3);
    
    //painter.setPen(Gfx::ColorF::fromRgb8(255, 0, 0));
    //painter.drawPolyline(triangle, 4);
}

//
// WorkspaceFrame
//

WorkspaceFrame::WorkspaceFrame(WorkspaceManager& workspace, Window& window)
: WindowFrame(workspace, window)
, _wm(&workspace)
, _window(&window)
, _borderWidth( workspace.borderWidth() )
, _titleHeight( workspace.titleHeight() )
, _state(WindowState::Normal)
, _moveOffset(0, 0)
, _resizeStartPointer(0, 0)
, _resizeStartFrameRect()
, _resizeStartClientSize()
, _lastPointer(0, 0)
, _movePending(false)
, _isCapture(false)
, _isClient(false)
, _isMoving(false)
, _isLeftResizing(false)
, _isRightResizing(false)
, _isTopResizing(false)
, _isBottomResizing(false)
, _needsRepaint(false)
, _currentFrameItem(OnNone)
{
    Base::onSetParent(&workspace);
    
    switch( window.type() )
    {
        case WindowType::Popup:
            setFrame(0, 0);
            break;
        
        default:
        case WindowType::Default:
            setFrame(_borderWidth, _titleHeight);
            break;
    }

    _maximizeButton.setParent(*this);
    _maximizeButton.clicked() += Pt::slot(*this, &WorkspaceFrame::onMaximize);
    _buttons.push_back(&_maximizeButton);

    _minimizeButton.setParent(*this);
    _minimizeButton.clicked() += Pt::slot(*this, &WorkspaceFrame::onMinimize);
    _buttons.push_back(&_minimizeButton);

    _closeButton.setParent(*this);
    _closeButton.clicked() += Pt::slot(*this, &WorkspaceFrame::onCloseButton);
    _buttons.push_back(&_closeButton);

    _menuButton.setParent(*this);
    _menuButton.clicked() += Pt::slot(*this, &WorkspaceFrame::onMenu);
    _buttons.push_back(&_menuButton);
}


WorkspaceFrame::~WorkspaceFrame()
{
}


void WorkspaceFrame::onInit(Window& w)
{
    PaintSurface& surface = pixmap();
    Gfx::PointF surfacePos = _clientBounds.topLeft();
    w.setSurface(&surface, surfacePos);

    w.setNextResponder(this);

    if( screen() )
    {
        double scaling = scaleFactor();
    
        RescaleEvent ev(w, scaling);
        w.processEvent(ev);
    }
}


void WorkspaceFrame::onRelease(Window& w)
{
    w.setNextResponder(0);
    w.setSurface( 0, Gfx::PointF() );
}


void WorkspaceFrame::onConnect(Screen& screen)
{
    Base::onConnect(screen);
}


void WorkspaceFrame::onDisconnect()
{
    Base::onDisconnect();
}


void WorkspaceFrame::onSetTitle(Window& w, const std::string& text)
{
    repaint();
}


void WorkspaceFrame::onSetIcon(Window& w, const Gfx::Image& icon)
{
    repaint();
}


void WorkspaceFrame::onSetState(Window& w, const WindowState& state)
{
    Window::State oldState = _state;

    if(oldState == WindowState::Normal)
        setRestore( _window->position(), _window->size() );

    if(state == WindowState::Maximized)
    {
        Gfx::SizeF maxSize = _wm->size();
        maxSize.subWidth( 2 * _borderWidth );
        maxSize.subHeight( (2 * _borderWidth) + _titleHeight );

        onMove( *_window, Gfx::PointF(0, 0) );
        onResize(*_window, maxSize);
    }
    else if(state == WindowState::Minimized)
    {
        if(oldState == WindowState::Maximized)
        {
            onMove( *_window, restorePosition() );
        }

        Gfx::SizeF minSize(restoreSize().width(), 0);
        onResize(*_window, minSize);
    }
    else if(state == WindowState::Normal)
    {
        if(oldState != WindowState::Normal)
        {
            onMove( *_window, restorePosition() );
            onResize(*_window, restoreSize());
        }
    }

    WindowStateEvent wse(*this, state);
    Application::instance().loop().commitEvent(wse);
}


void WorkspaceFrame::onProcessWindowStateEvent(const WindowStateEvent& ev)
{
    WindowStateEvent wse( *_window, ev.state() );
    Application::instance().processEvent(wse);

    Base::onProcessWindowStateEvent(ev);
}


void WorkspaceFrame::onWindowStateEvent(const WindowStateEvent& ev)
{
    _state = ev.state();
}


void WorkspaceFrame::onSetAbove(Window& w, bool above)
{
    _wm->onSetAbove(*this, above);
}


void WorkspaceFrame::onSetSizeLimits(Window& w, 
                                       const Gfx::SizeF& minSize, 
                                       const Gfx::SizeF& maxSize)
{
    _wm->onSetSizeLimits(*this, minSize, maxSize);
}


void WorkspaceFrame::onAutoCenter(Window& w, const Gfx::SizeF* size)
{
    if( ! size )
    {
        _wm->onAutoCenter(*this, size);
        return;
    }

    Gfx::SizeF alignedSize = scaling().align(*size);

    Gfx::SizeF frameSize = alignedSize;
    frameSize.addWidth(2 * _borderWidth);
    frameSize.addHeight(2 * _borderWidth);
    frameSize.addHeight(_titleHeight);

    _wm->onAutoCenter(*this, &frameSize);
}


const Gfx::PointF& WorkspaceFrame::restorePosition() const
{
    return _restorePos;
}


const Gfx::SizeF& WorkspaceFrame::restoreSize() const
{
    return _restoreSize;
}


void WorkspaceFrame::setRestore(const Gfx::PointF& pos, const Gfx::SizeF& size)
{
    _restorePos = pos;
    _restoreSize = size;
}


//Gfx::PointF WorkspaceFrame::clientPos() const
//{
//    double x = _borderWidth;
//    double y = _borderWidth + _titleHeight;
//
//    return Gfx::PointF(x, y);
//}


const Gfx::RectF& WorkspaceFrame::clientRect() const
{
    return _clientRect;
}


const Gfx::RectF& WorkspaceFrame::frameRect() const
{
    return _frameRect;
}


void WorkspaceFrame::setFrame(double bw, double th)
{
    const Gfx::Scaling& scaling = this->scaling();

    _borderWidth = scaling.align(bw);
    _titleHeight = scaling.align(th);

    Gfx::PointF clientBoundsPos(_borderWidth, _borderWidth + _titleHeight);
    _clientBounds.setOrigin(clientBoundsPos);

    PaintSurface& surface = pixmap();
    Gfx::PointF surfacePos = _clientBounds.topLeft();
    _window->setSurface(&surface, surfacePos);
}


void WorkspaceFrame::onMenu()
{
}


void WorkspaceFrame::onMinimize()
{
    if(_state == WindowState::Minimized)
        _window->setState(WindowState::Normal);
    else
        _window->setState(WindowState::Minimized);
}


void WorkspaceFrame::onMaximize()
{
    if(_state == WindowState::Maximized)
        _window->setState(WindowState::Normal);
    else
        _window->setState(WindowState::Maximized);
}


void WorkspaceFrame::onCloseButton()
{
    _window->close();
}


void WorkspaceFrame::onClose(Window& w)
{
    _wm->onClose(*this);
}


void WorkspaceFrame::onProcessCloseEvent(const CloseEvent& ev)
{
    onCloseEvent(ev);

    CloseEvent cev(*_window);
    _window->processEvent(cev);
}


void WorkspaceFrame::onCloseEvent(const CloseEvent& ev)
{
}


void WorkspaceFrame::onLayout()
{
    double buttonWidth = _titleHeight - _borderWidth;

    Gfx::PointF menuPos( _borderWidth, _borderWidth);
    _menuButton.moveEvent( MoveEvent(*_window, menuPos ) );
    _menuButton.resizeEvent( ResizeEvent(*_window, Gfx::SizeF(buttonWidth, buttonWidth) ) );

    double buttonX = _frameRect.width() - (_borderWidth + buttonWidth);
    double buttonY = _borderWidth;

    _closeButton.moveEvent( MoveEvent(*_window, Gfx::PointF(buttonX, buttonY) ) );
    _closeButton.resizeEvent( ResizeEvent(*_window, Gfx::SizeF(buttonWidth, buttonWidth) ) );

    buttonX -= _borderWidth + buttonWidth;
    _maximizeButton.moveEvent( MoveEvent(*_window, Gfx::PointF(buttonX, buttonY) ) );
    _maximizeButton.resizeEvent( ResizeEvent(*_window, Gfx::SizeF(buttonWidth, buttonWidth) ) );

    buttonX -= _borderWidth + buttonWidth;
    _minimizeButton.moveEvent( MoveEvent(*_window, Gfx::PointF(buttonX, buttonY) ) );
    _minimizeButton.resizeEvent( ResizeEvent(*_window, Gfx::SizeF(buttonWidth, buttonWidth) ) );
}


Gfx::PointF WorkspaceFrame::onToWindow(const Window& w, 
                                         const Gfx::PointF& pos) const
{
    double offX = _borderWidth;
    double offY = _borderWidth + _titleHeight;

    return pos - Gfx::PointF(offX, offY);
}


Gfx::PointF WorkspaceFrame::onFromWindow(const Window& w, 
                                           const Gfx::PointF& pos) const
{
    double offX = _borderWidth;
    double offY = _borderWidth + _titleHeight;

    return pos + Gfx::PointF(offX, offY);
}


Widget* WorkspaceFrame::onHitTest(const Gfx::PointF& pos)
{
    if( ! bounds().contains(pos) || ! isVisible() )
        return 0;

    if(_window)
    {
        Gfx::PointF p = onToWindow(*_window, pos);
        Widget* hit = _window->hitTest(p);
        if(hit)
            return hit;
    }

    return this;
}


Gfx::PointF WorkspaceFrame::onToParent(const Gfx::PointF& pos) const
{ 
    return _wm->fromFrame(*this, pos);
}

        
Gfx::PointF WorkspaceFrame::onFromParent(const Gfx::PointF& pos) const
{
    return _wm->toFrame(*this, pos);
}


void WorkspaceFrame::onProcessEvent(const Pt::Event& ev)
{
    Base::onProcessEvent(ev);
}


void WorkspaceFrame::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);

    double scaling = ev.scaleFactor();

    RescaleEvent rev(*_window, scaling);
    _window->processEvent(rev);
}


void WorkspaceFrame::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);

    // align frame
    setFrame(_borderWidth, _titleHeight);
}


void WorkspaceFrame::onShow(Window& w, bool visible)
{
    _wm->onShow(*this, visible);
}


void WorkspaceFrame::onProcessShowEvent(const ShowEvent& ev)
{
    Base::onProcessShowEvent(ev);

    ShowEvent rev( *_window, ev.visible() );
    _window->processEvent(rev);
}


void WorkspaceFrame::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);

    repaint(_frameBounds);
}


void WorkspaceFrame::onEnable(Window& w, bool enable)
{
    _wm->onEnable(*this, enable);
}


void WorkspaceFrame::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    EnableEvent eev( *_window, ev.enabled() );
    _window->processEvent(eev);
}


void WorkspaceFrame::onEnableEvent(const EnableEvent& ev)
{    
    Base::onEnableEvent(ev);

    repaint(_frameBounds);
}


void WorkspaceFrame::onActivate(Window& w, bool active)
{
    _wm->onActivate(*this, active);
}


void WorkspaceFrame::onProcessActivateEvent(const ActivateEvent& ev)
{
    Base::onProcessActivateEvent(ev);

    ActivateEvent aev( *_window, ev.isActive() );
    _window->processEvent(aev);
}


void WorkspaceFrame::onActivateEvent(const ActivateEvent& ev)
{
    Base::onActivateEvent(ev);

    repaint(_frameBounds);
}


void WorkspaceFrame::onRequestMove(const Gfx::PointF& pos)
{
    Gfx::PointF alignedPos = scaling().align(pos);

    if(alignedPos == _frameRect.topLeft())
        return;

        Gfx::RectF updateRect = _frameRect;

    _frameRect.setOrigin(alignedPos);

    MoveEvent immediateMove(*this, alignedPos);
    Base::onMoveEvent(immediateMove);

    Gfx::PointF clientPos = alignedPos;
    clientPos.addX(_borderWidth);
    clientPos.addY(_borderWidth + _titleHeight);
    _clientRect.setOrigin(clientPos);

    updateRect.unify(_frameRect);
    _wm->repaint(updateRect);

    if( ! _movePending )
    {
        _movePending = true;
        _wm->onMove(*this, alignedPos);
    }
}


void WorkspaceFrame::onMove(Window& w, const Gfx::PointF& pos)
{
    Gfx::PointF alignedPos = scaling().align(pos);

    if(alignedPos == _frameRect.topLeft())
        return;

        Gfx::RectF updateRect = _frameRect;

    _frameRect.setOrigin(alignedPos);

    MoveEvent immediateMove(*this, alignedPos);
    Base::onMoveEvent(immediateMove);

    Gfx::PointF clientPos = alignedPos;
    clientPos.addX(_borderWidth);
    clientPos.addY(_borderWidth + _titleHeight);
    _clientRect.setOrigin(clientPos);

    updateRect.unify(_frameRect);
    _wm->repaint(updateRect);

    if( ! _movePending )
    {
        _movePending = true;
        _wm->onMove(*this, alignedPos);
    }
}


void WorkspaceFrame::onProcessMoveEvent(const MoveEvent& ev)
{
    Base::onProcessMoveEvent(ev);

    MoveEvent mev( *_window, _frameRect.topLeft() );
    _window->processEvent(mev);
}


void WorkspaceFrame::onMoveEvent(const MoveEvent& ev)
{
    (void) ev;
    _movePending = false;

    MoveEvent currentMove(*this, _frameRect.topLeft());
    Base::onMoveEvent(currentMove);
}


void WorkspaceFrame::onRequestResize(const Gfx::SizeF& size)
{
    _clientRect.setSize(size);

    _clientBounds.setSize(size);

    Gfx::SizeF frameSize = size;
    frameSize.addWidth(2 * _borderWidth);
    frameSize.addHeight(2 * _borderWidth);
    frameSize.addHeight(_titleHeight);
    _frameRect.setSize(frameSize);

    _frameBounds.setSize(frameSize);

    //_wm->onResize(*this, size);
}


Gfx::SizeF WorkspaceFrame::onResize(Window& w, const Gfx::SizeF& s)
{
    Gfx::SizeF alignedSize = scaling().align(s);

    if( alignedSize.width() > w.maximumSize().width() )
        alignedSize.setWidth( w.maximumSize().width() );

    if( alignedSize.height() > w.maximumSize().height() )
        alignedSize.setHeight( w.maximumSize().height() );

    if( alignedSize.width() < w.minimumSize().width() )
        alignedSize.setWidth( w.minimumSize().width() );

    if( alignedSize.height() < w.minimumSize().height() )
        alignedSize.setHeight( w.minimumSize().height() );

    _clientRect.setSize(alignedSize);
    _clientBounds.setSize(alignedSize);

    Gfx::SizeF frameSize = alignedSize;
    frameSize.addWidth(2 * _borderWidth);
    frameSize.addHeight(2 * _borderWidth);
    frameSize.addHeight(_titleHeight);
    _frameRect.setSize(frameSize);

    _frameBounds.setSize(frameSize);

    _wm->onResize(*this, frameSize);

    return alignedSize;
}


void WorkspaceFrame::onProcessResizeEvent(const ResizeEvent& ev)
{
    Gfx::SizeF clientSize = ev.size();
    clientSize.subWidth(2 * _borderWidth);
    clientSize.subHeight(2 * _borderWidth);
    clientSize.subHeight(_titleHeight);
    
    _clientRect.setSize(clientSize);
    _clientBounds.setSize(clientSize);

    _frameRect.setSize( ev.size() );
    _frameBounds.setSize( ev.size() );

    Base::onProcessResizeEvent(ev);

    ResizeEvent rev(*_window, clientSize);
    _window->processEvent(rev);
}


void WorkspaceFrame::onResizeEvent(const ResizeEvent& ev)
{
    Gfx::RectF updateRect( size() );
    updateRect.unify( Gfx::RectF(ev.size()) );

    Base::onResizeEvent(ev);

    onLayout();

    repaint(updateRect);
}


bool WorkspaceFrame::onEnterEvent( const EnterEvent& ev)
{
    _currentFrameItem = OnNone;

    return Base::onEnterEvent(ev);
}


bool WorkspaceFrame::onLeaveEvent(const LeaveEvent& ev)
{
    _currentFrameItem = OnNone;

    return Base::onLeaveEvent(ev);;
}


void WorkspaceFrame::onProcessMouseEvent(const MouseEvent& ev)
{
    if( ! window().acceptsInput() )
        return;

    //
    // stop capture on press
    // 
    if(_isCapture)
    {
        if( ev.isRelease() )
        {
            setCapture(false);
            _isCapture = false;
        }
    }

    //
    // hit test
    //
    Gfx::PointF pos = fromGlobal( ev.position() );

    Window* window = checkWindow(pos);
    if(window)
    {
        window->processEvent(ev);
        return;
    }

    //
    // start capture on press
    //
    if( ev.isPress() )
    {
        setCapture(true);
        _isCapture = true;
    }
        
    Widget::onProcessMouseEvent(ev);
}


void WorkspaceFrame::onProcessTouchEvent(const TouchEvent& tev)
{
    Gfx::PointF pos = fromGlobal( tev.position() );

    Window* window = checkWindow(pos);
    if(window)
    {
        window->processEvent(tev);
        return;
    }
        
    Widget::onProcessTouchEvent(tev);
}


bool WorkspaceFrame::onMouseEvent(const MouseEvent& mev)
{
    Gfx::PointF pos = mev.position();

    WindowButton* button = checkButton(pos);
    if(button)
    {
        setCurrentFrameItem(OnButton);
        button->mouseEvent(mev);
    }
    else if( isTitle(pos) )
    {
        setCurrentFrameItem(OnTitle);
        checkMove(pos, mev.isPressed(), mev.isPress() );
    }
    else
    {
        bool onLeftBorder = isLeftBorder(pos);
        bool onRightBorder = isRightBorder(pos);
        bool onTopBorder = isTopBorder(pos);
        bool onBottomBorder = isBottomBorder(pos);

        if(onLeftBorder || onRightBorder || onTopBorder || onBottomBorder)
        {
            if(onTopBorder && onLeftBorder) 
                setCurrentFrameItem(OnFrameTopLeft);
            else if(onTopBorder && onRightBorder) 
                setCurrentFrameItem(OnFrameTopRight);
            else if(onTopBorder)
                setCurrentFrameItem(OnFrameTop);
            else if(onBottomBorder && onLeftBorder) 
                setCurrentFrameItem(OnFrameBottomLeft);
            else if(onBottomBorder && onRightBorder) 
                setCurrentFrameItem(OnFrameBottomRight);
            else if(onBottomBorder)
                setCurrentFrameItem(OnFrameBottom);
            else if(onLeftBorder)
                setCurrentFrameItem(OnFrameLeft);
            else if(onRightBorder)
                setCurrentFrameItem(OnFrameRight);

            checkResize(pos, mev.isPressed(), mev.isPress());
        }
        else
        {
            setCurrentFrameItem(OnNone);
        }
    }

    _lastPointer = pos;
    
    return true;
}


bool WorkspaceFrame::onTouchEvent(const TouchEvent& tev)
{
    Gfx::PointF pos = tev.position();

    WindowButton* button = checkButton(pos);
    if(button)
    {
        setCurrentFrameItem(OnButton);
        button->touchEvent(tev);
    }
    else if( isTitle(pos) )
    {
        setCurrentFrameItem(OnTitle);
        checkMove(pos, tev.isPressed(), tev.isPress() );
    }
    else
    {
        bool onLeftBorder = isLeftBorder(pos);
        bool onRightBorder = isRightBorder(pos);
        bool onTopBorder = isTopBorder(pos);
        bool onBottomBorder = isBottomBorder(pos);

        if(onLeftBorder || onRightBorder || onTopBorder || onBottomBorder)
        {
            if(onTopBorder && onLeftBorder) 
                setCurrentFrameItem(OnFrameTopLeft);
            else if(onTopBorder && onRightBorder) 
                setCurrentFrameItem(OnFrameTopRight);
            else if(onTopBorder)
                setCurrentFrameItem(OnFrameTop);
            else if(onBottomBorder && onLeftBorder) 
                setCurrentFrameItem(OnFrameBottomLeft);
            else if(onBottomBorder && onRightBorder) 
                setCurrentFrameItem(OnFrameBottomRight);
            else if(onBottomBorder)
                setCurrentFrameItem(OnFrameBottom);
            else if(onLeftBorder)
                setCurrentFrameItem(OnFrameLeft);
            else if(onRightBorder)
                setCurrentFrameItem(OnFrameRight);

            checkResize(pos, tev.isPressed(), tev.isPress());
        }
        else
        {
            setCurrentFrameItem(OnNone);
        }
    }

    _lastPointer = pos;

    return true;
}


void WorkspaceFrame::setCurrentFrameItem(FrameItem item)
{
    if(_currentFrameItem == item)
        return;

    switch(item)
    {
      case OnFrameLeft:
      case OnFrameRight:
          Application::instance().setCursor( &Forms::Cursor::sizeWECursor() );
          break;

      case OnFrameTopLeft:
      case OnFrameBottomRight:
          Application::instance().setCursor( &Forms::Cursor::sizeNWSECursor() );
          break;

      case OnFrameTop:
      case OnFrameBottom:
          Application::instance().setCursor( &Forms::Cursor::sizeNSCursor() );
          break;

      case OnFrameBottomLeft:
      case OnFrameTopRight:
          Application::instance().setCursor( &Forms::Cursor::sizeNESWCursor() );
          break;

      case OnTitle:
          Application::instance().setCursor( &Cursor::moveCursor() );
          break;

      case OnButton:
          Application::instance().setCursor( &Cursor::defaultCursor() );
          break;

      default:
          Application::instance().setCursor( &Cursor::defaultCursor() );
    }
    
    _currentFrameItem = item;
}


bool WorkspaceFrame::isTitle(const Gfx::PointF& p) const
{
    bool isResizing = _isLeftResizing || _isRightResizing ||
                      _isTopResizing || _isBottomResizing;

    Gfx::PointF localPos = p - _frameBounds.topLeft();

    bool overTitle = localPos.x() >= _borderWidth &&
                     localPos.x() < _borderWidth + _clientBounds.width() &&
                     localPos.y() >= _borderWidth &&
                     localPos.y() < _borderWidth + _titleHeight;

    return (_isMoving || overTitle) && ! isResizing;
}


bool WorkspaceFrame::isLeftBorder(const Pt::Gfx::PointF& p) const
{
    Gfx::PointF localPos = p - _frameBounds.topLeft();

    bool r =  localPos.x() >= 0 &&
              localPos.x() < _borderWidth &&
              localPos.y() >= 0 &&
              localPos.y() < _frameBounds.height();

    return _isLeftResizing || r;
}


bool WorkspaceFrame::isRightBorder(const Pt::Gfx::PointF& p) const
{
    Gfx::PointF localPos = p - _frameBounds.topLeft();

    bool r =   localPos.x() >= _borderWidth + _clientBounds.width() &&
               localPos.x() < 2 * _borderWidth + _clientBounds.width() &&
               localPos.y() >= 0 &&
               localPos.y() < _frameBounds.height();

    return _isRightResizing || r;
}


bool WorkspaceFrame::isTopBorder(const Pt::Gfx::PointF& p) const
{
    Gfx::PointF localPos = p - _frameBounds.topLeft();

    bool r =   localPos.x() >= 0 &&
               localPos.x() < _frameBounds.width() &&
               localPos.y() >= 0 &&
               localPos.y() < _borderWidth;

    return _isTopResizing || r;
}


bool WorkspaceFrame::isBottomBorder(const Pt::Gfx::PointF& p) const
{
    Gfx::PointF localPos = p - _frameBounds.topLeft();

    double minY = _clientBounds.height() + _borderWidth + _titleHeight;

    bool r =   localPos.x() >= 0 &&
               localPos.x() < _frameBounds.width() &&
               localPos.y() >= minY &&
               localPos.y() < _frameBounds.height();

    return _isBottomResizing || r;
}


Window* WorkspaceFrame::checkWindow(const Gfx::PointF& pos)
{
    bool isResizing = _isLeftResizing || _isRightResizing ||
                      _isTopResizing || _isBottomResizing;

    if( ! _isMoving && ! isResizing )
    {
        if(_clientBounds.contains( pos ) )
        {
            if( ! _isClient )
            {
                _isClient = true;
                //EnterEvent eev(*_window);
                //_window->processEvent(eev);
            }

            return _window;
        }

        if(_isClient)
        {
            _isClient = false;
            //LeaveEvent lev( *_window );
            //_window->processEvent(lev);
        }
    }

    return 0;
}


WindowButton* WorkspaceFrame::checkButton(const Gfx::PointF& pos)
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
                button->leaveEvent( LeaveEvent(*_window) );
            }
        }
    }

    return 0;
}


bool WorkspaceFrame::checkMove(const Gfx::PointF& pos, bool isDrag, bool isPress)
{
    if( isTitle(pos) )
    {
        _isMoving = (_isMoving && isDrag) || isPress;

        if(isPress)
        {
            _moveOffset = toGlobal(pos) - _frameRect.topLeft();
        }

        if(_isMoving && ! isPress)
        {
            Gfx::PointF to = toGlobal(pos) - _moveOffset;
            //_window->move(to);
            
            move(to);
        }

        return _isMoving;
    }

    return false;
}


bool WorkspaceFrame::checkResize(const Gfx::PointF& pos, bool isDrag, bool isPress)
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

        if(isResizing && isPress)
        {
            _resizeStartPointer = toGlobal(pos);
            _resizeStartFrameRect = _frameRect;
            _resizeStartClientSize = _clientBounds.size();
        }

        if(isResizing && ! isPress)
        {
            const Gfx::PointF currentPointer = toGlobal(pos);
            const Gfx::PointF delta = currentPointer - _resizeStartPointer;

            Gfx::SizeF winSize = _resizeStartClientSize;

            if( _isLeftResizing )
            {
                winSize.subWidth(delta.x());
            }

            if(_isRightResizing)
                winSize.addWidth( delta.x() );

            if(_isTopResizing)
            {
                winSize.subHeight(delta.y());
            }

            if(_isBottomResizing)
                winSize.addHeight( delta.y() );

            if( winSize != _clientBounds.size() )
            {
                _window->resize(winSize);
                //resize(winSize);
            }

            Gfx::PointF winpos = _resizeStartFrameRect.topLeft();

            if( _isLeftResizing )
                winpos.setX(_resizeStartFrameRect.right() - _frameRect.width());

            if(_isTopResizing)
                winpos.setY(_resizeStartFrameRect.bottom() - _frameRect.height());

            if( winpos != _frameRect.topLeft() )
            {
                _window->move(winpos);
                //move(winpos);
            }
        }

        return isResizing;
    }

    return false;
}


void WorkspaceFrame::onRepaint(Window& w, const Gfx::RectF& rect)
{
    _needsRepaint = true;

    Gfx::PointF windowPos = onFromWindow( *_window, rect.topLeft() );
    Gfx::RectF windowRect( windowPos, rect.size() );

    repaint(windowRect);
}


void WorkspaceFrame::onRequestRepaint(const Gfx::RectF& rect)
{
    _needsRepaint = true;

    Gfx::PointF updatePos = rect.topLeft() + _frameRect.topLeft();
    Gfx::RectF updateRect( updatePos, rect.size() );
    
    _wm->repaint(updateRect);
}


void WorkspaceFrame::onProcessPaintEvent(const PaintEvent& ev)
{
    if( ! _needsRepaint )
        return;

    _needsRepaint = false;

    const Gfx::RectF& rect = ev.rect();
    
    Gfx::PointF winPos = onToWindow( *_window, rect.topLeft() );
    Gfx::RectF winRect( winPos, rect.size() );

    PaintEvent pev(*_window, winRect);
    _window->processEvent(pev);

    PaintEvent fullEv(*this, _frameBounds);
    Base::onProcessPaintEvent(fullEv);
}


void WorkspaceFrame::onPaintEvent(const PaintEvent& ev)
{
    Base::onPaintEvent(ev);

    if( _borderWidth < 0.1 && _titleHeight < 0.1  )
        return;

    const Gfx::RectF& rect = ev.rect();
    PaintSurface& surface = this->surface();

    PaintContext ctx(surface);
    Painter painter(ctx);
    painter.setClip(rect);

    const Gfx::Scaling& scaling = this->scaling();

    Gfx::Color color = _window->isActive() ? _wm->activeColor()
                                           : _wm->inactiveColor();
    //
    // frame background
    //
    Gfx::Brush brush(color);
    painter.setBrush(brush);

    //Gfx::PointF pos = _window->position();
    Gfx::PointF pos(0, 0);
    
    Gfx::RectF leftBorder( pos.x(),
                           pos.x() + _borderWidth,
                           pos.y() + _borderWidth,
                           pos.y() + size().height() - _borderWidth);
    painter.fillRect(leftBorder);

    Gfx::RectF topBorder(pos.x(),
                         pos.x() + size().width(),
                         pos.y(),
                         pos.y() + _borderWidth);

    painter.fillRect(topBorder);
    
    Gfx::RectF rightBorder(pos.x() + size().width() - _borderWidth,
                           pos.x() + size().width(),
                           pos.y() + _borderWidth,
                           pos.y() + size().height() - _borderWidth);
    painter.fillRect(rightBorder);


    Gfx::RectF bottomBorder(pos.x(),
                            pos.x() + size().width(),
                            pos.y() + size().height() - _borderWidth,
                            pos.y() + size().height());
    painter.fillRect(bottomBorder);

    Gfx::RectF titleArea( pos.x() + _borderWidth,
                          pos.x() + size().width() - _borderWidth,
                          pos.y() + _borderWidth,
                          pos.y() + _borderWidth + _titleHeight);

    painter.fillRect(titleArea);

    const size_t penSize = 1;
    unsigned scaledPenSize = static_cast<unsigned>( scaling.toPhysical(penSize) );
    const double offset = scaling.toLogical(scaledPenSize) / 2.0;

    //
    // light outer and inner border contour
    //
    Gfx::Color borderLight = brighten(color, 1.25f);
    Gfx::Pen borderPenLight(borderLight, penSize);

    painter.setPen(borderPenLight);

    // outer top
    painter.drawLine(Gfx::PointF(pos.x(),
                                 pos.y() + offset),
                     Gfx::PointF(pos.x() + size().width(),
                                 pos.y() + offset) );
    // outer left
    painter.drawLine(Gfx::PointF(pos.x() + offset, 
                                 pos.y()),
                     Gfx::PointF(pos.x() + offset,
                                 pos.y() + size().height()) );

    // inner right
    painter.drawLine( Gfx::PointF(pos.x() + size().width() - _borderWidth + offset,
                                  pos.y() + _borderWidth + _titleHeight),
                      Gfx::PointF(pos.x() + size().width() - _borderWidth + offset,
                                  pos.y() + size().height() - _borderWidth) );

    // inner bottom
    painter.drawLine( Gfx::PointF(pos.x() + _borderWidth,
                                  pos.y() + size().height() - _borderWidth + offset),
                      Gfx::PointF(pos.x() + size().width() - _borderWidth,
                                  pos.y() + size().height() - _borderWidth + offset) );

    //
    // dark outer and inner border contour
    //
    
    Gfx::Color borderDark = brighten(color, 0.75f);
    Gfx::Pen borderPenDark(borderDark, penSize);

    painter.setPen(borderPenDark);
    
    // outer bottom
    painter.drawLine( Gfx::PointF(pos.x(),
                                  pos.y() + size().height() - offset),
                      Gfx::PointF(pos.x() + size().width() - offset,
                                  pos.y() + size().height() - offset) );
    
    // outer right
    painter.drawLine(Gfx::PointF( pos.x() + size().width() - offset,
                                  pos.y() ),
                     Gfx::PointF(pos.x() + size().width() - offset,
                                 pos.y() + size().height() - offset) );
    
    // inner left
    painter.drawLine( Gfx::PointF(pos.x() + _borderWidth - offset,
                                  pos.y() + _borderWidth + _titleHeight - offset),
                      Gfx::PointF(pos.x() + _borderWidth - offset,
                                  pos.y() + size().height() - _borderWidth) );

    // inner top
    painter.drawLine( Gfx::PointF(pos.x() + _borderWidth - offset,
                                  pos.y() + _borderWidth + _titleHeight - offset),
                      Gfx::PointF(pos.x() + size().width() - _borderWidth,
                                  pos.y() + _borderWidth + _titleHeight - offset) );

    //
    // title bar text
    //
    Pt::String title = _window->title().c_str();

    const Gfx::Font& font = Application::instance().styleOptions().font();
    painter.setFont(font);
    Gfx::TextMetrics tm = painter.textMetrics(title);
    Gfx::FontMetrics fm = painter.fontMetrics();

    double textOffset = (_borderWidth + _titleHeight - fm.height()) / 2.0;

    Gfx::PointF textPos(pos.x() + _borderWidth + _titleHeight,
                        pos.y() + textOffset + fm.ascent() );

    Gfx::Color textColor = _window->isActive() ? _wm->textColor()
                                               : _wm->inactiveTextColor();
    Gfx::Pen pen(textColor, 1);
    painter.setPen(pen);
    painter.drawText(textPos, title);

    //
    // grip area on title bar
    //
    Gfx::Color gripColorLight( static_cast<Pt::uint8_t>(std::min(color.red() * 11 / 10, 255)),
                               static_cast<Pt::uint8_t>(std::min(color.green() * 11 / 10, 255)),
                               static_cast<Pt::uint8_t>(std::min(color.blue() * 11 / 10, 255)) );
    Gfx::Pen gripPenLight(gripColorLight, 1);

    Gfx::Color gripColorDark( static_cast<Pt::uint8_t>(color.red() * 9 / 10),
                              static_cast<Pt::uint8_t>(color.green() * 9 / 10),
                              static_cast<Pt::uint8_t>(color.blue() * 9 / 10) );
    Gfx::Pen gripPenDark(gripColorDark, 1);

    unsigned linePenSize = static_cast<unsigned>( scaling.toPhysical(penSize) );
    const double lineSize = scaling.toLogical(linePenSize);

    double lineOffset = scaling.align(2.0);
    double gripHeight = (8 * lineSize) + (3 * lineOffset);
    
    double gripLeft = textPos.x() + tm.advance() + _borderWidth;
    double gripRight = pos.x() + size().width() - _borderWidth - 3 * _titleHeight;
    double gripOffset = (_titleHeight + _borderWidth - gripHeight) / 2.0;
    
    double gripY = pos.y() + scaling.align(gripOffset);
    gripY += offset;

    for(int n = 0; n < 4; ++n)
    {
        painter.setPen(gripPenLight);
        painter.drawLine( Gfx::PointF(gripLeft, gripY),
                          Gfx::PointF(gripRight, gripY) );

        gripY += lineSize;

        painter.setPen(gripPenDark);
        painter.drawLine( Gfx::PointF(gripLeft, gripY),
                          Gfx::PointF(gripRight, gripY) );

        gripY += lineOffset + lineSize;
    }

    //
    // window buttons
    //
    std::vector<WindowButton*>::iterator it;
    for(it = _buttons.begin(); it != _buttons.end(); ++it)
    {
        WindowButton* button = *it;

        Gfx::RectF buttonUpdateRect = button->geometry().intersect(rect);
        if( buttonUpdateRect.isNull() )
            continue;

        button->paint(ctx, buttonUpdateRect);
    }
}

} // namespace

} // namespace
