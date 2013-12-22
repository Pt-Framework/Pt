/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
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
#ifndef PT_SYSTEM_FILEDEVICEIMPL_H
#define PT_SYSTEM_FILEDEVICEIMPL_H

#include "Pt/WinVer.h"
#include "Pt/System/Api.h"
#include "Pt/System/IOError.h"
#include "Pt/System/FileDevice.h"
#include "IODeviceImpl.h"
#include <windows.h>
#include <ios>

namespace Pt {

namespace System {

#ifdef _WIN32_WCE
class FileDeviceImpl  : public IODeviceImpl
#else
class FileDeviceImpl  : public OverlappedIODeviceImpl
#endif
{
    public:
        typedef FileDevice::pos_type pos_type;
        typedef FileDevice::off_type off_type;

    public:
        FileDeviceImpl(FileDevice& dev);

        ~FileDeviceImpl();

        void open( const char* path, std::ios::openmode mode);

        bool beginOpen(EventLoop& loop, const char* path, std::ios::openmode mode);

        bool runOpen(EventLoop& loop);

        void endOpen(EventLoop& loop);

        pos_type seek( off_type offset, std::ios::seekdir sd );

        size_t peek( char* buffer, size_t count );

#ifdef _WIN32_WCE
        void setTimeout(size_t timeout);

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

    private:
        IODevice& _device;
        OVERLAPPED _readOv;
        OVERLAPPED _writeOv;
#endif
};

}//namespace System

}//namespace Pt

#endif
