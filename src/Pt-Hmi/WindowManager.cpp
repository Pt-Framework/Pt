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

WindowManager::WindowManager(Window* parent)
: _app( Application::instance() )
, _state(&WindowManager::onBackground)
, _managedWindow(0)
, _sizingDirection( ResizeDirection::None )
, _borderWidth(4)
, _inactiveColor(0.68f, 0.70f, 0.75f)
, _activeColor(0.4f, 0.5f, 0.8f)
, _textColor(0.0, 0.0, 0.0)
, _actionButton(0)
, _titleBarHeight(20)
, _parent(parent)
{    
}


WindowManager::~WindowManager()
{
}


void WindowManager::init(Window& parent)
{
    _parent = &parent;
}


void WindowManager::add( Window& w )
{   
    _children.insert(_children.begin(), &w);   
}


void WindowManager::remove( Window& w )
{
    std::vector<Window*>::iterator it = std::find( _children.begin(), _children.end(), &w );

    if( it == _children.end() )
        return;

    _children.erase( it );            
}


const std::vector<Window*>& WindowManager::windows() const
{
    return _children;
}


void WindowManager::activate(Window& w)
{
    deactivate();

    std::vector<Window*>::iterator it = std::find(_children.begin(), _children.end(), &w);

    if( it == _children.end() )
        return;

    _children.erase(it);
        
    _children.push_back(&w);    

    w.processEvent( ActivateEvent(true) );    
}


void WindowManager::deactivate()
{
    std::vector<Window*>::iterator it = _children.begin();
    
    for( ; it != _children.end(); ++it)
    {
        Window* w = *it;

        if( w->isActive() )
        {
            w->processEvent( ActivateEvent(false) );                
            break;
        }
    }        
}


bool WindowManager::keyInput( const Pt::Hmi::KeyEvent& keyEvent )
{
  Window* w = activeWindow(*this);

  if( w == 0 )
    return false;

    if( w->isEnabled() )
            w->processEvent( keyEvent );        

   return true;
}


void WindowManager::render(PaintSurface& surface, const Gfx::RectF& rect)
{        
    Painter painter(surface);

    for( size_t i = 0; i < _children.size(); ++i )
    {
        Window* w = _children[i];                

        if( ! w->isVisible() )
             continue;        
        
        Gfx::PointF clientPos = renderFrame(*w, surface);                          
        
        Gfx::PointF pos = rect.topLeft() - w->position();
        Gfx::RectF updateRect(pos, rect.size());

        w->render(updateRect);
        painter.drawSurface( clientPos, w->surface() );
    }
}


