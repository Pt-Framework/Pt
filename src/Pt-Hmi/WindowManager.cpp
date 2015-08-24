/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 
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
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/ChildWindow.h>
#include <Pt/Hmi/PointerEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Ui/Pen.h>
#include <Pt/Ui/Brush.h>
#include <Pt/Ui/Point.h>
#include <cmath>

namespace Pt {
namespace Hmi {

WindowManager::WindowManager(Window& parent)
: _parent( parent )
, _sizingDirection( ResizeDirection::No )
, _app( Application::instance() )
, _titleBarHeight(20)
, _borderWidth(6)
, _borderColor(0,0,1)
, _moving(false)
, _titleBarColor(0, 0, 1)
, _activeColor(0, 0, 0.5)
{
}


WindowManager::~WindowManager()
{
}


ChildWindow* WindowManager::active()
{	
	if( _windows.size() == 0)
		return 0;

	size_t index = _windows.size() - 1;

	while( index >= 0 )
	{		
		if( _windows[index]->Visible.get() )
			return _windows[ index];

		--index;
	}

	return 0;
}


void WindowManager::clearFocus()
{
	for( size_t i = 0; i < _windows.size(); ++i )			
	{
		if( _windows[i]->isFocused() )
		{
			_focusEvent.setFocus(false);
			_windows[i]->eventReceived().send(_focusEvent);
		}
	}		
}


void WindowManager::updateFocus()
{
	for( size_t i = 0; i < _windows.size() - 1; ++i )			
	{
		if( _windows[i]->isFocused() )
		{
			_focusEvent.setFocus(false);
			_windows[i]->eventReceived().send(_focusEvent);
		}
	}							
			
	if( !_windows[ _windows.size() - 1 ]->isFocused())
	{
		_focusEvent.setFocus(true);
		_windows[ _windows.size() - 1 ]->eventReceived().send( _focusEvent );
	}
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


Ui::PointF WindowManager::renderFrame(const ChildWindow* w)
{	
	const Ui::SizeF& size = w->Size.get();
	const Ui::SizeF winSize( size.width() + _borderWidth, size.height() + _borderWidth/2.0 + _titleBarHeight);	

	Painter& painter = _parent.windowSurface().painter();

	//Render Frame
	Ui::PointF pos( w->Position.get().x() + _borderWidth/2.0, w->Position.get().y() + _borderWidth/2.0 );
	Ui::RectF  rect( pos, winSize ) ;	

	Ui::Pen  pen((size_t) _borderWidth, _borderColor);
	painter.setPen( pen );
	painter.drawRect( rect );		
	
	//Render Titel bar
	Ui::Brush brush( w->isFocused() ? _activeColor : _titleBarColor );
	Ui::RectF rectTitle( pos, Ui::SizeF(winSize.width(), _titleBarHeight) ) ;

	painter.setBrush( brush );
	painter.fillRect( rectTitle );	
		
	return Ui::PointF( pos.x() + _borderWidth/2, pos.y()  +_titleBarHeight );	
}


void WindowManager::render()
{		
	Painter& painter = _parent.windowSurface().painter();

	for( size_t i = 0; i < _windows.size(); ++i )
	{
		ChildWindow* w = _windows[i];				

		if( !w->Visible.get() )
   		  continue;

		const Ui::PointF clientPos = renderFrame(w);				
		painter.drawSurface( clientPos, w->windowSurface() );
	}
}


Ui::PointF WindowManager::toClient(const ChildWindow* w, const Ui::PointF& p)
{
		return Ui::PointF( p.x() - w->Position.get().x() - _borderWidth ,  p.y() - w->Position.get().y() - _borderWidth - _titleBarHeight ) ;
}


bool WindowManager::updateActive( const Pt::Hmi::PointerEvent& mouseEvent )
{	
	for( int i = _windows.size() - 1;  i > -1; --i )
	{
		ChildWindow* w = _windows[i];

		Ui::PointF local( mouseEvent.x() - w->Position.get().x() , mouseEvent.y() - w->Position.get().y() );

		if( !contains( w, local ) )
			continue;
	
		const Ui::PointF& client = toClient( w, Ui::PointF( mouseEvent.x(), mouseEvent.y() ) );
				 
		if( w->hasPointer()  && !w->contains( client ) && w->Enabled.get() )
			w->onPointerLeaved();
							
		if( w == active() )
		{
			if( mouseEvent.buttons()[0].state() == DeviceButton::Pressed )
				updateFocus();
			return true;
		}

		if( mouseEvent.buttons()[0].state() != DeviceButton::Pressed )
			return false;
					
		activate( w );
		updateFocus();
		return true;
	}	 

	if( mouseEvent.buttons()[0].state() == DeviceButton::Pressed )
		clearFocus();

	return false;
}


void WindowManager::onKeyInput( const Pt::Hmi::KeyEvent& keyEvent )
{
	ChildWindow* w = active();

	if( w == 0 )
		return;

	if( w->Enabled.get() )
			w->eventReceived().send( keyEvent );		
}


void WindowManager::doSizing( ChildWindow* w, const PointerEvent& ev )
{
  if( _sizingDirection == ResizeDirection::No )
    return;

	const std::vector<DeviceButton>& button = ev.buttons();

	Ui::PointF point( ev.x(), ev.y() );

	if( button[0].state() != DeviceButton::Pressed )
	{
		_sizingDirection = ResizeDirection::No;
		_lastSizePoint =  point;
		return;
	}	
	
	double width  = w->Size.get().width();
	double height = w->Size.get().height();
	double posX   = w->Position.get().x();
	double posY   = w->Position.get().y();
	double deltaX = ( point.x() - _lastSizePoint.x());
	double deltaY = ( point.y() - _lastSizePoint.y());

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

	Ui::SizeF size(width, height);
	Ui::PointF pos(posX, posY);

	if( width < w->MinimumSize.get().width() )
		size.setWidth( w->MinimumSize.get().width() );

	if( height < w->MinimumSize.get().height() )
		size.setHeight( w->MinimumSize.get().height() );
				
	if( width > w->MaximumSize.get().width() )
		size.setWidth( w->MaximumSize.get().width() );

	if( height > w->MaximumSize.get().height() )
		size.setHeight( w->MaximumSize.get().height() );

	if( w->Position.get() != pos )
	{
		_positionEvent.setPosition(pos);
		w->eventReceived().send(_positionEvent);	
	}

	if( w->Size.get() != size )
	{
		_sizeEvent.setSize( size );
		w->eventReceived().send( _sizeEvent );
	}

	_lastSizePoint = point;
}

bool WindowManager::contains(const ChildWindow* w, const Ui::PointF& p)
{  
	Ui::SizeF winSize( w->Size.get().width() + _borderWidth*2, w->Size.get().height() + _borderWidth + _titleBarHeight );

	if( p.x() < winSize.width() && p.x() >= 0 && p.y() < winSize.height() && p.y() >= 0)
		return true;

	return false;
}

bool WindowManager::isMoving( const ChildWindow* w, const Pt::Hmi::PointerEvent& ev )
{			
	if( ev.buttons()[0].state() != DeviceButton::Pressed  || _moving )
		return false;

	const Ui::PointF& position = w->Position.get();
	 
	if( ev.x() < (position.x() + _borderWidth*2 + w->Size.get().width())  && ev.x() >= position.x()  && 
      (ev.y()) < (position.y() + _titleBarHeight) && (ev.y()+ position.y()) >= (_borderWidth) )
	{				
		_movingOffset = Ui::PointF( ev.x(), ev.y() );
    
		return true;
	}

	return false;
}


ResizeDirection::Type WindowManager::isSizing( const ChildWindow* w, const Pt::Hmi::PointerEvent& ev )
{
	ResizeDirection::Type resizeDir = ResizeDirection::No;

  if( w->WindowBorder.get() != WindowBorder::Sizeable )
		return resizeDir;	

	const Ui::SizeF	size   = w->Size.get();
	const double		border = _borderWidth;
	const double		sizeR  = size.width();
	const double		sizeB  = size.height();

	Ui::PointF localPos( ev.x() - w->Position.get().x(), ev.y() - w->Position.get().y() );

	if( contains(w, localPos) )
	{
		if(localPos.x() < border && localPos.y() <  border)
		{//Corner NW
			_app.setCursor( &Cursor::sizeNWSECursor() );
			resizeDir = ResizeDirection::NorthWest;
		}	
		else if(localPos.x() > sizeR && localPos.y() < border)
		{//corner NE
			_app.setCursor( &Cursor::sizeNESWCursor() );
			resizeDir = ResizeDirection::NorthEast;
		}
		else if(localPos.x() < border && localPos.y() > sizeB )
		{//corner SW
			_app.setCursor( &Cursor::sizeNESWCursor() );					
			resizeDir = ResizeDirection::SouthWest;
		}
		else if(localPos.x() > sizeR &&  localPos.y() > sizeB )
		{//corner SE          
			_app.setCursor( &Cursor::sizeNWSECursor() );
			resizeDir = ResizeDirection::SouthEast;
		}
		else
		{
			if( localPos.x() < border)				
			{//West            
				_app.setCursor( &Cursor::sizeWECursor() );
				resizeDir = ResizeDirection::West;
			}
			else if(localPos.x() > sizeR )
			{//East
				_app.setCursor( &Cursor::sizeWECursor() );
				resizeDir = ResizeDirection::East;
			}
			else if( localPos.y() < border)
			{//North
				_app.setCursor( &Cursor::sizeNSCursor() );
				resizeDir = ResizeDirection::North;
			}
			else if(localPos.y() > sizeB)
			{//South
				_app.setCursor( &Cursor::sizeNSCursor() );
				resizeDir = ResizeDirection::South;			
			}
		}
	}	

	return resizeDir;
}


void WindowManager::doMoving( ChildWindow* w, const PointerEvent& ev )
{	
	if( !_moving ) 
		return;

	const std::vector<DeviceButton>& button = ev.buttons();

	if( button[0].state() != DeviceButton::Pressed )
	{		
		_moving = false;
		return;
	}	

	Ui::PointF point( ev.x(), ev.y() );

	if( point.x() < 0 ) 
		point.setX(0);
	
	const double dtX =  point.x() - _movingOffset.x();
	const double dtY =  point.y() - _movingOffset.y();
	Ui::PointF newPos( w->Position.get().x() + dtX, w->Position.get().y() + dtY );
	
	if( newPos.y() < 0 ) 
		newPos.setY( 0 );
		
	_positionEvent.setPosition(newPos);

	w->eventReceived().send( _positionEvent );
	
	_movingOffset = Ui::PointF( point.x() , point.y() );
	invalidate();
}


void WindowManager::onPointerInput( const Pt::Hmi::PointerEvent& mouseEvent )
{
	if( _windows.size() == 0 )
	{
	   _app.setCursor( &Cursor::defaultCursor() );
		_sizingDirection = ResizeDirection::No;
		_moving = false;
		return;
	}

	if( mouseEvent.buttons()[0].state() == DeviceButton::Released )
	{     
		_sizingDirection = ResizeDirection::No;
		_moving = false;
	}

	if( _sizingDirection == ResizeDirection::No  && !_moving )
	{
		if( !updateActive( mouseEvent ) )
		{
		  _app.setCursor( &Cursor::defaultCursor() );		 
			return;
		}		
	}

	ChildWindow* childWindow = active();

	if( childWindow == 0 )
	{
    _app.setCursor( &Cursor::defaultCursor() );
		_sizingDirection = ResizeDirection::No;
		_moving = false;
		return;
	}

	if( !childWindow->Enabled.get() )
	{
	  _app.setCursor( &Cursor::defaultCursor() );
		_sizingDirection = ResizeDirection::No;
		_moving = false;
		return;
	}

	Pt::Hmi::PointerEvent localMouseEvent = mouseEvent;

	localMouseEvent.setX( mouseEvent.x() - childWindow->Position.get().x() - _borderWidth ) ;
	localMouseEvent.setY( mouseEvent.y() - childWindow->Position.get().y() - _titleBarHeight - _borderWidth ) ;

	if( _sizingDirection == ResizeDirection::No && !_moving )
	{
		_lastSizePoint = Ui::PointF( mouseEvent.x(), mouseEvent.y() ) ;

		_moving = isMoving( childWindow, mouseEvent );	

		if( _moving )
		  return;

		_sizingDirection = isSizing( childWindow, mouseEvent );	
		
		if( _sizingDirection != ResizeDirection::No)
		  return;
		  
		
		if( !childWindow->contains( Ui::PointF( localMouseEvent.x(), localMouseEvent.y() ) ) )
		  return;       
				
		 childWindow->eventReceived().send( localMouseEvent );

		 return;
	}
		
	doMoving( childWindow, mouseEvent );		
	doSizing( childWindow, mouseEvent );							
}


}} // namespace

