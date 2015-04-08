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

	eventReceived() += Pt::slot( _windowManager, &WindowManager::onPointerInput );
	eventReceived() += Pt::slot( _windowManager, &WindowManager::onKeyInput );
	
	Visible = false;
	Focused = true;
  Name = std::string("Window");
	AcceptFocus = false;
		
  Position.Changed += Pt::slot(*this, &MainWindow::onPositionChanged);
	Size.Changed += Pt::slot(*this, &MainWindow::onSizeChanged);
  Closed.Changed += Pt::slot(*this, &MainWindow::onClosedChanged);
  Visible.Changed += Pt::slot(*this, &MainWindow::onVisibleChanged);
  Caption.Changed += Pt::slot(*this, &MainWindow::onCaptionChanged);
  ShowTitle.Changed += Pt::slot(*this, &MainWindow::onShowTitleChanged);
  ShowMinimizeButton.Changed += Pt::slot(*this, &MainWindow::onShowMinimizedButtonChanged);
  ShowMaximizeButton.Changed += Pt::slot(*this, &MainWindow::onShowMaximizeButtonChanged);
  ShowSysMenu.Changed += Pt::slot(*this, &MainWindow::onShowSysMenuChanged);
  State.Changed += Pt::slot(*this, &MainWindow::onWindowStateChanged);
  Border.Changed += Pt::slot(*this, &MainWindow::onBorderChanged);
  ShowInTaskbar.Changed += Pt::slot(*this, &MainWindow::onShowInTaskbarChanged);
  Icon.Changed += Pt::slot(*this, &MainWindow::onIconChanged);
	Enabled.Changed += Pt::slot(*this, &MainWindow::onEnabledChanged);
	MinimumSize.Changed += Pt::slot(*this, &MainWindow::onMinSizeChnaged);
	MaximumSize.Changed += Pt::slot(*this, &MainWindow::onMaxSizeChnaged);

  Position = Pt::Gfx::PointF(20,20);
	Size =  Pt::Gfx::SizeF(200,200);
	_impl->setMinSize(MinimumSize.get());
	_impl->setMaxSize(MaximumSize.get());
}


MainWindow::~MainWindow()
{
	delete _impl;
}


void MainWindow::onInvalidate()
{
	render();		

	_windowManager.render();
	_impl->render();		
}

void MainWindow::setTopMost(bool topMost)
{
	_impl->setTopMost( topMost );
}

void MainWindow::onPositionChanged(const Property<Pt::Gfx::PointF>& prop)
{
  _impl->setPosition( prop.get() );
}


void MainWindow::onSizeChanged(const Property<Pt::Gfx::SizeF>& prop)
{ 
  _impl->setSize( Size.get() );  			
}


void MainWindow::onClosedChanged(const Property<bool> & closed)
{	
	if( !Enabled.get() )
		return;

	if( !CanClose.get() )
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


void MainWindow::onVisibleChanged(const Property<bool> & visible)
{
  //Set the closed flag
	if( visible.get() )
	{
    if( FirstShow.get() )
    {
      if( windowParent() != 0  && StartPostion.get() == WindowStartPosition::CenterParent )
      {
          double x = windowParent()->Position.get().x() + (windowParent()->Size.get().width()/2  - Size.get().width()/2);
					double y = windowParent()->Position.get().y() + (windowParent()->Size.get().height()/2  - Size.get().height()/2);
					Position = Gfx::PointF(x,y);
      }  

      FirstShow = false;
    }

    _impl->show();
	}
  else
  {
		_impl->hide();
  }

	invalidate();
}


void MainWindow::onCaptionChanged(const Property<std::string> & p)
{
  _impl->setCaption( p.get() );
}


void MainWindow::onShowTitleChanged(const Property<bool> & p)
{
  _impl->showTitle( p.get() );
}


void MainWindow::onShowMinimizedButtonChanged(const Property<bool> & p)
{
  _impl->showMinimizedButton( p.get() );
}


void MainWindow::onShowMaximizeButtonChanged(const Property<bool> & p)
{
  _impl->showMaximizeButton( p.get() );
}


void MainWindow::onShowSysMenuChanged(const Property<bool> & p)
{
  _impl->showSysMenu( p.get() );
}

void MainWindow::onWindowStateChanged(const Property<WindowState::Type> & p)
{
  _impl->setWindowState( p.get() );
}


void MainWindow::onBorderChanged(const Property<WindowBorder::Type> & p)
{
  _impl->setBorder( p.get() );

	Size.Changed.send(Size); //Notify size changed
}


void MainWindow::onShowInTaskbarChanged(const Property<bool> & p)
{
  _impl->showInTaskbar( p.get() );
}


void MainWindow::onIconChanged(const Property<Pt::Gfx::ARgbImage> & p)
{
  _impl->setIcon( p.get() );
}


void MainWindow::onEnabledChanged(const Property<bool> & p)
{
	_impl->setEnable( p.get() );
}

void MainWindow::onMinSizeChnaged(const Property<Pt::Gfx::SizeF>& prop)
{
	_impl->setMinSize( prop.get() );
}
		
void MainWindow::onMaxSizeChnaged(const Property<Pt::Gfx::SizeF>& prop)
{
	_impl->setMaxSize( prop.get() );
}

}}

