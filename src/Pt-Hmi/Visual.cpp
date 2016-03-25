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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/
#include <Pt/Hmi/Visual.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/MoveEvent.h>
#include <Pt/Hmi/ShowEvent.h>
#include <Pt/Hmi/EnableEvent.h>

namespace Pt{
namespace Hmi{


Visual::Visual()
: _vid( Application::instance().makeVid()  )
, _enabled(true)
, _visible(true)
, _size(10,10)
, _position(0,0)
{ 
    Application::instance().registerVisual(*this );   

    _eventReady += Pt::slot(*this, &Visual::onResizeEvent );
    _eventReady += Pt::slot(*this, &Visual::onMoveEvent );
    _eventReady += Pt::slot(*this, &Visual::onShowEvent );
    _eventReady += Pt::slot(*this, &Visual::onKeyEvent );
    _eventReady += Pt::slot(*this, &Visual::onPointerEvent );
    _eventReady += Pt::slot(*this, &Visual::onTouchEvent );
    _eventReady += Pt::slot(*this, &Visual::onScrollEvent );
    _eventReady += Pt::slot(*this, &Visual::onPaintEvent );
}


Visual::~Visual()
{
    Application::instance().unregisterVisual(*this );
}

        
void Visual::processEvent(const Pt::Event& ev)
{
    _eventReady.send(ev );    
}


void Visual::resize( const Gfx::SizeF& s )
{
        
}

void Visual::move( const Gfx::PointF& p )
{
    
}

void Visual::show( bool b )
{

}

void Visual::enable( bool b )
{

}

void Visual::repaint( const Gfx::RectF& rect )
{

}

void Visual::onResizeEvent(const ResizeEvent& ev)
{
    _size = ev.size();
}


void Visual::onMoveEvent( const MoveEvent& ev)
{
    _position = ev.position();
}


void Visual::onShowEvent( const ShowEvent& ev)
{
    _visible = ev.visible();
}


void Visual::onEnableEvent( const EnableEvent& ev)
{
    _enabled = ev.enabled();
}


void Visual::onKeyEvent( const KeyEvent& ev )
{
}
     
void Visual::onPointerEvent( const MouseEvent& ev )
{
}     

void Visual::onTouchEvent( const TouchEvent& ev )
{
}    

void Visual::onScrollEvent( const ScrollEvent& ev )
{
}    

void Visual::onPaintEvent( const PaintEvent& ev )
{
}


}}
