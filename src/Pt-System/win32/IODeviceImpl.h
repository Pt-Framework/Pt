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
#include "Pt/WinVer.h"
#include "Pt/System/Api.h"
#include "Pt/System/IOError.h"
#include <windows.h>

namespace Pt {

namespace System {

class IODevice;

class IODeviceImpl
{
    public:
        IODeviceImpl();

        virtual ~IODeviceImpl();

        void setHandle(HANDLE h);

        HANDLE deviceHandle() const
        { return _handle; }

        HANDLE handle() const
        { return _handle; }

        //virtual void setWaitHandle(HANDLE h) {}

        virtual void close();

        void sync() const
        {
            if( FALSE == ::FlushFileBuffers( handle() ) )
                throw IOError( PT_ERROR_MSG("Could not flush file buffer") );
        }

    private:
        HANDLE _handle;
};

#ifndef _WIN32_WCE

class OverlappedIODeviceImpl : public IODeviceImpl
{
    public:
        OverlappedIODeviceImpl(IODevice& dev);

        OverlappedIODeviceImpl();

        ~OverlappedIODeviceImpl();

        void init(IODevice& dev);

        void setTimeout(std::size_t msecs)
        {
            DWORD maxTimeout = std::numeric_limits<DWORD>::max() - 1;
            
            _timeout = (msecs == EventLoop::WaitInfinite) ? INFINITE
                           : (msecs > maxTimeout) ? maxTimeout 
                               : static_cast<DWORD>(msecs);
        }

        std::size_t timeout() const
        { return _timeout; }

        bool runRead(EventLoop&);

        bool runWrite(EventLoop&);

        virtual size_t beginRead(EventLoop& loop, char* buffer, size_t n, bool& eof);

        virtual size_t endRead(EventLoop& loop, char* buffer, size_t n, bool& eof);

        virtual size_t beginWrite(EventLoop& loop, const char* buffer, size_t n);

        virtual size_t endWrite(EventLoop& loop, const char* buffer, size_t n);

        virtual void close();

        virtual size_t read(char* buffer, size_t count, bool& eof);

        virtual size_t write(const char* buffer, size_t count);

        virtual void cancel(EventLoop& loop) ;

     protected:
        IOHandle _ioh;
        OVERLAPPED _readOv;
        OVERLAPPED _writeOv;
        HANDLE _ioEvent;
        DWORD _timeout;
};

#endif

} //namespace System

} //namespace Pt

#endif
