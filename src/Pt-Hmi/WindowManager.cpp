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


void WindowFrame::moveEvent(const MoveEvent& mev)
{
    _position = mev.position();

    onLayout();
}


void WindowFrame::resizeEvent(const ResizeEvent& rev)
{
    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();

    _windowSize = rev.size();
    
    _frameSize = rev.size();
    _frameSize.addWidth(2 * borderWidth);
    _frameSize.addHeight(2 * borderWidth);
    _frameSize.addHeight(titleHeight);
    
    onLayout();
}


void WindowFrame::onLayout()
{
    double borderWidth = _wm->borderWidth();
    double titleHeight = _wm->titleHeight();
    double buttonWidth = titleHeight - borderWidth;

    double buttonX = _position.x() + _frameSize.width() - (borderWidth + buttonWidth);
    double buttonY = _position.y() + borderWidth;
    
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

    //return Gfx::PointF( pos.x() + borderWidth, pos.y() + borderWidth + titleHeight) ;
}

/////////////////////////////////////////////////////////////////////////////
// WindowManager
/////////////////////////////////////////////////////////////////////////////

WindowManager::WindowManager()
: _app( Application::instance() )
, _state(&WindowManager::onBackground)
, _managedWindow(0)
, _sizingDirection( ResizeDirection::None )
, _inactiveColor(0.68f, 0.70f, 0.75f)
, _activeColor(0.4f, 0.5f, 0.8f)
, _textColor(0.0, 0.0, 0.0)
, _actionButton(0)
, _container(0)
, _borderWidth(4)
, _titleHeight(20)
{    
}


WindowManager::~WindowManager()
{
}


void WindowManager::init(Window& parent)
{
    _container = &parent;
}


void WindowManager::add(Window& w)
{   
    _children.insert(_children.begin(), &w);
    _frames[&w] = WindowFrame(*this, w);
}


void WindowManager::remove(Window& w)
{
    std::vector<Window*>::iterator it = std::find(_children.begin(), _children.end(), &w);
    if( it == _children.end() )
        return;

    _children.erase( it );
    _frames.erase(&w);   
    
    Gfx::PointF framePos = w.position();
    Gfx::SizeF frameSize = w.size();
    frameSize.addHeight(2 * _borderWidth + _titleHeight);
    frameSize.addWidth(2 * _borderWidth);

    Gfx::RectF updateRect(framePos, frameSize);
    _container->update( updateRect);        
}


const std::vector<Window*>& WindowManager::windows() const
{
    return _children;
}


std::vector<Window*>& WindowManager::windows()
{
    return _children;
}


PaintSurface& WindowManager::surface()
{ 
    return _container->surface(); 
}


void WindowManager::activate(Window& w)
{
    std::vector<Window*>::iterator it = std::find(_children.begin(), _children.end(), &w);
    if( it == _children.end() )
        return;

    _children.erase(it);
    _children.push_back(&w);
}


bool WindowManager::updateActive(const Pt::Hmi::MouseEvent& ev)
{    
    std::vector<Window*>::reverse_iterator rit =  _children.rbegin();

    for( ; rit != _children.rend(); ++rit )
    {
        Window* w = *rit;

        if( ! contains( *w, ev.x(), ev.y() ) )
            continue;
             
        w->activate();    
        return true;
    }         

    return false;
}


bool WindowManager::keyInput(const Pt::Hmi::KeyEvent& keyEvent)
{
    Window* w = _container->activeWindow();

    if( w == 0 )
        return false;

    if( w->isEnabled() )
        w->processEvent(keyEvent);

    return true;
}


bool WindowManager::pointerInput( const Pt::Hmi::MouseEvent& mev )
{
    if( mev.isPress(_actionButton) )
        updateActive(mev);    

    bool r = (this->*_state)(mev);
    _lastPointerPosition = Gfx::PointF( mev.x(), mev.y() );    
    return r;
}


void WindowManager::paint(PaintSurface& surface, const Gfx::RectF& rect)
{  
    for( size_t i = 0; i < _children.size(); ++i )
    {
        Window* w = _children[i];                

        if( ! w->isVisible() )
            continue; 
        
        const Gfx::PointF clientPos = renderFrame(*w, surface, rect);                               
        
        const Gfx::PointF pos = rect.topLeft() - clientPos;
        const Gfx::RectF  updateRect(pos, rect.size());        
        
        Gfx::RectF surfaceRect(Gfx::PointF(0, 0), w->size());
        surfaceRect = surfaceRect.intersect(updateRect);

        Painter painter(surface);
        painter.drawSurface(clientPos + surfaceRect.topLeft(),  w->surface(), surfaceRect);    
    }
}


Gfx::PointF WindowManager::renderFrame(Window& w, PaintSurface& surface, 
                                       const Gfx::RectF& rect)
{    
    std::map<Window*, WindowFrame>::iterator it = _frames.find(&w);
    if(it != _frames.end() )
    {
        PaintEvent pev(w.vid(), rect);
        it->second.paintEvent(pev);
    }
    
    Gfx::PointF pos = w.position();
    return Gfx::PointF( pos.x() + _borderWidth, pos.y() + _borderWidth + _titleHeight) ;     
}


