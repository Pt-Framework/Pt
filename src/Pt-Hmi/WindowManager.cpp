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
, _borderWidth(4)
, _moving(false)
, _inactiveColor(0.8f, 0.8f, 0.8f)
, _activeColor(0.6f, 0.6f, 0.8f)
, _textColor(0.0, 0.0, 0.0)
, _pointerLastState( DeviceButton::Released )
, _focusOnPointerOver( false )
, _actionButton(0)
, _titleBarHeight(20)
, _pointedWindow(0)
{
	Margin buttonMargin =  Margin(1,1,1,3);  	
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


void WindowManager::activate(ChildWindow* w)
{
	deactivate();

	std::vector<ChildWindow*>::iterator it = std::find(_windows.begin(), _windows.end(), w);
	if( it != _windows.end() )
		_windows.erase(it);
		
	_windows.push_back(w);    

	w->processEvent( ActivateEvent(true) );
}


void WindowManager::deactivate()
{
	std::vector<ChildWindow*>::iterator it = _windows.begin();
	
	for( ; it != _windows.end(); ++it)
	{
			ChildWindow* w = *it;

			if( w->isActive() )
			{
				w->processEvent( ActivateEvent(false) );				
				break;
			}
	}		
}


void WindowManager::invalidate()
{
	_parent.invalidate();
}


Gfx::PointF WindowManager::renderFrame( ChildWindow* w )
{	
	const Gfx::SizeF clientSize =  w->Size.get();
	const Gfx::SizeF winSize( clientSize.width()  + _borderWidth*2,  clientSize.height() + _borderWidth*2 + _titleBarHeight);	
	
	Gfx::Color color = w->isActive() ? _activeColor : _inactiveColor;  

	Painter& painter = _parent.surface().painter();

	Gfx::PointF pos( w->Position.get().x(), w->Position.get().y());
	
  switch( w->Border.get() )
  {
      case WindowBorder::Dialog:
      case WindowBorder::DialogSizeable:
      case WindowBorder::Fixed:
      case WindowBorder::Sizeable:
      case WindowBorder::Tool:
      case WindowBorder::ToolSizeable:
      {	        
	        Gfx::Brush brush(color);
	        painter.setBrush(brush);

					Gfx::Pen pen(1, _textColor);
					painter.setPen(pen);
					
					Gfx::RectF leftBorder( pos.x(), 
					                       pos.x() + _borderWidth,
													       pos.y() + _borderWidth, 
													       pos.y() + winSize.height() - _borderWidth - 1 );
					painter.fillRect(leftBorder);

					Gfx::RectF topBorder(pos.x(),
					                     pos.x() + winSize.width() - 1,
															 pos.y(),
															 pos.y() + _borderWidth);
					painter.fillRect(topBorder);

					Gfx::RectF rightBorder(pos.x() + winSize.width() - _borderWidth,
					                       pos.x() + winSize.width() - 1,
																 pos.y() + _borderWidth,
																 pos.y() + winSize.height() - _borderWidth - 1 );
					painter.fillRect(rightBorder);

					Gfx::RectF bottomBorder(pos.x(),
					                        pos.x() + winSize.width() - 1,
																	pos.y() + winSize.height() - _borderWidth,
																	pos.y() + winSize.height() - 1);
					painter.fillRect(bottomBorder);

					Gfx::RectF titleArea( pos.x() + _borderWidth,
					                      pos.x() + winSize.width() - _borderWidth - 1,
																pos.y() + _borderWidth,
																pos.y() + _borderWidth + _titleBarHeight - 1);
					painter.fillRect(titleArea);

					const Gfx::Font& font = w->Font.get();
					
					Gfx::FontMetrics fm = painter.fontMetrics(font, Pt::String("A") );
					double textMargin = (_titleBarHeight - fm.height()) / 2;
					Gfx::PointF textPos(pos.x() + _borderWidth + _titleBarHeight, pos.y() + _titleBarHeight - textMargin);

					painter.setFont(font);
					painter.drawText(textPos, Pt::String( w->Caption.get().c_str()) );
      }

      break;
  }

  return Gfx::PointF( pos.x() + _borderWidth, pos.y() + _borderWidth + _titleBarHeight) ;	 
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
		w->render();
		painter.drawSurface( clientPos, w->surface() );
	}
}


