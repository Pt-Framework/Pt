/* Copyright (C) 2013 Marc Boris Duerner

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

#ifndef Pt_Hmi_MainWindowImpl_h
#define Pt_Hmi_MainWindowImpl_h

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>

namespace Pt {

namespace Hmi {

class ScreenImpl;

class MainWindowImpl : public WindowImpl
{
    typedef WindowImpl Base;

    friend class ScreenImpl;

    public:
        MainWindowImpl(ScreenImpl& wm,  Window& w);

        virtual ~MainWindowImpl();

        void setType(Window::Type type);

        Gfx::PointF toScreen(const Gfx::PointF& pos) const;

        Gfx::PointF fromScreen(const Gfx::PointF& pos) const;

        void paint(const Gfx::RectF& rect);

        //void show(bool visible);

        //void activate();

        //void enable(bool e);

        //void move(const Gfx::PointF& pos);

        //void resize(const Gfx::SizeF& size);

        //void setAbove(bool above);

        //void setState(const WindowState& s);

        //void setTitle(const std::string& text);

        //void setIcon(const Gfx::Image& p);

        void setSizeLimits(const Gfx::SizeF& minSize,
                           const Gfx::SizeF& maxSize);

    public:
        ::Window& window()
        {
            return _window;
        }

        int width() const
        {
           return _width;
        }

        int height() const
        {
           return _height;
        }

        void setSize(int w, int h)
        {
           _width = w;
           _height = h;
        }

    protected:
        virtual void onInit(Window& w);

        virtual void onRelease(Window& w);

        virtual Gfx::PointF onToWindow(const Window& w, 
                                       const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromWindow(const Window& w, 
                                         const Gfx::PointF& pos) const;
        
        virtual void onSetTitle(Window& w, const std::string& text);

        virtual void onSetIcon(Window& w, const Gfx::Image& icon);

        virtual void onSetState(Window& w, const WindowState& state);

        virtual void onSetAbove(Window& w, bool above);

        virtual void onRepaint(Window& w, const Gfx::RectF& rect);

        virtual void onShow(Window& w, bool visible);

        virtual void onActivate(Window& w, bool active);

        virtual void onEnable(Window& w, bool enable);

        virtual void onMove(Window& w, const Gfx::PointF& to);

        virtual void onResize(Window& w, const Gfx::SizeF& s);

        virtual void onClose(Window& w);

    protected:
        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;
        
        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        
        virtual void onProcessPaintEvent(const PaintEvent& ev);

        virtual void onPaintEvent(const PaintEvent& ev);


        virtual void onProcessShowEvent(const ShowEvent& ev);

        virtual void onShowEvent(const ShowEvent& ev);


        virtual void onProcessEnableEvent(const EnableEvent& ev);

        virtual void onEnableEvent(const EnableEvent& ev);


        virtual void onProcessActivateEvent(const ActivateEvent& ev);

        virtual void onActivateEvent(const ActivateEvent& ev);


        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);
        
        virtual void onRescaleEvent(const RescaleEvent& ev);

        
        virtual void onProcessWindowStateEvent(const WindowStateEvent& ev);

        virtual void onWindowStateEvent(const WindowStateEvent& ev);


        virtual void onProcessCloseEvent(const CloseEvent& ev);

        virtual void onCloseEvent(const CloseEvent& ev);

    private:
        void create(Window::Type type);

        void destroy();

        bool isMinimized();

        bool isMaximized();

    private:
        ScreenImpl&    _wm;
        Window&        _client;
        ::Window       _window;
        ::Display*     _display;
        bool           _hasFirstShow;
        int            _width;
        int            _height;
};

} // namespace

} // namespace

#endif // include guard
