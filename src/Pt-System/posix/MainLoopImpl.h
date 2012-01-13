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


class WakePipe
{
    public:
        WakePipe()
        {
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
        }

        ~WakePipe()
        {
            if( _wakePipe[0] != -1 && _wakePipe[1] != -1 )
            {
                ::close(_wakePipe[0]);
                ::close(_wakePipe[1]);
            }
        }

        void wake()
        {
            ::write( _wakePipe[1], "W", 1);
            ::fsync( _wakePipe[1] );
        }

        bool isReady()
        {
            bool isWake = false;
            while(true)
            {
                int ret = ::read(_wakePipe[0], _buffer, sizeof(_buffer));
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

            return isWake;
        }

        int readFd()
        { return _wakePipe[0]; }

    private:
        int _wakePipe[2];
        char _buffer[1024];
};


class IOHandleQueue
{
    public:
        typedef std::vector<IOHandle*>::iterator Iterator;

        IOHandleQueue()
        {
        }

        ~IOHandleQueue()
        {   
        }

        Iterator begin()
        { return _dirty.begin(); }

        Iterator end()
        { return _dirty.end(); }

        bool enable(IOHandle* h)
        {
            if( (h->flags & IOHandle::Enabled) != IOHandle::Enabled )
            {
                h->flags |= IOHandle::Enabled;
                h->wflags |= IOHandle::Enabled;
                return true;
            }

            return false;
        }

        void beginRead(IOHandle* h)
        {
            h->flags |= IOHandle::Input;
            setChanged(h);
        }

        void endRead(IOHandle* h)
        {
            if(h->flags & IOHandle::Input)
            {
                h->flags &= ~IOHandle::Input;
                setChanged(h);
            }
        }

        void beginWrite(IOHandle* h)
        {
            h->flags |= IOHandle::Output;
            setChanged(h);
        }

        void endWrite(IOHandle* h)
        {
            if(h->flags & IOHandle::Output)
            {
                h->flags &= ~IOHandle::Output;
                setChanged(h);
            }
        }

        void clear()
        {
            _dirty.clear();
        }

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

    protected:
        void setChanged(IOHandle* h)
        { _dirty.push_back(h); }

    private:
        std::vector<IOHandle*> _dirty;
};

#ifdef PT_WITH_POSIX_POLL

class Selector
{
    public:
        Selector()
        {
            _current = _devices.end();
        
            pollfd pfd;
            pfd.fd = _wakePipe.readFd();
            pfd.events = POLLIN;
            pfd.revents = 0;
            _pollfds.push_back(pfd);

            _iohandles.push_back(0);
        }

        ~Selector()
        {         
            std::set<Selectable*>::iterator it;

            while( _selectables.size() )
            {
                it = _selectables.begin();
                (*it)->detach();
            }
        }

        void cancel(IOHandle& h)
        {
            std::set<Selectable*>::iterator it = _devices.find( h.sel );
            if( it == _devices.end() )
                return;

            assert(h.fd > 0);
            assert(h.pollfdsOffset != 0);

            size_t offset = h.pollfdsOffset;
            _pollfds.at(offset) = _pollfds.back();
            _pollfds.resize(_pollfds.size() - 1);

            _iohandles.at(offset) = _iohandles.back();
            _iohandles.resize(_iohandles.size() - 1);
            _iohandles[offset]->pollfdsOffset = offset;

            _dirty.remove(&h);

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
            if(_dirty.enable(h) )
            {
                size_t offset = _pollfds.size();
                assert(offset == _iohandles.size());

                h->pollfdsOffset = offset;
                _iohandles.push_back(h);

                pollfd pfd;
                pfd.fd = h->fd;
                pfd.events = 0;
                pfd.revents = 0;
                _pollfds.push_back(pfd);

                _devices.insert( h->sel );
            }

            _dirty.beginRead(h);
        }

        void endRead(IOHandle* h)
        {
            _dirty.endRead(h);
        }

        void beginWrite(IOHandle* h)
        {
            if(_dirty.enable(h) )
            {
                size_t offset = _pollfds.size();
                assert(offset == _iohandles.size());

                h->pollfdsOffset = offset;
                _iohandles.push_back(h);

                pollfd pfd;
                pfd.fd = h->fd;
                pfd.events = 0;
                pfd.revents = 0;
                _pollfds.push_back(pfd);

                _devices.insert( h->sel );
            }

            _dirty.beginWrite(h);
        }

        void endWrite(IOHandle* h)
        {
            _dirty.endWrite(h);
        }

        bool isReadable(IOHandle* h)
        {
            return _pollfds[h->pollfdsOffset].revents & (POLLIN|POLLHUP);
        }

        bool isWritable(IOHandle* h)
        {
            return _pollfds[h->pollfdsOffset].revents & (POLLOUT|POLLHUP);
        }

        bool isError(IOHandle* h)
        {
            return _pollfds[h->pollfdsOffset].revents & (POLLERR|POLLNVAL);
        }

        void wake()
        {
            _wakePipe.wake();
        }

