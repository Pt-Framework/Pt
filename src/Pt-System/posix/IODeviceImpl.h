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
#ifndef PT_SYSTEM_IODEVICEIMPL_H
#define PT_SYSTEM_IODEVICEIMPL_H

#include "SelectableImpl.h"
#include "Pt/System/IODevice.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

namespace Pt {

namespace System {

    struct DestructionSentry
    {
        DestructionSentry(DestructionSentry*& sentry)
        : _deleted(false)
        , _sentry(sentry)
        {
           sentry = this;
        }

        ~DestructionSentry()
        {
            if( ! _deleted )
                this->detach();
        }

        bool operator!() const
        { return _deleted; }

        void detach()
        {
            _sentry = 0;
            _deleted = true;
        }

        bool _deleted;
        DestructionSentry*& _sentry;
    };

    class IODeviceImpl : public SelectableImpl
    {
        public:
            IODeviceImpl(IODevice& device);

            virtual ~IODeviceImpl();

            int fd() const
            { return _fd; }

            void setTimeout(std::size_t msecs)
            { _timeout = msecs; }

            std::size_t timeout() const
            { return _timeout; }

            void open(int fd, bool isAsync, bool closeOnExec);

            virtual void close();

            virtual size_t beginRead(char* buffer, size_t n, bool& eof);

            virtual size_t endRead(bool& eof);

            virtual size_t read( char* buffer, size_t count, bool& eof );

            virtual size_t beginWrite(const char* buffer, size_t n);

            virtual size_t endWrite();

            virtual size_t write( const char* buffer, size_t count );

            void sigwrite( int signo );

            virtual void cancel();

            virtual void sync() const;

            virtual void attach(EventLoop& s);

            virtual void detach(EventLoop& s);

            virtual bool wait(std::size_t msecs);

            virtual bool wait(std::size_t msecs, fd_set* rfds, fd_set* wfds, fd_set* efds);

            virtual void initWait(fd_set& rfds, fd_set& wfds, fd_set& efds);

            virtual int initSelect(fd_set& rfds, fd_set& wfds, fd_set& efds);

            virtual void exitSelect();

            virtual int checkEvent(fd_set& rfds, fd_set& wfds, fd_set& efds);

        protected:
            IODevice& _device;
            int _fd;
            std::size_t _timeout;
            fd_set* _rfds;
            fd_set* _wfds;
            fd_set* _efds;
            DestructionSentry* _sentry;
            bool _errorPending;
    };

} //namespace System

} //namespace Pt

#endif
