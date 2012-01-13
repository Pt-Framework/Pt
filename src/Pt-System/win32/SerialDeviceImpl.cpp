/*
 * Copyright (C) 2007 Marc Boris D�rner
 * Copyright (C) 2007 Laurentiu-Gheorghe Crisan
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
#include "win32.h"
#include "SerialDeviceImpl.h"
#include "MainLoopImpl.h"
#include "Pt/System/IODevice.h"
#include "Pt/System/EventLoop.h"
#include "Pt/System/SystemError.h"
#include <iostream>

namespace Pt {

namespace System {

SerialDeviceImpl::SerialDeviceImpl(SerialDevice& device)
: OverlappedIODeviceImpl(device)
, _device(device)
, _waitHandle(INVALID_HANDLE_VALUE)
{
    _waitHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
    if( _waitHandle == NULL )
        throw SystemError("CreateEvent failed", PT_SOURCEINFO);

    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;
    _readOv.hEvent = NULL;

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
    _writeOv.hEvent = NULL;
}


SerialDeviceImpl::~SerialDeviceImpl()
{
    ::CloseHandle(_waitHandle);
}


void SerialDeviceImpl::open( const std::string& port_, IODevice::OpenMode mode)
{
    std::basic_string<TCHAR> port;
    win32::fromMultiByte( port_.c_str(), port );

    DWORD openFlags = 0;

    if( mode & IODevice::Write )
        openFlags |= GENERIC_WRITE;

    if( mode & IODevice::Read )
        openFlags |= GENERIC_READ;

    HANDLE h = INVALID_HANDLE_VALUE;

    h = CreateFile( port.c_str() , openFlags, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

    if( h == 0  || h == INVALID_HANDLE_VALUE )
        throw DeviceNotFound("Could not open port" , PT_SOURCEINFO);

    try
    {
        if( ! GetCommState( h, &_orgCommState ) )
            throw AccessFailed("Get port state failed" , PT_SOURCEINFO);

        // Do not use timeouts, return read data immediately.
        COMMTIMEOUTS comTimeOut;
        comTimeOut.ReadIntervalTimeout          = MAXDWORD;
        comTimeOut.ReadTotalTimeoutMultiplier   = MAXDWORD;
        comTimeOut.ReadTotalTimeoutConstant     = MAXDWORD-1; // MAXDWORD does not apply for all drivers
        comTimeOut.WriteTotalTimeoutMultiplier  = 0;
        comTimeOut.WriteTotalTimeoutConstant    = 1;

        if( !SetCommTimeouts( h, &comTimeOut ) )
            throw IOError("Set port time outs failed" , PT_SOURCEINFO);

        SetCommMask( h, 0 );
    }
    catch( ... )
    {
        CloseHandle( h );
        throw;
    }

    this->setHandle(h);
}


void SerialDeviceImpl::close()
{
    //Restore the port state.
    SetCommState( handle(), &_orgCommState );
    
    OverlappedIODeviceImpl::close();
}


void SerialDeviceImpl::cancel(EventLoop& loop)
{
    ::CancelIo( handle() );
    ::PurgeComm(handle(), PURGE_RXABORT | PURGE_TXABORT| PURGE_TXCLEAR | PURGE_RXCLEAR);

    OverlappedIODeviceImpl::cancel(loop);
}


void SerialDeviceImpl::writeCommState( DCB& commState )
{
    if( ! SetCommState( handle(), &commState ) )
        throw IOError( "Changing port state failed" , PT_SOURCEINFO );
}


void SerialDeviceImpl::readCommState( DCB& commState ) const
{
    if( ! GetCommState( handle(), &commState ) )
        throw IOError( "Get port state failed" , PT_SOURCEINFO );
}


void SerialDeviceImpl::setBaudRate( unsigned rate )
{
    DCB commState;
    readCommState( commState );
    commState.BaudRate =  rate ;
    writeCommState( commState );
}


unsigned SerialDeviceImpl::baudRate() const
{
    DCB commState;
    readCommState( commState );
    return commState.BaudRate ;
}


void SerialDeviceImpl::setCharSize( int size )
{
    DCB commState;
    readCommState( commState );
    commState.ByteSize  = size;
    writeCommState( commState );
}


int SerialDeviceImpl::charSize() const
{
    DCB commState;
    readCommState( commState );
    return commState.ByteSize;
}


void SerialDeviceImpl::setStopBits( SerialDevice::StopBits bits )
{
    DCB commState;
    readCommState( commState );

    switch( bits )
    {
        case SerialDevice::OneStopBit:
            commState.StopBits  = ONESTOPBIT;
        break;

        case SerialDevice::One5StopBits:
            commState.StopBits  = ONE5STOPBITS;
        break;

        case SerialDevice::TwoStopBits:
            commState.StopBits  = TWOSTOPBITS;
        break;
    }

    writeCommState( commState );
}


SerialDevice::StopBits SerialDeviceImpl::stopBits() const
{
    DCB commState;

    readCommState( commState );
    switch( commState.StopBits )
    {
        case ONESTOPBIT:
            return SerialDevice::OneStopBit;
        break;

        case ONE5STOPBITS:
            return SerialDevice::One5StopBits;
        break;

        case TWOSTOPBITS:
            return SerialDevice::TwoStopBits;
        break;
    }

    throw std::runtime_error( "Unknown stop bits" + PT_SOURCEINFO);

    return SerialDevice::OneStopBit;
}


void SerialDeviceImpl::setParity( SerialDevice::Parity parity )
{
    DCB commState;

    readCommState( commState );
    switch( parity )
    {
        case SerialDevice::ParityEven:
            commState.Parity = EVENPARITY;
        break;

        case SerialDevice::ParityOdd:
            commState.Parity = ODDPARITY;
        break;

        case SerialDevice::ParityNone:
            commState.Parity = NOPARITY;
        break;
    }

    writeCommState( commState );
}


SerialDevice::Parity SerialDeviceImpl::parity() const
{
    DCB commState;

    readCommState( commState );

    switch( commState.Parity )
    {
        case EVENPARITY:
            return SerialDevice::ParityEven;
        break;

        case ODDPARITY:
            return SerialDevice::ParityOdd;
        break;

        case NOPARITY :
            return SerialDevice::ParityNone;
        break;
    }

    throw std::runtime_error( "Invalid parity" + PT_SOURCEINFO);
    return SerialDevice::ParityEven;
}

bool SerialDeviceImpl::setSignal(SerialDevice::SerialLine signal)
{
    switch(signal)
    {
        case SerialDevice::CLR_BREAK:
            return EscapeCommFunction(handle(), CLRBREAK) > 0;
        break;
        case SerialDevice::CLR_DTR:
            return EscapeCommFunction(handle(), CLRDTR) > 0;
        break;
        case SerialDevice::CLR_RTS:
            return EscapeCommFunction(handle(), CLRRTS) > 0;
        break;
        case SerialDevice::SET_BREAK:
            return EscapeCommFunction(handle(), SETBREAK) > 0;
        break;
        case SerialDevice::SET_DTR:
            return EscapeCommFunction(handle(), SETDTR) > 0;
        break;
        case SerialDevice::SET_RTS:
            return EscapeCommFunction(handle(), SETRTS) > 0;
        break;
        case SerialDevice::SET_XOFF:
            return EscapeCommFunction(handle(), SETXOFF) > 0;
        break;
        case SerialDevice::SET_XON:
            return EscapeCommFunction(handle(), SETXON) > 0;
        break;
    }

    return false;
}

void SerialDeviceImpl::setFlowControl( SerialDevice::FlowControl flowControl )
{
    static const int ASCII_XON  = 0x11;
    static const int ASCII_XOFF = 0x13;

    DCB commState;

    readCommState( commState );

    commState.XonChar  = ASCII_XON;
    commState.XoffChar = ASCII_XOFF;
    commState.XonLim   = 100;
    commState.XoffLim  = 100;

    switch( flowControl )
    {
        case SerialDevice::FlowControlSoft:
            commState.fInX = commState.fOutX = 1;
            commState.fRtsControl = RTS_CONTROL_DISABLE;
        break;

        case SerialDevice::FlowControlBoth:
            commState.fInX = commState.fOutX = 1;
        case SerialDevice::FlowControlHard:
            commState.fOutxCtsFlow = 1;
            commState.fRtsControl = RTS_CONTROL_HANDSHAKE;
        break;
    }

    writeCommState( commState );
}


SerialDevice::FlowControl SerialDeviceImpl::flowControl() const
{
    DCB commState;

    readCommState( commState );

    //Check for both.
    if( commState.fInX == commState.fOutX && commState.fOutX == 1 &&
        commState.fOutxCtsFlow == 1 && commState.fRtsControl == RTS_CONTROL_HANDSHAKE )
        return SerialDevice::FlowControlBoth;

    //Check for hardware flow control.
    if( commState.fOutxCtsFlow == 1 && commState.fRtsControl == RTS_CONTROL_HANDSHAKE )
        return  SerialDevice::FlowControlHard;

    //Check for software flow control.
    if( commState.fInX == commState.fOutX && commState.fInX == 1 )
       return SerialDevice::FlowControlSoft;

    throw std::runtime_error( "Unknown flow control" + PT_SOURCEINFO );

    return SerialDevice::FlowControlBoth;
}


void SerialDeviceImpl::setTimeout( size_t msec )
{
    COMMTIMEOUTS comTimeOut;
    comTimeOut.ReadIntervalTimeout          = MAXDWORD;
    comTimeOut.ReadTotalTimeoutMultiplier   = MAXDWORD;
    comTimeOut.ReadTotalTimeoutConstant     = msec;
    comTimeOut.WriteTotalTimeoutMultiplier  = 10;
    comTimeOut.WriteTotalTimeoutConstant    = 100;

    if( !SetCommTimeouts( handle(), &comTimeOut ) )
        throw IOError("Set port time outs failed" , PT_SOURCEINFO);
}


size_t SerialDeviceImpl::timeout() const
{
    COMMTIMEOUTS comTimeOut;
    GetCommTimeouts( handle(), &comTimeOut );
    return  comTimeOut.ReadTotalTimeoutConstant;
}

}//namespace System

}//namespace Pt
