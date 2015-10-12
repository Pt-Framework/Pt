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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA  02110-1301  USA
*/

#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include "PaintSurfaceImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/System/Clock.h>
#include <algorithm>

namespace Pt {

namespace Hmi {
  
ScreenImpl::ScreenImpl(ApplicationImpl& app)
: _dpi(96.0)
{
  app.eventReady() += Pt::slot(_eventReceived);

  //Size.set( Gfx::SizeF(_frameBuffer.width(), _frameBuffer.height()) );
  BackColor.set( Gfx::Color(170/255.0f, 170/255.0f, 170/255.0f) );
  Visible.set(true);      
  setCursor(0);  
  
  eventReceived() += Pt::slot( *this, &ScreenImpl::onPointerInput );
}


ScreenImpl::~ScreenImpl()
{
}


void ScreenImpl::onPointerInput( const Pt::Hmi::PointerEvent& mouseEvent )
{    
  //Pt::System::Clock clock;
  //clock.start();

  //_drawCursor =  true;

  //if( !_cursorBackground.empty() )
  //  bitBlit( _cursorBackground.pixel(0,0), _cursorBackground.width(), _cursorBackground.height(), _cursorPos, (Pt::uint8_t*)  _image.pixel(0,0), CopyOp );

  //if( Cursor.get().width() != 0 )
  //  _cursorPos = Gfx::Point( mouseEvent.x() - Cursor.get().xHotspot() , mouseEvent.y() - Cursor.get().yHotspot());

  _windowManager.pointerInput( mouseEvent );    
  
  //if( _drawCursor )
  //  updateScreen();

  //std::clog << "screen update: " << clock.stop().toUSecs() / 1000.0 << " msecs" << std::endl;
}


void ScreenImpl::onInvalidate()
{    
  //Load the render pipeline
  Window::render();
 
}


void ScreenImpl::setCursor( const Hmi::Cursor* cursor )
{    
  Cursor = (cursor == 0 ? Hmi::Cursor::defaultCursor() : *cursor );    
}

}

}
