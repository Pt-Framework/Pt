/* Copyright (C) 2013 Marc Boris Duerner 
 * Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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


#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Application.h>
#include "WindowImpl.h"
#include <Pt/Hmi/PositionEvent.h>
#include <Pt/Gfx/Size.h>

namespace Pt{
namespace Hmi{

Window::Window()
: _impl(new WindowImpl(&paintSurface() ) )
, PT_HMI_INIT_PROPERTY_VALUE(MinimumSize,Pt::Gfx::SizeF(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(MaximumSize,Pt::Gfx::SizeF(65535,65535))
, PT_HMI_INIT_PROPERTY_VALUE(StartPostion, WindowStartPosition::Manual)
, PT_HMI_INIT_PROPERTY_VALUE(State, WindowState::Normal)
, PT_HMI_INIT_PROPERTY_VALUE(ShowInTaskbar,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowTitle,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowMinimizeButton,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowMaximizeButton,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowSysMenu,true)	
, PT_HMI_INIT_PROPERTY_VALUE(Caption,"")
, PT_HMI_INIT_PROPERTY_VALUE(Border,WindowBorder::Sizeable)
, PT_HMI_INIT_PROPERTY_VALUE(Icon, Pt::Gfx::ARgbImage(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(Closed,false)
, PT_HMI_INIT_PROPERTY_VALUE(CanClose,true)
, PT_HMI_INIT_PROPERTY_VALUE(TopMost, false)
, PT_HMI_INIT_PROPERTY_VALUE(FocuseMoveKey, "\t")
{
	Visible.set(false);
	Focused.set(true);
  Name.set("Window");
		
   Size.Changed += Pt::slot(*this, &Window::onSizeChanged);	
  Position.Changed += Pt::slot(*this, &Window::onPositionChanged);
  Closed.Changed += Pt::slot(*this, &Window::onClosedChanged);
  Visible.Changed += Pt::slot(*this, &Window::onVisibleChanged);
  Caption.Changed += Pt::slot(*this, &Window::onCaptionChanged);
  ShowTitle.Changed += Pt::slot(*this, &Window::onShowTitleChanged);
  ShowMinimizeButton.Changed += Pt::slot(*this, &Window::onShowMinimizedButtonChanged);
  ShowMaximizeButton.Changed += Pt::slot(*this, &Window::onShowMaximizeButtonChanged);
  ShowSysMenu.Changed += Pt::slot(*this, &Window::onShowSysMenuChanged);
  TopMost.Changed  += Pt::slot(*this, &Window::onTopMostChanged);
  State.Changed += Pt::slot(*this, &Window::onWindowStateChanged);
  Border.Changed += Pt::slot(*this, &Window::onBorderChanged);
  ShowInTaskbar.Changed += Pt::slot(*this, &Window::onShowInTaskbarChanged);
  Icon.Changed += Pt::slot(*this, &Window::onIconChanged);

	_impl->windowEvent() += Pt::slot(*this, &Window::onPointerInput);
	_impl->windowEvent() += Pt::slot(*this, &Window::onKeyInput);	
	_impl->windowEvent() += Pt::slot(*this, &Window::onResizeEvent);
	_impl->windowEvent() += Pt::slot(*this, &Window::onPositionEvent);
  _impl->windowEvent() += Pt::slot(*this, &Window::onCloseEvent);	

  Position = Pt::Gfx::PointF(20,20);
	Size =  Pt::Gfx::SizeF(200,200);

}

Window::~Window()
{
}

Pt::Signal<const Pt::Event&>& Window::eventReady()
{
  return _impl->windowEvent();
}


void Window::onInvalidate()
{
	render();
	_impl->render();
}


void Window::onKeyInput(const KeyEvent& ev)
{
	if( ! Enabled.get() )
		return;

	if(ev.toUTF8String() == FocuseMoveKey.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp && !ev.shift())
	{
		if( Focused.get())
		{
			if( !focusNext() )
				Focused = true;

			invalidate();
		}
	}

	if( ev.toUTF8String() ==  FocuseMoveKey.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp && ev.shift() )
	{
		if( Focused.get() )
		{
			if( !focusPrev() )
				Focused = true;

			invalidate();
		}
	}

  Widget::onKeyInput(ev);
}


void Window::onResizeEvent(const ResizeEvent& ev)
{
	const Pt::Gfx::SizeF& curSize = Size.get();

	if( curSize == ev.size() ) 
		return;

  State = ev.state();
	Size = ev.size();    
}


void Window::onPositionEvent(const PositionEvent& ev)
{
	const Pt::Gfx::PointF& curPos = Position.get();

	if( curPos == ev.position() ) 
		return;

	Position = ev.position();  
}


void Window::onCloseEvent(const CloseEvent& ev)
{
  Closed = true;
}


void Window::onPositionChanged(const Property<Pt::Gfx::PointF>& prop)
{
  _impl->setPosition( prop.get() );
}


void Window::onSizeChanged(const Property<Pt::Gfx::SizeF>& prop)
{
  _impl->setSize( prop.get() );  
}


void Window::onClosedChanged(const Property<bool> & closed)
{	
	if( ! CanClose.get() )
		return;
    
  //Set the closed flag
	if( closed.get() )
	{
    _impl->destroy();
	}
  else
  {
		_impl->create();
  }
}


void Window::onVisibleChanged(const Property<bool> & visible)
{
  //Set the closed flag
	if( visible.get() )
	{
    _impl->show();
	}
  else
  {
		_impl->hide();
  }
}


void Window::onCaptionChanged(const Property<std::string> & p)
{
  _impl->setCaption( p.get() );
}


void Window::onShowTitleChanged(const Property<bool> & p)
{
  _impl->showTitle( p.get() );
}


void Window::onShowMinimizedButtonChanged(const Property<bool> & p)
{
  _impl->showMinimizedButton( p.get() );
}


void Window::onShowMaximizeButtonChanged(const Property<bool> & p)
{
  _impl->showMaximizeButton( p.get() );
}


void Window::onShowSysMenuChanged(const Property<bool> & p)
{
  _impl->showSysMenu( p.get() );
}


void Window::onTopMostChanged(const Property<bool> & p)
{
  if( p.get() )
    _impl->setTopMost();
}


void Window::onWindowStateChanged(const Property<WindowState::Type> & p)
{
  _impl->setWindowState( p.get() );
}


void Window::onBorderChanged(const Property<WindowBorder::Type> & p)
{
  _impl->setBorder( p.get() );
}


void Window::onShowInTaskbarChanged(const Property<bool> & p)
{
  _impl->showInTaskbar( p.get() );
}


void Window::onIconChanged(const Property<Pt::Gfx::ARgbImage> & p)
{
  _impl->setIcon( p.get() );
}

}}
