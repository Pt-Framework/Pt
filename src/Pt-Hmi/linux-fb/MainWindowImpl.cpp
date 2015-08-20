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
: _apiWindow(window)
, _app(Application::instance() )
, _forceTopMost(false)
{ 	
  // route all events through MainWindow back to this impl
  eventReceived().disconnectAll();
  eventReceived() += Pt::slot( _apiWindow->eventReceived() );

	windowSurface().impl()->reserve( 1920 * 1080 * 4 );
}


MainWindowImpl::~MainWindowImpl()
{

}


	
void MainWindowImpl::onInvalidate()
{		
	ChildWindow::onInvalidate();
	_apiWindow->invalidate();
}


void MainWindowImpl::onRender(PaintSurface& paintSurface)
{
  _apiWindow->render( paintSurface );
  _apiWindow->windowManager().render();
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


void MainWindowImpl::render()
{
	_app.mainScreen().impl()->invalidate();
}


void MainWindowImpl::show()
{
	Visible = true;	
}


void MainWindowImpl::hide()
{
	Visible = false;
}


void MainWindowImpl::setWindowPos(const Ui::PointF& p)
{	  
	 if( Position.get()  ==  p )
			return;

	Window::setPosition( p );
}


void MainWindowImpl::setWindowSize( const  Ui::SizeF& size )
{   
	 if( Size.get()  == size )
			return;

	Window::setSize( size );
}


void MainWindowImpl::showTitle(bool p)
{
	ShowTitle = p;
}


void MainWindowImpl::setWindowCaption(const std::string& text)
{
	Caption = text;
}


void MainWindowImpl::showMinimizedButton(bool p)
{
	ShowMinimizeButton = p;
}
  

void MainWindowImpl::showMaximizeButton(bool p)
{
	ShowMaximizeButton = p;
}
  

void MainWindowImpl::showSysMenu(bool p)
{
	ShowSysMenu = true;
}


void MainWindowImpl::setForceTopMost(bool force)
{
	_forceTopMost = force;
}
  

void MainWindowImpl::setWindowState( WindowState::Type p )
{
	State = p;
}
  

void MainWindowImpl::setBorder( WindowBorder::Type p )
{
	Border = p;
}
  

void MainWindowImpl::showInTaskbar(bool p)
{
	ShowInTaskbar = p;
}
  

void MainWindowImpl::setIcon(const Ui::Image& p)
{
	Icon = p;
}


void MainWindowImpl::setEnable(bool e)
{
	Enabled = e;
}


void MainWindowImpl::setMinSize(const Ui::SizeF& s)
{
	MinimumSize = s;
}


void MainWindowImpl::setMaxSize(const Ui::SizeF& s)
{
	MaximumSize = s;
}

}} // namespace
