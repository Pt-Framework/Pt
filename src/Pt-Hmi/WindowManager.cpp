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

WindowManager::WindowManager()
: _app( Application::instance() )
, _parent(0)
, _activeWindow(0)
, _currentWindow(0)
, _grabbedWindow(0)
, _borderWidth(4.0f)
, _titleHeight(20.0f)
, _inactiveColor(0.68f, 0.70f, 0.75f)
, _activeColor(0.4f, 0.5f, 0.8f)
, _textColor(1.0f, 1.0f, 1.0f)
, _inactiveTextColor(0.2f, 0.2f, 0.2f)
{    
}


WindowManager::~WindowManager()
{
    while( ! _windows.empty() )
    {
        delete _windows.front();
        _windows.erase( _windows.begin() );
    }
}


void WindowManager::init(WindowBase& parent)
{
    _parent = &parent;
}


void WindowManager::add(Window& w)
{    
    _windows.insert( _windows.begin(), new WindowFrame(*this, w) );
}


void WindowManager::remove(Window& w)
{
    std::vector<WindowFrame*>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {
        if((*wit)->window() == &w)
        {
            if(_currentWindow && _currentWindow->window() == &w)
                _currentWindow = 0;

            if(_grabbedWindow && _grabbedWindow->window() == &w)
                _grabbedWindow = 0;

            if(_activeWindow && _activeWindow->window() == &w)
                _activeWindow  = 0;

            delete *wit;
            _windows.erase(wit);
            break;
        }
    }
}


WindowFrame* WindowManager::findWindow(const Gfx::PointF& p)
{
    std::vector<WindowFrame*>::reverse_iterator rit;
    for(rit =  _windows.rbegin() ; rit != _windows.rend(); ++rit )
    {
        if( ! (*rit)->window()->isVisible() )
            continue;

        if( ! (*rit)->frameRect().contains(p) )
            continue;

        return *rit;
    }

    return 0;
}


WindowFrame* WindowManager::findWindow(Window& w)
{
    std::vector<WindowFrame*>::iterator it;
    for(it = _windows.begin(); it != _windows.end(); ++it)
    {
        if( (*it)->window() == &w)
            return *it;
    }

    return 0;
}


WindowBase* WindowManager::parent()
{
    return _parent;
}


void WindowManager::enterEvent(const EnterEvent& ev)
{
}


void WindowManager::leaveEvent(const LeaveEvent& ev)
{
    _grabbedWindow = 0;

    if(_currentWindow)
    {
      WindowFrame* w = _currentWindow;
      _currentWindow = 0;
      
      LeaveEvent lev( w->window()->vid() );
      w->leaveEvent(lev);
    }
}


bool WindowManager::keyEvent(const KeyEvent& keyEvent)
{
    Window* w = 0;

    if( _activeWindow)
        w = _activeWindow->window();

    if( ! w )
        return false;

    if( w->isEnabled() )
        w->processEvent(keyEvent);

    return true;
}


bool WindowManager::mouseEvent( const MouseEvent& mev )
{
    WindowFrame* windowFrame = 0;

    if(_grabbedWindow)
        windowFrame = _grabbedWindow;
    else
        windowFrame = findWindow( mev.position() );

    //
    // window activation
    //
    if( mev.isPress() )
    {
        if( ! windowFrame && _activeWindow )
        {
            onActivate(0);
        }
        
        if( windowFrame && ! windowFrame->window()->isActive())
        {
            windowFrame->window()->activate();
        }
    }

    //
    // window enter/leave
    //
    if(_currentWindow != windowFrame)
    {
      if(_currentWindow)
      {
          LeaveEvent lev( _currentWindow->window()->vid() );
          _currentWindow->leaveEvent(lev);
      }
      if(windowFrame)
      {
          EnterEvent eev( windowFrame->window()->vid() );
          windowFrame->enterEvent(eev);
      }
    }

    //
    // forward mouse event
    //
    if(windowFrame)
    {         
        if(windowFrame->mouseEvent(mev))
            _grabbedWindow = windowFrame;
        else
            _grabbedWindow = 0;
    }

    _currentWindow = windowFrame;

    return windowFrame != 0;
}


