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
, _image( _frameBuffer.buffer(),  Ui::Size( (size_t)_frameBuffer.width(), (size_t)_frameBuffer.height() ) ,  _frameBuffer.format(), _frameBuffer.strideInBytes() )
, _dpi(96.0)
{  
	Visible   = true;	
	Size      = Ui::SizeF( _frameBuffer.width(), _frameBuffer.height() );
	BackColor = Ui::Color( 170/255.0f, 170/255.0f, 170/255.0f );	
	
  eventReceived() += Pt::slot( *this, &ScreenImpl::onPointerInput );
}


ScreenImpl::~ScreenImpl()
{

}


void ScreenImpl::onPointerInput( const Pt::Hmi::PointerEvent& mouseEvent )
{	
	if( _cursorBackground.width() != 0  && _cursorBackground.height() != 0 )
		_frameBuffer.bitBlit( _cursorBackground.pixel(0,0), _cursorBackground.width(), _cursorBackground.height(), _cursorPos, FrameBuffer::CopyOp );

	_windowManager.pointerInput( mouseEvent );	

	if( Cursor.get().width() == 0 )
		return;	

   _cursorPos = Ui::Point( mouseEvent.x(), mouseEvent.y() );	

  _frameBuffer.grabImage( _cursorBackground, _cursorPos, Ui::Size( Cursor.get().width(), Cursor.get().height() ) );

  _frameBuffer.bitBlit( &Cursor.get().andRgb888()[0], Cursor.get().width(), Cursor.get().height(), Ui::Point( ( int) mouseEvent.x(), (int) mouseEvent.y()), FrameBuffer::AndOp );

  _frameBuffer.bitBlit( &Cursor.get().xorRgb888()[0], Cursor.get().width(), Cursor.get().height(), Ui::Point( (int) mouseEvent.x(), (int) mouseEvent.y()), FrameBuffer::XorOp );  
}


void ScreenImpl::onInvalidate()
{		
	Window::onInvalidate(); 

  Painter& painter = surface().painter();

  painter.setOrigin( Ui::PointF( 0,0) );
  painter.setClip( Ui::RectF( painter.origin(), size() ) );
  painter.flush();  
}


void ScreenImpl::setCursor( const Hmi::Cursor* cursor )
{	
	 Cursor = (cursor == 0 ? Hmi::Cursor::defaultCursor() : *cursor );
}

}}
