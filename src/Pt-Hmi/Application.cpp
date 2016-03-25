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


void Application::repaint(Window& w, const Gfx::RectF& rect)
{        
    EraseEvent erv(w.vid(), rect);
    loop().commitEvent(erv);  

    if( w.mainWidget() )
        updateWidget( *w.mainWidget(), rect );

    PaintEvent pev(w.vid(), rect);
    loop().commitEvent(pev);  

    if( w.parent() )
    {
        Gfx::PointF pos( w.position() + rect.topLeft() );

        pos.addX( windowBorderWidth() );
        pos.addY( windowBorderWidth() + windowTitleHeight() );

        repaint( *w.parent(), Gfx::RectF( pos, rect.size() ) );
    }
}


void Application::repaint(Widget& w, const Gfx::RectF& rect)
{
    if( w.window() == 0)
        return;

    Gfx::RectF wrect( w.toWindowPosition(rect.topLeft()) , rect.size() );
    repaint(*w.window(), wrect );
}


void Application::updateWidget( Widget& parent, const Gfx::RectF& rect )
{
    PaintEvent pev( parent.vid(), rect);
    loop().commitEvent(pev); 

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


void Application::move( Widget& w, const Gfx::PointF& to )
{
    Gfx::RectF updateRect(w.position(), w.size()); 
    updateRect.unify(Gfx::RectF(to, w.size())); 

    MoveEvent mev(w.vid(), to );    
    w.processEvent(mev);

//    loop().commitEvent(mev);

    if( !w.window() )
        return;

    if( w.parent() )
        repaint( *w.parent(), updateRect  );

    //Todo: check transparency
    //repaint( w, Gfx::RectF( Gfx::PointF(0,0), w.size() ) );
}



void Application::move( Window& w, const Gfx::PointF& to )
{   
    Gfx::PointF pos = w.position();

    MoveEvent rev(w.vid(), to);
//    loop().commitEvent(rev);
    w.processEvent(rev);

    if(! w.parent() )
        return;
    
    const double borderWidth = Application::instance().windowBorderWidth();
    const double titleHeight = Application::instance().windowTitleHeight();
        
    Gfx::SizeF size = w.size();    

    size.addWidth( 2 * borderWidth );
    size.addHeight( 2 * borderWidth + titleHeight );

    Gfx::RectF updateRect( pos, size);   
    Gfx::RectF movedRect( to, size);
    updateRect.unify(movedRect); 
    repaint(*w.parent(), updateRect);
    
    //TODO: only child transparent
    //repaint(w, Gfx::RectF( Gfx::PointF(0,0), w.size() ) );
}


void Application::resize( Widget& w, const Gfx::SizeF& s )
{

    Gfx::SizeF newSize( std::max( w.size().width(), s.width()), 
                        std::max( w.size().height(), s.height()) );

    //Resize
    ResizeEvent rev(w.vid(), s);
    sendEvent(w, rev);

//    loop().commitEvent(rev);

    if( !w.window() )
        return;
    
    if( w.parent() )
        repaint(*w.parent(), Gfx::RectF( w.position(), newSize ) );    

    repaint(w, Gfx::RectF(Gfx::PointF(0,0), newSize ) );    
}


void Application::resize( Window& w, const Gfx::SizeF& to )
{   
    Gfx::SizeF wSize =  w.size();

    ResizeEvent rev(w.vid(), to);
//    loop().commitEvent(rev);
   sendEvent(w, rev);

    
    if( !w.isVisible())
        return;
    
    const double borderWidth = windowBorderWidth();
    const double titleHeight = windowTitleHeight();
    
    const Gfx::PointF updatePos(-borderWidth, -(borderWidth + titleHeight) );


    Gfx::SizeF updateSize( std::max( to.width(), wSize.width() ),
                           std::max( to.height(), wSize.height() ));

    
    updateSize.addHeight(2 * borderWidth + titleHeight);
    updateSize.addWidth(2 * borderWidth);

    repaint(w, Gfx::RectF( updatePos, updateSize ) );
}



void Application::show( Widget& w, bool s )
{
    ShowEvent ev(w.vid(), s );
    loop().commitEvent(ev);

    if( w.window() == 0)
      return;

    if( w.parent() )
        repaint(*w.parent(), Gfx::RectF( w.position(), w.size() ) );

    if( s ) 
        repaint(w, Gfx::RectF( Gfx::PointF(0,0), w.size() ) );
}


void Application::enable( Widget& w, bool s )
{
    EnableEvent ev(w.vid(), s );
    loop().commitEvent(ev);

    if( w.window() == 0)
      return;

    repaint(w, Gfx::RectF( Gfx::PointF(0,0), w.size() ) );
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
        
    repaint(*w.parent(), updateRect);

    //Window
    if( visible )
        repaint(w, Gfx::RectF( Gfx::PointF(0,0), w.size() ) );
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

    repaint(*w.parent(), updateRect);
   
    repaint(w, Gfx::RectF( Gfx::PointF(0,0), w.size() ) );
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
