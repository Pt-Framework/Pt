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
#include "Pt/System/IOError.h"
#include "Pt/System/SerialDevice.h"
#include <string>
#include <windows.h>

namespace Pt{

namespace System{

class SerialDeviceImpl : public OverlappedIODeviceImpl
{
    public:
        SerialDeviceImpl(SerialDevice& device);
        
        ~SerialDeviceImpl();

        void open( const std::string& file, IODevice::OpenMode mode, EventLoop* loop);

        void close(EventLoop* loop);

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
        
        void setTimeout( size_t msec );
        
        size_t timeout() const;    
        
        bool setSignal(SerialDevice::SerialLine signal);
        
    private:
        void writeCommState( DCB& commState );
        
        void readCommState( DCB& commState ) const;

    private:
        SerialDevice& _device;
        HANDLE _waitHandle;
        OVERLAPPED _readOv;
        OVERLAPPED _writeOv;
        DCB _orgCommState;
};

}//namespace System

}//namespaec Pt

#endif
