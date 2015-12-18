/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 
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
 
 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, write to the Free Software Foundation, 
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/PointerEvent.h>
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

WindowManager::WindowManager()
: _app( Application::instance() )
, _state(&WindowManager::Initial)
, _managedWindow(0)
, _sizingDirection( ResizeDirection::None )
, _borderWidth(4)
, _inactiveColor(0.8f, 0.8f, 0.8f)
, _activeColor(0.6f, 0.6f, 0.8f)
, _textColor(0.0, 0.0, 0.0)
, _actionButton(0)
, _titleBarHeight(20)
{    
}


WindowManager::~WindowManager()
{
}


void WindowManager::add( Window& w )
{   
    if( w.isActive() )
    {
        _children.push_back(&w);
        activate(w);
    }
    else
    {
        _children.insert(_children.begin(), &w);
    }    
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

    if( it != _children.end() )
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


bool WindowManager::pointerInput( const Pt::Hmi::PointerEvent& pev )
{
    bool r = (this->*_state)(pev);
    _lastPointer = pev; 
    return r;
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


void WindowManager::render(PaintSurface& surface)
{        
    Painter& painter = surface.painter();

    for( size_t i = 0; i < _children.size(); ++i )
    {
        Window* w = _children[i];                

        if( ! w->isVisible() )
             continue;        
        
        const Gfx::PointF clientPos = renderFrame(*w, surface);                          
        
        w->render();
        painter.drawSurface( clientPos, w->surface() );
    }
}


Gfx::PointF WindowManager::renderFrame(const Window& w, PaintSurface& surface)
{    
    const Gfx::SizeF clientSize =  w.size();
    const Gfx::SizeF winSize( clientSize.width()  + _borderWidth*2,  
                              clientSize.height() + _borderWidth*2 + _titleBarHeight);    
    Gfx::Color color = w.isActive() ? _activeColor : _inactiveColor;  

    Hmi::Painter& painter = surface.painter();
    
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
    Gfx::SizeF winSize( w.size().width() + _borderWidth*2, w.size().height() + _borderWidth*2 + _titleBarHeight );

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


bool WindowManager::updateActive(const Pt::Hmi::PointerEvent& ev)
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


bool WindowManager::Initial(const Pt::Hmi::PointerEvent& pev)
{
    std::clog << "Initial: " << pev.buttons()[_actionButton].state() << std::endl;

   if( pev.buttons()[_actionButton].state() == DeviceButton::Pressed && 
       _lastPointer.buttons()[_actionButton].state() == DeviceButton::Released)
        updateActive(pev);

    _managedWindow = findWindow( pev.x(), pev.y());

    if( !_managedWindow )
        return false;

    if( isMoving(*_managedWindow, pev) )
    {
        _app.mainScreen().setCursor( &Cursor::moveCursor() );
        _state = &WindowManager::IndicateMove;
        return true;
    }

    _sizingDirection = isSizing(*_managedWindow, pev);

    if( _sizingDirection != ResizeDirection::None )
    {
        setSizingCursor(_sizingDirection);
        _state = &WindowManager::IndicateSizing;
        return true;
    }
    
    Pt::Hmi::PointerEvent childEvent = pev;
    childEvent.setX( pev.x() - _managedWindow->position().x() - _borderWidth ) ;
    childEvent.setY( pev.y() - _managedWindow->position().y() - _titleBarHeight - _borderWidth ) ;  

    _managedWindow->processEvent(childEvent);
    return true;
}


bool WindowManager::IndicateMove(const Pt::Hmi::PointerEvent& pev)
{
    std::clog << "IndicateMove: " << pev.buttons()[_actionButton].state() << std::endl;

    if(pev.buttons()[_actionButton].state() == DeviceButton::Pressed )
    {
        updateActive(pev);
        _state = &WindowManager::MovingWindow;
        return true;
    }

    if( ! isMoving(*_managedWindow, pev) )
    {
        _state = &WindowManager::Initial;
        return false;
    }

    return true;
}


bool WindowManager::MovingWindow(const Pt::Hmi::PointerEvent& pev)
{
    std::clog << "MovingWindow: " << pev.buttons()[_actionButton].state() << std::endl;

    if( pev.buttons()[_actionButton].state() == DeviceButton::Released )
    {
        _state = &WindowManager::IndicateMove;
        return true;
    }

    Gfx::PointF pos( pev.x(), pev.y() );
    if( pos.x() < 0 ) 
        pos.setX(0);
    
    const double dX =  pos.x() - _lastPointer.x();
    const double dY =  pos.y() - _lastPointer.y();

    Gfx::PointF winpos = _managedWindow->position();
    winpos.addX(dX);
    winpos.addY(dY);
    
    if( winpos.y() < 0 ) 
        winpos.setY( 0 );
        
    MoveEvent mev(winpos);
    _managedWindow->processEvent(mev);
     
    return true;
}


bool WindowManager::isMoving(const Window& w, const Pt::Hmi::PointerEvent& ev)
{            
    const Gfx::PointF& position = w.position();
     
    if( (ev.x() < (position.x() + _borderWidth*2 + w.size().width()) && 
         ev.x() >= position.x()) && 
        (ev.x() >= (position.x() + _borderWidth)) &&
        (ev.y() < (position.y() + _titleBarHeight) ) && 
        ((ev.y()+ position.y()) >= _borderWidth ) &&
        (ev.y() >= (position.y() + _borderWidth)))           
        return true;

    return false;
}



bool WindowManager::IndicateSizing(const Pt::Hmi::PointerEvent& pev)
{
    std::clog << "IndicateSizing: " << pev.buttons()[_actionButton].state() << std::endl;

    if(pev.buttons()[_actionButton].state() == DeviceButton::Pressed )
    {
        updateActive(pev);
        _state = &WindowManager::SizingWindow;
        return true;
    }

    ResizeDirection::Type sizingDirection = isSizing(*_managedWindow, pev);

    if( sizingDirection == ResizeDirection::None )
    {
        _state = &WindowManager::Initial;
        return false;
    }

    return true; 
}


ResizeDirection::Type WindowManager::isSizing( const Window& w, const Pt::Hmi::PointerEvent& ev )
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


bool WindowManager::SizingWindow(const PointerEvent& ev )
{  
    std::clog << "SizingWindow: " << ev.buttons()[_actionButton].state() << std::endl;

    if( ev.buttons()[_actionButton].state() == DeviceButton::Released )
    {
        _state = &WindowManager::IndicateSizing;
        return true;
    }

    const std::vector<DeviceButton>& button = ev.buttons();

    Gfx::PointF point( ev.x(), ev.y() );
    
    double width  = _managedWindow->size().width();
    double height = _managedWindow->size().height();
    double posX   = _managedWindow->position().x();
    double posY   = _managedWindow->position().y();
    double deltaX = ( point.x() - _lastPointer.x());
    double deltaY = ( point.y() - _lastPointer.y());

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
      {

      }
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

}} // namespace
