/*
 * Copyright (C) 2006-20012 Marc Boris Duerner
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

#ifndef PT_SYSTEM_POSIX_EVENTLOOPIMPL_H
#define PT_SYSTEM_POSIX_EVENTLOOPIMPL_H

#include "Pt/System/Api.h"
#include "Pt/System/IOError.h"
#include "Pt/System/SystemError.h"

#include <iostream>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

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

} //namespace System

} //namespace Pt

#ifdef __APPLE__
    #define PT_WITH_BSD_KQUEUE
#endif

#ifdef __linux__
    #define PT_WITH_LINUX_EPOLL
#endif

#if defined (PT_WITH_LINUX_EPOLL)
    #include "EventLoopImpl_epoll.h"
#elif defined(PT_WITH_BSD_KQUEUE)
    #include "EventLoopImpl_kqueue.h"
#elif defined(PT_WITH_POSIX_POLL)
    #include "EventLoopImpl_poll.h"
#else
    #include "EventLoopImpl_select.h"
#endif

namespace Pt {

namespace System {

class EventLoopImpl
{
    public:
        EventLoopImpl();

        virtual ~EventLoopImpl();

        virtual void idle(Selectable& s) = 0;

        virtual void avail(Selectable& s) = 0;

        virtual void cancel(IOHandle& h) = 0;

        virtual void beginRead(IOHandle* h) = 0;

        virtual void endRead(IOHandle* h) = 0;

        virtual void beginWrite(IOHandle* h) = 0;

        virtual void endWrite(IOHandle* h) = 0;

        virtual bool isReadable(IOHandle* h) = 0;

        virtual bool isWritable(IOHandle* h) = 0;

        virtual bool isError(IOHandle* h) = 0;
};

} //namespace System

} //namespace Pt

#endif