bool WindowManager::contains(const Window& w, double x, double y)
{  
    Gfx::SizeF winSize(w.size().width() + _borderWidth*2, 
                       w.size().height() + _borderWidth*2 + _titleHeight);

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


bool WindowManager::isMoving(const Window& w, const Pt::Hmi::MouseEvent& ev)
{            
    const Gfx::PointF& position = w.position();
     
    if( (ev.x() < (position.x()  + _borderWidth + w.size().width() ) ) &&                  
        (ev.x() >= (position.x() + _borderWidth)) &&
        (ev.y() < (position.y()  + _titleHeight + _borderWidth) ) && 
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
    //std::clog << "onBackground: " << (_container ? _container->title() : "WM") << std::endl;    
    _managedWindow = findWindow( mev.x(), mev.y() );
    
    // pointer on window background 
    if( ! _managedWindow )
    {
        _app.setPointerWindow(_container);
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
    _app.setPointerWindow( _managedWindow);
    _managedWindow->processEvent( toWindow(_managedWindow, mev) );
    return true;
}


bool WindowManager::onWindowFrame(const Pt::Hmi::MouseEvent& mev)
{
    //std::clog << "onWindowFrame: " << (_container ? _container->title() : "WM") << std::endl;   
        
    _managedWindow = findWindow( mev.x(), mev.y() );

    // pointer on window background 
    if( ! _managedWindow)
    {
        _app.setPointerWindow(_container);
        _state = &WindowManager::onBackground;
        return false;
    }
    
    std::map<Window*, WindowFrame>::iterator frame = _frames.find(_managedWindow);
    if( frame == _frames.end() )
        return false;

    if( frame->second.mouseEvent(mev) )
        return true;

    // pointer on window title bar
    if( isMoving(*_managedWindow, mev) )
    {
        _app.setCursor( &Cursor::moveCursor() );
        _app.setPointerWindow( 0);

        if( mev.isPress(_actionButton) )
        {
            _state = &WindowManager::onWindowMove;
            _managedWindowPosition = _managedWindow->position();
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
            _managedWindowPosition = _managedWindow->position();
            _managedWindowSize = _managedWindow->size();
        }
        else
            _state = &WindowManager::onWindowFrame;
        
        return true;
    }   

    // pointer on window content
    _state = &WindowManager::onWindowContent;
    _app.setPointerWindow(_managedWindow);
    _managedWindow->processEvent( toWindow(_managedWindow, mev) );
    return true;
}


bool WindowManager::onWindowContent(const Pt::Hmi::MouseEvent& mev)
{    
    //std::clog << "onWindowContent: " << (_container ? _container->title() : "WM") << std::endl;    
    
    _managedWindow = findWindow( mev.x(), mev.y() );
    
    // pointer on window background
    if( ! _managedWindow )
    {        
        _app.setPointerWindow( _container);
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
    _managedWindow->processEvent( toWindow( _managedWindow,  mev) );
    return true;
}


bool WindowManager::onWindowMove(const Pt::Hmi::MouseEvent& mev)
{
    //std::clog << "onWindowMove: " << (_container ? _container->title() : "WM") << std::endl;

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
    onMove(*_managedWindow, to);
    
    return true;
}


bool WindowManager::onWindowResize(const MouseEvent& mev)
{  
    //std::clog << "onWindowResize: " << (_container ? _container->title() : "WM") << std::endl;

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

    if( width < _managedWindow->minimumSize().width() )
        size.setWidth( _managedWindow->minimumSize().width() );

    if( height < _managedWindow->minimumSize().height() )
        size.setHeight( _managedWindow->minimumSize().height() );
                
    if( width > _managedWindow->maximumSize().width() )
        size.setWidth( _managedWindow->maximumSize().width() );

    if( height > _managedWindow->maximumSize().height() )
        size.setHeight( _managedWindow->maximumSize().height() );

    if( _managedWindowPosition != pos )
    {
        _managedWindowPosition = pos;
        onMove(*_managedWindow, pos);
    }

    if( _managedWindowSize != size )
    {
        _managedWindowSize = size;
        onResize(*_managedWindow, size);
    }

    return true;
}


void WindowManager::onResize(Window& w, const Gfx::SizeF& to)
{   
    ResizeEvent rev(w.vid(), to);

    std::map<Window*, WindowFrame>::iterator it = _frames.find(&w);
    if(it == _frames.end() )
      return;

    it->second.resizeEvent(rev);

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
    
    std::map<Window*, WindowFrame>::iterator it = _frames.find(&w);
    if(it == _frames.end() )
      return;

    it->second.moveEvent(mev);

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
    _container->update(updateRect);
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


void WindowManager::updateAll(Window& w)
{
    Gfx::PointF framePos(0, 0);
    framePos.subX( _borderWidth );
    framePos.subY( _borderWidth +  _titleHeight );

    Gfx::SizeF frameSize = w.size();
    frameSize.addHeight(2 * _borderWidth + _titleHeight);
    frameSize.addWidth(2 * _borderWidth);

    Gfx::RectF updateRect(framePos, frameSize);

    w.update(updateRect);
}


void WindowManager::onActivate(Window& w)
{
    std::vector<Window*>::const_iterator it;

    for(it = _children.begin(); it != _children.end(); ++it)
    {
        if((*it)->isActive() && *it != &w)
        {
            ActivateEvent aev( (*it)->vid(), false );
            updateAll(**it);
        }
    }

    activate(w);
    _container->activate();

    ActivateEvent aev( w.vid(), true );
    Application::instance().loop().commitEvent(aev);

    updateAll(w);
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
     
    _container->update( updateRect);
}


void WindowManager::onClosing(Window& w)
{
    CloseEvent ev( w.vid());
    Application::instance().loop().commitEvent( ev );
}

} // namespace

} // namespace