Gfx::PointF WindowManager::toClient(const ChildWindow* w, const Gfx::PointF& p)
{
  return Gfx::PointF( p.x() - w->Position.get().x() - _borderWidth ,  p.y() - w->Position.get().y() - _borderWidth - _titleBarHeight ) ;
}


void WindowManager::updateActive( const Pt::Hmi::PointerEvent& pointerEvent )
{	
	if( pointerEvent.buttons()[_actionButton].state() != DeviceButton::Pressed)
			return;    

  if( _sizingDirection != ResizeDirection::No)
		return;
		
	if( _moving )
		return;

	std::vector<ChildWindow*>::reverse_iterator rit =  _windows.rbegin();

	for( ; rit != _windows.rend(); ++rit )
	{
		ChildWindow* w = *rit;

		Gfx::PointF pos( pointerEvent.x(), pointerEvent.y());

		if( !contains( w, pos ) )
			continue;

		const Gfx::PointF& client = toClient( w, Gfx::PointF( pointerEvent.x(), pointerEvent.y() ) );
		
		if( w->isActive() )		 													
			return;
			 
		activate( w );    
		return;
	}	 	

	deactivate();
}


Window* WindowManager::getFosusedWindow( WindowManager* manager )
{     
  for( size_t i = 0; i < manager->windows().size(); ++i )
  {
    Window* child = manager->windows()[i];
    
    if( child->isActive() )
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
			w->processEvent( keyEvent );		

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
		w->processEvent(_positionEvent);	
	}

	if( w->Size.get() != size )
	{
		_sizeEvent.setSize( size );
		w->processEvent( _sizeEvent );
	}

	_lastSizePoint = point;  
}


bool WindowManager::contains(const ChildWindow* w, const Gfx::PointF& p)
{  
	Gfx::SizeF winSize( w->Size.get().width() + _borderWidth*2, w->Size.get().height() + _borderWidth*2 + _titleBarHeight );

	if( p.x() >= w->Position.get().x() && p.x() <  w->Position.get().x() + winSize.width() &&
	    p.y() >= w->Position.get().y() && p.y() <  w->Position.get().y() + winSize.height() )
				return true;

	return false;
}


bool WindowManager::isMoving( const ChildWindow* w, const Pt::Hmi::PointerEvent& ev )
{			
	if( ev.buttons()[_actionButton].state() != DeviceButton::Pressed  || _moving  ||  _pointerLastState !=  DeviceButton::Released )
		return false;

	const Gfx::PointF& position = w->Position.get();
	 
	if(  (ev.x() < (position.x() + _borderWidth*2 + w->Size.get().width())  && ev.x() >= position.x())  && 
			 (ev.x() >= (position.x() + _borderWidth)) &&
       (ev.y() < (position.y() + _titleBarHeight) ) && ((ev.y()+ position.y()) >= _borderWidth ) &&
			 (ev.y() >= (position.y() + _borderWidth)))
	{				
		_movingOffset =Gfx::PointF( ev.x(), ev.y() );
		return true;
	}

	return false;
}


ResizeDirection::Type WindowManager::getSizingDirection( const ChildWindow* w, const Pt::Hmi::PointerEvent& ev )
{	
	const Gfx::SizeF  wsize = w->Size.get();
	const Gfx::PointF wpos  = w->Position.get();
	double titleHeight = _titleBarHeight;

	bool left = ev.x() < (wpos.x() +	_borderWidth);
	bool right = ev.x() >= wpos.x() + _borderWidth + wsize.width();
	bool top = ev.y() < wpos.y() + _borderWidth;
	bool bottom = ev.y() >= wpos.y() + _borderWidth + titleHeight + wsize.height();

	if(top && left)
		return ResizeDirection::NorthWest;

	if(top && right)
		return ResizeDirection::NorthEast;
	
	if(bottom && left)
		return ResizeDirection::SouthWest;
	
	if(bottom && right)
		return ResizeDirection::SouthEast;

	if(left)				
		return ResizeDirection::West;

	if(right)
		return ResizeDirection::East;

	if(top)
		return ResizeDirection::North;

	if(bottom)
		return ResizeDirection::South;			

	return ResizeDirection::No;			
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

	w->processEvent( _positionEvent );
	
	_movingOffset = Gfx::PointF( point.x() , point.y() );  
	invalidate();
}


