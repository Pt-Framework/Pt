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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "MainWindowImpl.h"
#include "PaintSurfaceImpl.h"
#include "ApplicationImpl.h"
#include "ScreenImpl.h"
#include <Pt/Hmi/Application.h>
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

MainWindowImpl::MainWindowImpl(Window* window)
: _apiWindow(window)
, _app(Application::instance() )
{ 	

}


MainWindowImpl::~MainWindowImpl()
{

}


void MainWindowImpl::onPointerInput( const PointerEvent& ev )
{
  _apiWindow->eventReceived().send(ev);
}


void MainWindowImpl::onKeyInput( const KeyEvent& ev )
{
  _apiWindow->eventReceived().send( ev );
}

	
void MainWindowImpl::onInvalidate()
{
	ChildWindow::onInvalidate();

	_apiWindow->invalidate();
}


void MainWindowImpl::onRender()
{
  ChildWindow::onRender();

	Painter& painter = paintSurface().painter();
  
  painter.drawSurface( Ui::PointF(0, 0), _apiWindow->paintSurface() );	

  _windowManager.render();
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
	Visible = true;	
	render();
}


void MainWindowImpl::hide()
{
	Visible = false;
	render();
}


void MainWindowImpl::setPos(const Ui::PointF& p)
{	
	if( Position.get() != p ) 
	{
		Position = p;
		render();
	}
}


void MainWindowImpl::setSize( const  Ui::SizeF& size )
{  
 
	if( Size.get() != size )
		Size = size;
}


void MainWindowImpl::showTitle(bool p)
{
	ShowTitle = p;
}


void MainWindowImpl::setCaption(const std::string& text)
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
