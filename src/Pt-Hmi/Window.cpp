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
: PT_HMI_INIT_PROPERTY_VALUE(MinimumSize,Ui::SizeF(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(MaximumSize,Ui::SizeF(std::numeric_limits<Pt::uint16_t>::max() ,std::numeric_limits<Pt::uint16_t>::max()))
, PT_HMI_INIT_PROPERTY_VALUE(StartPostion, Hmi::WindowStartPosition::Manual)
, PT_HMI_INIT_PROPERTY_VALUE(State, Hmi::WindowState::Normal)
, PT_HMI_INIT_PROPERTY_VALUE(ShowInTaskbar,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowTitle,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowMinimizeButton,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowMaximizeButton,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowSysMenu,true)	
, PT_HMI_INIT_PROPERTY_VALUE(Border,WindowBorder::Sizeable)
, PT_HMI_INIT_PROPERTY_VALUE(Icon, Ui::Image(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(CanClose,true)
, PT_HMI_INIT_PROPERTY_VALUE(FocuseMoveKey, "\t")
, PT_HMI_INIT_PROPERTY_VALUE(FirstShow,true)
, PT_HMI_INIT_PROPERTY_VALUE(WindowBorder, WindowBorder::Sizeable)
, _winParent(parent)
, _windowManager(*this)
, _isClosed(true)
{

	Visible = false;
	Name = std::string("Window");
	AcceptFocus = false ;	

	eventReceived() += Pt::slot(*this, &Window::onSizeEvent);
	eventReceived() += Pt::slot(*this, &Window::onPositionEvent);
	eventReceived() += Pt::slot(*this, &Window::onFocusEvent);
	eventReceived() += Pt::slot(*this, &Window::onCloseEvent);
	eventReceived() += Pt::slot(*this, &Window::onKeyInput);
	eventReceived() += Pt::slot(*this, &Window::onPointerInput);	
}


Window::~Window()
{
}


PaintSurface* Window::widgetBuffer()
{
	return &_surface;
}


void Window::onInvalidate()
{	  
  render(windowSurface());		
  _windowManager.render();
}

void Window::setSize(const Ui::SizeF& size)
{	
  windowSurface().resize( size );	
  Widget::setSize( size); 
  invalidate();
}
	  	

const std::vector<ChildWindow*>& Window::childWindows() const
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


void Window::onKeyInput(const KeyEvent& ev)
{
	if( !Enabled.get() )
		return;
	
	if( ev.toUTF8String() == FocuseMoveKey.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp )
	{
		if(  ev.shift() )
		{		
				if( ! focusPrev() )
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


void Window::onSizeEvent(const SizeEvent& ev)
{
	Window::setSize( ev.size() );
	Size.changed().send( ev.size() );	
	State = ev.state();
}


void Window::onPositionEvent( const PositionEvent& ev)
{
	Widget::setPosition( ev.position() );
	Position.changed().send( ev.position() );	
}


void Window::onFocusEvent( const FocusEvent& ev)
{
	setFocus( ev.isFocussed() );	
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

}}
