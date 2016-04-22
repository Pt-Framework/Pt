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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Point.h>
#include <Pt/String.h>
#include <cmath>

namespace Pt {

namespace Hmi {

/////////////////////////////////////////////////////////////////////////////
// WindowFrame
/////////////////////////////////////////////////////////////////////////////

WindowFrame::WindowFrame()
: _wm(0)
, _window(0)
, _inactiveColor(0.68f, 0.70f, 0.75f)
, _activeColor(0.4f, 0.5f, 0.8f)
, _textColor(0.0, 0.0, 0.0)
{
}


WindowFrame::WindowFrame(WindowManager& wm, Window& window)
: _wm(&wm)
, _window(&window)
, _closeButton(0, 10, 0, 10)
, _maximizeButton(0, 10, 0, 10)
, _minimizeButton(0, 10, 0, 10)
, _inactiveColor(0.68f, 0.70f, 0.75f)
, _activeColor(0.4f, 0.5f, 0.8f)
, _textColor(0.0, 0.0, 0.0)
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

    Gfx::Color color = _window->isActive() ? _activeColor 
                                           : _inactiveColor;  
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

    Gfx::Pen pen(1, _textColor);
    painter.setPen(pen);

    Gfx::FontMetrics fm = painter.fontMetrics( font, Pt::String("A") );

    double textMargin = (titleHeight - fm.height()) / 2;
    Gfx::PointF textPos(pos.x() + borderWidth + titleHeight, 
                        pos.y() + titleHeight - textMargin);

    painter.drawText(textPos, Pt::String( _window->title().c_str()) );


    brush = Gfx::Color(0.9, 0.1, 0.0);
    painter.setBrush(brush);
    painter.fillRect(_closeButton);

    brush = Gfx::Color(0.95, 0.8, 0);
    painter.setBrush(brush);
    painter.fillRect(_maximizeButton);

