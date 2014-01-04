/*
 * Copyright (C) 2007 Marc Boris Drner
 * Copyright (C) 2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2008 Peter Barth
 * Copyright (C) 2006-2008 PTV AG
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
#include "SerialDeviceImpl.h"

#include <iostream>
#include <sstream>

// symbian APIs
#include <bt_sock.h>

// Helper
#include "SymbianTools.h"

namespace Pt {

namespace System {

SerialDeviceImpl::SerialDeviceImpl( ) 
: _socketConnected(false), _servConnected(false), _timeOut(IOResult::WaitInfinite),
_buff(0), _tempBuffer(0,0,0)
{
}


SerialDeviceImpl::~SerialDeviceImpl()
{
    close();

    if (_buff)
        delete[] _buff;
}

void SerialDeviceImpl::open(const std::string& path, std::ios_base::openmode mode, bool isAsync)
{
    if ((mode & std::ios_base::out) || !(mode & std::ios_base::in))    
    {
        throw AccessFailed("open: Writing to the port is currently not supported.", PT_SOURCEINFO);
    }
    
    // try to determine whether user tries to connect BTCOMM
    // syntax is BTCOMM::n where n is a decimal port number
    
    std::string::size_type pos = path.rfind("::");
    
    if (pos == std::string::npos)
    {
        throw AccessFailed("open: Failed to open port. Wrong port name syntax.", PT_SOURCEINFO);
    }

    // extract base name
    std::string strDeviceName = path.substr(0, pos);

    if (strDeviceName != "BTCOMM")
    {
        throw AccessFailed("open: Failed to open port. Unknown port.", PT_SOURCEINFO);
    }
    
    std::string strPortNumber = path.substr(pos+2, path.length());
    
    std::istringstream iss(strPortNumber);
    int portNum;
    iss >> portNum;
    
    openBluetoothSocket(doBluetoothDeviceQuery(), portNum);
}

TBTDevAddr SerialDeviceImpl::doBluetoothDeviceQuery()
{
    // 1. Create a notifier
    RNotifier notifier;
    TInt err = notifier.Connect();
    if (err != KErrNone)
        throw AccessFailed("open: Can't query bluetooth devices (RNotifier::Connect() failed)", PT_SOURCEINFO);

    // 2. Start the device selection plug-in
    TBTDeviceSelectionParams selectionFilter;
    TUUID targetServiceClass(0x1101);
    selectionFilter.SetUUID(targetServiceClass);
    TBTDeviceSelectionParamsPckg pckg(selectionFilter);
    TBTDeviceResponseParams result;
    TBTDeviceResponseParamsPckg resultPckg(result);
    TRequestStatus status;
    notifier.StartNotifierAndGetResponse(status, KDeviceSelectionNotifierUid, pckg, resultPckg);
    User::After(2000000);

    // 3. Wait for dialog to hide, if it was canceled we can't proceed
    User::WaitForRequest(status);
    err = status.Int();
    if (err != KErrNone)
        throw AccessFailed("open: Can't query bluetooth devices (Waiting for StartNotifierAndGetResponse failed)", PT_SOURCEINFO);
        
    // 4. Clean up
    notifier.CancelNotifier(KDeviceSelectionNotifierUid);
    notifier.Close();    
    
    return resultPckg().BDAddr();
}

_LIT(KRfComm,"RFCOMM");

void SerialDeviceImpl::openBluetoothSocket(const TBTDevAddr& devAddr, int portNum)
{
    TInt err = _servConnected ? KErrNone : _socketServ.Connect();
    
    if (err != KErrNone && err != KErrAlreadyExists)
    {
        _servConnected = false;
        throw AccessFailed("open: Can't open Bluetooth socket (RSocketServ::Connect() failed)", PT_SOURCEINFO);
    }
    
    _servConnected = true;
    
    TProtocolDesc pdesc;
    
    err = _socketServ.FindProtocol(KRfComm(), pdesc);
    if (err != KErrNone)
    {
        throw AccessFailed("open: Can't open Bluetooth socket (RSocketServ::FindProtocol() failed)", PT_SOURCEINFO);
    }

    err = _listenSock.Open(_socketServ, pdesc.iAddrFamily, pdesc.iSockType, KRFCOMM);
    if (err != KErrNone)
    {
        throw AccessFailed("open: Can't open Bluetooth socket (RSocket::Open() failed)", PT_SOURCEINFO);
    }
    
    // Bluetooth socket address object
    TBTSockAddr btsockaddr;
    btsockaddr.SetPort(portNum);
    btsockaddr.SetBTAddr(devAddr);
    
    TRequestStatus stat;
    _listenSock.Connect(btsockaddr, stat);
    User::WaitForRequest(stat);
    
    err = stat.Int();    
    if (err != KErrNone)
    {
        throw AccessFailed("open: Can't open Bluetooth socket (Waiting for RSocket::Connect() failed)", PT_SOURCEINFO);
    }
    
    _socketConnected = true;
    // default is infinite timout value
    _timeOut = IOResult::WaitInfinite;
}

void SerialDeviceImpl::open(int fd, bool isAsync)
{
    throw AccessFailed("Open with file descriptor not supported on Symbian", PT_SOURCEINFO);
}

void SerialDeviceImpl::close()
{
    if (_socketConnected)
    {
       _listenSock.Close();
       _socketConnected = false;
    }
    
    if (_servConnected)
    {
        _socketServ.Close();
        _servConnected = false;
    }
}

IOResult& SerialDeviceImpl::beginRead(char* buffer, size_t n, bool& eof)
{
    if (_readResult.isReadPending())
    {
        return _readResult;                                                        
    }
    
    _readResult.attach(buffer, n);
    _readResult.allocSymbianBuffer(n);
    
    if ((std::size_t)_readResult._tempBuffer.MaxSize() != n)
    {
        throw IOError("Could not allocate native Symbian buffer with the requested size. Try 8/16 byte aligned sizes.", PT_SOURCEINFO);                                                
    }
    
    TRequestStatus& status = _readResult._status;
    _listenSock.Read(_readResult._tempBuffer, _readResult._status);
    
    // How do we know yet?
    eof = false;
    
    // TODO: check result
    bool res = _readResult.start();    
    
    return _readResult;
}

std::size_t SerialDeviceImpl::endRead(IOResult& result, bool& eof)
{
    if (_readResult._status.Int() == KErrNone || 
        _readResult._status.Int() == KErrEof)
    {
        // TODO: check result
        bool res = _readResult.end();    

        eof = _readResult._status.Int() == KErrEof;
        return _readResult.transferData();
    }
    else if (_readResult.isReadPending())
    {
        throw IOError("There is still a pending read. Data can not be fetched yet.", PT_SOURCEINFO);                                                        
    }
    
    return 0;
}

std::size_t SerialDeviceImpl::read(char* buffer, std::size_t count, bool& eof)
{
    if (_buff)
    {
        delete[] _buff;
        _buff = 0;
    }
    
    _buff = new char[count];
    
    _tempBuffer.Set((TUint8*)_buff, 0, count);
    _tempBuffer.Zero();
    
    if ((std::size_t)_tempBuffer.MaxSize() != count)
    {
        throw IOError("Could not allocate native Symbian buffer with the requested size. Try 8/16 byte aligned sizes.", PT_SOURCEINFO);                                                
    }

    TRequestStatus status;
    _listenSock.Read(_tempBuffer, status);
    
    //User::WaitForRequest(status);
    bool res;
    
    if (_timeOut == IOResult::WaitInfinite)
        res = SymbianTools::WaitForRequestWithTimeOut(status, -1);
    else
        res = SymbianTools::WaitForRequestWithTimeOut(status, _timeOut);
    
    if (status.Int() != KErrNone && 
        status.Int() != KErrEof)
    {
        throw IOError(res ? "Read failed for unkown reason." : "Read timed out.", PT_SOURCEINFO);        
    }   

    eof = status.Int() == KErrEof;
    
    if ((unsigned)_tempBuffer.Size() > count)
    {
        throw IOError("Read too much data.", PT_SOURCEINFO);                                        
    }

    char* dst = buffer;
    for (int j = 0; j < _tempBuffer.Size(); j++)
        dst[j] = _tempBuffer[j];  
    
    std::size_t result = (std::size_t)_tempBuffer.Size();
    
    return result;
}

IOResult& SerialDeviceImpl::beginWrite(const char* buffer, std::size_t n)
{
    // TODO: Implement me
    throw std::logic_error("Writing to the port is currently not supported." + PT_SOURCEINFO);                
    return _writeResult;
}

std::size_t SerialDeviceImpl::endWrite(IOResult& result)
{
    // TODO: Implement me
    throw std::logic_error("Writing to the port is currently not supported." + PT_SOURCEINFO);                
    return 0;
}

std::size_t SerialDeviceImpl::write( const char* buffer, std::size_t count )
{
    // TODO: Implement me
    throw std::logic_error("Writing to the port is currently not supported." + PT_SOURCEINFO);                
    return 0;
}

void SerialDeviceImpl::sync() const
{
}

void SerialDeviceImpl::setBaudRate( unsigned br )
{
    // we're currently simulating Bluetooth serial communication 
    // in Symbian over Bluetooth sockets
    // There is no way we can change all the low level details
    // but we should behave gracefully when user tries to set these
    // details
    _rate = br;
}


unsigned SerialDeviceImpl::baudRate() const
{
    return _rate;
}


void SerialDeviceImpl::setCharSize( int size )
{
    _charSize = size;
}


int SerialDeviceImpl::charSize() const
{
    return _charSize;
}


void SerialDeviceImpl::setStopBits( SerialDevice::StopBits bits )
{
    _bits = bits;
}


SerialDevice::StopBits SerialDeviceImpl::stopBits() const
{
    return _bits;
}


void SerialDeviceImpl::setParity( SerialDevice::Parity parity )
{
    _parity = parity;
}


SerialDevice::Parity SerialDeviceImpl::parity() const
{
    return _parity;
}


void SerialDeviceImpl::setFlowControl( SerialDevice::FlowControl flowControl )
{
    _flowControl = flowControl;
}

bool SerialDeviceImpl::setSignal(SerialDevice::SerialLine signal)
{
    return false;
}

SerialDevice::FlowControl SerialDeviceImpl::flowControl() const
{
    return _flowControl;
}

void SerialDeviceImpl::setTimeout( std::size_t msec )
{
    _timeOut = msec;    
}

std::size_t SerialDeviceImpl::timeout() const
{
    return _timeOut;
}

void SerialDeviceImpl::flush()
{
}

} //namespace System
} //namespace Pt
