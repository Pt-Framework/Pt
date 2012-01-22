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
#import <CoreFoundation/CFFileDescriptor.h>

namespace Pt {

namespace Gui {

class Selector
{
    struct IOEntry
    {
        IOEntry()
        : iohandle(0)
        , flags(0)
        { }

        IOEntry(System::IOHandle& io, CFRunLoopSourceRef s, CFFileDescriptorRef fd)
        : iohandle(&io)
        , source(s)
        , fd(fd)
        , flags(0)
        { }

        IOEntry(const IOEntry& e)
        : iohandle(e.iohandle)
        , source(e.source)
        , fd(e.fd)
        , flags(0)
        { }

        IOEntry& operator=(const IOEntry& e)
        {
            iohandle = e.iohandle;
            source = e.source;
            fd = e.fd;
            return *this;
        }

        System::IOHandle* iohandle;
        CFRunLoopSourceRef source;
        CFFileDescriptorRef fd;
        CFOptionFlags flags;
    };

    public:
        Selector();

        virtual ~Selector();

        virtual void cancel(System::IOHandle& h);

        virtual void beginRead(System::IOHandle* h);

        virtual void endRead(System::IOHandle* h);

        virtual void beginWrite(System::IOHandle* h);

        virtual void endWrite(System::IOHandle* h);

        virtual bool isReadable(System::IOHandle* h);

        virtual bool isWritable(System::IOHandle* h);

        virtual bool isError(System::IOHandle* h);

        IOEntry& enableIOHandle(System::IOHandle* h);

    private:
        std::vector<IOEntry> _iotable;
};

class MainLoopImpl : public System::EventLoopImpl
{
    struct IOEntry
    {
        IOEntry()
        : iohandle(0)
        , flags(0)
        { }

        IOEntry(System::IOHandle& io, CFRunLoopSourceRef s, CFFileDescriptorRef fd)
        : iohandle(&io)
        , source(s)
        , fd(fd)
        , flags(0)
        { }

        IOEntry(const IOEntry& e)
        : iohandle(e.iohandle)
        , source(e.source)
        , fd(e.fd)
        , flags(0)
        { }

        IOEntry& operator=(const IOEntry& e)
        {
            iohandle = e.iohandle;
            source = e.source;
            fd = e.fd;
            return *this;
        }

        System::IOHandle* iohandle;
        CFRunLoopSourceRef source;
        CFFileDescriptorRef fd;
        CFOptionFlags flags;
    };

    public:
        MainLoopImpl(Signal<const Pt::Event&>& eventSignal);

        MainLoopImpl(Signal<const Pt::Event&>& eventSignal, Allocator& a);

        virtual ~MainLoopImpl();

        void run();

        void exit();

        void wake();

        void commitEvent(const Pt::Event& ev);

        void queueEvent(const Pt::Event& ev);

        bool processEvents();

        void processTimers();

        void attach(System::Timer& timer);

        void detach(System::Timer& timer);

        void attach(System::Selectable& s);

        void detach(System::Selectable& s);

        void idle(System::Selectable& s);

        void avail(System::Selectable& s);

        virtual void cancel(System::IOHandle& h);

        virtual void beginRead(System::IOHandle* h);

        virtual void endRead(System::IOHandle* h);

        virtual void beginWrite(System::IOHandle* h);

        virtual void endWrite(System::IOHandle* h);

        virtual bool isReadable(System::IOHandle* h);

        virtual bool isWritable(System::IOHandle* h);

        virtual bool isError(System::IOHandle* h);

    private:
        void init();

        IOEntry& enableIOHandle(System::IOHandle* h);

    private:
        System::Mutex _mutex;
        System::SelectableList _selectables;
        std::vector<IOEntry> _iotable;
        std::vector<System::Selectable*> _avail;
        System::TimerQueue _timerQueue;
        System::EventQueue _eventQueue;
        Signal<const Pt::Event&>* _event;
        CFRunLoopSourceRef _wakeSource;
        CFRunLoopTimerRef _masterTimer;
};


class MainLoop : public Pt::System::EventLoop
               , public Pt::Singleton<MainLoop>
{
    public:
        MainLoop();

        ~MainLoop();

        System::EventLoopImpl& impl()
        { return _impl; }

    protected:
        virtual void onAttachSelectable(System::Selectable&);

        virtual void onDetachSelectable(System::Selectable&);

        virtual void onIdle(System::Selectable& s);

        virtual void onReady(System::Selectable& s);

        virtual void onRun();

        virtual void onExit();

        virtual void onCommitEvent(const Pt::Event& event);

        virtual void onQueueEvent(const Pt::Event& event);

        virtual void onProcessEvents();

        virtual void onWake();

        virtual void onAttachTimer(System::Timer& timer);

        virtual void onDetachTimer(System::Timer& timer);

    private:
        MainLoopImpl _impl;
};

} // namespace Gui

} // namespace Pt

#endif

