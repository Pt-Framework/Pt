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
#include "ReadResultSymbian.h"
#include "WriteResultSymbian.h"
#include <string>

// symbian APIs
#include <btextnotifiers.h>
#include <es_sock.h>

namespace Pt {

namespace System {

/** @brief Provide access to Symbian Bluetooth socket

    This class will simulate serial device communication using Bluetooth
    through the underlying Symbian Bluetooth socket API.
    The purpose of this class is to allow to read data from Bluetooth devices
    without the need to access any real COM port.
    
    Explanation:
    It seems that serial communication through Bluetooth virtual COM port
    on Symbian is not possible with the S60 SDK. The API (btdefcommport.h)
    which will let you map a Bluetooth device address to a virtual COM port 
    has been removed from the SDK in version 8 due to unknown reasons.
    
    Quote from the S60 SDK (3rd edition):
    "There are a number of architectural changes in the Bluetooth APIs 
    provided by Symbian OS in v8.0, affecting both source and binary 
    compatibility. The Bluetooth Registry API managing Bluetooth device 
    registry has been removed (and replaced by the RBTRegServ, 
    RBTRegistry, TBTRegistrySearch, and the CBTRegistryResponse 
    classes defined in btmanclient.h). 
    The btdefcommport.h managing port and service settings of the 
    legacy serial port applications has also been removed, and there 
    are changes in the Bluetooth Security Manager API, Bluetooth Device API, 
    and in Bluetooth sockets. 
    See S60 Platform: Source and Binary Compatibility for more details." 
    
*/
class SerialDeviceImpl : public IODeviceImpl
{
    public:
        SerialDeviceImpl( );

        ~SerialDeviceImpl();

        // Open port:
        // Currently only BTCOMM::n is supported, n is the socket port number
        // n = 1 will usually let you read data from a Bluetooth GPS receiver
        void open(const std::string& path, std::ios_base::openmode mode, bool isAsync);

        // unsupported
        void open(int fd, bool isAsync);

        // Close port:
        // Will close the socket connection and release socket server reference
        void close();

        // Works as expected
        IOResult& beginRead(char* buffer, size_t n, bool& eof);

        // Works as expected
        size_t endRead(IOResult& result, bool& eof);

        // Works as expected
        size_t read( char* buffer, size_t count, bool& eof );

        // Currently unsupported
        IOResult& beginWrite(const char* buffer, size_t n);

        // Currently unsupported
        size_t endWrite(IOResult& result);

        // Currently unsupported
        size_t write( const char* buffer, size_t count );

        // Currently unsupported
        void sync() const;        
        
        // Currently unsupported
        void setBaudRate( unsigned rate );

        // Currently unsupported
        unsigned baudRate() const;

        // Currently unsupported
        void setCharSize( int size );

        // Currently unsupported
        int charSize() const;

        // Currently unsupported
        void setStopBits( SerialDevice::StopBits bits );

        // Currently unsupported
        SerialDevice::StopBits stopBits() const;

        // Currently unsupported
        void setParity( SerialDevice::Parity parity );

        // Currently unsupported
        SerialDevice::Parity parity() const;

        // Currently unsupported
        void setFlowControl( SerialDevice::FlowControl flowControl );

        // Currently unsupported
        SerialDevice::FlowControl flowControl() const;

        // Works as expected
        void setTimeout( size_t msec );

        // Works as expected
        size_t timeout() const;

        // Currently unsupported
        void flush();

        bool setSignal(SerialDevice::SerialLine signal);

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
        char* _buff;
        TPtr8 _tempBuffer;     
        
        friend class ReadResultSymbian;
        friend class WriteResultSymbian;
};

} //namespace System

} //namespaec Pt

#endif
