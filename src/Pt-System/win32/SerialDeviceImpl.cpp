/***************************************************************************
 *   Copyright (C) 2007 Marc Boris Dürner                                  *
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
#include "SerialDeviceImpl.h"
#include "win32.h"
#include <iostream>
#include <Pt/System/Thread.h>

namespace Pt{
namespace System{

const int SerialDeviceImpl::ASCII_XON  = 0x11; 
const int SerialDeviceImpl::ASCII_XOFF = 0x13;

SerialDeviceImpl::SerialDeviceImpl()
{ }

SerialDeviceImpl::~SerialDeviceImpl()
{ }

void SerialDeviceImpl::open( const std::string& port_, std::ios_base::openmode mode )
{
    std::basic_string<TCHAR> port = win32::fromMultiByte( port_.c_str() );

    DWORD openFlags = 0;

    if( mode & std::ios_base::out )
        openFlags |= GENERIC_WRITE;

    if( mode & std::ios_base::in )
        openFlags |= GENERIC_READ;

    _handle = CreateFile( port.c_str() , openFlags, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

    if( _handle == 0  || _handle == INVALID_HANDLE_VALUE )
        throw IO::IOError("Could not open port" , PT_SOURCEINFO);

    COMMTIMEOUTS comTimeOut;
    comTimeOut.ReadIntervalTimeout          = MAXDWORD;
    comTimeOut.ReadTotalTimeoutMultiplier   = 0;
    comTimeOut.ReadTotalTimeoutConstant     = 0;
    comTimeOut.WriteTotalTimeoutMultiplier  = 10;
    comTimeOut.WriteTotalTimeoutConstant    = 1000;

    if( !SetCommTimeouts( _handle, &comTimeOut ) )
        throw IO::IOError("Set port time outs failed" , PT_SOURCEINFO);

    if( !GetCommState( _handle, &_commState ) )
        throw IO::IOError("Get port state failed" , PT_SOURCEINFO);

    if( !GetCommState( _handle, &_orgCommState ) )
        throw IO::IOError("Get port state failed" , PT_SOURCEINFO);

    if( !GetCommMask( _handle, &_waitCommMask ) )
        throw IO::IOError("Get port wait mask failed" , PT_SOURCEINFO);        

    //Create the wait events.            
    memset(&_overlapped,0,sizeof(_overlapped) );
    
    _overlapped.hEvent = CreateEvent( 0, FALSE ,0, 0 ); // The port event.
    _terminateEv       = CreateEvent( 0, FALSE ,0, 0 ); // The terminate event.       
}

void SerialDeviceImpl::close()
{
    if( _handle == 0 || _handle == INVALID_HANDLE_VALUE )
        return;
    
    //Signalize the terminate event.
    SetEvent( _terminateEv );
    
    //Reset the wait mask
    SetCommMask( _handle, 0 );

    //Restore the port state.
    SetCommState( _handle, &_orgCommState );
    
    //Close the port handle.
    CloseHandle( _handle );
    
    //Terminate event
    CloseHandle( _terminateEv );
    
    //Port event.
    CloseHandle( _overlapped.hEvent );

    _handle = 0;
}

size_t SerialDeviceImpl::read( char* buffer, size_t count, bool& eof )
{
    DWORD   length;
    DWORD   error;
    COMSTAT cs;

    ClearCommError( _handle, &error, &cs );

    if( !ReadFile( _handle, buffer, count, &length, &_overlapped ) )
    {
        if( ERROR_HANDLE_EOF == GetLastError() )
        {
            eof = true;
            length = 0;
        }
        else if( ERROR_IO_PENDING == GetLastError() )
        {
            if ( FALSE == GetOverlappedResult(_handle, &_overlapped, &length, FALSE) ) 
            {
                length = 0;
            }
        }
        else
        {
            throw IO::IOError("Read port failed" , PT_SOURCEINFO);
        }             
    }

    return length;
}

size_t SerialDeviceImpl::write( const char* buffer, size_t count )
{
    DWORD length = 0;

    if( !WriteFile(  _handle,  buffer,  count, &length, &_overlapped ) )
    {
        if( ERROR_IO_PENDING == GetLastError() )
        {
            if( FALSE == GetOverlappedResult(_handle, &_overlapped, &length, FALSE) )
            {
                length = 0;
            }            
        }
        else
        {
            throw IO::IOError("Could not write to file handle", PT_SOURCEINFO);
        }
    }

    return length;
}

void SerialDeviceImpl::writeCommState()
{
    if( !SetCommState( _handle, &_commState ) )
        throw IO::IOError("Changing port state failed" , PT_SOURCEINFO);
}

void SerialDeviceImpl::readCommState()
{
    if( !GetCommState( _handle, &_commState ) )
        throw IO::IOError("Get port state failed" , PT_SOURCEINFO);
}

void SerialDeviceImpl::setBaudRate( SerialDevice::BaudRate rate )
{
    _commState.BaudRate = static_cast<DWORD>( rate );
    writeCommState();
}

SerialDevice::BaudRate SerialDeviceImpl::baudRate() const
{
    return static_cast<SerialDevice::BaudRate>( _commState.BaudRate );
}

void SerialDeviceImpl::setCharSize( int size )
{
    _commState.ByteSize  = size;
    writeCommState();
}

int SerialDeviceImpl::charSize() const
{
    return _commState.ByteSize;
}

void SerialDeviceImpl::setStopBits( SerialDevice::StopBits bits )
{
    switch( bits )
    {
        case SerialDevice::OneStopBit: 
            _commState.StopBits  = ONESTOPBIT;
        break;

        case SerialDevice::One5StopBits:
            _commState.StopBits  = ONE5STOPBITS;
        break;

        case SerialDevice::TwoStopBits:
            _commState.StopBits  = TWOSTOPBITS;
        break;
    }

    writeCommState();
}

SerialDevice::StopBits SerialDeviceImpl::stopBits() const
{
    switch( _commState.StopBits )
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
    switch( parity )
    {
        case SerialDevice::ParityEven:
            _commState.Parity = EVENPARITY;
        break;

        case SerialDevice::ParityOdd:
            _commState.Parity = ODDPARITY;
        break;

        case SerialDevice::ParityNone:
            _commState.Parity = NOPARITY;
        break;
    }

    writeCommState();
}

SerialDevice::Parity SerialDeviceImpl::parity() const
{
    switch( _commState.Parity )
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

void SerialDeviceImpl::setFlowControl( SerialDevice::FlowControl flowControl )
{
    _commState.XonChar  = ASCII_XON;
    _commState.XoffChar = ASCII_XOFF;
    _commState.XonLim   = 100;
    _commState.XoffLim  = 100;

    switch( flowControl )
    {
        case SerialDevice::FlowControlSoft:
            _commState.fInX = _commState.fOutX = 1;
        break;

        case SerialDevice::FlowControlBoth:
            _commState.fInX = _commState.fOutX = 1;
        case SerialDevice::FlowControlHard:
            _commState.fOutxCtsFlow = 1;
            _commState.fRtsControl = RTS_CONTROL_HANDSHAKE;
        break;
    }

    writeCommState();
}

void SerialDeviceImpl::flush()
{
    PurgeComm( _handle, PURGE_RXABORT | PURGE_RXCLEAR);
}

SerialDevice::FlowControl SerialDeviceImpl::flowControl() const
{
    //Check for both.
    if( _commState.fInX == _commState.fOutX && _commState.fOutX == 1 &&
        _commState.fOutxCtsFlow == 1 && _commState.fRtsControl == RTS_CONTROL_HANDSHAKE )
        return SerialDevice::FlowControlBoth;

    //Check for hardware flow control.
    if( _commState.fOutxCtsFlow == 1 && _commState.fRtsControl == RTS_CONTROL_HANDSHAKE )
        return  SerialDevice::FlowControlHard;

    //Check for software flow control.
    if( _commState.fInX == _commState.fOutX && _commState.fInX == 1 )
       return SerialDevice::FlowControlSoft;

    throw std::runtime_error( "Unknown flow control" + PT_SOURCEINFO );

    return SerialDevice::FlowControlBoth;
}

bool SerialDeviceImpl::wait( SerialDevice::WaitMode mode, unsigned int  msec )
{
    DWORD timeout = static_cast<DWORD>( msec );     
    
    if( msec != SerialDevice::WaitTimeInfinite )
        timeout = INFINITE;            

    if( mode == SerialDevice::WaitOutput)
        SetCommMask( _handle, EV_TXEMPTY | EV_BREAK);
    else if ( mode == SerialDevice::WaitInput)
        SetCommMask( _handle, EV_RXCHAR | EV_BREAK);

    DWORD waitMask = 0;

    if( WaitCommEvent( _handle, &waitMask, &_overlapped ) == FALSE )
    {
        if( GetLastError () != ERROR_IO_PENDING )
            throw std::runtime_error( "WaitCommEvent failed" + PT_SOURCEINFO );
    }

    HANDLE  eventHandles[2];
    eventHandles[0] = _overlapped.hEvent;
    eventHandles[1] = _terminateEv;

    const DWORD reason = WaitForMultipleObjects( 2, eventHandles, FALSE, timeout );
    
    if( reason == WAIT_FAILED )
        throw std::runtime_error( "Could not wait for file handle: " + PT_SOURCEINFO);
    
    GetCommMask( _handle, &waitMask );
    
    if( mode == SerialDevice::WaitOutput )
        return ( reason == WAIT_OBJECT_0 && ( waitMask & EV_TXEMPTY ) );
    else
        return ( reason == WAIT_OBJECT_0 && ( waitMask & EV_RXCHAR ) );
}

}//namespace System
}//namespace Pt
