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

        virtual void close(EventLoop* loop);

    private:
        HANDLE _handle;
};


class OverlappedIODeviceImpl : public IODeviceImpl
{
    public:
        OverlappedIODeviceImpl(IODevice& dev);

        ~OverlappedIODeviceImpl();

        void attach(EventLoop& loop);

        void detach(EventLoop& loop);

        bool runRead(EventLoop&);

        bool runWrite(EventLoop&);

        virtual size_t beginRead(EventLoop& loop, char* buffer, size_t n, bool& eof);

        virtual size_t endRead(EventLoop& loop, bool& eof);

        virtual size_t beginWrite(EventLoop& loop, const char* buffer, size_t n);

        virtual size_t endWrite(EventLoop& loop);

        virtual void close(EventLoop* loop);

        virtual size_t read(char* buffer, size_t count, bool& eof);

        virtual size_t write(const char* buffer, size_t count);

        virtual void sync() const;

        virtual void cancel(EventLoop& loop) ;

     protected:
        IODevice& _device;
        OVERLAPPED _readOv;
        OVERLAPPED _writeOv;
};

} //namespace System

} //namespace Pt

#endif
