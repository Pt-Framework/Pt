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
    WindowFrame* frame = new WindowFrame(*this, w);

    switch( w.type() )
    {
        case Window::Popup:
            frame->setFrame(0, 0);
            break;
        
        default:
        case Window::Normal:
            frame->setFrame(_borderWidth, _titleHeight);
            break;
    }

    _windows.push_back(frame);
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
    std::vector<WindowFrame*>::iterator it;
    
    for(it = _windows.begin(); it != _windows.end(); ++it )
    {
        WindowFrame* frame = *it;
        Window* w = frame->window();
        
        if( ! w->isVisible() )
            continue; 

        Gfx::RectF frameRect = frame->frameRect().intersect(rect);
        
        surface.setClip(frameRect);
        frame->paint(surface, frameRect);
        surface.setClip( Gfx::RectF( Gfx::PointF(0,0), surface.size()) );

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
    WindowFrame* frame = findWindow(w);
    if( ! frame )
        return;

    ResizeEvent rev(w.vid(), to);
    Application::instance().loop().commitEvent(rev);
    
    Gfx::RectF updateRect = frame->frameRect();
    
    frame->resizeEvent(rev);
    
    updateRect.unify( frame->frameRect() );

    if( ! w.isVisible() )
        return;

    Gfx::PointF updatePos = fromParent( w, updateRect.topLeft() );
    updateRect.setOrigin(updatePos);
    
    w.update(updateRect);
}


void WindowManager::onMove(Window& w, const Gfx::PointF& to)
{   
    MoveEvent mev(w.vid(), to);
    
    //
    // TODO: use old/new frameRect to calculate update rect
    //

    WindowFrame* frame = findWindow(w);
    if(frame)
        frame->moveEvent(mev);

    Gfx::PointF from = w.position();
    Application::instance().loop().commitEvent(mev);

    Gfx::SizeF size = toFrameSize( w, w.size() );

    Gfx::RectF movedRect(to, size);
    Gfx::RectF updateRect(from, size);  
    updateRect.unify(movedRect);

    if( ! _parent )
        throw std::logic_error("WindowManager not initialized");

    _parent->update(updateRect);
}


void WindowManager::onUpdate(Window& w, const Gfx::RectF& rect)
{
    Gfx::PointF updatePos = toParent( w, rect.topLeft() );

    const Gfx::RectF updateRect(updatePos, rect.size());

    if( ! _parent )
        throw std::logic_error("WindowManager not initialized");
        
    _parent->update(updateRect);
}


void WindowManager::onShow( Window& w, bool visible )
{
    ShowEvent sev( w.vid(), visible );
    Application::instance().loop().commitEvent(sev);

    Gfx::PointF framePos = w.position();
    Gfx::SizeF frameSize = toFrameSize( w, w.size() );

    Gfx::RectF updateRect(framePos, frameSize);

    if( ! _parent )
        throw std::logic_error("WindowManager not initialized");

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

    // move active frame to the back
    std::vector<WindowFrame*>::iterator it =
        std::find(_windows.begin(), _windows.end(), frame);

    _windows.erase(it);
    _windows.push_back(frame);

    ActivateEvent aev(w->vid(), true);
    Application::instance().loop().commitEvent(aev);

    _activeWindow = frame;
    _activeWindow->update();
}


void WindowManager::onEnable(Window& w, bool enable)
{
    EnableEvent eev( w.vid(), enable );
    Application::instance().loop().commitEvent( eev );

    Gfx::PointF framePos = w.position();
    Gfx::SizeF frameSize = toFrameSize( w, w.size() );

    Gfx::RectF updateRect(framePos, frameSize);

    if( ! _parent )
        throw std::logic_error("WindowManager not initialized");

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
    Gfx::SizeF frameSize = toFrameSize( w, w.size() );

    Gfx::RectF updateRect(framePos, frameSize);

    if( ! _parent )
        throw std::logic_error("WindowManager not initialized");

   _parent->update(updateRect);
}


Gfx::SizeF WindowManager::toFrameSize(Window& w, const Gfx::SizeF& clientSize)
{  
    const double borderWidth = w.hasBorder() ? _borderWidth : 0;
    const double titleHeight = w.hasBorder() ? _titleHeight : 0;

    Gfx::SizeF size = clientSize;
    size.addHeight(2 * borderWidth + titleHeight);
    size.addWidth(2 * borderWidth);

    return size;
}


//Gfx::RectF WindowManager::toFrameRect(Window& w, const Gfx::RectF& clientRect)
//{  
//    const double borderWidth = w.hasBorder() ? _borderWidth : 0;
//    const double titleHeight = w.hasBorder() ? _titleHeight : 0;
//
//    Gfx::PointF pos = clientRect.topLeft();
//    pos.subX(borderWidth);
//    pos.subY(borderWidth + titleHeight);
//
//    Gfx::SizeF size = clientRect.size();
//    size.addHeight(2 * borderWidth + titleHeight);
//    size.addWidth(2 * borderWidth);
//
//    return Gfx::RectF(pos, size);
//}


Gfx::PointF WindowManager::toParent(const Window& w, const Gfx::PointF& pos) const
{    
    const double borderWidth = w.hasBorder() ? _borderWidth : 0;
    const double titleHeight = w.hasBorder() ? _titleHeight : 0;

    double offY = borderWidth + titleHeight;
    double offX = borderWidth;

    return w.position() + pos + Gfx::PointF(offX, offY);
}


Gfx::PointF WindowManager::fromParent(const Window& w, const Gfx::PointF& pos) const
{
    const double borderWidth = w.hasBorder() ? _borderWidth : 0 ;
    const double titleHeight = w.hasBorder() ? _titleHeight : 0 ;

    double offY = borderWidth + titleHeight;
    double offX = borderWidth;

    Gfx::PointF p = pos - w.position() - Gfx::PointF(offX, offY);
    return p;
}

} // namespace

} // namespace
