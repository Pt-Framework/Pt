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

#ifndef PT_HMI_VISUAL_H
#define PT_HMI_VISUAL_H

#include <Pt/Hmi/Api.h>
#include <Pt/Types.h>
#include <Pt/Event.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <string>

namespace Pt {
namespace Hmi {

class ResizeEvent;
class MoveEvent;
class ShowEvent;
class EnableEvent;
class MouseEvent;
class TouchEvent;
class ScrollEvent;
class EnterEvent;
class LeaveEvent;
class PaintEvent;
class KeyEvent;

class PT_HMI_API Visual : public Pt::Connectable
{
    public:
        virtual ~Visual();

        void processEvent(const Pt::Event& ev);    
                
        virtual void resize( const Gfx::SizeF& s ) = 0;

        virtual void move( const Gfx::PointF& p ) = 0;

        virtual void show( bool b = true ) = 0;

        virtual void enable( bool b = true ) = 0;

        virtual void repaint( const Gfx::RectF& rect ) = 0;  

        const Gfx::SizeF& size() const
        {
            return _size;
        }

        const Gfx::PointF& position() const
        {
            return _position;
        }

        Pt::uint64_t vid() const
        {
            return _vid;
        }

        bool enabled() const
        {
            return _enabled;
        }

        bool isVisible() const
        {
            return _visible;
        }

        void setName( const std::string& n )
        {
            _name = n;
        }

        const std::string& name() const
        {
            return _name;
        }
    protected:
        Visual();       
                       
        virtual void onResizeEvent( const ResizeEvent& ev );

        virtual void onMoveEvent( const MoveEvent& ev );

        virtual void onShowEvent( const ShowEvent& ev );
 
        virtual void onEnableEvent( const EnableEvent& ev );

        virtual void onKeyEvent( const KeyEvent& ev );

        virtual void onPointerEvent( const MouseEvent& ev );

        virtual void onTouchEvent( const TouchEvent& ev );
    
        virtual void onScrollEvent( const ScrollEvent& ev );
    
        virtual void onPaintEvent( const PaintEvent& ev );


    protected:
         Pt::Signal<const Pt::Event&>  _eventReady;

        Pt::uint64_t _vid;
        std::string  _name;        
        bool         _enabled;
        bool         _visible;
        Gfx::PointF  _position;
        Gfx::SizeF   _size;

};

}}

#endif