void WindowManager::paint(PaintSurface& surface, const Gfx::RectF& rect)
{  
    std::vector<WindowFrame*>::reverse_iterator it;

    for(it = _windows.rbegin(); it != _windows.rend(); ++it )
    {
        WindowFrame* frame = *it;
        Window* w = frame->window();                
        
        if( ! w->isVisible() )
            continue; 

        Gfx::RectF frameRect = frame->frameRect();
        frameRect.setOrigin( Gfx::PointF(0, 0) );
        frame->paint(surface, frameRect);

        // update rect in client coordinates
        Gfx::PointF updatePos = rect.topLeft() - frame->clientRect().topLeft();
        Gfx::RectF updateRect(updatePos, rect.size());        
        
        // clip update rect against client rect
        Gfx::RectF clientRect(Gfx::PointF(0, 0), w->size());
        updateRect = updateRect.intersect(clientRect);

        Gfx::PointF to = updateRect.topLeft() + frame->clientRect().topLeft();

        Painter painter(surface);
        painter.drawSurface(to, w->surface(), updateRect); 
        //painter.drawRect( pev.rect() ); 
        //std::clog << w->title() << ": "
        //          << to.x() << "," << to.y() << "  "
        //          << updateRect.width() << "x" << updateRect.height() << std::endl;  
    }
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
    std::clog << "WindowManager::onMove " << w.title() << " to " 
              << to.x() << ' ' << to.y() << std::endl;

    MoveEvent mev(w.vid(), to);
    
    WindowFrame* frame = findWindow(w);
    if(frame)
        frame->moveEvent(mev);

    Gfx::PointF from = w.position();
    Application::instance().loop().commitEvent(mev);
            
    Gfx::SizeF size = w.size();    
    size.addWidth( 2 * _borderWidth );
    size.addHeight( 2 * _borderWidth + _titleHeight );

    Gfx::RectF movedRect(to, size);
    Gfx::RectF updateRect(from, size);  
    updateRect.unify(movedRect);

    if( _parent )
        _parent->update(updateRect);
}


void WindowManager::onUpdate(Window& child, const Gfx::RectF& rect)
{
    Gfx::PointF updatePos = rect.topLeft() + child.position();
    updatePos.addX( _borderWidth );
    updatePos.addY( _borderWidth + _titleHeight );

    const Gfx::RectF updateRect(updatePos, rect.size());

    if(_parent)
        _parent->update(updateRect);
}


void WindowManager::onShow( Window& w, bool visible )
{
    Gfx::PointF framePos = w.position() - w.position();
    //framePos.subX(_borderWidth);
    //framePos.subY(_borderWidth +  _titleHeight);

    Gfx::SizeF frameSize = w.size();
    frameSize.addHeight(2 * _borderWidth + _titleHeight);
    frameSize.addWidth(2 * _borderWidth);

    Gfx::RectF updateRect(framePos, frameSize);
    
    ShowEvent sev( w.vid(), visible );
    Application::instance().loop().commitEvent( sev );
      
    if(_parent)
        _parent->update(updateRect);
}


void WindowManager::onActivate(Window* w)
{
    if(_activeWindow && _activeWindow->window() != w)
    {
        ActivateEvent aev(_activeWindow->window()->vid(), false);
        Application::instance().loop().commitEvent(aev);
        _activeWindow->update();
        _activeWindow = 0;
    }

    if( ! w )
        return;

    WindowFrame* frame = findWindow(*w);
    if( ! frame )
        return;

    // this moves frame to the back
    std::remove(_windows.begin(), _windows.end(), frame);

    ActivateEvent aev(w->vid(), true);
    Application::instance().loop().commitEvent(aev);

    _activeWindow = frame;
    _activeWindow->update();
}


void WindowManager::onEnable(Window& w, bool enable)
{
    Gfx::PointF framePos = w.position();
    //framePos.subX(_borderWidth);
    //framePos.subY(_borderWidth +  _titleHeight);

    Gfx::SizeF frameSize = w.size();
    frameSize.addHeight(2 * _borderWidth + _titleHeight);
    frameSize.addWidth(2 * _borderWidth);

    Gfx::RectF updateRect(framePos, frameSize);

    EnableEvent eev( w.vid(), enable );
    Application::instance().loop().commitEvent( eev );
     
    if(_parent)
        _parent->update(updateRect);
}


void WindowManager::onClosing(Window& w)
{
    CloseEvent ev( w.vid());
    Application::instance().loop().commitEvent( ev );
}


void WindowManager::onClose(Window& w)
{
    remove(w);

    Gfx::PointF framePos = w.position();
    //framePos.subX(_borderWidth);
    //framePos.subY(_borderWidth +  _titleHeight);

    Gfx::SizeF frameSize = w.size();
    frameSize.addHeight(2 * _borderWidth + _titleHeight);
    frameSize.addWidth(2 * _borderWidth);

    Gfx::RectF updateRect(framePos, frameSize);

    if(_parent)
        _parent->update(updateRect);
}


Gfx::PointF WindowManager::toParent(const Window& w, const Gfx::PointF& pos) const
{
    return toParent(w.position(), pos);
}


Gfx::PointF WindowManager::toParent(const Gfx::PointF& winPos, const Gfx::PointF& pos) const
{
    double offY = _borderWidth + _titleHeight;
    double offX = _borderWidth;

    return winPos + pos + Gfx::PointF(offX, offY);
}


Gfx::PointF WindowManager::fromParent(const Window& w, const Gfx::PointF& pos) const
{
    return fromParent(w.position(), pos);
}


Gfx::PointF WindowManager::fromParent(const Gfx::PointF& winPos, const Gfx::PointF& pos) const
{
    double offY = _borderWidth + _titleHeight;
    double offX = _borderWidth;

    Gfx::PointF p = pos - winPos - Gfx::PointF(offX, offY);
    return p;
}

} // namespace

} // namespace
