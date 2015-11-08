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
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/ChildWindow.h>

namespace Pt{
namespace Hmi{

Window::Window(Window* parent)
: PT_HMI_INIT_PROPERTY_VALUE(MinimumSize,Gfx::SizeF(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(MaximumSize,Gfx::SizeF(std::numeric_limits<Pt::uint16_t>::max() ,std::numeric_limits<Pt::uint16_t>::max()))
, PT_HMI_INIT_PROPERTY_VALUE(StartPostion, Hmi::WindowStartPosition::Manual)
, PT_HMI_INIT_PROPERTY_VALUE(State, Hmi::WindowState::Normal)
, PT_HMI_INIT_PROPERTY_VALUE(ShowInTaskbar,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowTitle,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowMinimizeButton,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowMaximizeButton,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowSysMenu,true)	
, PT_HMI_INIT_PROPERTY_VALUE(Border,WindowBorder::Sizeable)
, PT_HMI_INIT_PROPERTY_VALUE(Icon,Gfx::Image(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(CanClose,true)
, PT_HMI_INIT_PROPERTY_VALUE(FocuseMoveKey, "\t")
, PT_HMI_INIT_PROPERTY_VALUE(FirstShow,true)
, PT_HMI_INIT_PROPERTY_VALUE(WindowBorder, WindowBorder::Sizeable)
, _winParent(parent)
, _windowManager(*this)
, _isClosed(true)
, _isActive(false)
, _pointedWidget( 0 )
{
	Name = std::string("Window");
	AcceptFocus = false ;	

	eventReceived() += Pt::slot(*this, &Window::onMoveEvent);
	eventReceived() += Pt::slot(*this, &Window::onResizeEvent);
	eventReceived() += Pt::slot(*this, &Window::onActivateEvent);
	eventReceived() += Pt::slot(*this, &Window::onCloseEvent);
}


Window::~Window()
{
}
	

const std::vector<ChildWindow*> Window::childWindows() const
{
	return _windowManager.windows();
}


void Window::addChildWindow(ChildWindow& w)
{
	w.setWindowParent(this);
	_windowManager.add(&w);		
}


void Window::removeChildWindow(ChildWindow& w)
{
	_windowManager.remove(&w);
	w.setWindowParent(0);
}


void Window::setWindowParent(Window* parent)
{
	_winParent = parent;
}


Window* Window::windowParent() const
{
	return _winParent;
}


void Window::activate()
{
	onActivate();
}


void Window::setPointedWidget( Widget* widget ) 
{
	if( _pointedWidget == widget )
		return;

	if( _pointedWidget )			
		_pointedWidget->onPointerLeaved();

	_pointedWidget = widget;

	if( _pointedWidget )
		_pointedWidget->onPointerEnter();
}


void Window::onPointerInput(const PointerEvent& ev)
{	
  if( _windowManager.pointerInput( ev ) )
	{
			this->setPointedWidget( 0 );	
      return;
	}

	Widget* widget = findWidget( Gfx::PointF( ev.x(), ev.y() ) );

	this->setPointedWidget( widget );	

	if( widget && widget != this )
		widget->processEvent(ev);		
}



void Window::onKeyInput(const KeyEvent& ev)
{
  if( _windowManager.keyInput( ev ) )
      return;

	if( !Enabled.get() )
		return;
	
	if( ev.toUTF8String() == FocuseMoveKey.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp )
	{
		if(  ev.shift() )
		{		
				if( !focusPrev() )
					focusPrev();
		}
		else
		{
				if( ! focusNext() )
					focusNext();
		}

		invalidate();
	}

  Widget::onKeyInput(ev);
}


void Window::onResizeEvent(const ResizeEvent& ev)
{	
	Window::setSize( ev.size() );
	Size.changed().send( ev.size() );	
	State = ev.state();
	invalidate();
}


void Window::onMoveEvent( const MoveEvent& ev)
{
	Widget::setPosition( ev.position() );
	Position.changed().send( ev.position() );	
	invalidate();
}


void Window::onActivateEvent(const ActivateEvent& ev)
{ 
	_isActive = ev.isActive();

	if( ! _isActive )
	{
		_windowManager.deactivate();
		return;
	}
	  
	invalidate();
}


void Window::close()
{
	setClosed(true);	
}


void Window::onCloseEvent(const CloseEvent& ev)
{
	setClosed(true);
}

void Window::setClosed( bool c)
{
	_isClosed = c;

	if( _isClosed )
		Closed.send();							
}		


void Window::onRender( PaintSurface& surface )
{
	Widget::onRender( surface );
	_windowManager.render();
}

}}
