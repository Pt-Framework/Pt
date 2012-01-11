/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
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
#ifndef PT_SYSTEM_POSIX_MAINLOOPIMPL_H
#define PT_SYSTEM_POSIX_MAINLOOPIMPL_H

#include "Pt/System/Api.h"
#include "Pt/System/IODevice.h"
#include "Pt/System/Clock.h"
#include "Pt/System/Mutex.h"
#include "Pt/System/EventLoop.h"
#include "Pt/System/SystemError.h"
#include "IODeviceImpl.h"

#include <set>
#include <list>
#include <iostream>

#include <sys/select.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

namespace Pt {

namespace System {

class Selector
{
    enum IOFlags
    {
        Input = 1,
        Output = 2,
        Error = 4,
        Enabled = 8
    };

    enum WaitResult
    {
        None = 0,
        Wake = 1
    };

    public:
        Selector()
        {
            _current = _devices.end();
        
            //Open a pipe to send wake up message.
            if( ::pipe( _wakePipe ) )
                throw SystemError( PT_ERROR_MSG("pipe failed") );
        
            int flags = ::fcntl(_wakePipe[0], F_GETFL);
            if(-1 == flags)
                throw SystemError(PT_ERROR_MSG("fcntl failed"));
        
            int ret = ::fcntl(_wakePipe[0], F_SETFL, flags|O_NONBLOCK);
            if(-1 == ret)
                throw SystemError( PT_ERROR_MSG("fcntl failed") );
        
            flags = ::fcntl(_wakePipe[1], F_GETFL);
            if(-1 == flags)
                throw SystemError( PT_ERROR_MSG("fcntl failed") );
        
            ret = ::fcntl(_wakePipe[1], F_SETFL, flags|O_NONBLOCK);
            if(-1 == ret)
                throw SystemError( PT_ERROR_MSG("fcntl failed") );
        
            FD_ZERO(&_rfds);
            FD_ZERO(&_wfds);
            FD_ZERO(&_efds);
        
            FD_ZERO(&_rfdsR);
            FD_ZERO(&_wfdsR);
            FD_ZERO(&_efdsR);
        
            FD_SET(_wakePipe[0], &_rfds);
        }

        ~Selector()
        {         
            std::set<Selectable*>::iterator it;

            while( _selectables.size() )
            {
                it = _selectables.begin();
                (*it)->detach();
            }

            if( _wakePipe[0] != -1 && _wakePipe[1] != -1 )
            {
                ::close(_wakePipe[0]);
                ::close(_wakePipe[1]);
            }
        }

        void cancel(IOHandle& h)
        {
           std::set<Selectable*>::iterator it = _devices.find( h.sel );
           if( it == _devices.end() )
                return;

           assert(h.fd > 0);

            if(h.wflags & Input)
                FD_CLR(h.fd, &_rfds);

            if(h.wflags & Output)
                FD_CLR(h.fd, &_wfds);

            FD_CLR(h.fd, &_efds);
            h.wflags = 0;
            h.flags = 0;

            remove(&h);

            if( _current == _devices.end() )
            {
                _devices.erase(it);
            }
            else if(*_current == *it)
            {
                _devices.erase(_current++);
            }
            else
            {
                _devices.erase(it);
            }
        }

        void beginRead(IOHandle* h)
        {
            if( (h->flags & Enabled) != Enabled )
            {
                h->flags |= Enabled;
                h->wflags |= Enabled;
                FD_SET(h->fd, &_efds);
                _devices.insert( h->sel );
            }

            h->flags |= Input;
            setChanged(h);
        }

        void endRead(IOHandle* h)
        {
            if(h->flags & Input)
            {
                h->flags &= ~Input;
                setChanged(h);
            }
        }

        void beginWrite(IOHandle* h)
        {
            if( (h->flags & Enabled) != Enabled )
            {
                h->flags |= Enabled;
                h->wflags |= Enabled;
                FD_SET(h->fd, &_efds);
                _devices.insert( h->sel );
            }

            h->flags |= Output;
            setChanged(h);
        }

        void endWrite(IOHandle* h)
        {
            if(h->flags & Output)
            {
                h->flags &= ~Output;
                setChanged(h);
            }
        }

        bool isReadable(IOHandle* h)
        {
            return FD_ISSET(h->fd, &_rfdsR);
        }

        bool isWritable(IOHandle* h)
        {
            return FD_ISSET(h->fd, &_wfdsR);
        }

        bool isError(IOHandle* h)
        {
            return FD_ISSET(h->fd, &_efdsR);
        }

