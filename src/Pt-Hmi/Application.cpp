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
#include "WindowImpl.h"
#include "ScreenImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/System/MainLoop.h>
#include <cassert>

namespace Pt {

namespace Hmi {

Application::Application(int argc, char** argv)
: System::Application(0, argc, argv)
, _impl( new ApplicationImpl() ) 
, _mainScreen(0)
, _lastVid(1)
{ 	
	this->init(*_impl);

  _mainScreen = new Screen(*_impl);

  loop().eventReceived() += Pt::slot(*this, &Application::onResizeEvent );
  loop().eventReceived() += Pt::slot(*this, &Application::onMoveEvent );
  loop().eventReceived() += Pt::slot(*this, &Application::onKeyEvent );
  loop().eventReceived() += Pt::slot(*this, &Application::onUpdateEvent ); 
  loop().eventReceived() += Pt::slot(*this, &Application::onPaintEvent ); 
  loop().eventReceived() += Pt::slot(*this, &Application::onMouseEvent );
  loop().eventReceived() += Pt::slot(*this, &Application::onActivateEvent );
  loop().eventReceived() += Pt::slot(*this, &Application::onEnableEvent );
  loop().eventReceived() += Pt::slot(*this, &Application::onShowEvent );
  loop().eventReceived() += Pt::slot(*this, &Application::onCloseEvent );
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

    VisualMap::value_type elem(visual.vid(), &visual);
    _visuals.insert(elem);
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


void Application::onUpdateEvent(const UpdateEvent& ev)
{
    VisualMap::iterator vit = _visuals.find( ev.vid() );
    if( vit == _visuals.end() )
        return;

    vit->second->processEvent(ev);
}


void Application::onPaintEvent(const PaintEvent& ev)
{
    VisualMap::iterator vit = _visuals.find( ev.vid() );

    if( vit == _visuals.end() )
        return;

    vit->second->processEvent(ev);
}


void Application::onResizeEvent(const ResizeEvent& ev)
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onMouseEvent(const MouseEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onMoveEvent(const MoveEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onActivateEvent( const ActivateEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onEnableEvent( const EnableEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onShowEvent( const ShowEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onKeyEvent( const KeyEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);    
}


void Application::onCloseEvent(const CloseEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);   
}

} // namespace

} // namespace
