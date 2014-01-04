/***************************************************************************
 *   Copyright (C) 2007 Marc Boris D�rner                                  *
 *   Copyright (C) 2007 Laurentiu-Gheorghe Crisan                          *
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

namespace Pt{

namespace System{

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
        
        bool setSignal(SerialDevice::Signal signal);
        
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
        HANDLE _waitHandle;
        OVERLAPPED _readOv;
        OVERLAPPED _writeOv;
        DCB _orgCommState;
#endif
};

}//namespace System

}//namespaec Pt

#endif
