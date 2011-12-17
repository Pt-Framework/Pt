/***************************************************************************
 *   Copyright (C) 2008 Marc Boris Duerner                                 *
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
#ifndef Pt_Gui_cocoa_ApplicationImpl_h
#define Pt_Gui_cocoa_ApplicationImpl_h

#include "posix/MainLoopImpl.h"

#include <Pt/Gui/Api.h>
#include <Pt/Gui/Event.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Selectable.h>
#include <Pt/Singleton.h>

#import <CoreFoundation/CFRunLoop.h>

namespace Pt {

namespace Gui {

class MainLoopImpl : public System::EventDispatcher
{
    public:
        MainLoopImpl();

        MainLoopImpl(Allocator& a);

        virtual ~MainLoopImpl();

        void attach(System::Selectable& s)
        {}

        void detach(System::Selectable& s)
        {}

        void enable(System::Selectable& s)
        {}

        void disable(System::Selectable& s)
        {}

        void idle(System::Selectable& s)
        {}

        void active(System::Selectable& s)
        {}

        void avail(System::Selectable& s)
        {}

    protected:
        virtual void onRun();

        virtual void onWake();

    private:
        CFRunLoopSourceRef _wakeSource;
};


class MainLoop : public Pt::System::EventLoop
               , public Pt::Singleton<MainLoop>
{
    public:
        MainLoop();

        ~MainLoop();

        System::EventLoopImpl& impl()
        { return *_eimpl; }

    protected:
        virtual void onAttach(System::Selectable&);

        virtual void onDetach(System::Selectable&);

        virtual void onEnable(System::Selectable& s);

        virtual void onDisable(System::Selectable& s);

        virtual void onIdle(System::Selectable& s);

        virtual void onActive(System::Selectable& s);

        virtual void onAvail(System::Selectable& s);

        virtual void onRun();

        virtual Signal<const Pt::Event&>& onEvent();

        virtual void onExit();

        virtual void onCommitEvent(const Pt::Event& event);

        virtual void onQueueEvent(const Pt::Event& event);

        virtual void onProcessEvents();

        virtual void onWake();

        virtual void onAddTimer(System::Timer& timer);

        virtual void onRemoveTimer(System::Timer& timer);

    private:
        MainLoopImpl _impl;
        System::EventLoopImpl* _eimpl;
};

} // namespace Gui

} // namespace Pt

#endif

