/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Drner                                   *
 *   Copyright (C) 2007 Sebastian Pieck                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef Pt_Gui_x11_ApplicationImpl_h
#define Pt_Gui_x11_ApplicationImpl_h

#include <fcntl.h>
#include <unistd.h>
#include <gf/Gf.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <Pt/Gui/Api.h>
#include <Pt/Gui/KeyEvent.h>
#include <Pt/Gui/Application.h>
#include <Pt/Singleton.h>
#include <Pt/Signal.h>
#include <Pt/System/Application.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Mutex.h>
#include <Pt/Event.h>

#include <list>

struct input_event;
struct js_event;


namespace Pt {

namespace Gui {

    class PT_GUI_API InputHandler : public Pt::System::Thread
    {
        public:
            InputHandler();

            ~InputHandler();

            void stop()
            { _exit = true; }

            Signal<int, int> keyEvent;

        protected:
            void run();


        private:
            bool _exit;
            int _highestFd;
            int _fd1;
            int _fd2;
            int _fd3;
            fd_set _fds;
    };


    class PT_GUI_API GfEventLoop : public Pt::System::EventLoop, public Singleton<GfEventLoop>
    {
        friend class Singleton<GfEventLoop>;

        public:
            GfEventLoop();

            ~GfEventLoop();

            void registerWidget(Widget& widget);

            void unregisterWidget(Widget& widget);

            void handleKeyEvent(int code, int value);

        private:
            std::list<Widget*> _widgets;
            Pt::System::Mutex _mutex;
            InputHandler _input;
    };


    class Screen : public Singleton<Screen>
    {
        public:
            Screen();

            ~Screen();

            Pt::ssize_t depth() const
            {
                if (m_display_info.format == GF_FORMAT_PACK_ARGB1555)
                {  return 16; }
                else
                {  return -1;}
            }


            Pt::ssize_t width() const
            { return m_display_info.xres;}

            Pt::ssize_t height() const
            { return m_display_info.yres; }

            gf_surface_t* layerSurface()
            { return &m_layerSurface; }

            gf_surface_t* offscreenSurface()
            { return &m_offscreenSurface; }

            gf_context_t* drawContext()
            { return &m_context; }

            gf_dev_t * devContext()
            { return &m_gdev; }

        private:
            gf_context_t        m_context;
            gf_surface_t        m_offscreenSurface;
            gf_surface_t        m_layerSurface;
            gf_dev_t            m_gdev;
            gf_dev_info_t       m_gdev_info;
            gf_display_t        m_display;
            gf_display_info_t   m_display_info;

            Pt::size_t m_bufferSize;
    };


    class PT_GUI_API ApplicationImpl
    {
        public:
            ApplicationImpl(Application& app);

            ~ApplicationImpl();

            void commitEvent(const Pt::Event& event);

            void queueEvent(const Pt::Event& event);

            void processEvents();

            int run();

            void wake();

            void exit();
    };

} // namespace Gui

} // namespace Pt

#endif
