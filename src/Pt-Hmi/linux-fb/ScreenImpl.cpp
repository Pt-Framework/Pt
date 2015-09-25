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
#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include "FrameBuffer.h"
#include "PaintSurfaceImpl.h"
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/System/Clock.h>
#include <Pt/Hmi/Application.h>
#include <algorithm>

namespace Pt{
namespace Hmi{
  
ScreenImpl::ScreenImpl()
: _frameBuffer()
, _image( Gfx::Size( (size_t)_frameBuffer.width(), (size_t)_frameBuffer.height() ) ,  _frameBuffer.format(), _frameBuffer.strideInBytes() )
, _dpi(96.0)
, _cursorPos( 0,0 )
{
   
}


ScreenImpl::~ScreenImpl()
{

}

void ScreenImpl::init()
{
	Visible   = true;	
	Size = Gfx::SizeF( _frameBuffer.width(), _frameBuffer.height() ) ;
	BackColor =Gfx::Color( 170/255.0f, 170/255.0f, 170/255.0f );			
	setCursor(0);	
	

    eventReceived() += Pt::slot( *this, &ScreenImpl::onPointerInput );
}


void ScreenImpl::onPointerInput( const Pt::Hmi::PointerEvent& mouseEvent )
{		
	if( !_cursorBackground.empty() )
		_frameBuffer.bitBlit( _cursorBackground.pixel(0,0), _cursorBackground.width(), _cursorBackground.height(), _cursorPos, FrameBuffer::CopyOp );

	_windowManager.pointerInput( mouseEvent );	

	if( Cursor.get().width() == 0  || Cursor.get().height() == 0 )
		return;	

   _cursorPos =Gfx::Point( mouseEvent.x(), mouseEvent.y() );	
	
	if( (_cursorBackground.width() != Cursor.get().width())  || (_cursorBackground.height() !=  Cursor.get().height()) )
		_cursorBackground.resize(Gfx::Size( Cursor.get().width(),Cursor.get().height()), _frameBuffer.format() ); 
		

  _frameBuffer.grabImage( _cursorPos, _cursorBackground );

  _frameBuffer.bitBlit( &Cursor.get().andRgb888()[0], Cursor.get().width(), Cursor.get().height(), _cursorPos, FrameBuffer::AndOp );

  _frameBuffer.bitBlit( &Cursor.get().xorRgb888()[0], Cursor.get().width(), Cursor.get().height(), _cursorPos, FrameBuffer::XorOp ); 

}


void ScreenImpl::onInvalidate()
{		
  Window::onInvalidate(); 
  
  Hmi::Painter& painter = surface().painter();
  painter.flush();
  memcpy( _frameBuffer.buffer(), _image.pixel(0,0), _frameBuffer.bufferSize() );		
}


void ScreenImpl::setCursor( const Hmi::Cursor* cursor )
{	
	Cursor = (cursor == 0 ? Hmi::Cursor::defaultCursor() : *cursor );	
}

}}