ChildWindow* WindowManager::findWindow( const Gfx::PointF& pos )
{
	for( int i = _windows.size() - 1;  i > -1; --i )
	{
		ChildWindow* w = _windows[i];

		if( !w->Visible.get() )
			continue;

		if( !contains( w, pos ) )
			continue;

		return w;
	}

	return 0;
}

void WindowManager::setPointedWindow( ChildWindow* window )
{
	if( _pointedWindow == window )
		return;

	if( _pointedWindow )
		_pointedWindow->setPointedWidget(0);

	_pointedWindow = window;
}


void WindowManager::setSizingCursor( ResizeDirection::Type type )
{
	Screen& screen = _app.mainScreen();

	switch( type )
	{
		case ResizeDirection::East:
		case ResizeDirection::West:
			screen.setCursor( &Hmi::Cursor::sizeWECursor() );
		break;

		case ResizeDirection::NorthEast:
		case ResizeDirection::SouthWest:
			screen.setCursor( &Hmi::Cursor::sizeNESWCursor() );
		break;

		case ResizeDirection::North:		
		case ResizeDirection::South:
			screen.setCursor( &Hmi::Cursor::sizeNSCursor() );
		break;
		
		case ResizeDirection::NorthWest:
		case ResizeDirection::SouthEast:
			screen.setCursor( &Hmi::Cursor::sizeNWSECursor() );
		break;
		
		default:
		  screen.setCursor( &Hmi::Cursor::defaultCursor() );
			break;	
	}
}


bool WindowManager::pointerInput( const Pt::Hmi::PointerEvent& pointerEvent )
{
	Screen& screen = _app.mainScreen();

	if( _windows.size() == 0 )
	{
		_sizingDirection = ResizeDirection::No;
		_moving = false;
		_pointerLastState = pointerEvent.buttons()[_actionButton].state();        
		setPointedWindow(0);
		return false;
	}

	if( pointerEvent.buttons()[_actionButton].state() == DeviceButton::Released )
	{     
		_sizingDirection = ResizeDirection::No;
		_moving = false;
	}

	updateActive( pointerEvent );

	ChildWindow* childWindow = 0;
	
	if( _sizingDirection == ResizeDirection::No && !_moving )
			childWindow = findWindow( Gfx::PointF( pointerEvent.x(),  pointerEvent.y() ) );
	else
			childWindow = active();

	if( childWindow == 0 )
	{
		_sizingDirection = ResizeDirection::No;
		_moving = false;
    _pointerLastState = pointerEvent.buttons()[_actionButton].state();    
    setPointedWindow(0);
		return false;
	}

	Pt::Hmi::PointerEvent localMouseEvent = pointerEvent;

	localMouseEvent.setX( pointerEvent.x() - childWindow->Position.get().x() - _borderWidth ) ;
	localMouseEvent.setY( pointerEvent.y() - childWindow->Position.get().y() - _titleBarHeight - _borderWidth ) ;  

	if( (_sizingDirection != ResizeDirection::No || _moving) && childWindow->isActive() )
	{
	  doMoving( childWindow, pointerEvent );		
	  doSizing( childWindow, pointerEvent );				
    _pointerLastState = pointerEvent.buttons()[_actionButton].state();		
    return true;
  }

	_lastSizePoint = Gfx::PointF( pointerEvent.x(), pointerEvent.y() ) ;
	
	_moving = isMoving( childWindow, pointerEvent );	

	if( _moving )
  {		
    _pointerLastState = pointerEvent.buttons()[_actionButton].state();
		return true;
  }
	
	ResizeDirection::Type sizingDirection = getSizingDirection( childWindow, pointerEvent );	

	setSizingCursor( sizingDirection );

	_sizingDirection = ResizeDirection::No;		

	if( sizingDirection != ResizeDirection::No )
	{
		if( pointerEvent.buttons()[_actionButton].state() == DeviceButton::Pressed )
		{
			_sizingDirection  = sizingDirection;		
			_pointerLastState = pointerEvent.buttons()[_actionButton].state();
		}		

		setPointedWindow(0);
		return true;
  }

	setPointedWindow( childWindow );
	childWindow->processEvent( localMouseEvent );
	_pointerLastState = pointerEvent.buttons()[_actionButton].state();

	return true;
}

}} // namespace
