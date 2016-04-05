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

#ifndef Pt_Hmi_Screen_H
#define Pt_Hmi_Screen_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Visual.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Hmi {

class ScreenImpl;
class ApplicationImpl;

class PT_HMI_API Screen : public Visual
{
    friend class Window;
    friend class WindowManager;
    friend class MainWindowImpl;

    public:
        Screen(ApplicationImpl& app);

        virtual ~Screen();

        double width() const;

        double height() const;
    
        Gfx::SizeF size() const
        {
            return Gfx::SizeF( width(), height() );
        }

        Gfx::PointF toUnit( const Gfx::Point& value );
      
        Gfx::SizeF toUnit( const Gfx::Size& value );
      
        double toUnit( int value );

        Gfx::Point fromUnit( const Gfx::PointF& value );
      
        Gfx::Size fromUnit( const Gfx::SizeF& value );
      
        Gfx::Rect fromUnit( const Gfx::RectF& value );
      
        int fromUnit( double value );

        double unitSizeInch() const;
      
        double unitSizeMm() const;

        void setResolution( double dpi );
      
        double resolutionDPI() const;
    
        void setCursor( const Cursor* cursor = 0 );

        ScreenImpl* impl()
        {
            return _impl;
        }        

        const std::vector<Window*>& windows() const
        {
          return _windows;
        }

        std::vector<Window*>& windows()
        {
          return _windows;
        }

    protected:
        void registerWindow(Window& w);

        void unregisterWindow(Window& w);

        virtual void onEvent( const Event& ev );

        void setPointerWindow( Window* w)
        {
            if( _pointerWindow == w )
                return;

            if( _pointerWindow )    
            {
                Pt::Hmi::LeaveEvent leaveEvent;
                _pointerWindow->processEvent(leaveEvent);
            }

            _pointerWindow = w;

            if( _pointerWindow )
            {
                Pt::Hmi::EnterEvent enterEvent;
                _pointerWindow->processEvent(enterEvent);
            }
          }

    private:
        ScreenImpl*          _impl;
        std::vector<Window*> _windows;
        Window*              _pointerWindow;
};

} // namespace

} // namespace

#endif
