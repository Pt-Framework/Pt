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
 
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA 02110-1301 USA
*/

#include "ApplicationImpl.h"
#include "ScreenImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/EraseEvent.h>
#include <Pt/System/MainLoop.h>
#include <cassert>
namespace Pt {

namespace Hmi {

Application::Application(int argc, char** argv)
: System::Application(0, argc, argv)
, _impl( new ApplicationImpl() ) 
, _mainScreen(0)
, _windowBorderWidth(4)
, _windowTitleHeight(20)
, _lastVid(1)
{ 	
	this->init(*_impl);

  _mainScreen = new Screen(*_impl);

  loop().eventReceived() += Pt::slot(*this, &Application::onResizeEvent );
  loop().eventReceived() += Pt::slot(*this, &Application::onMoveEvent );
  loop().eventReceived() += Pt::slot(*this, &Application::onUpdateEvent ); 
  loop().eventReceived() += Pt::slot(*this, &Application::onPaintEvent ); 
  loop().eventReceived() += Pt::slot(*this, &Application::onEraseEvent ); 
  
}


Application::~Application()
{
	delete _mainScreen;
	delete _impl;
}



Application& Application::instance()
{
    return static_cast<Application&>( System::Application::instance() );
}
	

void Application::registerVisual( Visual& visual )
{
    VisualMap::const_iterator it = _visuals.find( visual.vid() );

    assert( it == _visuals.end() );

    std::pair<Pt::uint64_t, Visual*> vpair( visual.vid(), &visual );

    _visuals.insert( vpair );
}


void Application::unregisterVisual( Visual& visual )
{
   _visuals.erase( visual.vid() );
}


void Application::nextEvent()
{
	_impl->nextEvent();
}


void Application::sendEvent(Visual& w, const Pt::Event& ev)
{
	// TODO: check event filter before dispatching the event

	w.processEvent(ev);
}


void Application::onUpdate(Window& w, const Gfx::RectF& rect)
{        
    // NOTE: currently, the window is always a top-level window

    UpdateMap::iterator it = _updates.find( w.vid() );
    if( it == _updates.end() )
    {
        UpdateInfo uinfo(rect);
        _updates.insert( std::make_pair(w.vid(), uinfo) );
    }
    else
    {
        it->second.push(rect);
    }

    // TODO: should translate rect to screen coordinates and application
    //       should find all windows affected by the update rect

    UpdateEvent uev(w.vid(), rect);
    loop().commitEvent(uev);
}


void Application::onUpdateEvent(const UpdateEvent& ev)
{
    UpdateMap::iterator it = _updates.find( ev.vid() );
    if( it == _updates.end() )
        return;

    // only last update event
    if( it->second.pop() != 0)
        return;

    Gfx::RectF updateRect = it->second.rect();
    _updates.erase( ev.vid() );

    VisualMap::iterator vit = _visuals.find( ev.vid() );
    if( vit == _visuals.end() )
        return;

    // TODO: this cast can be removed once the update rect is in screen
    //       coordinates and Application finds all windows affected by
    //       the update rect of the event
    Window* w = static_cast<Window*>(vit->second);
    updateWindow(*w, updateRect);
}


void Application::updateWindow(Window& w, const Gfx::RectF& rect)
{        
    EraseEvent erv(w.vid(), rect);
    loop().commitEvent(erv);  

    if( w.mainWidget() )
        updateWidget( *w.mainWidget(), rect );

    std::vector<Window*>& windows = w.windows();
    std::vector<Window*>::iterator child;
    for(child = windows.begin(); child != windows.end(); ++child)
    {
        Gfx::PointF pos( rect.topLeft() - (*child)->position() );
        pos.subX( windowBorderWidth() );
        pos.subY( windowBorderWidth() + windowTitleHeight() );

        Gfx::RectF updateRect( pos, rect.size() );
        updateWindow(**child, updateRect);
    }

    PaintEvent pev(w.vid(), rect);
    loop().commitEvent(pev); 
}


void Application::updateWidget( Widget& parent, const Gfx::RectF& rect )
{
    PaintEvent pev( parent.vid(), rect);
    loop().queueEvent(pev); 

    std::vector<Widget*>& widgets     = parent.widgets();
    std::vector<Widget*>::iterator it = widgets.begin();

    for( ; it != widgets.end(); ++it )
    {        
        Widget* w = (*it);

        if( w->geometry().intersect(rect).isNull() )
            continue;

        Gfx::RectF updateRect( rect.topLeft() - w->position(), rect.size() );

        updateWidget( *w, updateRect );            
    }
}


void Application::onPaintEvent(const PaintEvent& ev)
{
    VisualMap::iterator vit = _visuals.find( ev.vid() );

    if( vit == _visuals.end() )
        return;

    vit->second->processEvent(ev);
}


void Application::onResizeEvent(const ResizeEvent& ev )
{
      VisualMap::iterator it = _visuals.find( ev.vid() );

      if( it == _visuals.end() )
        return;

     it->second->processEvent(ev );
}


void Application::onMoveEvent(const MoveEvent& ev )
{
      VisualMap::iterator it = _visuals.find( ev.vid() );

      if( it == _visuals.end() )
        return;

     it->second->processEvent(ev );
}


void Application::onEraseEvent( const EraseEvent& ev )
{
      VisualMap::iterator it = _visuals.find( ev.vid() );

      if( it == _visuals.end() )
        return;

     it->second->processEvent(ev );
}


void Application::move(Window& w, const Gfx::PointF& to)
{   
    Gfx::PointF from = w.position();

    MoveEvent mev(w.vid(), to);
    loop().commitEvent(mev);

    // TODO: fix this and go through event loop
    //sendEvent(w, mev);

    if( ! w.parent() )
        return;
    
    const double borderWidth = Application::instance().windowBorderWidth();
    const double titleHeight = Application::instance().windowTitleHeight();
        
    Gfx::SizeF size = w.size();    
    size.addWidth( 2 * borderWidth );
    size.addHeight( 2 * borderWidth + titleHeight );

    Gfx::RectF movedRect(to, size);

    Gfx::RectF updateRect(from, size);  
    updateRect.unify(movedRect);
    
    w.parent()->update(updateRect);
}


void Application::resize(Window& w, const Gfx::SizeF& to)
{   
    Gfx::SizeF from = w.size();

    ResizeEvent rev(w.vid(), to);
    loop().commitEvent(rev);
    
    // TODO: fix this and go through event loop
    //sendEvent(w, rev);

    if( ! w.isVisible() )
        return;
    
    const double borderWidth = windowBorderWidth();
    const double titleHeight = windowTitleHeight();
    
    const Gfx::PointF updatePos(-borderWidth, -(borderWidth + titleHeight) );

    Gfx::SizeF updateSize( std::max( to.width(), from.width() ),
                           std::max( to.height(), from.height() ));

    updateSize.addHeight(2 * borderWidth + titleHeight);
    updateSize.addWidth(2 * borderWidth);

    Gfx::RectF updateRect( updatePos, updateSize );
    w.update(updateRect);
}


void Application::show( Window& w, bool visible )
{
    const double borderWidth = Application::instance().windowBorderWidth();
    const double titleHeight = Application::instance().windowTitleHeight();

    Gfx::PointF framePos = w.position() - w.position();
    framePos.subX(borderWidth);
    framePos.subY(borderWidth +  titleHeight);

    Gfx::SizeF frameSize = w.size();
    frameSize.addHeight(2 * borderWidth + titleHeight);
    frameSize.addWidth(2 * borderWidth);

    Gfx::RectF updateRect(framePos, frameSize);
    
    ShowEvent sev( w.vid(), visible );
    loop().commitEvent( sev );
      
    w.parent()->update(updateRect);
}


void Application::enable( Window& w, bool enable )
{
    const double borderWidth = Application::instance().windowBorderWidth();
    const double titleHeight = Application::instance().windowTitleHeight();

    Gfx::PointF framePos = w.position() - w.position();
    framePos.subX(borderWidth);
    framePos.subY(borderWidth +  titleHeight);

    Gfx::SizeF frameSize = w.size();
    frameSize.addHeight(2 * borderWidth + titleHeight);
    frameSize.addWidth(2 * borderWidth);

    Gfx::RectF updateRect(framePos, frameSize);

    EnableEvent eev( w.vid(), enable );
    loop().commitEvent( eev );
   
    w.update( Gfx::RectF( Gfx::PointF(0,0), w.size() ) );
    //update(w, Gfx::RectF( Gfx::PointF(0,0), w.size() ) );
}




void Application::activate( Window& w )
{
/*
    const double borderWidth = Application::instance().windowBorderWidth();
    const double titleHeight = Application::instance().windowTitleHeight();

    Gfx::PointF framePos(0, 0);
    framePos.subX( borderWidth );
    framePos.subY( borderWidth +  titleHeight );

    Gfx::SizeF frameSize = w.size();
    frameSize.addHeight(2 * borderWidth + titleHeight);
    frameSize.addWidth(2 * borderWidth);

    Gfx::RectF updateRect(framePos, frameSize);

    loop().commitEvent( ActivateEvent( w.vid(), true ) );
    loop().commitEvent( PaintEvent( w.vid(), Gfx::RectF(Gfx::PointF(0,0), w.size()) ));

    Window* child = &w;


    for(Window* parent = w.parent(); parent; parent = w.parent())
    {
        std::vector<Window*>::const_iterator it;
        for( it = parent->windows().begin(); it != parent->windows().end(); ++it)
        {
            if( (*it)->isActive() && *it != child )
            {
                loop().commitEvent( ActivateEvent( (*it)->vid(), false ) );
            }
        }

        loop().commitEvent( ActivateEvent( parent->vid(), true ) );

        child = parent;
    }
    */
}

/*
Widget* Application::findWidget(const std::string& name)
{
	return _mainScreen->findWidget( name);
}


Window* Application::findWindow(const std::string& name)
{
    return _mainScreen->findWindow(name);
}
*/
} // namespace

} // namespace
