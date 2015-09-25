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
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Point.h>
#include <Pt/String.h>
#include <cmath>

namespace Pt {
namespace Hmi {

WindowManager::WindowManager(Window& parent)
: _app( Application::instance() )
, _parent( parent )
, _sizingDirection( ResizeDirection::No )
, _borderWidth(5)
, _moving(false)
, _inactiveColor(0.8f, 0.8f, 0.8f)
, _activeColor(0.7f, 0.7f, 0.7f)
, _pointerLastState( DeviceButton::Released )
, _focusOnPointerOver( false )
, _actionButton(0)
{
	Margin buttonMargin =  Margin(1,1,1,3);

	_closeButton.PanelBorderRoundEdge = false;
  _closeButton.Dock    = Docking::Right;
	_closeButton.Margin  = buttonMargin;
	_closeButton.Size		 =Gfx::SizeF(20,5 );
  _closeButton.Caption = "x";
	_closeButton.TextAlign = Align::TopCenter;
  _closeButton.Visible = true;
  _closeButton.Enabled = true;
 
  
	_maxButton.PanelBorderRoundEdge = false;
	_maxButton.Dock    = Docking::Right;
	_maxButton.Margin  = buttonMargin;
	_maxButton.Size		 =Gfx::SizeF(20,5 );
  _maxButton.Caption = "+";
  _maxButton.Visible = true;
  _maxButton.Enabled = true;

	_minButton.PanelBorderRoundEdge = false;
	_minButton.Dock    = Docking::Right;
	_minButton.Margin  = buttonMargin;
	_minButton.Size		 =Gfx::SizeF(20,5 );
  _minButton.Caption = "-";
  _minButton.Visible = true;
  _minButton.Enabled = true;

  _titleLabel.Dock = Docking::Fill;
  _titleLabel.TextAlign = Align::MidleCenter;
  _titleLabel.Visible = true;

	_titleBarPanel.BackColor = _inactiveColor;
  _titleBarPanel.Position =Gfx::PointF(0,0);
  _titleBarPanel.Size =Gfx::SizeF( 16, 20 );

  _titleBarPanel.addChild( &_titleLabel );  
  _titleBarPanel.addChild( &_closeButton );
	_titleBarPanel.addChild( &_maxButton );
	_titleBarPanel.addChild( &_minButton );
  _titleBarPanel.Margin = Margin(0);  
	_titleBarPanel.Visible = true;	  	
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


void WindowManager::add( ChildWindow* w )
{
	_windows.push_back(w);
	invalidate();  
}


void WindowManager::remove( ChildWindow* w )
{
	std::vector<ChildWindow*>::iterator it = std::find( _windows.begin(), _windows.end(), w );

	if( it == _windows.end() )
		return;

	_windows.erase( it );		    
  
	invalidate();
}


void WindowManager::activate( ChildWindow* w )
{
	std::vector<ChildWindow*>::iterator it = std::find( _windows.begin(), _windows.end(), w );

	if( it == _windows.end() )
		return;

	_windows.erase( it );	
	_windows.push_back( w );    

  w->eventReceived().send( FocusEvent() );
  invalidate();
}


void WindowManager::invalidate()
{
	_parent.invalidate();
}


Gfx::PointF WindowManager::renderFrame( ChildWindow* w )
{	
	const Gfx::SizeF size =  w->Size.get();
	const Gfx::SizeF winSize( size.width() + _borderWidth, size.height() + _borderWidth + _titleBarPanel.Size.get().height() );	
	
	Gfx::Color color = w->isWindowFocused() ? _activeColor : _inactiveColor;  

	Painter& painter = _parent.surface().painter();

	Gfx::PointF pos( w->Position.get().x() + _borderWidth/2.0, w->Position.get().y() + _borderWidth/2.0 );
	
  switch( w->Border.get() )
  {
      case WindowBorder::Dialog:
      case WindowBorder::DialogSizeable:
      case WindowBorder::Fixed:
      case WindowBorder::Sizeable:
      {	        
	       Gfx::RectF  rect( pos, winSize ) ;	
	       Gfx::Pen    pen((size_t) _borderWidth, color);
	        painter.setPen( pen );
	        painter.drawRect( rect );		
      }

      break;

      case WindowBorder::Tool:
      case WindowBorder::ToolSizeable:
      {
	       Gfx::RectF  rect( pos, winSize ) ;	
	       Gfx::Pen    pen((size_t) _borderWidth, color);
	        painter.setPen( pen );
	        painter.drawRect( rect );		
      }
      break;
  }

  if( w->Border.get() != WindowBorder::NoBorder )
  {
		Gfx::PointF  to( pos.x() + _borderWidth/2, pos.y() + _borderWidth/2 ); 
		Gfx::SizeF   size =Gfx::SizeF( winSize.width() - _borderWidth, _titleBarPanel.Size.get().height() );
    		
    _closeButton.BorderColor =Gfx::Color(0.5f,0.5f,0.5f);
    _minButton.BorderColor = _closeButton.BorderColor.get();
    _maxButton.BorderColor = _closeButton.BorderColor.get();
    _titleLabel.BackColor = color;
    _titleLabel.Caption    = w->Caption.get();
    _titleLabel.Visible    = w->ShowTitle.get();

		_titleBarPanel.Position    = to;
    _titleBarPanel.Size        = size;
    _titleBarPanel.BackColor   = color; 
    _titleBarPanel.BorderColor = color;    		 				
		_titleBarPanel.setParent( &_parent );		
		_titleBarPanel.render();	  		
    _parent.surface().painter().drawSurface( to, _titleBarPanel.surface() );
		_titleBarPanel.setParent( 0 );
  }

  return Gfx::PointF( pos.x() + _borderWidth/2, pos.y()  + _titleBarPanel.Size.get().height()  + _borderWidth/2 );	 
}


void WindowManager::render()
{		
	Painter& painter = _parent.surface().painter();

	for( size_t i = 0; i < _windows.size(); ++i )
	{
		ChildWindow* w = _windows[i];				

		if( !w->Visible.get() )
   		  continue;		

		const Gfx::PointF clientPos = renderFrame(w);						  
		painter.drawSurface( clientPos, w->surface() );
	}
}


Gfx::PointF WindowManager::toClient(const ChildWindow* w, const Gfx::PointF& p)
{
  return Gfx::PointF( p.x() - w->Position.get().x() - _borderWidth ,  p.y() - w->Position.get().y() - _borderWidth - _titleBarPanel.Size.get().height() ) ;
}


bool WindowManager::updateActive( const Pt::Hmi::PointerEvent& pointerEvent )
{	
	for( int i = _windows.size() - 1;  i > -1; --i )
	{
		ChildWindow* w = _windows[i];

		Gfx::PointF local( pointerEvent.x() - w->Position.get().x() , pointerEvent.y() - w->Position.get().y() );

		if( !contains( w, local ) )
			continue;

		const Gfx::PointF& client = toClient( w,Gfx::PointF( pointerEvent.x(), pointerEvent.y() ) );
				 
		if( w->hasPointer()  && !w->contains( client ) && w->Enabled.get() && !_moving && _sizingDirection == ResizeDirection::No )
			w->onPointerLeaved();
							
		if( w == active() )
			return true;

		if( pointerEvent.buttons()[_actionButton].state() != DeviceButton::Pressed )
			return false;    
					
		activate( w );    
		return true;
	}	 

	return false;
}


Window* WindowManager::getFosusedWindow( WindowManager* manager )
{     
  for( size_t i = 0; i < manager->windows().size(); ++i )
  {
    Window* child = manager->windows()[i];
    
    if( child->isWindowFocused() )
        return child;

    Window* focused = getFosusedWindow( &child->windowManager() );

    if( focused != 0 )
      return focused;
  }

  return 0;
}


bool WindowManager::keyInput( const Pt::Hmi::KeyEvent& keyEvent )
{
  Window* w = getFosusedWindow(this);

  if( w == 0 )
    return false;

	if( w->Enabled.get() )
			w->eventReceived().send( keyEvent );		

   return true;
}


void WindowManager::doSizing( ChildWindow* w, const PointerEvent& ev )
{
  if( _sizingDirection == ResizeDirection::No )
    return;

	const std::vector<DeviceButton>& button = ev.buttons();

	Gfx::PointF point( ev.x(), ev.y() );

	if( button[_actionButton].state() != DeviceButton::Pressed )
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

	Gfx::SizeF size(width, height);
	Gfx::PointF pos(posX, posY);

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


bool WindowManager::contains(const ChildWindow* w, const Gfx::PointF& p)
{  
	Gfx::SizeF winSize( w->Size.get().width() + _borderWidth*2, w->Size.get().height() + _borderWidth + _titleBarPanel.Size.get().height() );

	if( p.x() < winSize.width() && p.x() >= 0 && p.y() < winSize.height() && p.y() >= 0)
		return true;

	return false;
}


bool WindowManager::isMoving( const ChildWindow* w, const Pt::Hmi::PointerEvent& ev )
{			
	if( ev.buttons()[_actionButton].state() != DeviceButton::Pressed  || _moving  ||  _pointerLastState !=  DeviceButton::Released )
		return false;

	const Gfx::PointF& position = w->Position.get();
	 
	if( ev.x() < (position.x() + _borderWidth*2 + w->Size.get().width())  && ev.x() >= position.x()  && 
      (ev.y()) < (position.y() + _titleBarPanel.Size.get().height()) && (ev.y()+ position.y()) >= (_borderWidth) )
	{				
		_movingOffset =Gfx::PointF( ev.x(), ev.y() );
		return true;
	}

	return false;
}


ResizeDirection::Type WindowManager::isSizing( const ChildWindow* w, const Pt::Hmi::PointerEvent& ev )
{
	ResizeDirection::Type resizeDir = ResizeDirection::No;

  if( w->WindowBorder.get() != WindowBorder::Sizeable  ||  _pointerLastState !=  DeviceButton::Released )
		return resizeDir;	

	const Gfx::SizeF	size   = w->Size.get();
	const double		border = _borderWidth;
	const double		sizeR  = size.width();
	const double		sizeB  = size.height() + _titleBarPanel.Size.get().height();
	Screen& screen = _app.mainScreen();
	
	Gfx::PointF localPos( ev.x() - w->Position.get().x(), ev.y() - w->Position.get().y() );  

	if( contains(w, localPos) )
	{
		if(localPos.x() < border && localPos.y() <  border)
		{//Corner NW
			screen.setCursor( &Cursor::sizeNWSECursor() );
			resizeDir = ResizeDirection::NorthWest;
		}	
		else if(localPos.x() > sizeR && localPos.y() < border)
		{//corner NE
			screen.setCursor( &Cursor::sizeNESWCursor() );
			resizeDir = ResizeDirection::NorthEast;
		}
		else if(localPos.x() < border && localPos.y() > sizeB )
		{//corner SW
			screen.setCursor( &Cursor::sizeNESWCursor() );					
			resizeDir = ResizeDirection::SouthWest;
		}
		else if(localPos.x() > sizeR &&  localPos.y() > sizeB )
		{//corner SE          
			screen.setCursor( &Cursor::sizeNWSECursor() );
			resizeDir = ResizeDirection::SouthEast;
		}
		else
		{
			if( localPos.x() < border)				
			{//West            
				screen.setCursor( &Cursor::sizeWECursor() );
				resizeDir = ResizeDirection::West;
			}
			else if(localPos.x() > sizeR )
			{//East
				screen.setCursor( &Cursor::sizeWECursor() );
				resizeDir = ResizeDirection::East;
			}
			else if( localPos.y() < border)
			{//North
				screen.setCursor( &Cursor::sizeNSCursor() );
				resizeDir = ResizeDirection::North;
			}
			else if(localPos.y() > sizeB)
			{//South
				screen.setCursor( &Cursor::sizeNSCursor() );
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

	if( button[_actionButton].state() != DeviceButton::Pressed )
	{		
		_moving = false;
		return;
	}	

	Gfx::PointF point( ev.x(), ev.y() );

	if( point.x() < 0 ) 
		point.setX(0);
	
	const double dtX =  point.x() - _movingOffset.x();
	const double dtY =  point.y() - _movingOffset.y();
	Gfx::PointF newPos( w->Position.get().x() + dtX, w->Position.get().y() + dtY );
	
	if( newPos.y() < 0 ) 
		newPos.setY( 0 );
		
	_positionEvent.setPosition(newPos);

	w->eventReceived().send( _positionEvent );
	
	_movingOffset =Gfx::PointF( point.x() , point.y() );  
	invalidate();
}


bool WindowManager::pointerInput( const Pt::Hmi::PointerEvent& pointerEvent )
{
	Screen& screen = _app.mainScreen();

	if( _windows.size() == 0 )
	{
		_sizingDirection = ResizeDirection::No;
		_moving = false;
    _pointerLastState = pointerEvent.buttons()[_actionButton].state();        
    screen.setCursor( &(_parent.Cursor.get()) );
		return false;
	}

	if( pointerEvent.buttons()[_actionButton].state() == DeviceButton::Released )
	{     
		_sizingDirection = ResizeDirection::No;
		_moving = false;
	}

	if( _sizingDirection == ResizeDirection::No && !_moving )
	{
		if( !updateActive( pointerEvent ) )
		{
      _pointerLastState = pointerEvent.buttons()[_actionButton].state();
      screen.setCursor( &(_parent.Cursor.get()) );      
			return false;
		}		
	}

	ChildWindow* childWindow = active();

	if( childWindow == 0 )
	{
		_sizingDirection = ResizeDirection::No;
		_moving = false;
    _pointerLastState = pointerEvent.buttons()[_actionButton].state();    
    screen.setCursor( &(_parent.Cursor.get()) );
    
		return false;
	}

	Pt::Hmi::PointerEvent localMouseEvent = pointerEvent;

	localMouseEvent.setX( pointerEvent.x() - childWindow->Position.get().x() - _borderWidth ) ;
	localMouseEvent.setY( pointerEvent.y() - childWindow->Position.get().y() - _titleBarPanel.Size.get().height() - _borderWidth ) ;  

	if( _sizingDirection != ResizeDirection::No || _moving )
	{
	  doMoving( childWindow, pointerEvent );		
	  doSizing( childWindow, pointerEvent );				
    _pointerLastState = pointerEvent.buttons()[_actionButton].state();
    return true;
  }

	_lastSizePoint =Gfx::PointF( pointerEvent.x(), pointerEvent.y() ) ;

	_moving = isMoving( childWindow, pointerEvent );	

	if( _moving )
  {
    _pointerLastState = pointerEvent.buttons()[_actionButton].state();
		return true;
  }

	_sizingDirection = isSizing( childWindow, pointerEvent );	
		
	if( _sizingDirection != ResizeDirection::No)
  {
    _pointerLastState = pointerEvent.buttons()[_actionButton].state();
		return true;
  }
		
  if( !childWindow->Enabled.get() )
  {
    _pointerLastState = pointerEvent.buttons()[_actionButton].state();
		return true;
  }

	if( childWindow->contains(Gfx::PointF( localMouseEvent.x(), localMouseEvent.y() ) ) )	
    childWindow->eventReceived().send( localMouseEvent );
	
  _pointerLastState = pointerEvent.buttons()[_actionButton].state();
  return true;
}

}} // namespace