Gfx::PointF WindowManager::renderFrame(const Window& w, PaintSurface& surface)
{    
    const Gfx::SizeF clientSize =  w.size();
    const Gfx::SizeF winSize( clientSize.width()  + _borderWidth*2,  
                              clientSize.height() + _borderWidth*2 + _titleBarHeight);    
    Gfx::Color color = w.isActive() ? _activeColor : _inactiveColor;  

    Painter painter(surface);
    
    Gfx::PointF pos( w.position().x(), w.position().y() );
    
    switch( w.border() )
    {
        case WindowBorder::Dialog:
        case WindowBorder::DialogSizeable:
        case WindowBorder::Fixed:
        case WindowBorder::Sizeable:
        case WindowBorder::Tool:
        case WindowBorder::ToolSizeable:
        {            
            Gfx::Brush brush(color);
            painter.setBrush(brush);

            Gfx::Pen pen(1, _textColor);
            painter.setPen(pen);
                    
            Gfx::RectF leftBorder( pos.x(), 
                                    pos.x() + _borderWidth,
                                    pos.y() + _borderWidth, 
                                    pos.y() + winSize.height() - _borderWidth - 1 );
            painter.fillRect(leftBorder);

            Gfx::RectF topBorder(pos.x(),
                                    pos.x() + winSize.width() - 1,
                                    pos.y(),
                                    pos.y() + _borderWidth);
            painter.fillRect(topBorder);

            Gfx::RectF rightBorder(pos.x() + winSize.width() - _borderWidth,
                                    pos.x() + winSize.width() - 1,
                                    pos.y() + _borderWidth,
                                    pos.y() + winSize.height() - _borderWidth - 1 );
            painter.fillRect(rightBorder);

            Gfx::RectF bottomBorder(pos.x(),
                                    pos.x() + winSize.width() - 1,
                                    pos.y() + winSize.height() - _borderWidth,
                                    pos.y() + winSize.height() - 1);
            painter.fillRect(bottomBorder);

            Gfx::RectF titleArea( pos.x() + _borderWidth,
                                    pos.x() + winSize.width() - _borderWidth - 1,
                                    pos.y() + _borderWidth,
                                    pos.y() + _borderWidth + _titleBarHeight - 1);
            painter.fillRect(titleArea);

            const Gfx::Font& font = w.font();
                    
            Gfx::FontMetrics fm = painter.fontMetrics(font, Pt::String("A") );
            double textMargin = (_titleBarHeight - fm.height()) / 2;
            Gfx::PointF textPos(pos.x() + _borderWidth + _titleBarHeight, pos.y() + _titleBarHeight - textMargin);

            painter.setFont(font);
            painter.drawText(textPos, Pt::String( w.title().c_str()) );
        }

        break;
    }

  return Gfx::PointF( pos.x() + _borderWidth, pos.y() + _borderWidth + _titleBarHeight) ;     
}


Window* WindowManager::activeWindow( WindowManager& manager )
{     
  for( size_t i = 0; i < manager.windows().size(); ++i )
  {
    Window* child = manager.windows()[i];
    
    if( child->isActive() )
        return child;

    Window* focused = activeWindow( child->windowManager() );

    if( focused != 0 )
      return focused;
  }

  return 0;
}


bool WindowManager::contains(const Window& w, double x, double y)
{  
    Gfx::SizeF winSize(w.size().width() + _borderWidth*2, 
                       w.size().height() + _borderWidth*2 + _titleBarHeight);

    if( x >= w.position().x() && x <  w.position().x() + winSize.width() &&
        y >= w.position().y() && y <  w.position().y() + winSize.height() )
        return true;

    return false;
}


Window* WindowManager::findWindow(double x, double y)
{
    for( int i = _children.size() - 1;  i > -1; --i )
    {
        Window* w = _children[i];

        if( ! w->isVisible() )
            continue;

        if( ! contains(*w, x, y) )
            continue;

        return w;
    }

    return 0;
}


bool WindowManager::updateActive(const Pt::Hmi::MouseEvent& ev)
{    
    std::vector<Window*>::reverse_iterator rit =  _children.rbegin();

    for( ; rit != _children.rend(); ++rit )
    {
        Window* w = *rit;

        if( ! contains( *w, ev.x(), ev.y() ) )
            continue;

        if( w->isActive() )                                                             
            return true;
             
        activate( *w );    
        return true;
    }         

    deactivate();
    return false;
}


bool WindowManager::isMoving(const Window& w, const Pt::Hmi::MouseEvent& ev)
{            
    const Gfx::PointF& position = w.position();
     
    if( (ev.x() < (position.x()  + _borderWidth + w.size().width() ) ) &&                  
        (ev.x() >= (position.x() + _borderWidth)) &&
        (ev.y() < (position.y()  + _titleBarHeight + _borderWidth) ) && 
        (ev.y() >= (position.y() + _borderWidth) ))
    {      
        return true;
    }

    return false;
}


