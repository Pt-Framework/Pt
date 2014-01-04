/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Drner                                  *
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
#include <linux/fb.h>
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
#include <Pt/System/Event.h>

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

            void handleEvents(input_event* events, int rd);
            void handleJEvents(js_event& events, int rd);
        private:
            bool _exit;
            int _highestFd;
            int _fd1;
            int _fd2;
            int _fd3;
            fd_set _fds;
    };


    class PT_GUI_API LfbEventLoop : public Pt::System::EventLoop, public Singleton<LfbEventLoop>
    {
        friend class Singleton<LfbEventLoop>;

        public:
            LfbEventLoop();

            ~LfbEventLoop();

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
            { return _screenInfo.bits_per_pixel;}

            Pt::ssize_t width() const
            { return _screenInfo.xres;}

            Pt::ssize_t height() const
            { return _screenInfo.yres;}

            char* frameBuffer()
            { return (char*)_buffer; }

        private:
            int _fd;
            fb_var_screeninfo _screenInfo;
            fb_fix_screeninfo _fixedInfo;
            void* _buffer;
            Pt::size_t _bufferSize;
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
