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
#include "WindowFrame.h"

namespace Pt {

namespace Hmi {


void setResizeCursor(Pt::uint8_t dir )
{
    Application&  app =  Application::instance();

    if( (dir & WindowFrame::North && dir & WindowFrame::East) ||
        (dir & WindowFrame::South && dir & WindowFrame::West ) )
    {
        app.setCursor( &Hmi::Cursor::sizeNESWCursor() );
        return;
    }

    if( (dir & WindowFrame::North && dir & WindowFrame::West) ||
        (dir & WindowFrame::South && dir & WindowFrame::East ) )
    {
        app.setCursor( &Hmi::Cursor::sizeNWSECursor() );
        return;
    }

    if( (dir & WindowFrame::East || dir & WindowFrame::West))
    {
        app.setCursor( &Hmi::Cursor::sizeWECursor() );
        return;
    }

    if( (dir & WindowFrame::North || dir & WindowFrame::South))
    {
        app.setCursor( &Hmi::Cursor::sizeNSCursor() );     
    }
}


/////////////////////////////////////////////////////////////////////////////
// WindowManager
/////////////////////////////////////////////////////////////////////////////

WindowManager::WindowManager()
: _app( Application::instance() )
, _state(&WindowManager::onBackground)
, _managedWindow(0)
, _inactiveColor(0.68f, 0.70f, 0.75f)
, _activeColor(0.4f, 0.5f, 0.8f)
, _textColor(0.0, 0.0, 0.0)
, _parent(0)
, _borderWidth(4)
, _titleHeight(20)
, _resizeDirection(0)
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
    if( mev.isPress() )
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

    Gfx::PointF framePos = mev.position() - _managedWindow->frameRect().topLeft();

    // pointer on window title bar
    if( _managedWindow->isTitle(framePos) )
    {
        _app.setCursor( &Cursor::moveCursor() );
        _app.setPointerWindow( 0 );
        _state = &WindowManager::onWindowFrame;
        return true;
    }

    // pointer on window border
    Pt::uint8_t resizeDirection = _managedWindow->isResize(mev.position());

    if( resizeDirection != WindowFrame::None )
    {
        setResizeCursor(resizeDirection);
        _app.setPointerWindow( 0);
        _state = &WindowManager::onWindowFrame;
        return true;
    }                                
    
    // pointer on window content
    _state = &WindowManager::onWindowContent;    
    _app.setPointerWindow( _managedWindow->window() );

    // mouse event in managed window client coordinates
    Gfx::PointF pos = mev.position() - _managedWindow->clientRect().topLeft();
    MouseEvent mev2 = mev;
    mev2.setPosition(pos);
    _managedWindow->window()->processEvent(mev2);
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

    Gfx::PointF framePos = mev.position() - _managedWindow->frameRect().topLeft();

    // pointer on window title bar
    if( _managedWindow->isTitle(framePos) )
    {
        _app.setCursor( &Cursor::moveCursor() );
        _app.setPointerWindow(0);
        _state = &WindowManager::onWindowFrame;

        if( mev.isPress() )
        {
            _managedWindowPosition = _managedWindow->window()->position();
            _state = &WindowManager::onWindowMove;
        }

        return true;
    }

    // pointer on window border
    Pt::uint8_t resizeDirection = _managedWindow->isResize(mev.position());
    if( resizeDirection != WindowFrame::None )
    {                      
        setResizeCursor(resizeDirection);
        _app.setPointerWindow( 0);

        if( mev.isPress() )
        {
            _resizeDirection = resizeDirection;
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
    _app.setPointerWindow( _managedWindow->window() );

    // mouse event in managed window client coordinates
    Gfx::PointF pos = mev.position() - _managedWindow->clientRect().topLeft();
    MouseEvent mev2 = mev;
    mev2.setPosition(pos);
    _managedWindow->window()->processEvent(mev2);
    return true;
}


bool WindowManager::onWindowContent(const Pt::Hmi::MouseEvent& mev)
{    
    //std::clog << "onWindowContent: " << (_parent ? _parent->title() : "WM") << std::endl;    
    
    _managedWindow = findWindow( mev.position() );
    
    // pointer on window background
    if( ! _managedWindow )
    {        
        _app.setPointerWindow(_parent);
        _state = &WindowManager::onBackground;
        return false;
    }        

    Gfx::PointF framePos = mev.position() - _managedWindow->frameRect().topLeft();

    // pointer on window title bar
    if( _managedWindow->isTitle(framePos) )
    {
        _app.setCursor( &Cursor::moveCursor() );
        _app.setPointerWindow(0);
        _state = &WindowManager::onWindowFrame;
        return true;
    }

    // pointer on window border
    Pt::uint8_t resizeDirection = _managedWindow->isResize(mev.position());

    if( resizeDirection != WindowFrame::None )
    {            
        setResizeCursor(resizeDirection);
        _app.setPointerWindow( 0);
        _state = &WindowManager::onWindowFrame;
        return true;
    }                                

    // pointer on window content

    // mouse event in managed window client coordinates
    Gfx::PointF pos = mev.position() - _managedWindow->clientRect().topLeft();
    MouseEvent mev2 = mev;
    mev2.setPosition(pos);
    _managedWindow->window()->processEvent(mev2);
    return true;
}


bool WindowManager::onWindowMove(const Pt::Hmi::MouseEvent& mev)
{
    //std::clog << "onWindowMove: " << (_parent ? _parent->title() : "WM") << std::endl;

    Gfx::PointF framePos = mev.position() - _managedWindow->frameRect().topLeft();

    if( ! mev.isPressed() )
    {
        _state = _managedWindow->isTitle(framePos) ? &WindowManager::onWindowFrame
                                                   : &WindowManager::onBackground;

        return false;
    }
    
    _app.setCursor( &Cursor::moveCursor() );        
    _managedWindowPosition += mev.position() - _lastPointerPosition;
    
    onMove(*_managedWindow->window(), _managedWindowPosition);
    return true;
}


bool WindowManager::onWindowResize(const MouseEvent& mev)
{  
    //std::clog << "onWindowResize: " << (_parent ? _parent->title() : "WM") << std::endl;
   
    if( ! mev.isPressed() )
    {
        Pt::uint8_t resizeDirection = _managedWindow->isResize(mev.position());
        _state = resizeDirection == WindowFrame::None ? &WindowManager::onWindowFrame
                                              : &WindowManager::onBackground;
        return false;
    }

    
    double width  = _managedWindowSize.width();
    double height = _managedWindowSize.height();
    double posX   = _managedWindowPosition.x();
    double posY   = _managedWindowPosition.y();
    double deltaX = ( mev.x() - _lastPointerPosition.x());
    double deltaY = ( mev.y() - _lastPointerPosition.y());

    // TODO: NorthEast is North + East

    if( _resizeDirection & WindowFrame::North )
    {
        posY +=  deltaY;
        height -= deltaY;
    }

    if( _resizeDirection & WindowFrame::East )
    {
        width += deltaX;
    }

    if( _resizeDirection & WindowFrame::South )
    {
         height += deltaY;
    }    

    if( _resizeDirection & WindowFrame::West )
    {
        posX +=  deltaX;
        width -= deltaX;
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
