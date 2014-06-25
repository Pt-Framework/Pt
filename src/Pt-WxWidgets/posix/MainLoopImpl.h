/* 
 * Copyright (C) 2014 Marc Boris Dürner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef Pt_WxWidgets_MainLoopImpl_h
#define Pt_WxWidgets_MainLoopImpl_h

#include <posix/Selector.h>
#include <posix/../SelectableList.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Mutex.h>
#include <wx/evtloop.h>
#include <wx/evtloopsrc.h>
#include <wx/timer.h>
#include <vector>

namespace Pt {

namespace WxWidgets {

class IOHandler : public wxEventLoopSourceHandler
{
    public:
        IOHandler(System::IOHandle& h)
        : _h(&h)
        , _readSource(0)
        , _writeSource(0)
        { }

        ~IOHandler()
        {
            delete _readSource;
            delete _writeSource;
        }

        void OnReadWaiting()
        {
            setReadSource(0);

            _h->events &= ~System::IOHandle::Read;
            _h->ready = System::IOHandle::Read;

            System::Selectable* s = _h->sel;
            s->run();
        }

        void OnWriteWaiting()
        {
            setWriteSource(0);

            _h->events &= ~System::IOHandle::Write;
            _h->ready = System::IOHandle::Write;

            System::Selectable* s = _h->sel;
            s->run();
        }

        void OnExceptionWaiting()
        {}

        void setReadSource(wxEventLoopSource* source)
        { 
            delete _readSource;
            _readSource = source; 
        }

        void setWriteSource(wxEventLoopSource* source)
        { 
            delete _writeSource;
            _writeSource = source; 
        }

    private:
        System::IOHandle* _h;
        wxEventLoopSource* _readSource;
        wxEventLoopSource* _writeSource;
};


class Selector : public System::Selector
{
    public:
        Selector(wxEventLoopBase& wxLoop)
        : _wxLoop(wxLoop)
        , _current(0)
        { }

        ~Selector()
        {
            while( ! _selectables.empty() )
            {
                _selectables.first()->detach();
            }
        }

        void attach(System::Selectable& s)
        {
            _selectables.insert(s);
        }
        
        void detach(System::Selectable& s)
        {
            System::SelectableList::unlink(s);
        }

        void cancel(System::IOHandle& h)
        {
            if(h.id == System::IOHandle::InvalidId)
                return;

            IOMap::iterator it = _iomap.find(&h);
            if( it != _iomap.end() )
            {
                delete it->second;
                _iomap.erase(it);
            }

            h.id = System::IOHandle::InvalidId;
            h.ready = 0;
            h.events = 0;
        }
        
        IOHandler& getHandler(System::IOHandle* h)
        {
            if(h->id == System::IOHandle::InvalidId)
            {
                IOHandler* handler = new IOHandler(*h);
                _iomap[h] = handler;
                h->id = 1;
                return *handler;
            }

            return *_iomap[h];
        }

        void beginRead(System::IOHandle* h)
        {
            IOHandler& handler = getHandler(h);

            wxEventLoopSource* source = _wxLoop.AddSourceForFD(h->fd, &handler, wxEVENT_SOURCE_INPUT);
            handler.setReadSource(source);

            h->events = System::IOHandle::Read;
        }

        void endRead(System::IOHandle* h)
        {
            if(h->events & System::IOHandle::Read)
            {
                 IOHandler& handler = getHandler(h);
                 handler.setReadSource(0);
            }

            h->ready = 0;
            h->events &= ~System::IOHandle::Read;
        }

        void beginWrite(System::IOHandle* h)
        {
            IOHandler& handler = getHandler(h);

            wxEventLoopSource* source = _wxLoop.AddSourceForFD(h->fd, &handler, wxEVENT_SOURCE_OUTPUT);
            handler.setWriteSource(source);

            h->events = System::IOHandle::Write;
        }

        void endWrite(System::IOHandle* h)
        {
            if(h->events & System::IOHandle::Write)
            {
                 IOHandler& handler = getHandler(h);
                 handler.setWriteSource(0);
            }

            h->ready = 0;
            h->events &= ~System::IOHandle::Write;
        }

        bool isReadable(System::IOHandle* h)
        {
            bool isReady = h->ready == System::IOHandle::Read;
            return isReady;
        }

        bool isWritable(System::IOHandle* h)
        {
            bool isReady = h->ready == System::IOHandle::Write;
            return isReady;
        }

        bool isError(System::IOHandle* h)
        {
            return false;
        }

        void wake()
        {
            _wakePipe.wake();
        }

        bool isWoken()
        { return _wakePipe.isReady(); }

        int wakeFd()
        { return _wakePipe.readFd(); }

        typedef std::map<System::IOHandle*, IOHandler*> IOMap;

    private:
        wxEventLoopBase& _wxLoop;
        System::WakePipe _wakePipe;
        System::SelectableList _selectables;
        IOMap _iomap;
        System::Selectable* _current;
};

class MainLoopImpl : public wxEventLoopSourceHandler
                   , public wxEvtHandler
{
    public:
        MainLoopImpl(wxEventLoopBase& wxLoop, Signal<const Pt::Event&>& ev);

        virtual ~MainLoopImpl();

        Pt::System::Selector& selector()
        { return _selector; }
   
        virtual void attachSelectable(System::Selectable&);

        virtual void detachSelectable(System::Selectable&);

        virtual void cancel(System::Selectable& s);

        virtual void ready(System::Selectable& s);

        virtual void run();

        virtual void exit();

        virtual void commitEvent(const Pt::Event& ev);

        virtual void queueEvent(const Pt::Event& ev);

        virtual void wake();

        virtual void attachTimer(System::Timer& timer);

        virtual void detachTimer(System::Timer& timer);

    protected:
        void OnReadWaiting()
        { onWake(); }

        void OnWriteWaiting()
        {}

        void OnExceptionWaiting()
        {}

        void onMasterTimer(wxTimerEvent& )
        { processTimers(); }

        void onWake();

        void processTimers();

    private:
        System::Mutex _mutex;
        Signal<const Event&>& _event;
        wxTimer _masterTimer;
        wxEventLoopSource* _wakeSource;
        System::TimerQueue _timerQueue;
        System::EventQueue _eventQueue;
        std::vector<System::Selectable*> _avail;
        Selector _selector;
};

} // namespace

} // namespace

#endif
