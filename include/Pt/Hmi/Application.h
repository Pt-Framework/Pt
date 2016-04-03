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
#ifndef Pt_Hmi_Application_h
#define Pt_Hmi_Application_h

#include <Pt/System/Application.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/Screen.h>

namespace Pt {

namespace Hmi {

class ApplicationImpl;
class Window;
class Widget;
class EraseEvent;

class PT_HMI_API Application : public Pt::System::Application
{
    public:
        Application(int argc = 0, char** argv = 0);

        virtual ~Application();

        static Application& instance();
    
        void nextEvent();

        void sendEvent(Visual& w, const Pt::Event& ev);

        const Screen& mainScreen() const
        {
            return *_mainScreen;
        }

        Screen& mainScreen()
        {
            return *_mainScreen;
        }
/*
        Widget* findWidget( const std::string& name );

        Window* findWindow(const std::string& name);
        */
        ApplicationImpl* impl()
        {
            return _impl;
        }

        //TODO: Style
        double windowBorderWidth() const
        {
            return _windowBorderWidth;
        }

        double windowTitleHeight()  const
        {
            return _windowTitleHeight;
        }
        
        Pt::uint64_t makeVid()
        {
            return _lastVid++;
        }

        void registerVisual(Visual& visual);

        void unregisterVisual(Visual& visual);

        void resize(Window& w, const Gfx::SizeF& s);

        void move(Window& w, const Gfx::PointF& p);

        void show(Window& w, bool visible);

        void enable(Window& w, bool enable);

        void activate(Window& w);

        void onUpdate(Window& w, const Gfx::RectF& rect);
        
    protected:        
        void onResizeEvent( const ResizeEvent& ev );

        void onUpdateEvent( const UpdateEvent& ev );

        void onPaintEvent( const PaintEvent& ev );

        void onMoveEvent( const MoveEvent& ev );

        void onEraseEvent( const EraseEvent& ev );

    private:
        void updateWindow(Window& w, const Gfx::RectF& rect);

        void updateWidget( Widget& parent, const Gfx::RectF& rect );
                
    private:
        typedef std::map<Pt::uint64_t, Visual*> VisualMap;

        class UpdateInfo
        {
            public:
                explicit UpdateInfo(const Gfx::RectF& rect)
                : _n(1)
                , _rect(rect)
                { }

                const Gfx::RectF& rect() const
                {
                    return _rect;
                }

                void push(const Gfx::RectF& rect)
                {
                    if(_n == 0)
                        _rect = rect;
                    else
                        _rect.unify(rect);

                    ++_n;
                }

                int pop()
                {
                    return --_n;
                }

            private:
                int _n;
                Gfx::RectF _rect;
        };

        typedef std::multimap<Pt::uint64_t, UpdateInfo> UpdateMap;

        ApplicationImpl* _impl; 
        Screen* _mainScreen;  
        double _windowBorderWidth;
        double _windowTitleHeight;
        Pt::uint64_t _lastVid;
        VisualMap _visuals;
        UpdateMap _updates;
};

} // namespace

} // namespace

#endif
