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
#include <sys/select.h>

namespace Pt {

namespace System {
    struct IOHandle;

    class IODeviceImpl
    {
        public:
            IODeviceImpl(IODevice& device);

            virtual ~IODeviceImpl();

            int fd() const
            { return _ioh.fd; }

            int setFd(int fd)
            {
                int replaceFd = _ioh.fd;
                _ioh.fd = fd;
                return replaceFd;
            }

            void setTimeout(std::size_t msecs)
            { _timeout = msecs; }

            std::size_t timeout() const
            { return _timeout; }

            void open(int fd, bool closeOnExec);

            bool isOpen() const;

            void cancel(EventLoop& loop);

            void close();

            size_t beginRead(EventLoop& loop, char* buffer, size_t n, bool& eof);

            size_t endRead(EventLoop& loop, char* buffer, size_t n, bool& eof);

            size_t read(char* buffer, size_t count, bool& eof);

            size_t beginWrite(EventLoop& loop, const char* buffer, size_t n);

            size_t endWrite(EventLoop& loop, const char* buffer, size_t n);

            size_t write(const char* buffer, size_t count);

            void sync() const;

            bool runRead(EventLoop& loop);

            bool runWrite(EventLoop& loop);

            static bool wait(std::size_t msecs, fd_set* rfds, fd_set* wfds, fd_set* efds);

        protected:
            virtual void onCancel(EventLoop& loop);

            virtual void onClose();

            virtual std::size_t onBeginRead(EventLoop& loop, char* buffer,
                                            std::size_t count, bool& eof);

            virtual std::size_t onEndRead(EventLoop& loop, char* buffer,
                                          std::size_t count, bool& eof);

            virtual std::size_t onRead(char* buffer, std::size_t count, bool& eof);

            virtual std::size_t onBeginWrite(EventLoop& loop, const char* buffer,
                                             std::size_t count);

            virtual std::size_t onEndWrite(EventLoop& loop, const char* buffer,
                                           std::size_t count);

            virtual std::size_t onWrite(const char* buffer, std::size_t count);

            virtual void onSync() const;

            IOHandle& ioHandle()
            { return _ioh; }

        private:
            std::size_t readImpl(char* buffer, std::size_t count, bool& eof);

            std::size_t writeImpl(const char* buffer, std::size_t count);

            IOHandle    _ioh;
            std::size_t _timeout;
            bool        _readErrorPending;
            bool        _writeErrorPending;
    };

} //namespace System

} //namespace Pt

#endif