ResizeDirection::Type WindowManager::isSizing(const Window& w, const Pt::Hmi::MouseEvent& ev)
{    
    const Gfx::SizeF  wsize = w.size();
    const Gfx::PointF wpos  = w.position();
    double titleHeight = _titleBarHeight;

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
    Screen& screen = _app.mainScreen();

    switch( type )
    {
        case ResizeDirection::East:
        case ResizeDirection::West:
            screen.setCursor( &Hmi::Cursor::sizeWECursor() );
        break;

        case ResizeDirection::NorthEast:
        case ResizeDirection::SouthWest:
            screen.setCursor( &Hmi::Cursor::sizeNESWCursor() );
        break;

        case ResizeDirection::North:        
        case ResizeDirection::South:
            screen.setCursor( &Hmi::Cursor::sizeNSCursor() );
        break;
        
        case ResizeDirection::NorthWest:
        case ResizeDirection::SouthEast:
            screen.setCursor( &Hmi::Cursor::sizeNWSECursor() );
        break;        
    }
}


MouseEvent WindowManager::toWindow(Window* w, const MouseEvent& mev)
{
    Pt::Hmi::MouseEvent childEvent = mev;

    double childX = mev.x() - w->position().x() - _borderWidth;
    double childY = mev.y() - w->position().y() - _titleBarHeight - _borderWidth;

    childEvent.setX(childX);
    childEvent.setY(childY);
    return childEvent;
}


bool WindowManager::pointerInput( const Pt::Hmi::MouseEvent& mev )
{
    if( mev.isPress(_actionButton) )
        updateActive(mev);    

    bool r = (this->*_state)(mev);
    _lastPointerPosition = Gfx::PointF( mev.x(), mev.y() );    
    return r;
}


bool WindowManager::onBackground(const Pt::Hmi::MouseEvent& mev)
{
    //std::clog << "onBackground: " << _parent->title() << std::endl;    
    _managedWindow = findWindow( mev.x(), mev.y() );
    
    // pointer on window background 
    if( ! _managedWindow )
    {
        _app.mainScreen().setPointerWindow(_parent);
        _state = &WindowManager::onBackground;        
        return false;
    }    

    // pointer on window title bar
    if( isMoving(*_managedWindow, mev) )
    {
        _app.mainScreen().setCursor( &Cursor::moveCursor() );
        _app.mainScreen().setPointerWindow( 0 );
        _state = &WindowManager::onWindowFrame;
        return true;
    }

    // pointer on window border
    _sizingDirection = isSizing(*_managedWindow, mev);

    if( _sizingDirection != ResizeDirection::None )
    {
        setSizingCursor(_sizingDirection);
        _app.mainScreen().setPointerWindow( 0);
        _state = &WindowManager::onWindowFrame;
        return true;
    }                                
    
    // pointer on window content
    _state = &WindowManager::onWindowContent;    
    _app.mainScreen().setPointerWindow( _managedWindow);
    _managedWindow->processEvent( toWindow(_managedWindow, mev) );
    return true;
}


bool WindowManager::onWindowFrame(const Pt::Hmi::MouseEvent& mev)
{
    //std::clog << "onWindowFrame: " << _parent->title() << std::endl;   
    _managedWindow = findWindow( mev.x(), mev.y() );

    // pointer on window background 
    if( ! _managedWindow)
    {
        _app.mainScreen().setPointerWindow(_parent);
        _state = &WindowManager::onBackground;
        return false;
    }

    // pointer on window title bar
    if( isMoving(*_managedWindow, mev) )
    {
        _app.mainScreen().setCursor( &Cursor::moveCursor() );
        _app.mainScreen().setPointerWindow( 0);

        if( mev.isPress(_actionButton) )
            _state = &WindowManager::onWindowMove;
        else
            _state = &WindowManager::onWindowFrame;

        return true;
    }

    // pointer on window border
    _sizingDirection = isSizing(*_managedWindow, mev);
    if( _sizingDirection != ResizeDirection::None )
    {                      
        setSizingCursor(_sizingDirection);
        _app.mainScreen().setPointerWindow( 0);

        if( mev.isPress(_actionButton) )
            _state = &WindowManager::onWindowResize;
        else
            _state = &WindowManager::onWindowFrame;
        
        return true;
    }   

    // pointer on window content
    _state = &WindowManager::onWindowContent;
    _app.mainScreen().setPointerWindow(_managedWindow);
    _managedWindow->processEvent( toWindow(_managedWindow, mev) );
    return true;
}


