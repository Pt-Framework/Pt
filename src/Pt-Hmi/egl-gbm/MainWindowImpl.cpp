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
#include "MainWindowImpl.h"
#include "PaintSurfaceImpl.h"
#include "ApplicationImpl.h"
#include "ScreenImpl.h"
#include <Pt/Hmi/MainWindow.h>
#include <Pt/Hmi/Application.h>
#include <Pt/System/Logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <linux/types.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

namespace Pt {

namespace Hmi {

MainWindowImpl::MainWindowImpl(MainWindow* window)
: _app(Application::instance() )
, _apiWindow(window)
{ 	
}


MainWindowImpl::~MainWindowImpl()
{
}

	
void MainWindowImpl::onEvent(const Pt::Event& ev)
{
	_app.sendEvent(*_apiWindow, ev);
}


void MainWindowImpl::onInvalidate()
{		
	_app.mainScreen().impl()->update();
}


void MainWindowImpl::onPaint(PaintSurface& paintSurface)
{
  _apiWindow->render();

	Hmi::Painter& painter = paintSurface.painter();
	
	painter.drawSurface( Gfx::PointF(0,0), _apiWindow->surface() );
}


void MainWindowImpl::create()
{
	this->setWindowParent( _app.mainScreen().impl() );
	_app.mainScreen().impl()->windowManager().add( this );
}
	

void MainWindowImpl::destroy()
{
	_app.mainScreen().impl()->windowManager().remove( this );
	this->setWindowParent(0);
}


void MainWindowImpl::show()
{
	setVisible(true);
}


void MainWindowImpl::hide()
{
	setVisible(false);
}


void MainWindowImpl::activate()
{
  _app.mainScreen().impl()->windowManager().activate( this ); 
}


void MainWindowImpl::setWindowPos(const Gfx::PointF& p)
{
	 if( position()  ==  p )
			return;

	Window::setPosition( p );
}


void MainWindowImpl::setWindowSize( const Gfx::SizeF& size )
{
	 if( Window::size()  == size )
			return;

	Window::setSize( size );
}


void MainWindowImpl::showTitle(bool p)
{
	setShowTitle(p);
}


void MainWindowImpl::setWindowCaption(const std::string& text)
{
	setCaption(text);
}


void MainWindowImpl::showMinimizedButton(bool p)
{
	setShowMinimizeButton(p);
}


void MainWindowImpl::showMaximizeButton(bool p)
{
	setShowMaximizeButton(p);
}
  

void MainWindowImpl::showSysMenu(bool p)
{
	setShowSysMenu(true);
}


void MainWindowImpl::setTopMost(bool force)
{
	//ToDo:
}
  

void MainWindowImpl::setWindowState( WindowState::Type p )
{
	setState(p);
}


void MainWindowImpl::setBorder( WindowBorder::Type p )
{
	setBorder(p);
}


void MainWindowImpl::showInTaskbar(bool p)
{
	setShowInTaskbar(p);
}


void MainWindowImpl::setIcon(const Gfx::Image& p)
{
	setIcon(p);
}


void MainWindowImpl::setEnable(bool e)
{
	setEnabled(e);
}


void MainWindowImpl::setMinSize(const Gfx::SizeF& s)
{
	setMinimumSize(s);
}


void MainWindowImpl::setMaxSize(const Gfx::SizeF& s)
{
	setMaximumSize(s);
}

void MainWindowImpl::bringToFront()
{
  //ToDo:
}

void MainWindowImpl::focus()
{
  //ToDo:
}


}} // namespace
