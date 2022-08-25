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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#ifndef Pt_Hmi_cocoa_ApplicationImpl_h
#define Pt_Hmi_cocoa_ApplicationImpl_h

#include "posix/Selector.h"
#include "SelectableList.h"

#include <Pt/System/EventLoop.h>
#include <Pt/System/Selectable.h>
#include <Pt/System/IONotifier.h>
#include <Pt/Singleton.h>

#include <vector>

#import <CoreFoundation/CFRunLoop.h>
#import <CoreFoundation/CFFileDescriptor.h>

namespace Pt {

namespace Hmi {
	
class Cursor;
class Widget;
class Window;
class KeyEvent;
class MouseEvent;

class ApplicationImpl : public Pt::System::EventLoop, 
                        public System::Selector
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
        ApplicationImpl();

        virtual ~ApplicationImpl();

        void setCursor(const Cursor* cursor)
        {}

        void setFontDir(const Pt::System::Path& dir)
        {}

        void setDefaultFont(const std::string& fontName);

        Pt::Timespan inactivityTime() const
        { return Pt::Timespan(0); }

        void sendKeyEvent(const KeyEvent& ev)
        {}

        void sendMouseEvent(const MouseEvent& ev)
        {}

    public:
        System::Selector& selector()
        { 
            return *this; 
        }

        void processTimers();
    
        void nextEvent();

    protected:
        virtual void onAttachSelectable(System::Selectable&);

        virtual void onDetachSelectable(System::Selectable&);

        virtual void onCancel(System::Selectable& s);

        virtual void onReady(System::Selectable& s);

        virtual void onRun();

        virtual void onExit();

        virtual void onCommitEvent(const Pt::Event& event);

        virtual void onQueueEvent(const Pt::Event& event);

        virtual void onWake();

        virtual void onProcessEvents();

        virtual void onAttachTimer(System::Timer& timer);

        virtual void onDetachTimer(System::Timer& timer);

    protected:
        virtual void cancel(System::IOHandle& h);

        virtual void beginWait(System::IOHandle* h, int flags);
        
        virtual int endWait(System::IOHandle* h);

        virtual void beginRead(System::IOHandle* h);

        virtual void endRead(System::IOHandle* h);

        virtual void beginWrite(System::IOHandle* h);

        virtual void endWrite(System::IOHandle* h);

        virtual bool isReadable(System::IOHandle* h);

        virtual bool isWritable(System::IOHandle* h);

        virtual bool isReady(System::IOHandle* h);

        virtual bool isError(System::IOHandle* h);

    private:
        void init();

        void waitNext();

        IOEntry& enableIOHandle(System::IOHandle* h);

    private:
        System::Mutex                    _mutex;
        System::SelectableList           _selectables;
        std::vector<IOEntry>             _iotable;
        std::vector<System::Selectable*> _avail;
        System::TimerQueue               _timerQueue;
        System::EventQueue               _eventQueue;
        CFRunLoopSourceRef               _wakeSource;
        CFRunLoopTimerRef                _masterTimer;
};

} // namespace

} // namespace

#endif // include guard
