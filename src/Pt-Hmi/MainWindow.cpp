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
#include <Pt/Hmi/MainWindow.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Application.h>
#include "MainWindowImpl.h"
#include <Pt/Gfx/Size.h>

namespace Pt{
namespace Hmi{

MainWindow::MainWindow(MainWindow* parent)
: _impl(0)
{
	_impl = new MainWindowImpl( this );
	
	_impl->create();
	_impl->setMinimumSize(minimumSize());
	_impl->setMaximumSize(maximumSize());
	
	Hmi::Application::instance().registerWindow(*this);
}


MainWindow::~MainWindow()
{
	Hmi::Application::instance().unregisterWindow(*this);

	_impl->destroy();
	delete _impl;
}


void MainWindow::onInvalidate()
{
	_impl->invalidate();	
}


void MainWindow::setTopMost(bool topMost)
{
	_impl->setTopMost( topMost );
}


void MainWindow::onActivate()
{
		_impl->activate(); 
}


void MainWindow::onSetVisible( bool b )
{
  if(b )
     _impl->show();
  else
    _impl->hide();
    
    Window::onSetVisible( b);
}

void MainWindow::onSetPosition(const Gfx::PointF& pos ) 
{
	_impl->setPosition( pos );
	Window::onSetPosition( pos );

}

void MainWindow::onSetSize(const Gfx::SizeF& size)
{
	_impl->setSize( size );
	Window::onSetSize( size );
}

void MainWindow::onShowTitle( bool s )
{
	_impl->setShowTitle( s );
	Window::onShowTitle( s );
}


void MainWindow::onSetCaption( const std::string& s )
{
	_impl->setCaption( s );
	Window::onSetCaption( s );
}

void MainWindow::onShowMinimizeButton( bool s )
{
	_impl->setShowMinimizeButton( s );
	Window::onShowMinimizeButton( s );
}


void MainWindow::onShowMaximizeButton( bool s )
{
	_impl->setShowMaximizeButton( s );
	Window::onShowMaximizeButton( s );
}


void MainWindow::onShowSystemMenu( bool  s )
{
	_impl->setShowSystemMenu( s );
	Window::onShowSystemMenu( s );
}


void MainWindow::onState(const Hmi::WindowState::Type& s)
{
	_impl->setState( s );
	Window::onState( s );
}


void MainWindow::onBorder(const Hmi::WindowBorder::Type& b)
{
	_impl->setBorder( b );
	Window::onBorder( b );
}


void MainWindow::onShowInTaskbar(bool s)
{
	_impl->setShowInTaskbar( s );
	Window::onShowInTaskbar( s );
}


void MainWindow::onIcon(const Gfx::Image& i)
{
	_impl->setIcon( i );
	Window::onIcon( i );
}

void MainWindow::onSetEnabled( bool e )
{
	_impl->setEnabled( e );
	Window::onSetEnabled( e );	
}

void MainWindow::onSetMinimumSize( const Gfx::SizeF& s )
{
	_impl->setMinimumSize( s );
	Window::onSetMinimumSize( s );	
}


void MainWindow::onSetMaximumSize(const Gfx::SizeF& s)
{
	_impl->setMaximumSize( s );
	Window::onSetMaximumSize( s );	
}

void MainWindow::onClose()
{
	if( !isEnabled() )
		return;

	if( !canClose() || isClosed() )
		return;
	
	_impl->destroy();
					
	Window::onClose();	
}

}}

