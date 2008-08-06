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
#include "SelectorImpl.h"
#include "IODeviceImpl.h"
#include "Pt/System/IOError.h"
#include "Pt/System/Application.h"
#include "Pt/System/Selector.h"
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <cassert>
#include <iostream>

namespace Pt {

namespace System {

const short SelectorImpl::POLL_ERROR_MASK= POLLERR | POLLHUP | POLLNVAL;

SelectorImpl::SelectorImpl()
: _isDirty(true), _app(0)
{
    _current = _devices.end();

    //Open a pipe to send wake up message.
    if( ::pipe( _wakePipe ) )
        throw std::runtime_error("Could not open pipe." + PT_SOURCEINFO);

    int flags = ::fcntl(_wakePipe[0], F_GETFL);
    if(-1 == flags)
        throw std::runtime_error("Could not get pipe flags." + PT_SOURCEINFO);

    int ret = ::fcntl(_wakePipe[0], F_SETFL, flags|O_NONBLOCK);
    if(-1 == ret)
        throw std::runtime_error("Could not set pipe to non-blocking." + PT_SOURCEINFO);

    flags = ::fcntl(_wakePipe[1], F_GETFL);
    if(-1 == flags)
        throw std::runtime_error("Could not get pipe flags." + PT_SOURCEINFO);

    ret = ::fcntl(_wakePipe[1], F_SETFL, flags|O_NONBLOCK);
    if(-1 == ret)
        throw std::runtime_error("Could not set pipe to non-blocking." + PT_SOURCEINFO);

}


SelectorImpl::~SelectorImpl()
{
    std::set<Selectable*>::iterator it;
    while( _devices.size() )
    {
        it = _devices.begin();
        (*it)->setSelector(0);
    }

    if( _wakePipe[0] != -1 && _wakePipe[1] != -1 )
    {
        ::close(_wakePipe[0]);
        ::close(_wakePipe[1]);
    }
}


void SelectorImpl::add(Selectable& dev)
{
    _devices.insert(&dev);
    _isDirty = true;
}


void SelectorImpl::remove(Selectable& dev)
{
   std::set<Selectable*>::iterator it = _devices.find( &dev );
   if( it == _devices.end() )
        return;

    if (_current == _devices.end())
    {
        _devices.erase(it);
    }
    else if (*_current == *it)
    {
        _devices.erase(_current++);
    }
    else
    {
        _devices.erase(it);
    }

    _isDirty = true;
}


bool SelectorImpl::wait(unsigned int umsecs)
{
    _clock.start();

    int msecs = umsecs;

    if (umsecs != SelectorBase::WaitInfinite &&
        umsecs > std::numeric_limits<int>::max())
    {
        msecs= std::numeric_limits<int>::max();
    }

    if (_isDirty)
    {
        _pollfds.clear();

        // Groesse neu berechnen
        size_t pollSize= 1;

        if (_app != 0)
        {
            ++pollSize;
        }

        std::set<Selectable*>::iterator iter;
        for( iter= _devices.begin(); iter != _devices.end(); ++iter)
        {
            if( (*iter)->enabled() )
                pollSize+= (*iter)->simpl().pollSize();
        }

        pollfd pfd;
        pfd.fd = -1;
        pfd.events = 0;
        pfd.revents = 0;

        _pollfds.assign(pollSize, pfd);

        // Eintraege einfuegen
        pollfd* pCurr= &_pollfds[0];

        // Event Pipe einfuegen TODO Pt::System::Pipe verwenden
        pCurr->fd = _wakePipe[0];
        pCurr->events = POLLIN;

        ++pCurr;

        if (_app != 0)
        {
            //pCurr->fd= _app->getSignalFd();
            //pCurr->events = POLLIN;
            //++pCurr;
        }

        for( iter= _devices.begin(); iter != _devices.end(); ++iter)
        {
            if( (*iter)->enabled() )
            {
                const size_t availableSpace= &_pollfds.back() - pCurr + 1;
                size_t required = (*iter)->simpl().pollSize();
                assert( required <= availableSpace);
                pCurr+= (*iter)->simpl().initializePoll( pCurr, required);
            }
        }

        _isDirty= false;
    }

    while( true )
    {
        if(msecs != SelectorBase::WaitInfinite)
        {
            int64_t diff = _clock.stop().totalMSecs();
            _clock.start();

            if (diff < msecs)
            {
                msecs -= int(diff);
            }
            else
            {
                msecs = 0;
            }
        }

        int ret = ::poll(&_pollfds[0], _pollfds.size(), msecs);
        if( ret != -1 )
            break;

        if( errno != EINTR )
            throw IOError( "Could not poll on file descriptors", PT_SOURCEINFO );

    }

    bool avail = false;
    try
    {
        if (_app != 0)
        {
            if (_pollfds[1].revents != 0)
            {

                if ( _pollfds[1].revents & POLL_ERROR_MASK)
                {
                    throw IOError("poll error on signal pipe", PT_SOURCEINFO);
                }


                int sigNo;
                ssize_t readSize= 0;

                while(true)
                {
                    int ret = ::read(_pollfds[1].fd, &sigNo+readSize, sizeof(sigNo)-readSize);
                    if(ret > 0)
                    {
                        avail = true;
                        readSize+= ret;
                        if ( readSize == sizeof(sigNo) )
                        {
                            _app->systemSignal.send(sigNo);
                            readSize= 0;

                        }
                        continue;
                    }

                    if (ret == -1)
                    {
                        if(errno == EINTR)
                            continue;

                        if(errno == EAGAIN)
                        {
                            if (readSize != 0)
                            {
                                throw IOError("Cound not read from signal pipe", PT_SOURCEINFO);
                            }
                            else
                            {
                                break;
                            }
                        }
                    }

                    throw IOError("Cound not read from pipe", PT_SOURCEINFO);
                }
            }
        }

        if (_pollfds[0].revents != 0)
        {

            if ( _pollfds[0].revents & POLL_ERROR_MASK)
            {
                throw IOError("poll error on event pipe", PT_SOURCEINFO);
            }

            static char buffer[1024];
            while(true)
            {
                int ret = ::read(_wakePipe[0], buffer, sizeof(buffer));
                if(ret > 0)
                {
                    avail = true;
                    continue;
                }

                if (ret == -1)
                {
                    if(errno == EINTR)
                        continue;

                    if(errno == EAGAIN)
                        break;
                }

                throw IOError("Cound not read from pipe", PT_SOURCEINFO);
            }
        }

        for( _current = _devices.begin(); _current != _devices.end(); )
        {
            Selectable* dev = *_current;

            if ( dev->enabled() && dev->simpl().checkPollEvent() )
            {
                avail = true;
            }

            if (_current != _devices.end())
            {
                if (*_current == dev)
                {
                    ++_current;
                }
            }
        }
    }
    catch (...)
    {
        _current= _devices.end();
        throw;
    }

    return avail;
}


void SelectorImpl::wake()
{
    ::write( _wakePipe[1], "W", 1);
    ::fsync( _wakePipe[1] );
}

} //namespace System

} //namespace Pt