        void wake()
        {
            ::write( _wakePipe[1], "W", 1);
            ::fsync( _wakePipe[1] );
        }

    protected:
        void remove(IOHandle* h)
        {
            for( std::vector<IOHandle*>::iterator it = _dirty.begin(); it != _dirty.end();)
            {
                if(*it == h)
                    it = _dirty.erase(it);
                else
                    ++it;
            }
        }

        void setChanged(IOHandle* h)
        { _dirty.push_back(h); }

    public:
        bool waitForWake(size_t msecs)
        {
            bool isWake = false;
        
            for( std::vector<IOHandle*>::iterator it = _dirty.begin(); it != _dirty.end(); ++it)
            {
                IOHandle* h = *it;
        
                if(h->flags == h->wflags)
                    continue;
        
                if(h->flags & Input &&  0 == (h->wflags & Input))
                {
                    FD_SET(h->fd, &_rfds);
                    h->wflags |= Input;
                }
        
                if(0 == (h->flags & Input) && h->wflags & Input)
                {
                    FD_CLR( h->fd, &_rfds );
                    h->wflags &= ~Input;
                }
        
                if(h->flags & Output &&  0 == (h->wflags & Output))
                {
                    FD_SET(h->fd, &_wfds);
                    h->wflags |= Output;
                }
        
                if(0 == (h->flags & Output) &&  h->wflags & Output)
                {
                    FD_CLR( h->fd, &_wfds );
                    h->wflags &= ~Output;
                }
            }
        
            _dirty.clear();
        
            FD_ZERO(&_rfdsR);
            FD_ZERO(&_wfdsR);
            FD_ZERO(&_efdsR);
        
            _rfdsR = _rfds;
            _wfdsR = _wfds;
            _efdsR = _efds;
        
            int avail = -1;
        
            while( true )
            {
                struct timeval* timeout = 0;
                struct timeval tv;
                if(msecs != EventLoop::WaitInfinite)
                {
                    tv.tv_sec = msecs / 1000;
                    tv.tv_usec = (msecs % 1000) * 1000;
                    timeout = &tv;
                }
        
                _clock.start();
                avail = ::select(FD_SETSIZE, &_rfdsR, &_wfdsR, &_efdsR, timeout);
                Pt::int64_t elapsed = _clock.stop().totalMSecs();
        
                if( avail < 0 && errno != EINTR )
                {
                    throw IOError( PT_ERROR_MSG("select failed") );
                }
        
                if( avail > 0 || msecs == 0 )
                    break;
        
                if(msecs == EventLoop::WaitInfinite)
                    continue;
        
                if(static_cast<Pt::uint64_t>(elapsed) >= msecs)
                    return isWake; // timeout
        
                msecs -= int(elapsed);
            }
        
            if( FD_ISSET(_wakePipe[0], &_efdsR) )
            {
                throw IOError( PT_ERROR_MSG("pipe failed") );
            }
        
            if( FD_ISSET(_wakePipe[0], &_rfdsR) )
            {
                --avail;
        
                static char buffer[1024];
                while(true)
                {
                    int ret = ::read(_wakePipe[0], buffer, sizeof(buffer));
                    if(ret > 0)
                    {
                        isWake = true;
                        continue;
                    }
        
                    if (ret == -1)
                    {
                        if(errno == EINTR)
                            continue;
        
                        if(errno == EAGAIN)
                            break;
                    }
        
                    throw IOError( PT_ERROR_MSG("pipe read failed") );
                }
            }
        
            try
            {
                for( _current = _devices.begin(); _current != _devices.end(); )
                {
                    Selectable* selectable = *_current;
        
                    bool isAvail = selectable->run();
        
                    if( isAvail )
                        --avail;
        
                    if(avail <= 0)
                        break;
        
                    if(_current != _devices.end())
                    {
                        if(*_current == selectable)
                        {
                            ++_current;
                        }
                    }
                }
            }
            catch (...)
            {
                _current = _devices.end();
                throw;
            }
        
            return isWake;
        }

        void attach(Selectable& s)
        {
            _selectables.insert(&s);
        }
        
        void detach(Selectable& s)
        {
            _selectables.erase(&s);
        }

    private:
        int _wakePipe[2];
        std::vector<IOHandle*> _dirty;
        std::set<Selectable*> _selectables; // inactive
        std::set<Selectable*>::iterator _current;
        std::set<Selectable*> _devices; // active
        fd_set _rfds;
        fd_set _wfds;
        fd_set _efds;
        fd_set _rfdsR;
        fd_set _wfdsR;
        fd_set _efdsR;
        Clock _clock;
};


class EventLoopImpl
{
    enum IOFlags
    {
        Input = 1,
        Output = 2,
        Error = 4,
        Enabled = 8
    };

