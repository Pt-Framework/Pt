/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2008 Peter Barth                                        *
 *   Copyright (C) 2006-2008 PTV AG                                        *
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

#include "Pt/System/IODevice.h"
#include "Pt/System/IOError.h"
#include "Pt/System/SerialDevice.h"
#include "IODeviceImpl.h"
#include <string>

// symbian APIs
#include "btextnotifiers.h"
#include "es_sock.h"

namespace Pt {

namespace System {

class SerialDeviceImpl : public IODeviceImpl
{
    public:
        SerialDeviceImpl( );

        ~SerialDeviceImpl();

        void open(const std::string& path, std::ios_base::openmode mode, bool isAsync);

        void open(int fd, bool isAsync);

        void close();

        IOResult& beginRead(char* buffer, size_t n, bool& eof);

        size_t endRead(IOResult& result, bool& eof);

        size_t read( char* buffer, size_t count, bool& eof );

        IOResult& beginWrite(const char* buffer, size_t n);

        size_t endWrite(IOResult& result);

        size_t write( const char* buffer, size_t count );

        void sync() const;        
        
        void setBaudRate( SerialDevice::BaudRate rate );

        SerialDevice::BaudRate baudRate() const;

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

        void flush();

    private:
        TBTDevAddr doBluetoothDeviceQuery();
        
        void openBluetoothSocket(const TBTDevAddr& devAddr, int portNum);
        
        // Listening socket
        RSocket _listenSock;      
        bool _socketConnected;
        RSocketServ _socketServ;
        // indicates connection to RSocketServ
        bool _servConnected;
        
        ReadResultSymbian _readResult;
        WriteResultSymbian _writeResult;

        SerialDevice::BaudRate _rate;
        int _charSize;
        SerialDevice::StopBits _bits;
        SerialDevice::Parity _parity;
        SerialDevice::FlowControl _flowControl;
        size_t _timeOut;
        
        // some variables to overcome Panic 14 in synchronous read
        HBufC8* _hBuf;
        TPtr8 _tempBuffer;     
};

} //namespace System

} //namespaec Pt

#endif
