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
, _windowImpl(0)
{ 	

}


MainWindowImpl::~MainWindowImpl()
{
	destroy();
}


void MainWindowImpl::onSizeChanged(const Property<Pt::Gfx::SizeF>& prop)
{
	_apiWindow->Size = prop.get();
}

void MainWindowImpl::onPositionChanged(const Property<Pt::Gfx::PointF>& prop)
{
	_apiWindow->Position = prop.get();
}


void MainWindowImpl::create()
{
	_windowImpl = new ChildWindow();
	_app.mainScreen().impl()->windowManager().add( _windowImpl );

	_windowImpl->Size.Changed += Pt::slot(*this, &MainWindowImpl::onSizeChanged);
	_windowImpl->Position.Changed += Pt::slot(*this, &MainWindowImpl::onPositionChanged);
}
	

void MainWindowImpl::destroy()
{
	if( _windowImpl == 0 )
		return;

	_app.mainScreen().impl()->windowManager().remove( _windowImpl );
	delete _windowImpl;
	_windowImpl = 0;
}


void MainWindowImpl::show()
{
	_windowImpl->Visible = true;
}


void MainWindowImpl::hide()
{
	_windowImpl->Visible = false;
}


void MainWindowImpl::render()
{
	_app.mainScreen().impl()->windowManager().render();
}


void MainWindowImpl::setPosition(const Gfx::PointF& p)
{
	_windowImpl->Position = p;
}

void MainWindowImpl::setSize(const Gfx::SizeF& size)
{
	_windowImpl->Size = size;
}


void MainWindowImpl::showTitle(bool p)
{
	_windowImpl->ShowTitle = p;
}


void MainWindowImpl::setCaption(const std::string& text)
{
	_windowImpl->Caption = text;
}


void MainWindowImpl::showMinimizedButton(bool p)
{
	_windowImpl->ShowMinimizeButton = p;
}
  

void MainWindowImpl::showMaximizeButton(bool p)
{
	_windowImpl->ShowMaximizeButton = p;
}
  

void MainWindowImpl::showSysMenu(bool p)
{
	_windowImpl->ShowSysMenu = true;
}


void MainWindowImpl::setForceTopMost(bool force)
{
	
}
  

void MainWindowImpl::setWindowState(WindowState::Type p)
{
	_windowImpl->State = p;
}
  

void MainWindowImpl::setBorder(WindowBorder::Type p)
{
	_windowImpl->Border = p;
}
  

void MainWindowImpl::showInTaskbar(bool p)
{
	_windowImpl->ShowInTaskbar = p;
}
  

void MainWindowImpl::setIcon(const Pt::Gfx::ARgbImage& p)
{
	_windowImpl->Icon = p;
}


void MainWindowImpl::setEnable(bool e)
{
	_windowImpl->Enabled = e;
}


void MainWindowImpl::setMinSize(const Pt::Gfx::SizeF& s)
{
	_windowImpl->MinimumSize = s;
}
	

void MainWindowImpl::setMaxSize(const Pt::Gfx::SizeF& s)
{
	_windowImpl->MaximumSize = s;
}

}} // namespace