    public:
        EventLoopImpl(Signal<const Event&>& eventSignal);

        ~EventLoopImpl();

        void cancel(IOHandle& h)
        {
            _selector.cancel(h);
           /*std::set<Selectable*>::iterator it = _devices.find( h.sel );
           if( it == _devices.end() )
                return;

           assert(h.fd > 0);

            if(h.wflags & Input)
                FD_CLR(h.fd, &_rfds);

            if(h.wflags & Output)
                FD_CLR(h.fd, &_wfds);

            FD_CLR(h.fd, &_efds);
            h.wflags = 0;
            h.flags = 0;

            remove(&h);

            if( _current == _devices.end() )
            {
                _devices.erase(it);
            }
            else if(*_current == *it)
            {
                _devices.erase(_current++);
            }
            else
            {
                _devices.erase(it);
            }*/
        }

        void beginRead(IOHandle* h)
        {
            _selector.beginRead(h);
            /*if( (h->flags & Enabled) != Enabled )
            {
                h->flags |= Enabled;
                h->wflags |= Enabled;
                FD_SET(h->fd, &_efds);
                _devices.insert( h->sel );
            }

            h->flags |= Input;
            setChanged(h);*/
        }

        void endRead(IOHandle* h)
        {
            _selector.endRead(h);
            /*if(h->flags & Input)
            {
                h->flags &= ~Input;
                setChanged(h);
            }*/
        }

        void beginWrite(IOHandle* h)
        {
            _selector.beginWrite(h);
            /*if( (h->flags & Enabled) != Enabled )
            {
                h->flags |= Enabled;
                h->wflags |= Enabled;
                FD_SET(h->fd, &_efds);
                _devices.insert( h->sel );
            }

            h->flags |= Output;
            setChanged(h);*/
        }

        void endWrite(IOHandle* h)
        {
            _selector.endWrite(h);
            /*if(h->flags & Output)
            {
                h->flags &= ~Output;
                setChanged(h);
            }*/
        }

        bool isReadable(IOHandle* h)
        {
            return _selector.isReadable(h);
            //return FD_ISSET(h->fd, &_rfdsR);
        }

        bool isWritable(IOHandle* h)
        {
            return _selector.isWritable(h);
            //return FD_ISSET(h->fd, &_wfdsR);
        }

        bool isError(IOHandle* h)
        {
            return _selector.isError(h);
            //return FD_ISSET(h->fd, &_efdsR);
        }

        /*void remove(IOHandle* h)
        {
            for( std::vector<IOHandle*>::iterator it = _dirty.begin(); it != _dirty.end();)
            {
                if(*it == h)
                    it = _dirty.erase(it);
                else
                    ++it;
            }
        }*/

        /*void setChanged(IOHandle* h)
        { _dirty.push_back(h); }*/

        void idle(Selectable& s);

        void avail(Selectable& s);

        void attach(Selectable& s);

        void detach(Selectable& s);

        void run();

        void exit();

        void wake();

        void commitEvent(const Event& event);

        void queueEvent(const Event& event);

        bool processEvents();

        void attach(Timer& timer)
        { _timerQueue.addTimer(timer); }

        void detach( Timer& timer )
        { _timerQueue.removeTimer(timer); }

    protected:
        bool waitNext();

        //bool waitForWake(size_t msecs);

    private:
        Mutex _mutex;
        TimerQueue _timerQueue;
        EventQueue _eventQueue;
        Signal<const Event&>* _event;
        Selector _selector;
        /*int _wakePipe[2];
        std::vector<IOHandle*> _dirty;
        std::set<Selectable*> _selectables;
        std::set<Selectable*>::iterator _current;
        std::set<Selectable*> _devices; // active*/
        std::vector<Selectable*> _avail;
        /*fd_set _rfds;
        fd_set _wfds;
        fd_set _efds;
        fd_set _rfdsR;
        fd_set _wfdsR;
        fd_set _efdsR;
        Clock _clock;*/
};

class MainLoopImpl : public EventLoopImpl
{
    public:
        MainLoopImpl(Signal<const Event&>& eventSignal);

        MainLoopImpl(Signal<const Event&>& eventSignal, Allocator& a);

        ~MainLoopImpl();
};

} //namespace System

} //namespace Pt

#endif
