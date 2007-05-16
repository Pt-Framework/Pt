/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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
#ifndef PT_SYSTEM_IORESULTIMPL_H
#define PT_SYSTEM_IORESULTIMPL_H

#include <Pt/System/IOError.h>
#include <Pt/System/IOResult.h>
#include <ios>

#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>


namespace Pt {

namespace System {

    class IOResultImpl : public IOResult
    {
        public:
            virtual IOResultImpl* impl()
            { return this; }

            void setFd(int fd)
            { _fd = fd; }

            int fd() const
            { return _fd; }

            void attach(char* buffer, size_t size)
            {
                _buffer = buffer;
                _bufferSize = size;
            }

            char* buffer() const
            { return _buffer; }

            size_t bufferSize() const
            { return _bufferSize; }

        protected:
            IOResultImpl()
            : IOResult()
            , _fd(-1)
            {}

            bool select(int maxfd, fd_set rfds, fd_set wfds, unsigned int msecs)
            {
                timeval* timeout = 0;
                struct   timeval tv;
                if(msecs != IOResult::WaitInfinite)
                {
                    tv.tv_sec = msecs / 1000;
                    tv.tv_usec = (msecs % 1000) * 1000;
                    timeout = &tv;
                }

                int ret = -1;
                while( true )
                {
                    ret = ::select( maxfd + 1, &rfds, &wfds, 0, timeout );
                    if( ret != -1 )
                        break;

                    if( errno != EINTR )
                        throw IOError( "select failed", PT_SOURCEINFO );
                }

                return ret > 0;
            }

        private:
            int _fd;
            char* _buffer;
            size_t _bufferSize;
    };

    class ReadResult : public IOResultImpl
    {
        public:
            ReadResult()
            {}

        protected:
            virtual bool _wait(unsigned int msecs)
            {
                fd_set wfds;
                FD_ZERO(&wfds);

                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(  this->fd(), &fds );

                return select( this->fd(), fds, wfds, msecs);
            }
    };

    class WriteResult : public IOResultImpl
    {
        public:
            WriteResult()
            {}

        protected:
            virtual bool _wait(unsigned int msecs)
            {
                fd_set rfds;
                FD_ZERO(&rfds);

                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(  this->fd(), &fds );

                return select( this->fd(), rfds, fds, msecs);
            }
    };

}//namespace System

}//namespace Pt

#endif