bool WindowManager::onWindowContent(const Pt::Hmi::MouseEvent& mev)
{    
    //std::clog << "onWindowContent: " << _parent->title() << std::endl;    
    _managedWindow = findWindow( mev.x(), mev.y() );
    
    // pointer on window background
    if( ! _managedWindow )
    {        
        _app.mainScreen().setPointerWindow( _parent);
        _state = &WindowManager::onBackground;
        return false;
    }        

    // pointer on window title bar
    if( isMoving(*_managedWindow, mev) )
    {
        _app.mainScreen().setCursor( &Cursor::moveCursor() );
        _app.mainScreen().setPointerWindow( 0);
        _state = &WindowManager::onWindowFrame;
        return true;
    }

    // pointer on window border
    _sizingDirection = isSizing(*_managedWindow, mev);

    if( _sizingDirection != ResizeDirection::None )
    {            
        setSizingCursor(_sizingDirection);
        _app.mainScreen().setPointerWindow( 0);
        _state = &WindowManager::onWindowFrame;
        return true;
    }                                

    // pointer on window content
    _managedWindow->processEvent( toWindow( _managedWindow,  mev) );
    return true;
}


bool WindowManager::onWindowMove(const Pt::Hmi::MouseEvent& mev)
{
    //std::clog << "onWindowMove: " << _parent->title() << std::endl;

    if( ! mev.isPressed(_actionButton) )
    {
        _state = isMoving(*_managedWindow, mev) ? &WindowManager::onWindowFrame
                                               : &WindowManager::onBackground;

        return false;
    }
    
    _app.mainScreen().setCursor( &Cursor::moveCursor() );

    double dX = mev.x() - _lastPointerPosition.x();
    double dY = mev.y() - _lastPointerPosition.y();

    Gfx::PointF winpos = _managedWindow->position();
    winpos.addX(dX);
    winpos.addY(dY);
        
    MoveEvent ev(winpos);
    _managedWindow->processEvent(ev);
     
    return true;
}


bool WindowManager::onWindowResize(const MouseEvent& mev)
{  
    //std::clog << "onWindowResize: " << _parent->title() << std::endl;

    if( ! mev.isPressed(_actionButton) )
    {
        _sizingDirection = isSizing(*_managedWindow, mev);

        _state = _sizingDirection == ResizeDirection::None ? &WindowManager::onWindowFrame
                                                           : &WindowManager::onBackground;
        return false;
    }

    setSizingCursor(_sizingDirection);

    Gfx::PointF point( mev.x(), mev.y() );
    
    double width  = _managedWindow->size().width();
    double height = _managedWindow->size().height();
    double posX   = _managedWindow->position().x();
    double posY   = _managedWindow->position().y();
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

    if( width < _managedWindow->minimumSize().width() )
        size.setWidth( _managedWindow->minimumSize().width() );

    if( height < _managedWindow->minimumSize().height() )
        size.setHeight( _managedWindow->minimumSize().height() );
                
    if( width > _managedWindow->maximumSize().width() )
        size.setWidth( _managedWindow->maximumSize().width() );

    if( height > _managedWindow->maximumSize().height() )
        size.setHeight( _managedWindow->maximumSize().height() );

    if( _managedWindow->position() != pos )
    {
        MoveEvent mev(pos);
        _managedWindow->processEvent(mev);    
    }

    if( _managedWindow->size() != size )
    {
        ResizeEvent rev(size);
        _managedWindow->processEvent(rev);
    }

    return true;
}

} // namespace

} // namespace
