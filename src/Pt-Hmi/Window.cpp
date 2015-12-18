/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Application.h>
#include "MainWindowImpl.h"
#include "ChildWindowImpl.h"

namespace Pt {

namespace Hmi {

Window::Window(Window* parent)
: _impl(0)
, _parent(0)
, _pointerWidget(0)
, _focusedWidget(0)
, _minimumSize(0,0)
, _maximumSize( 2000,2000)
, _startPostion( WindowPosition::Manual )
, _state(WindowState::Normal )
, _enabled(true)
, _border( WindowBorder::Sizeable)
, _icon()
, _canClose(true)
, _isClosed(false)
, _isActive(false)
, _mainWidget(0)
, _position(0,0)
, _size(200,200)
, _visible(true)
{
    _eventReady += Pt::slot(*this, &Window::onKeyEvent);
    _eventReady += Pt::slot(*this, &Window::onPointerEvent);  
    _eventReady += Pt::slot(*this, &Window::onMoveEvent);
    _eventReady += Pt::slot(*this, &Window::onResizeEvent);
    _eventReady += Pt::slot(*this, &Window::onActivateEvent);
    _eventReady += Pt::slot(*this, &Window::onCloseEvent);

    if(parent)
        parent->add(*this);
    else
    {
        _impl = new MainWindowImpl(this);
        Application::instance().mainScreen().registerWindow(*this);
    }

}


Window::~Window()
{
    if(_parent)
        Application::instance().mainScreen().unregisterWindow(*this);

    setMainWidget(0);

    const std::vector<Window*>& children = _windowManager.windows();
    std::vector<Window*>::const_iterator it;

    for(it = children.begin(); it != children.end(); ++it)
        this->remove(**it);
 
    if(_parent)
        _parent->remove(*this);

    delete _impl;
}


void Window::add(Window& child)
{
    if( child._parent == this )
        return;

    if( ! child._parent )
        Application::instance().mainScreen().unregisterWindow(child);

    if( child._parent )
        child._parent->_windowManager.remove(child);     
    
    delete child._impl;
    
    child._impl = new ChildWindowImpl(&child);            
    child._parent = this;
    _windowManager.add(child);             
}


void Window::remove(Window& child)
{
    if(child._parent != this)
        return;
    
    _windowManager.remove(child);       
    child._parent = 0;
    delete child.impl();             
    
    child._impl = new MainWindowImpl(&child);    
    
    Application::instance().mainScreen().registerWindow(child);
}


const std::vector<Window*>& Window::windows() const 
{
    return _windowManager.windows();
}


Window* Window::parent()
{
   return _parent;
}


const Window* Window::parent() const
{
    return _parent;
}
    

Widget* Window::mainWidget() 
{
    return _mainWidget;
}


const Widget* Window::mainWidget()  const 
{
    return _mainWidget;
}


Window* Window::findWindow(const std::string& name)
{
    if(_name == name)
        return this;

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


void Window::setMainWidget(Widget* widget)
{   
    if(_mainWidget)
        _mainWidget->setWindow(0);

    _mainWidget = widget;

    if(_mainWidget)
    {
        if( _mainWidget->parent() )
            _mainWidget->parent()->removeWidget(*_mainWidget);

        _mainWidget->setWindow(this);
    }
}


void Window::setPointedWidget( Widget* widget ) 
{
    if( _pointerWidget == widget )
        return;

    if( _pointerWidget )            
        _pointerWidget->onPointerLeave();

    _pointerWidget = widget;

    if( _pointerWidget )
        _pointerWidget->onPointerEnter();
}




void Window::removeWidget(Widget& w)
{
    if( _pointerWidget == &w )
        _pointerWidget = 0;

    if( _focusedWidget == &w )
        _focusedWidget = 0;        
}


void Window::setFocusedWidget( Widget* widget ) 
{
    if( _focusedWidget == widget )
        return;

    if( _focusedWidget )            
        _focusedWidget->setFocus(false);
    
    _focusedWidget = widget;

    if( _focusedWidget )
        _focusedWidget->setFocus(true);
}


Widget* Window::findWidget(const std::string& name)
{
    return _mainWidget ? _mainWidget->findWidget(name) : 0;
}


void Window::activate()
{
    _impl->activate();     
}


void Window::close()
{
    _impl->close();
}

void Window::processEvent(const Pt::Event& ev)
{
    onEvent(ev);
}


void Window::onEvent(const Pt::Event& ev)
{
    _eventReady.send(ev);
}


void Window::onPointerEvent(const PointerEvent& ev)
{    
    if( _windowManager.pointerInput( ev ) )
    {
        this->setPointedWidget( 0 );
        return;
    }

    if( !_mainWidget  || ! _mainWidget->visible() )
    {
        Application::instance().mainScreen().setCursor( &Cursor::defaultCursor() );  
        return;
    }
    
    Widget* widget = _mainWidget->findWidget( Gfx::PointF( ev.x(), ev.y() ) );

    this->setPointedWidget( widget );    

    if( widget )
        widget->processEvent(ev);       
}


void Window::onKeyEvent(const KeyEvent& ev)
{
  if( _windowManager.keyInput( ev ) )
      return;

    if( ! isEnabled() )
        return;
    
 if( _focusedWidget != 0 )
    _focusedWidget->onKeyEvent(ev);

}


void Window::onResizeEvent(const ResizeEvent& ev)
{    
    _size = ev.size();
    _state = ev.state();
    _surface.resize(_size);
    invalidate();
}


void Window::onMoveEvent( const MoveEvent& ev)
{
    _position  = ev.position();  
    invalidate();
}


void Window::onActivateEvent(const ActivateEvent& ev)
{ 
    _isActive = ev.isActive();

    if( !_isActive )
    {
        _windowManager.deactivate();  

        invalidate();
    }
    else
    {
       if( _parent )
           _parent->processEvent(ActivateEvent(true));
       else
          invalidate();
    }                
}


void Window::onCloseEvent(const CloseEvent& ev)
{
     _isClosed =  true;
}


bool Window::isClosed() const
{
  return _isClosed;       
}


bool Window::isActive() const
{
    return _isActive;
}


const Gfx::SizeF& Window::minimumSize() const
{
    return _minimumSize;
}


void Window::setMinimumSize(const Gfx::SizeF& s)
{
    _impl->setMinimumSize(s);
    _minimumSize = s;
}


const Gfx::SizeF& Window::maximumSize() const
{
    return _maximumSize;
}


void Window::setMaximumSize(const Gfx::SizeF& s)
{
    _impl->setMaximumSize(s);
    _maximumSize = s;
}


void Window::setDecoration( WindowDecoration::Flags d )
{
    _impl->setDecoration( d );
    _decoration = d;
}
    
WindowDecoration::Flags Window::decoration() const
{
    return _decoration;
}


Hmi::WindowPosition::Type Window::defaultPosition() const
{
    return _startPostion;
}


void Window::setDefaultPosition( Hmi::WindowPosition::Type p)
{    
    _startPostion = p;
}


Hmi::WindowState::Type Window::state() const
{
    return _state;
}


void Window::setState( WindowState::Type s)
{
    _impl->setState(s);
    _state = s;
}


WindowBorder::Type Window::border() const
{
    return _border;
}


void Window::setBorder(WindowBorder::Type t)
{
    _impl->setBorder(t);
    _border = t;
}


const Gfx::Image& Window::icon() const
{
    return _icon;
}


void Window::setIcon(const Gfx::Image& i)
{
    _impl->setIcon(i);
    _icon = i;
}


bool Window::isClosable() const
{
    return _canClose;
}


void Window::setClosable(bool c)
{
    _canClose = c;
}


const std::string& Window::title() const
{
    return _title;
}


void Window::setTitle( const std::string& t )
{
    _impl->setTitle(t);
    _title = t;
}


bool Window::isEnabled() const
{
    return _enabled;
}


void Window::setEnabled( bool e )
{
    _impl->setEnabled(e);
    _enabled = e;
}


void Window::setVisible( bool b )
{
    if(b)
        _impl->show();
    else
        _impl->hide();
}


bool Window::isVisible() const
{
    return _visible;
}


const Gfx::SizeF& Window::size() const
{
    return _size;
}


void Window::setSize( const Gfx::SizeF& s )
{
    _size = s;
    _impl->setSize(s);    
    _surface.resize(s);
}


const Gfx::PointF& Window::position() const
{
    return _position;
}


void Window::setPosition( const Gfx::PointF& p)
{
    _impl->setPosition(p);
    _position = p;
}

void Window::setFont(const Gfx::Font& ft)
{
    _font = ft;
}
    
const Gfx::Font& Window::font() const
{
    return _font; 
}


void Window::invalidate()
{
    _impl->invalidate();
}


void Window::render()
{
    if( _mainWidget != 0 )
    {
        _mainWidget->render();
        surface().painter().drawSurface(_mainWidget->position(), _mainWidget->surface() );
    }
    else
    {
         Hmi::Painter& painter = surface().painter();
         painter.setBrush( Pt::Gfx::Color(0,0,0) );
         Pt::Gfx::RectF rect(Pt::Gfx::PointF(0,0), size() );
         painter.fillRect(rect);
    }
    
    _windowManager.render( surface() );
}

} // namespace

} // namespace

