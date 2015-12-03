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
#include "WindowImpl.h"
#include "MainWindowImpl.h"
#include "ChildWindowImpl.h"

namespace Pt {

namespace Hmi {

Window::Window()
: _impl(0)
{
    _eventReady += Pt::slot(*this, &Window::onKeyEvent);
    _eventReady += Pt::slot(*this, &Window::onPointerEvent);  
    _eventReady += Pt::slot(*this, &Window::onMoveEvent);
    _eventReady += Pt::slot(*this, &Window::onResizeEvent);
    _eventReady += Pt::slot(*this, &Window::onActivateEvent);
    _eventReady += Pt::slot(*this, &Window::onCloseEvent);
    
    _impl = new MainWindowImpl(this);
}


Window::~Window()
{
    delete _impl;
}


const std::vector<Window*>& Window::windows() const
{
    return _impl->windows();
}

void Window::add(Window& window)
{    
    _impl->add( window );
}


void Window::remove(Window& window)
{    
    _impl->remove( window);
}


 Window* Window::parent()
 {
    return _impl->parent();
 }

const Window* Window::parent() const
{
    return _impl->parent();
}
    

Widget* Window::mainWidget() 
{
    return _impl->mainWidget();
}

const Widget* Window::mainWidget()  const 
{
    return _impl->mainWidget();
}

Window* Window::findWindow(const std::string& name)
{
    return _impl->findWindow( name);    
}


void Window::setMainWidget(Widget* widget)
{   
    _impl->setMainWidget( widget);
}


Widget* Window::findWidget(const std::string& name)
{
    return  _impl->findWidget(name);
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
    _impl->onPointerEvent(ev);        
}


void Window::onKeyEvent(const KeyEvent& ev)
{
    _impl->onKeyEvent(ev);
}


void Window::onResizeEvent(const ResizeEvent& ev)
{    
    _impl->onResizeEvent(ev);
}


void Window::onMoveEvent( const MoveEvent& ev)
{
    _impl->onMoveEvent(ev);        
}


void Window::onActivateEvent(const ActivateEvent& ev)
{ 
    _impl->onActivateEvent(ev);
}


void Window::onCloseEvent(const CloseEvent& ev)
{
    _impl->onCloseEvent(ev);
}


bool Window::isClosed() const
{
  return _impl->isClosed();       
}


bool Window::isActive() const
{
    return _impl->isActive();
}


const Gfx::SizeF& Window::minimumSize() const
{
    return _impl->minimumSize();
}


void Window::setMinimumSize(const Gfx::SizeF& s)
{
    _impl->setMinimumSize(s);
}


const Gfx::SizeF& Window::maximumSize() const
{
    return _impl->maximumSize();
}


void Window::setMaximumSize(const Gfx::SizeF& s)
{
    _impl->setMaximumSize(s);
}


Hmi::WindowPosition::Type Window::defaultPosition() const
{
    return _impl->defaultPosition();
}


void Window::setDefaultPosition( Hmi::WindowPosition::Type p)
{
    _impl->setDefaultPosition(p);
}


Hmi::WindowState::Type Window::state() const
{
    return _impl->state();
}


void Window::setState( WindowState::Type s)
{
    _impl->setState(s);
}


WindowBorder::Type Window::border() const
{
    return _impl->border();
}


void Window::setBorder(WindowBorder::Type t)
{
    _impl->setBorder(t);
}


const Gfx::Image& Window::icon() const
{
    return _impl->icon();
}


void Window::setIcon(const Gfx::Image& i)
{
    _impl->setIcon(i);
}


bool Window::isClosable() const
{
    return _impl->isClosable();
}


void Window::setClosable(bool c)
{
    _impl->setClosable(c);
}


const std::string& Window::title() const
{
    return _impl->title();
}


void Window::setTitle( const std::string& t )
{
    _impl->setTitle(t);
}


bool Window::isEnabled() const
{
    return _impl->isEnabled();
}


void Window::setEnabled( bool e )
{
    _impl->setEnabled(e);
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
    return _impl->isVisible();
}


const Gfx::SizeF& Window::size() const
{
    return _impl->size();
}


void Window::setSize( const Gfx::SizeF& s )
{
    _impl->setSize(s);
}


const Gfx::PointF& Window::position() const
{
    return _impl->position();
}


void Window::setPosition( const Gfx::PointF& p)
{
    _impl->setPosition(p);
}

void Window::setFont(const Gfx::Font& ft)
{
    _impl->setFont(ft);
}
    
const Gfx::Font& Window::font() const
{
    return _impl->font(); 
}


void Window::removeWidget(Widget& w)
{
    _impl->removeWidget(w);
}

void Window::invalidate()
{
    _impl->invalidate();
}

PaintSurface& Window::surface()
{
    return _impl->surface();
}

void Window::render()
{
    _impl->render();
}

} // namespace

} // namespace

