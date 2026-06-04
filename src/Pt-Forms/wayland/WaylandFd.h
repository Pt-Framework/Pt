/* Copyright (C) 2015-2026 Marc Boris Duerner 
  
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

#ifndef Pt_Forms_WaylandFd_h
#define Pt_Forms_WaylandFd_h

#include "posix/Selector.h"

#include <Pt/Forms/Api.h>
#include <Pt/System/Selectable.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Signal.h>

#include <wayland-client.h>

namespace Pt {
namespace Forms {

class WaylandFd : public System::Selectable
{
    public:
        WaylandFd(struct wl_display* display);
        ~WaylandFd();

        void begin();
        void close();
        void processEvents();

        Pt::Signal<int>& eventReady()
        { return _eventReady; }

    protected:
        virtual bool onRun();
        virtual void onCancel();
        void onAttach(System::EventLoop& loop);
        void onDetach(System::EventLoop& loop);

    private:
        struct wl_display*         _display;
        Pt::System::IOHandle       _ioh;
        Pt::System::EventLoop*     _loop;
        Pt::Signal<int>            _eventReady;
};

} // namespace Forms
} // namespace Pt

#endif
