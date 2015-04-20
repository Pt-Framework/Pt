/* 
* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
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
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/ChildWindow.h>
#include <Pt/Hmi/PointerEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>

namespace Pt {
	namespace Hmi {

		WindowManager::WindowManager(Window& parent)
			: _parent( parent )
			, _sizingDirection( ResizeDirection::No )
			, _app( Application::instance() )
		{
		}


		WindowManager::~WindowManager()
		{
		}


		ChildWindow* WindowManager::active()
		{	
			if( _windows.size() == 0)
				return 0;

			return _windows[ _windows.size() - 1 ];
		}


		void WindowManager::updateFocus()
		{
			if( _windows.size() == 0 )
				return;

			for( size_t i = 0; i < _windows.size() - 1; ++i )
				_windows[i]->Focused = false;

			_windows[ _windows.size() - 1 ]->Focused = true;
		}


		void WindowManager::add( ChildWindow* w )
		{
			_windows.push_back(w);

			updateFocus();

			invalidate();
		}


		void WindowManager::remove( ChildWindow* w )
		{
			std::vector<ChildWindow*>::iterator it = std::find(_windows.begin(), _windows.end(), w);

			if( it == _windows.end() )
				return;

			_windows.erase( it );

			updateFocus();			
			invalidate();
		}


		void WindowManager::activate( ChildWindow* w )
		{
			std::vector<ChildWindow*>::iterator it = std::find(_windows.begin(), _windows.end(), w);

			if( it == _windows.end() )
				return;

			_windows.erase( it );	
			_windows.push_back(w);

			updateFocus();	
		}


		void WindowManager::invalidate()
		{
			_parent.invalidate();
		}


		void WindowManager::render()
		{		

			Painter& painter = _parent.paintSurface().painter();

			for( size_t i = 0; i < _windows.size(); ++i )
			{
				ChildWindow* w = _windows[i];
				w->render();
				painter.drawSurface( w->Position.get(), w->paintSurface() );
			}
		}


		 Gfx::PointF WindowManager::toClient(const ChildWindow* w, Gfx::PointF& p)
		{
			Pt::Gfx::PointF client;

				if( w->titleBar() != 0 ) 
				{
					client =  Gfx::PointF( p.x() - w->Position.get().x() - w->BorderWidth.get() , 
						p.y() - w->Position.get().y() -  w->titleBar()->Size.get().height() ) ;
				}
				else
				{
					client =  Gfx::PointF( p.x() - w->Position.get().x() - w->BorderWidth.get() , 
						p.y() - w->Position.get().y() - w->BorderWidth.get() ) ;
				}
				return client;
		}

		bool WindowManager::updateActive( const Pt::Hmi::PointerEvent& mouseEvent )
		{
			for( int i = _windows.size() - 1;  i > -1; --i )
			{
				Pt::Gfx::PointF local( mouseEvent.x() - _windows[i]->Position.get().x() , mouseEvent.y() - _windows[i]->Position.get().y() );

				if( !contains( _windows[i], local ) )
					continue;
				
				const Pt::Gfx::PointF& client = toClient( _windows[i], Gfx::PointF( mouseEvent.x(), mouseEvent.y() ) );
				 
				if( _windows[i]->hasPointer()  && ! _windows[i]->contains( client ) )
					_windows[i]->onPointerLeaved();
							
				if( _windows[i] == active() )
					return true;

				if( mouseEvent.buttons()[0].state() != DeviceButton::Pressed )
					return false;
					
				activate( _windows[i] );			
				return true;
			}	 

			_app.setCursor( &_parent.Cursor.get() );
			return false;
		}


		void WindowManager::onKeyInput( const Pt::Hmi::KeyEvent& keyEvent )
		{
			ChildWindow* w = active();

			if( w == 0 )
				return;

			w->eventReceived().send( keyEvent );		
		}


		void WindowManager::doSizing( ChildWindow* w, const PointerEvent& ev )
		{
			const std::vector<DeviceButton>& button = ev.buttons();

			Pt::Gfx::PointF point( ev.x(), ev.y() );

			if( button[0].state() != DeviceButton::Pressed )
			{
				_sizingDirection = ResizeDirection::No;
				_lastSizePoint =  point;
				return;
			}	

			double width  = w->winSize().width();
			double height = w->winSize().height();
			double posX   = w->Position.get().x();
			double posY   = w->Position.get().y();
			double deltaX = ( point.x() - _lastSizePoint.x() );
			double deltaY = ( point.y() - _lastSizePoint.y() );

			switch( _sizingDirection )
			{
			case  ResizeDirection::North:
				{			
					posY +=  deltaY;
					height -= deltaY;
				}
				break;

			case ResizeDirection::NorthEast:
				{
					posY +=  deltaY;
					height -= deltaY;
					width += deltaX;
				}
				break;

			case ResizeDirection::East:
				{
					width += deltaX;
				}
				break;

			case ResizeDirection::SouthEast:
				{
					height += deltaY;
					width += deltaX;
				}
				break;

			case ResizeDirection::South:
				{
					height += deltaY;
				}
				break;

			case ResizeDirection::SouthWest:
				{
					height += deltaY;
					posX +=  deltaX;
					width -= deltaX;
				}
				break;

			case ResizeDirection::West:
				{
					posX +=  deltaX;
					width -= deltaX;
				}		
				break;

			case ResizeDirection::NorthWest:
				{
					posX +=  deltaX;
					width -= deltaX;
					posY +=  deltaY;
					height -= deltaY;
				}
				break;
			default:
				{

				}
				break;
			}

			if( w->Position.get() != Pt::Gfx::PointF(posX, posY) )
				w->Position =  Pt::Gfx::PointF(posX, posY) ;

			if( w->winSize() != Pt::Gfx::SizeF(width, height) )
				w->Size = sizeFromWinSize(w, width, height);

			_lastSizePoint = point;
			invalidate();
		}


		Pt::Gfx::SizeF WindowManager::sizeFromWinSize( const ChildWindow* w, double width, double height)
		{
			if( w->titleBar() != 0 )
				return Pt::Gfx::SizeF( width - w->BorderWidth.get() * 2, height - w->BorderWidth.get() - w->titleBar()->Size.get().height() );
			else
				return Pt::Gfx::SizeF( width - w->BorderWidth.get() * 2, height - w->BorderWidth.get() * 2 );
		}


		bool WindowManager::contains(const ChildWindow* w, const Pt::Gfx::PointF& p)
		{  
			if( p.x() < w->winSize().width() && p.x() >= 0 && p.y() < w->winSize().height() && p.y() >= 0)
				return true;

			return false;
		}

		ResizeDirection::Type WindowManager::detSizeDirection( ChildWindow* w, const Pt::Hmi::PointerEvent& ev )
		{
			if( w->WindowBorder.get() != WindowBorder::Sizeable )
				return ResizeDirection::No;

			ResizeDirection::Type resizeDir = ResizeDirection::No;
			Pt::Gfx::SizeF				size   = w->winSize();
			double								border = w->BorderWidth.get();
			double								sizeR  = size.width() - border;
			double								sizeB  = size.height() - border;

			Pt::Gfx::PointF p( ev.x() - w->Position.get().x(), ev.y() - w->Position.get().y() );

			if( contains(w, p) )
			{
				if(p.x() < border && p.y() <  border)
				{//Corner NW
					_app.setCursor( &Cursor::sizeNWSECursor() );
					resizeDir = ResizeDirection::NorthWest;
				}	
				else if(p.x() > sizeR && p.y() < border)
				{//corner NE
					_app.setCursor( &Cursor::sizeNESWCursor() );
					resizeDir= ResizeDirection::NorthEast;
				}
				else if(p.x() < border &&  p.y() > sizeB )
				{//corner SW
					_app.setCursor( &Cursor::sizeNESWCursor() );					
					resizeDir = ResizeDirection::SouthWest;
				}
				else if(p.x() > sizeR &&  p.y() > sizeB )
				{//corner SE          
					_app.setCursor( &Cursor::sizeNWSECursor() );
					resizeDir = ResizeDirection::SouthEast;
				}
				else
				{
					if( p.x() < border)				
					{//West            
						_app.setCursor( &Cursor::sizeWECursor() );
						resizeDir = ResizeDirection::West;
					}
					else if(p.x() >= sizeR )
					{//East
						_app.setCursor( &Cursor::sizeWECursor() );
						resizeDir = ResizeDirection::East;
					}
					else if( p.y() < border)
					{//North
						_app.setCursor( &Cursor::sizeNSCursor() );
						resizeDir = ResizeDirection::North;
					}
					else if(p.y() >sizeB)
					{//South
						_app.setCursor( &Cursor::sizeNSCursor() );
						resizeDir = ResizeDirection::South;
					}
				}
			}	

			return resizeDir;
		}


		void WindowManager::onPointerInput( const Pt::Hmi::PointerEvent& mouseEvent )
		{
			if( _windows.size() == 0 )
			{
				_sizingDirection = ResizeDirection::No;
				return;
			}

			if( mouseEvent.buttons()[0].state() == DeviceButton::Released )
				_sizingDirection = ResizeDirection::No;

			if(_sizingDirection == ResizeDirection::No )
			{
				if( !updateActive( mouseEvent ) )
					return;
			}

			ChildWindow* childWindow = active();

			if( childWindow == 0 )
			{
				_sizingDirection = ResizeDirection::No;
				return;
			}

			Pt::Hmi::PointerEvent localMouseEvent = mouseEvent;

			localMouseEvent.setX( mouseEvent.x() - childWindow->Position.get().x() - childWindow->BorderWidth.get() ) ;

			if( childWindow->titleBar() != 0 ) 	
				localMouseEvent.setY( mouseEvent.y() - childWindow->Position.get().y() - childWindow->titleBar()->Size.get().height() ) ;
			else
				localMouseEvent.setY( mouseEvent.y() - childWindow->Position.get().y()) ;

			if( _sizingDirection == ResizeDirection::No )
			{
				_lastSizePoint = Pt::Gfx::PointF( mouseEvent.x(), mouseEvent.y() ) ;
				_sizingDirection = detSizeDirection( childWindow, mouseEvent );	

				if( childWindow->contains( Gfx::PointF( localMouseEvent.x(), localMouseEvent.y() ) ) )
					childWindow->eventReceived().send( localMouseEvent );
			}
			else
			{
				doSizing( childWindow, mouseEvent );				
			}			
		}


	}} // namespace

