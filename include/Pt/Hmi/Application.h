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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 MA 02110-1301 USA
*/

#ifndef Pt_Hmi_Application_h
#define Pt_Hmi_Application_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/Screen.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/System/Application.h>

namespace Pt {

namespace Hmi {

class ApplicationImpl;
class Window;
class Widget;

class PT_HMI_API Application : public Pt::System::Application
{
    public:
        Application(int argc = 0, char** argv = 0);

        virtual ~Application();

        static Application& instance();
    
        void nextEvent();

        void sendEvent(Visual& w, const Pt::Event& ev);

        const Screen& screen() const
        {
            return *_mainScreen;
        }

        Screen& screen()
        {
            return *_mainScreen;
        }
        
        ApplicationImpl* impl()
        {
            return _impl;
        }
        
        Pt::uint64_t makeVid()
        {
            return _lastVid++;
        }

        void registerVisual(Visual& visual);

        void unregisterVisual(Visual& visual);
        
    protected:        
        void onResizeEvent( const ResizeEvent& ev );

        void onMouseEvent(const MouseEvent& ev );

        void onUpdateEvent(const UpdateEvent& ev);

        void onPaintEvent( const PaintEvent& ev );

        void onMoveEvent( const MoveEvent& ev );

        void onActivateEvent( const ActivateEvent& ev );
                
    private:
        typedef std::map<Pt::uint64_t, Visual*> VisualMap;

        ApplicationImpl* _impl; 
        Screen* _mainScreen;  
        Pt::uint64_t _lastVid;
        VisualMap _visuals;
};

} // namespace

} // namespace

#endif
