/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/

#include "WindowImpl.h"
#include "ChildWindowImpl.h"
#include "MainWindowImpl.h"
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Widget.h>

namespace Pt{

namespace Hmi{

WindowImpl::WindowImpl(Window* api)
: _apiWindow(api)
, _parent(0)
, _pointerWidget(0)
, _focusedWidget(0)
, _minimumSize(0,0)
, _maximumSize( 2000,2000)
, _startPostion( WindowPosition::Manual )
, _state(WindowState::Normal )
, _border( WindowBorder::Sizeable)
, _icon()
, _canClose(true)
, _isClosed(true)
, _isActive(false)
, _windowManager(api)
, _mainWidget(0)
, _position(10,10)
, _size(200,200)
{
}

WindowImpl::WindowImpl(Window* api, const WindowImpl* impl)
: _apiWindow(api)
, _parent(impl->_parent)
, _pointerWidget(impl->_pointerWidget)
, _focusedWidget(impl->_focusedWidget)
, _minimumSize(impl->_minimumSize)
, _maximumSize(impl->_maximumSize)
, _startPostion(impl->_startPostion )
, _state(impl->_state )
, _border(impl->_border)
, _icon(impl->_icon)
, _canClose( impl->_canClose)
, _isClosed(impl->_isClosed)
, _isActive(false)
, _windowManager(api)
, _mainWidget(impl->_mainWidget)
, _position(impl->_position)
, _size(impl->_size)
, _enabled( impl->_enabled)
, _visible( impl->_visible)
//, _icon( impl->_icon)
, _title( impl->_title)
, _decoration( impl->_decoration)
, _name(impl->_name)
, _font(impl->_font)
{
    _surface.resize(_size);
}

WindowImpl::~WindowImpl()
{
   setMainWidget(0);

    const std::vector<Window*>& children = _windowManager.windows();
    std::vector<Window*>::const_iterator it;

    for(it = children.begin(); it != children.end(); ++it)
        (*it)->impl()->_parent = 0;
 
    if(_parent)
        _parent->remove(*_apiWindow);
}


void WindowImpl::add(Window& window)
{
    if(window.parent())
        window.parent()->impl()->remove(window);
    
    WindowImpl* newImpl = new ChildWindowImpl(&window, window.impl());

    delete window.impl();
    
    window._impl = newImpl;
    
    window.impl()->_parent = _apiWindow;
        
    _windowManager.add(window);            
}


void WindowImpl::remove(Window& window )
{
    if( window.parent() != _apiWindow )
        return;
    
    _windowManager.remove( window );       

    delete window.impl();             
    
    window._impl = new MainWindowImpl(&window);
}



Window* WindowImpl::findWindow(const std::string& name)
{
    if(_name == name)
        return _apiWindow;

    const std::vector<Window*>& windows = _windowManager.windows();
	std::vector<Window*>::const_iterator it;

	for(it = windows.begin(); it != windows.end(); ++it)
	{
        Window* w = (*it)->findWindow(name);
		if(w)
			return w;
	}

    return 0;
}

void WindowImpl::setMainWidget(Widget* widget)
{   
    if(_mainWidget)
        _mainWidget->setWindow(0);

    _mainWidget = widget;

    if(_mainWidget)
    {
        if( _mainWidget->parent() )
            _mainWidget->parent()->removeWidget(*_mainWidget);

        _mainWidget->setWindow(_apiWindow);
    }
}

void WindowImpl::removeWidget(Widget& w)
{
    if( _pointerWidget == &w )
        _pointerWidget = 0;

    if( _focusedWidget == &w )
        _focusedWidget = 0;        
}


Widget* WindowImpl::findWidget(const std::string& name)
{
    return _mainWidget ? _mainWidget->findWidget(name) : 0;
}


void WindowImpl::setPointedWidget( Widget* widget ) 
{
    if( _pointerWidget == widget )
        return;

    if( _pointerWidget )            
        _pointerWidget->onPointerLeave();

    _pointerWidget = widget;

    if( _pointerWidget )
        _pointerWidget->onPointerEnter();
}


void WindowImpl::setFocusedWidget( Widget* widget ) 
{
    if( _focusedWidget == widget )
        return;

    if( _focusedWidget )            
        _focusedWidget->setFocus(false);
    
    _focusedWidget = widget;

    if( _focusedWidget )
        _focusedWidget->setFocus(true);
}


void WindowImpl::onPointerEvent(const PointerEvent& ev)
{    
    if( _windowManager.pointerInput( ev ) )
    {
        this->setPointedWidget( 0 );
        return;
    }

    if( _mainWidget )
    {
        Widget* widget = _mainWidget->findWidget( Gfx::PointF( ev.x(), ev.y() ) );

        this->setPointedWidget( widget );    

        if( widget )
            widget->processEvent(ev);        
    }
}


void WindowImpl::onKeyEvent(const KeyEvent& ev)
{
  if( _windowManager.keyInput( ev ) )
      return;

    if( ! isEnabled() )
        return;
    
 if( _focusedWidget != 0 )
    _focusedWidget->onKeyEvent(ev);

}

void WindowImpl::onResizeEvent(const ResizeEvent& ev)
{    
    _size = ev.size();
    _state = ev.state();
    _surface.resize(_size);
    invalidate();
}


void WindowImpl::onMoveEvent( const MoveEvent& ev)
{
    _position  = ev.position();  
    invalidate();
}


void WindowImpl::onActivateEvent(const ActivateEvent& ev)
{ 
    _isActive = ev.isActive();

    if( ! _isActive )
    {
        _windowManager.deactivate();
        return;
    }      

    invalidate();
}

void WindowImpl::onCloseEvent(const CloseEvent& ev)
{
    _isClosed =  true;
}

void WindowImpl::render()
{
    if( _mainWidget != 0 )
    {
        _mainWidget->render();
        surface().painter().drawSurface(_mainWidget->position(), _mainWidget->surface() );
    }

    _windowManager.render();
}

}}
