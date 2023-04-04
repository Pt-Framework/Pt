/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
   Copyright (C) 2015 Marc Duerner
   
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

#ifndef Pt_Hmi_MainWindowImpl_H
#define Pt_Hmi_MainWindowImpl_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Windows.h>

namespace Pt {

namespace Hmi {

class Screen;

class MainWindowImpl : public WindowImpl
{
    typedef WindowImpl Base;

    public:
        MainWindowImpl(WindowManager& wm,  Window& w);

        ~MainWindowImpl();

        void setType(WindowType type);

        Gfx::PointF toScreen(const Gfx::PointF& pos) const;

        Gfx::PointF fromScreen(const Gfx::PointF& pos) const;

        void paint(const Gfx::RectF& rect);

        void show(bool v);
    
        void activate();

        void enable(bool e);  
       
        void move(const Gfx::PointF& pos);

        void setAbove(bool isTop);

        //void setState(const WindowState& s);

        //void setTitle(const std::string& text);

        //void setIcon(const Gfx::Image& p);

        void setMinimumSize(const Gfx::SizeF& s);
    
        void setMaximumSize(const Gfx::SizeF& s);

        HWND hwnd()
        {
            return _hwnd;
        }

    protected:
        virtual void onSetTitle(Window& w, const std::string& text);

        virtual void onSetIcon(Window& w, const Gfx::Image& icon);

        virtual void onSetState(Window& w, const WindowState& state);

        virtual void onResize(Window& w, const Gfx::SizeF& s);

        virtual void onClose(Window& w);

    protected:
        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);
        
        virtual void onRescaleEvent(const RescaleEvent& ev);

        
        virtual void onProcessWindowStateEvent(const WindowStateEvent& ev);

        virtual void onWindowStateEvent(const WindowStateEvent& ev);

        
        virtual void onProcessCloseEvent(const CloseEvent& ev);

        virtual void onCloseEvent(const CloseEvent& ev);

    private:
        Window& _window;
        HWND    _hwnd;
};

} // namespace

} // namespace

#endif

