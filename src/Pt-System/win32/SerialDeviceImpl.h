/*
 * Copyright (C) 2007-2016 Marc Boris Duerner
 * Copyright (C) 2007-2016 Laurentiu-Gheorghe Crisan
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

#ifndef PT_SYSTEM_SERIALDEVICEIMPL_H
#define PT_SYSTEM_SERIALDEVICEIMPL_H

#include "IODeviceImpl.h"
#include "Pt/WinVer.h"
#include "Pt/System/Api.h"
#include "Pt/System/IODevice.h"
#include "Pt/System/IOError.h"
#include "Pt/System/SerialDevice.h"
#include "Pt/System/Thread.h"
#include <string>
#include <windows.h>

namespace Pt {

namespace System {

#ifdef _WIN32_WCE
class SerialDeviceImpl : public Pt::System::IODeviceImpl
#else
class SerialDeviceImpl : public OverlappedIODeviceImpl
#endif
{
    public:
        SerialDeviceImpl(SerialDevice& device);
        
        ~SerialDeviceImpl();

        void open( const std::string& file, std::ios::openmode mode);

        void close();

        void cancel(EventLoop& loop);

        virtual size_t beginRead(EventLoop& loop, char* buffer, size_t n, bool& eof);

        virtual size_t endRead(EventLoop& loop, char* buffer, size_t n, bool& eof);

        virtual bool runRead(EventLoop& loop);

        size_t read( char* buffer, size_t count, bool& eof );

        void setBaudRate( unsigned rate );
        
        unsigned baudRate() const;

        void setCharSize( int size );
        
        int charSize() const;

        void setStopBits( SerialDevice::StopBits bits );
        
        SerialDevice::StopBits stopBits() const;

        void setParity( SerialDevice::Parity parity );
        
        SerialDevice::Parity parity() const;

        void setFlowControl( SerialDevice::FlowControl flowControl );
        
        SerialDevice::FlowControl flowControl() const;
        
        void setRts(bool on);

        void setDtr(bool on);

        void setBreak(bool on);
        
        void sendBreak(int duration);

        bool isCts() const;

        bool isDsr() const;

        void clear();

        void sync() const;

#ifdef _WIN32_WCE
        bool runRead(EventLoop&);

        bool runWrite(EventLoop&);

        size_t beginRead(EventLoop& loop, char* buffer, size_t n, bool& eof);    

        size_t endRead(EventLoop& loop, char* buffer, size_t n, bool& eof);

        size_t read( char* buffer, size_t count, bool& eof );

        size_t beginWrite(EventLoop& loop, const char* buffer, size_t n);

        size_t endWrite(EventLoop& loop, const char* buffer, size_t n);

        size_t write( const char* buffer, size_t count );

        void setTimeout( size_t msec );
        
        size_t timeout() const;    
#endif

    private:
        void writeCommState( DCB& commState );
        
        void readCommState( DCB& commState ) const;

#ifdef _WIN32_WCE
        void run();       
#endif

#ifdef _WIN32_WCE
    private:
        SerialDevice& _device;
        HANDLE _beginWait;
        DCB _orgCommState;        
        AttachedThread* _thread;  
        bool _terminateThread;
        DWORD _event;

#else // normal WIN32
    private:
        SerialDevice& _device;
        DWORD _eventMask;
        DCB _orgCommState;
#endif
};

} //namespace System

} //namespaec Pt

#endif