    public:
        bool waitForWake(size_t umsecs)
        {
            int msecs = umsecs;
            if(umsecs == EventLoop::WaitInfinite)
            {
                msecs = -1;
            }
            if (umsecs > static_cast<std::size_t>(std::numeric_limits<int>::max()))
            {
                msecs = std::numeric_limits<int>::max();
            }

            bool isWake = false;
        
            for( std::vector<IOHandle*>::iterator it = _dirty.begin(); it != _dirty.end(); ++it)
            {
                IOHandle* h = *it;
        
                if(h->flags == h->wflags)
                    continue;
        
                if(h->flags & IOHandle::Input &&  0 == (h->wflags & IOHandle::Input))
                {
                    _pollfds[h->pollfdsOffset].events |= POLLIN;
                    h->wflags |= IOHandle::Input;
                }
        
                if(0 == (h->flags & IOHandle::Input) && h->wflags & IOHandle::Input)
                {
                    _pollfds[h->pollfdsOffset].events &= ~POLLIN;
                    h->wflags &= ~IOHandle::Input;
                }
        
                if(h->flags & IOHandle::Output &&  0 == (h->wflags & IOHandle::Output))
                {
                    _pollfds[h->pollfdsOffset].events |= POLLOUT;
                    h->wflags |= IOHandle::Output;
                }
        
                if(0 == (h->flags & IOHandle::Output) &&  h->wflags & IOHandle::Output)
                {
                    _pollfds[h->pollfdsOffset].events &= ~POLLOUT;
                    h->wflags &= ~IOHandle::Output;
                }
            }
        
            _dirty.clear();
               
            int avail = -1;

            while( true )
            {            
                _clock.start();
                avail = ::poll(&_pollfds[0], _pollfds.size(), msecs);
                Pt::int64_t elapsed = _clock.stop().totalMSecs();

                if( avail < 0 && errno != EINTR )
                {
                    throw IOError( PT_ERROR_MSG("select failed") );
                }
        
                if( avail > 0 || msecs == 0 )
                    break;
        
                if(msecs < 0) // negative poll time means infinite
                    continue;
        
                if(elapsed >= msecs)
                {
                    return isWake; // timeout
                }
        
                msecs -= int(elapsed);
            }

            if( _pollfds[0].revents & POLLIN )
            {
                --avail;
                isWake = _wakePipe.isReady();
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
        WakePipe _wakePipe;
        std::vector<pollfd> _pollfds;
        std::vector<IOHandle*> _iohandles;
        IOHandleQueue _dirty;
        std::set<Selectable*> _selectables; // inactive
        std::set<Selectable*>::iterator _current;
        std::set<Selectable*> _devices; // active
        Clock _clock;
};

#else

class Selector
{
    public:
        Selector()
        {
            _current = _devices.end();

            FD_ZERO(&_rfds);
            FD_ZERO(&_wfds);
            FD_ZERO(&_efds);
        
            FD_ZERO(&_rfdsR);
            FD_ZERO(&_wfdsR);
            FD_ZERO(&_efdsR);
        
            FD_SET(_wakePipe.readFd(), &_rfds);
        }

        ~Selector()
        {         
            std::set<Selectable*>::iterator it;

            while( _selectables.size() )
            {
                it = _selectables.begin();
                (*it)->detach();
            }
        }

        void cancel(IOHandle& h)
        {
           std::set<Selectable*>::iterator it = _devices.find( h.sel );
           if( it == _devices.end() )
                return;

           assert(h.fd > 0);

            if(h.wflags & IOHandle::Input)
                FD_CLR(h.fd, &_rfds);

            if(h.wflags & IOHandle::Output)
                FD_CLR(h.fd, &_wfds);

            FD_CLR(h.fd, &_efds);
            h.wflags = 0;
            h.flags = 0;

            _dirty.remove(&h);

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
            if(_dirty.enable(h) )
            {
                FD_SET(h->fd, &_efds);
                _devices.insert( h->sel );
            }

            _dirty.beginRead(h);
        }

        void endRead(IOHandle* h)
        {
            _dirty.endRead(h);
        }

        void beginWrite(IOHandle* h)
        {
            if(_dirty.enable(h) )
            {
                FD_SET(h->fd, &_efds);
                _devices.insert( h->sel );
            }

            _dirty.beginWrite(h);
        }

        void endWrite(IOHandle* h)
        {
            _dirty.endWrite(h);
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
            _wakePipe.wake();
        }

    public:
        bool waitForWake(size_t msecs)
        {
            bool isWake = false;
        
            for( std::vector<IOHandle*>::iterator it = _dirty.begin(); it != _dirty.end(); ++it)
            {
                IOHandle* h = *it;
        
                if(h->flags == h->wflags)
                    continue;
        
                if(h->flags & IOHandle::Input &&  0 == (h->wflags & IOHandle::Input))
                {
                    FD_SET(h->fd, &_rfds);
                    h->wflags |= IOHandle::Input;
                }
        
                if(0 == (h->flags & IOHandle::Input) && h->wflags & IOHandle::Input)
                {
                    FD_CLR( h->fd, &_rfds );
                    h->wflags &= ~IOHandle::Input;
                }
        
                if(h->flags & IOHandle::Output &&  0 == (h->wflags & IOHandle::Output))
                {
                    FD_SET(h->fd, &_wfds);
                    h->wflags |= IOHandle::Output;
                }
        
                if(0 == (h->flags & IOHandle::Output) &&  h->wflags & IOHandle::Output)
                {
                    FD_CLR( h->fd, &_wfds );
                    h->wflags &= ~IOHandle::Output;
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
        
            if( FD_ISSET(_wakePipe.readFd(), &_rfdsR) )
            {
                --avail;
                isWake = _wakePipe.isReady();
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
        WakePipe _wakePipe;
        //std::vector<IOHandle*> _dirty;
        IOHandleQueue _dirty;
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

#endif

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