    brush = Gfx::Color(0.1, 0.7, 0.1);
    painter.setBrush(brush);
    painter.fillRect(_minimizeButton);
}

/////////////////////////////////////////////////////////////////////////////
// WindowManager
/////////////////////////////////////////////////////////////////////////////

WindowManager::WindowManager()
: _app( Application::instance() )
, _state(&WindowManager::onBackground)
, _managedWindow(0)
, _sizingDirection( ResizeDirection::None )
, _actionButton(0)
, _parent(0)
, _borderWidth(4)
, _titleHeight(20)
{    
}


WindowManager::~WindowManager()
{
}


void WindowManager::init(Window& parent)
{
    _parent = &parent;
}


void WindowManager::add(Window& w)
{    
    _windows.insert( _windows.begin(), WindowFrame(*this, w) );
}


void WindowManager::remove(Window& w)
{
    std::vector<WindowFrame>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {
        if(wit->window() == &w)
        {
            _windows.erase(wit);
            break;
        }
    }
}


WindowFrame* WindowManager::findWindow(const Gfx::PointF& p)
{
    std::vector<WindowFrame>::reverse_iterator rit;
    for(rit =  _windows.rbegin() ; rit != _windows.rend(); ++rit )
    {
        if( ! rit->window()->isVisible() )
            continue;

        if( ! rit->frameRect().contains(p) )
            continue;

        return &*rit;
    }

    return 0;
}


WindowFrame* WindowManager::findWindow(Window& w)
{
    std::vector<WindowFrame>::iterator it;
    for(it = _windows.begin(); it != _windows.end(); ++it)
    {
        if( it->window() == &w)
            return &*it;
    }

    return 0;
}


PaintSurface& WindowManager::surface()
{
    return _parent->surface();
}


bool WindowManager::keyInput(const Pt::Hmi::KeyEvent& keyEvent)
{
    Window* w = _parent->activeWindow();

    if( w == 0 )
        return false;

    if( w->isEnabled() )
        w->processEvent(keyEvent);

    return true;
}


bool WindowManager::pointerInput( const Pt::Hmi::MouseEvent& mev )
{
    if( mev.isPress(_actionButton) )
    {
        WindowFrame* wf = findWindow( mev.position() );
        if(wf)
            wf->window()->activate();
    }

    bool r = (this->*_state)(mev);
    _lastPointerPosition = Gfx::PointF( mev.x(), mev.y() );    
    return r;
}


void WindowManager::paintEvent(const PaintEvent& pev)
{  
    PaintSurface& surface = _parent->surface();
    const Gfx::RectF& rect = pev.rect();

    std::vector<WindowFrame>::reverse_iterator it;

    for(it = _windows.rbegin(); it != _windows.rend(); ++it )
    {
        WindowFrame& frame = *it;
        Window* w = frame.window();                
        
        if( ! w->isVisible() )
            continue; 

        Gfx::RectF frameRect = frame.frameRect();
        frameRect.setOrigin( Gfx::PointF(0, 0) );
        PaintEvent ev(0, frameRect);
        frame.paintEvent(ev);

        // update rect in client coordinates
        Gfx::PointF updatePos = rect.topLeft() - frame.clientRect().topLeft();
        Gfx::RectF updateRect(updatePos, rect.size());        
        
        // clip update rect against client rect
        Gfx::RectF clientRect(Gfx::PointF(0, 0), w->size());
        updateRect = updateRect.intersect(clientRect);

        Gfx::PointF to = updateRect.topLeft() + frame.clientRect().topLeft();

        Painter painter(surface);
        painter.drawSurface(to, w->surface(), updateRect);    
    }
}


bool WindowManager::isMoving(const WindowFrame& wf, const Pt::Hmi::MouseEvent& ev)
{
    // position in frame coordinates
    Gfx::PointF p = ev.position() - wf.frameRect().topLeft();
    return wf.isTitle(p);
}


ResizeDirection::Type WindowManager::isSizing(const WindowFrame& wf, const Pt::Hmi::MouseEvent& ev)
{    
    const Window* w = wf.window();

    const Gfx::SizeF  wsize = w->size();
    const Gfx::PointF wpos  = w->position();
    double titleHeight = _titleHeight;

    if( ev.x() < wpos.x() ||
        ev.x() > wpos.x() + 2*_borderWidth + wsize.width() ||
        ev.y() < wpos.y() ||
        ev.y() >= wpos.y() + 2*_borderWidth + titleHeight + wsize.height() )
        return ResizeDirection::None;

    bool left = ev.x() < (wpos.x() + _borderWidth);
    bool right = ev.x() >= wpos.x() + _borderWidth + wsize.width();
    bool top = ev.y() < wpos.y() + _borderWidth;
    bool bottom = ev.y() >= wpos.y() + _borderWidth + titleHeight + wsize.height();

    if(top && left)
        return ResizeDirection::NorthWest;

    if(top && right)
        return ResizeDirection::NorthEast;
    
    if(bottom && left)
        return ResizeDirection::SouthWest;
    
    if(bottom && right)
        return ResizeDirection::SouthEast;

    if(left)                
        return ResizeDirection::West;

    if(right)
        return ResizeDirection::East;

    if(top)
        return ResizeDirection::North;

    if(bottom)
        return ResizeDirection::South;            

    return ResizeDirection::None;            
}


void WindowManager::setSizingCursor( ResizeDirection::Type type )
{
    switch( type )
    {
        case ResizeDirection::East:
        case ResizeDirection::West:
            _app.setCursor( &Hmi::Cursor::sizeWECursor() );
        break;

        case ResizeDirection::NorthEast:
        case ResizeDirection::SouthWest:
            _app.setCursor( &Hmi::Cursor::sizeNESWCursor() );
        break;

        case ResizeDirection::North:        
        case ResizeDirection::South:
            _app.setCursor( &Hmi::Cursor::sizeNSCursor() );
        break;
        
        case ResizeDirection::NorthWest:
        case ResizeDirection::SouthEast:
            _app.setCursor( &Hmi::Cursor::sizeNWSECursor() );
        break;        
    }
}


MouseEvent WindowManager::toWindow(Window* w, const MouseEvent& mev)
{
    Pt::Hmi::MouseEvent childEvent = mev;

    childEvent.setId( w->vid() );

    double childX = mev.x() - w->position().x() - _borderWidth;
    double childY = mev.y() - w->position().y() - _titleHeight - _borderWidth;

    childEvent.setX(childX);
    childEvent.setY(childY);
    return childEvent;
}


bool WindowManager::onBackground(const Pt::Hmi::MouseEvent& mev)
{
    //std::clog << "onBackground: " << (_parent ? _parent->title() : "WM") << std::endl;    
    _managedWindow = findWindow( mev.position() );
    
    // pointer on window background 
    if( ! _managedWindow )
    {
        _app.setPointerWindow(_parent);
        _state = &WindowManager::onBackground;        
        return false;
    }    

    // pointer on window title bar
    if( isMoving(*_managedWindow, mev) )
    {
        _app.setCursor( &Cursor::moveCursor() );
        _app.setPointerWindow( 0 );
        _state = &WindowManager::onWindowFrame;
        return true;
    }

    // pointer on window border
    _sizingDirection = isSizing(*_managedWindow, mev);

    if( _sizingDirection != ResizeDirection::None )
    {
        setSizingCursor(_sizingDirection);
        _app.setPointerWindow( 0);
        _state = &WindowManager::onWindowFrame;
        return true;
    }                                
    
    // pointer on window content
    _state = &WindowManager::onWindowContent;    
    _app.setPointerWindow( _managedWindow->window() );
    _managedWindow->window()->processEvent( toWindow(_managedWindow->window(), mev) );
    return true;
}


bool WindowManager::onWindowFrame(const Pt::Hmi::MouseEvent& mev)
{
    //std::clog << "onWindowFrame: " << (_parent ? _parent->title() : "WM") << std::endl;   
        
    _managedWindow = findWindow( mev.position() );

    // pointer on window background 
    if( ! _managedWindow)
    {
        _app.setPointerWindow(_parent);
        _state = &WindowManager::onBackground;
        return false;
    }
    
    if( _managedWindow->mouseEvent(mev) )
        return true;

    // pointer on window title bar
    if( isMoving(*_managedWindow, mev) )
    {
        _app.setCursor( &Cursor::moveCursor() );
        _app.setPointerWindow( 0);

        if( mev.isPress(_actionButton) )
        {
            _state = &WindowManager::onWindowMove;
            _managedWindowPosition = _managedWindow->window()->position();
        }
        else
            _state = &WindowManager::onWindowFrame;

        return true;
    }

    // pointer on window border
    _sizingDirection = isSizing(*_managedWindow, mev);
    if( _sizingDirection != ResizeDirection::None )
    {                      
        setSizingCursor(_sizingDirection);
        _app.setPointerWindow( 0);

        if( mev.isPress(_actionButton) )
        {
            _state = &WindowManager::onWindowResize;
            _managedWindowPosition = _managedWindow->window()->position();
            _managedWindowSize = _managedWindow->window()->size();
        }
        else
            _state = &WindowManager::onWindowFrame;
        
        return true;
    }   

    // pointer on window content
    _state = &WindowManager::onWindowContent;
    _app.setPointerWindow(_managedWindow->window());
    _managedWindow->window()->processEvent( toWindow(_managedWindow->window(), mev) );
    return true;
}


bool WindowManager::onWindowContent(const Pt::Hmi::MouseEvent& mev)
{    
    //std::clog << "onWindowContent: " << (_parent ? _parent->title() : "WM") << std::endl;    
    
    _managedWindow = findWindow( mev.position() );
    
    // pointer on window background
    if( ! _managedWindow )
    {        
        _app.setPointerWindow( _parent);
        _state = &WindowManager::onBackground;
        return false;
    }        

    // pointer on window title bar
    if( isMoving(*_managedWindow, mev) )
    {
        _app.setCursor( &Cursor::moveCursor() );
        _app.setPointerWindow( 0);
        _state = &WindowManager::onWindowFrame;
        return true;
    }

    // pointer on window border
    _sizingDirection = isSizing(*_managedWindow, mev);

    if( _sizingDirection != ResizeDirection::None )
    {            
        setSizingCursor(_sizingDirection);
        _app.setPointerWindow( 0);
        _state = &WindowManager::onWindowFrame;
        return true;
    }                                

    // pointer on window content
    _managedWindow->window()->processEvent( toWindow( _managedWindow->window(),  mev) );
    return true;
}


bool WindowManager::onWindowMove(const Pt::Hmi::MouseEvent& mev)
{
    //std::clog << "onWindowMove: " << (_parent ? _parent->title() : "WM") << std::endl;

    if( ! mev.isPressed(_actionButton) )
    {
        _state = isMoving(*_managedWindow, mev) ? &WindowManager::onWindowFrame
                                               : &WindowManager::onBackground;

        return false;
    }
    
    _app.setCursor( &Cursor::moveCursor() );        

    const double dX = mev.x() - _lastPointerPosition.x();
    const double dY = mev.y() - _lastPointerPosition.y();

    //Gfx::PointF from = _managedWindow->position();

    Gfx::PointF to( _managedWindowPosition.x() + dX, 
                    _managedWindowPosition.y() + dY) ;     

    _managedWindowPosition = to;
    onMove(*_managedWindow->window(), to);
    
    return true;
}


bool WindowManager::onWindowResize(const MouseEvent& mev)
{  
    //std::clog << "onWindowResize: " << (_parent ? _parent->title() : "WM") << std::endl;

    if( ! mev.isPressed(_actionButton) )
    {
        _sizingDirection = isSizing(*_managedWindow, mev);

        _state = _sizingDirection == ResizeDirection::None ? &WindowManager::onWindowFrame
                                                           : &WindowManager::onBackground;
        return false;
    }

    setSizingCursor(_sizingDirection);

    Gfx::PointF point( mev.x(), mev.y() );
    
    double width  = _managedWindowSize.width();
    double height = _managedWindowSize.height();
    double posX   = _managedWindowPosition.x();
    double posY   = _managedWindowPosition.y();
    double deltaX = ( point.x() - _lastPointerPosition.x());
    double deltaY = ( point.y() - _lastPointerPosition.y());

    switch( _sizingDirection )
    {
      case  ResizeDirection::North:
        {            
            posY +=  deltaY;
            height -= deltaY;
        }
        break;

      case ResizeDirection::NorthEast:
      {
          posY +=  deltaY;
          height -= deltaY;
          width += deltaX;
      }
      break;

      case ResizeDirection::East:
      {
          width += deltaX;
      }
      break;

      case ResizeDirection::SouthEast:
        {
            height += deltaY;
            width += deltaX;
        }
        break;

      case ResizeDirection::South:
        {
            height += deltaY;
        }
        break;

      case ResizeDirection::SouthWest:
        {
            height += deltaY;
            posX +=  deltaX;
            width -= deltaX;
        }
        break;

      case ResizeDirection::West:
        {
            posX +=  deltaX;
            width -= deltaX;
        }        
        break;

      case ResizeDirection::NorthWest:
      {
          posX +=  deltaX;
          width -= deltaX;
          posY +=  deltaY;
          height -= deltaY;
      }
      break;

      default:
        break;
    }

    Gfx::SizeF size(width, height);
    Gfx::PointF pos(posX, posY);

    if( width < _managedWindow->window()->minimumSize().width() )
        size.setWidth( _managedWindow->window()->minimumSize().width() );

    if( height < _managedWindow->window()->minimumSize().height() )
        size.setHeight( _managedWindow->window()->minimumSize().height() );
                
    if( width > _managedWindow->window()->maximumSize().width() )
        size.setWidth( _managedWindow->window()->maximumSize().width() );

    if( height > _managedWindow->window()->maximumSize().height() )
        size.setHeight( _managedWindow->window()->maximumSize().height() );

    if( _managedWindowPosition != pos )
    {
        _managedWindowPosition = pos;
        onMove(*_managedWindow->window(), pos);
    }

    if( _managedWindowSize != size )
    {
        _managedWindowSize = size;
        onResize(*_managedWindow->window(), size);
    }

    return true;
}


void WindowManager::onResize(Window& w, const Gfx::SizeF& to)
{   
    ResizeEvent rev(w.vid(), to);

    WindowFrame* frame = findWindow(w);
    if(frame)
        frame->resizeEvent(rev);

    Gfx::SizeF from = w.size();
    Application::instance().loop().commitEvent(rev);

    if( ! w.isVisible() )
        return;

    const Gfx::PointF updatePos(-_borderWidth, -(_borderWidth + _titleHeight) );

    Gfx::SizeF updateSize( std::max( to.width(), from.width() ),
                           std::max( to.height(), from.height() ));

    updateSize.addHeight(2 * _borderWidth + _titleHeight);
    updateSize.addWidth(2 * _borderWidth);

    Gfx::RectF updateRect( updatePos, updateSize );
    w.update(updateRect);
}


void WindowManager::onMove(Window& w, const Gfx::PointF& to)
{   
    MoveEvent mev(w.vid(), to);
    
    WindowFrame* frame = findWindow(w);
    if(frame)
        frame->moveEvent(mev);

    Gfx::PointF from = w.position();
    Application::instance().loop().commitEvent(mev);

    if( ! w.parent() )
        return;
            
    Gfx::SizeF size = w.size();    
    size.addWidth( 2 * _borderWidth );
    size.addHeight( 2 * _borderWidth + _titleHeight );

    Gfx::RectF movedRect(to, size);

    Gfx::RectF updateRect(from, size);  
    updateRect.unify(movedRect);
    
    w.parent()->update(updateRect);
}


void WindowManager::onUpdate(Window& child, const Gfx::RectF& rect)
{
    Gfx::PointF updatePos = rect.topLeft() + child.position();
    updatePos.addX( _borderWidth );
    updatePos.addY( _borderWidth + _titleHeight );

    const Gfx::RectF updateRect(updatePos, rect.size());
    _parent->update(updateRect);
}


void WindowManager::onShow( Window& w, bool visible )
{
    Gfx::PointF framePos = w.position() - w.position();
    framePos.subX(_borderWidth);
    framePos.subY(_borderWidth +  _titleHeight);

    Gfx::SizeF frameSize = w.size();
    frameSize.addHeight(2 * _borderWidth + _titleHeight);
    frameSize.addWidth(2 * _borderWidth);

    Gfx::RectF updateRect(framePos, frameSize);
    
    ShowEvent sev( w.vid(), visible );
    Application::instance().loop().commitEvent( sev );
      
    w.parent()->update(updateRect);
}


void WindowManager::onActivate(Window& w)
{
    std::vector<WindowFrame>::iterator it;
    for(it = _windows.begin(); it != _windows.end(); ++it)
    {
        Window* child = it->window();
        if(child->isActive() && child != &w)
        {
            ActivateEvent aev(child->vid(), false);
            it->update();
        }
    }

    for(it = _windows.begin(); it != _windows.end(); ++it)
    {
        if(it->window() == &w)
        {
            WindowFrame wf = *it;
            _windows.erase(it);
            _windows.push_back(wf);
            break;
        }
    }

    _parent->activate();

    ActivateEvent aev( w.vid(), true );
    Application::instance().loop().commitEvent(aev);

    _windows.back().update();
}


void WindowManager::onEnable(Window& w, bool enable)
{
    Gfx::PointF framePos = w.position();
    framePos.subX(_borderWidth);
    framePos.subY(_borderWidth +  _titleHeight);

    Gfx::SizeF frameSize = w.size();
    frameSize.addHeight(2 * _borderWidth + _titleHeight);
    frameSize.addWidth(2 * _borderWidth);

    Gfx::RectF updateRect(framePos, frameSize);

    EnableEvent eev( w.vid(), enable );
    Application::instance().loop().commitEvent( eev );
     
    _parent->update( updateRect);
}


void WindowManager::onClosing(Window& w)
{
    CloseEvent ev( w.vid());
    Application::instance().loop().commitEvent( ev );
}

} // namespace

} // namespace
