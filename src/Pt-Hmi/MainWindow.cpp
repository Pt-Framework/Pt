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

	setVisible(false);
	setAcceptFocus(false);
		  
	//Todo: change from ValueProperty to Property.
  /*
  Visible.changed() += Pt::slot(*this, &MainWindow::onVisibleChanged);
  ShowTitle.changed() += Pt::slot(*this, &MainWindow::onShowTitleChanged);
  ShowMinimizeButton.changed() += Pt::slot(*this, &MainWindow::onShowMinimizedButtonChanged);
  ShowMaximizeButton.changed() += Pt::slot(*this, &MainWindow::onShowMaximizeButtonChanged);
  ShowSysMenu.changed() += Pt::slot(*this, &MainWindow::onShowSysMenuChanged);
  State.changed() += Pt::slot(*this, &MainWindow::onWindowStateChanged);
  Border.changed() += Pt::slot(*this, &MainWindow::onBorderChanged);
  ShowInTaskbar.changed() += Pt::slot(*this, &MainWindow::onShowInTaskbarChanged);
  Icon.changed() += Pt::slot(*this, &MainWindow::onIconChanged);
	Enabled.changed() += Pt::slot(*this, &MainWindow::onEnabledChanged);
	MinimumSize.changed() += Pt::slot(*this, &MainWindow::onMinSizeChnaged);
	MaximumSize.changed() += Pt::slot(*this, &MainWindow::onMaxSizeChnaged);
  */
  setPosition( Gfx::PointF(20,20) );
	setSize(Gfx::SizeF(200,200));
	_impl->setMinSize(minimumSize());
	_impl->setMaxSize(maximumSize());
	setClosed(false);
}


MainWindow::~MainWindow()
{
	//TODO: remove from window manager
	_windowManager.clear();

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


void MainWindow::onClosedChanged(const bool& closed)
{	
	if( !isEnabled() )
		return;

	if( !canClose() )
		return;
    
  //Set the closed flag
	if( closed )
    _impl->destroy();
  else
		_impl->create();
}


void MainWindow::onVisibleChanged(const bool& visible)
{
	if( isClosed() )
		setClosed(false);

  //Set the closed flag
	if( visible )
	{
    if( firstShow() )
    {
      if( windowParent() != 0  && startPostion() == WindowStartPosition::CenterParent )
      {
          double x = windowParent()->position().x() + (windowParent()->size().width()/2  - size().width()/2);
					double y = windowParent()->position().y() + (windowParent()->size().height()/2  - size().height()/2);
					setPosition( Gfx::PointF(x,y) );
      }  

      setFirstShow(false);
    }

    _impl->show();
	}
  else
  {
		_impl->hide();
  }

	setClosed(false);
	invalidate();
}


void MainWindow::setCaption( const std::string& c )
{
	Widget::setCaption( c );
  _impl->setWindowCaption( c );
}


void MainWindow::onShowTitleChanged(const bool& p)
{
  _impl->showTitle( p );
}


void MainWindow::onShowMinimizedButtonChanged(const bool& p)
{
  _impl->showMinimizedButton( p );
}


void MainWindow::onShowMaximizeButtonChanged(const bool& p)
{
  _impl->showMaximizeButton( p );
}


void MainWindow::onShowSysMenuChanged(const bool& p)
{
  _impl->showSysMenu( p );
}

void MainWindow::onWindowStateChanged(const WindowState::Type& p)
{
  _impl->setWindowState( p );
}


void MainWindow::onBorderChanged(const WindowBorder::Type& p)
{
  _impl->setBorder( p );
}


void MainWindow::onShowInTaskbarChanged(const bool& p)
{
  _impl->showInTaskbar( p );
}


void MainWindow::onIconChanged(const Gfx::Image& p)
{
  _impl->setIcon( p );
}


void MainWindow::onEnabledChanged(const bool& p)
{
	_impl->setEnable( p );
}

void MainWindow::onMinSizeChnaged(const Gfx::SizeF& prop)
{
	_impl->setMinSize( prop );
}
		
void MainWindow::onMaxSizeChnaged(const Gfx::SizeF& prop)
{
	_impl->setMaxSize( prop );
}

void MainWindow::setSize(const Gfx::SizeF& size)
{
	Widget::setSize( size); 	
	_impl->setWindowSize(size);
}

void MainWindow::setPosition(const Gfx::PointF& pos)
{
	Widget::setPosition( pos);
	_impl->setWindowPos( pos);
}

void MainWindow::onActivate()
{
		_impl->activate(); 
}


void MainWindow::onVisible( bool b )
{
  if(b )
     _impl->show();
  else
    _impl->hide();
    
    Window::onVisible( b);
}

void MainWindow::setClosed(bool close)
{
	if( !isEnabled() )
		return;

	if( close )
	{

		if( !canClose() || isClosed() )
			return;
	
		_impl->destroy();
		setVisible (false);				
						
	}
	else
	{
		
		if(  !isClosed()  )
			return;

		_impl->create();
	}

	Window::setClosed(close);	
}

}}

