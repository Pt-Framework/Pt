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

#include "Selector.h"
#include "Pt/System/IODevice.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

namespace Pt {

namespace System {

    class IOHandle;

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

    class IODeviceImpl
    {
        public:
            IODeviceImpl(IODevice& device);

            virtual ~IODeviceImpl();

            int fd() const
            { return _ioh.fd; }

            void setTimeout(std::size_t msecs)
            { _timeout = msecs; }

            std::size_t timeout() const
            { return _timeout; }

            void open(int fd, bool closeOnExec);

            bool isOpen() const;

            virtual void cancel(EventLoop& loop);

            virtual void close();

            virtual size_t beginRead(EventLoop& loop, char* buffer, size_t n, bool& eof);

            virtual size_t endRead(EventLoop& loop, char* buffer, size_t n, bool& eof);

            virtual size_t read( char* buffer, size_t count, bool& eof );

            virtual size_t beginWrite(EventLoop& loop, const char* buffer, size_t n);

            virtual size_t endWrite(EventLoop& loop, const char* buffer, size_t n);

            virtual size_t write( const char* buffer, size_t count );
            
            virtual void sync() const;

            bool runRead(EventLoop& loop);

            bool runWrite(EventLoop& loop);

            static bool wait(std::size_t msecs, fd_set* rfds, fd_set* wfds, fd_set* efds);

        protected:
            IOHandle _ioh;
            std::size_t _timeout;
            DestructionSentry* _sentry;
            bool _errorPending;
    };

} //namespace System

} //namespace Pt

#endif
