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
#include "win32.h"
#include "SerialDeviceImpl.h"
#include "Pt/System/Thread.h"
#include "Pt/System/Selector.h"
#include <iostream>


namespace Pt{
namespace System{

SerialDeviceImpl::SerialDeviceImpl()
: _terminateEv( 0 )
{        
    memset( &_ovRead, 0, sizeof( _ovRead ) );
    memset( &_ovWrite, 0, sizeof( _ovWrite ) );
    memset( &_ovStatus, 0, sizeof( _ovStatus ) );

    _ovWrite.hEvent  = CreateEvent( 0, TRUE ,FALSE, 0 );
    _ovRead.hEvent   = CreateEvent( 0, TRUE ,FALSE, 0 );
    _ovStatus.hEvent = CreateEvent( 0, TRUE ,FALSE, 0 );
    _terminateEv     = CreateEvent( 0, TRUE , FALSE, 0 );
}

SerialDeviceImpl::~SerialDeviceImpl()
{ 
    CloseHandle( _ovWrite.hEvent );
    CloseHandle( _ovRead.hEvent );
    CloseHandle( _ovStatus.hEvent );
    CloseHandle( _terminateEv );
}

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
        throw OpenFailed("Could not open port" , PT_SOURCEINFO);

    try
    {
        if( !GetCommState( _handle, &_orgCommState ) )
            throw OpenFailed("Get port state failed" , PT_SOURCEINFO);
                   
        COMMTIMEOUTS comTimeOut;
        comTimeOut.ReadIntervalTimeout          = MAXDWORD;
        comTimeOut.ReadTotalTimeoutMultiplier   = 0;
        comTimeOut.ReadTotalTimeoutConstant     = 0;
        comTimeOut.WriteTotalTimeoutMultiplier  = 10;
        comTimeOut.WriteTotalTimeoutConstant    = 100;

        if( !SetCommTimeouts( _handle, &comTimeOut ) )
            throw IOError("Set port time outs failed" , PT_SOURCEINFO);

        SetCommMask( _handle, 0 );
    }
    catch( ... )
    {
        CloseHandle( _handle );
        _handle = 0;
        throw;
    }
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

    _handle = 0;
}

size_t SerialDeviceImpl::read( char* buffer, size_t count, bool& eof )
{
    DWORD length;

    if( ReadFile( _handle, buffer, count, &length, &_ovRead ) )
        return length;

    if( ERROR_HANDLE_EOF == GetLastError() )
    {
        eof = true;
        length = 0;
    }
    else if( ERROR_IO_PENDING == GetLastError() )
    {
        if( S_OK != GetOverlappedResult(_handle, &_ovRead, &length, FALSE) )
            throw IOError("Read port failed" , PT_SOURCEINFO);
    }
    else
    {
        throw IOError("Read port failed" , PT_SOURCEINFO);
    }
    
    return length;
}

size_t SerialDeviceImpl::write( const char* buffer, size_t count )
{
    DWORD length = 0;
    
    if( WriteFile(  _handle,  buffer,  count, &length, &_ovWrite ) )
        return length;

    if( ERROR_IO_PENDING == GetLastError() )
    {
        if( S_OK != GetOverlappedResult(_handle, &_ovWrite, &length, FALSE) )
            throw IOError("Could not write to file handle", PT_SOURCEINFO);
    }
    else
    {
        throw IOError("Could not write to file handle", PT_SOURCEINFO);
    }

    return length;
}

void SerialDeviceImpl::writeCommState( DCB& commState )
{
    if( !SetCommState( _handle, &commState ) )
        throw IOError( "Changing port state failed" , PT_SOURCEINFO );
}

void SerialDeviceImpl::readCommState( DCB& commState ) const
{
    if( !GetCommState( _handle, &commState ) )
        throw IOError( "Get port state failed" , PT_SOURCEINFO );
}

void SerialDeviceImpl::setBaudRate( SerialDevice::BaudRate rate )
{
    DCB commState;
    readCommState( commState );
    commState.BaudRate = static_cast<DWORD>( rate );
    writeCommState( commState );
}

SerialDevice::BaudRate SerialDeviceImpl::baudRate() const
{
    DCB commState;
    readCommState( commState );
    return static_cast<SerialDevice::BaudRate>( commState.BaudRate );
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

    if( !SetCommTimeouts( _handle, &comTimeOut ) )
        throw IOError("Set port time outs failed" , PT_SOURCEINFO);
}

size_t SerialDeviceImpl::timeout() const
{
    COMMTIMEOUTS comTimeOut;
    GetCommTimeouts( _handle, &comTimeOut );
    return  comTimeOut.ReadTotalTimeoutConstant;    
}

void SerialDeviceImpl::flush()
{
    FlushFileBuffers( _handle );
}

void SerialDeviceImpl::resetEvent( HANDLE handle )
{     
    if( handle != _ovStatus.hEvent )
        throw std::logic_error("Unknown event handle ");
        
    if( WaitCommEvent( _handle, &_eventMask, &_ovStatus ) == FALSE )
    {
        if( GetLastError () != ERROR_IO_PENDING )
            throw std::runtime_error( "WaitCommEvent failed" + PT_SOURCEINFO );
    }
}

/*void SerialDeviceImpl::eventHandles( std::vector<HANDLE>& handles, size_t waitMode )
{
    handles.clear();
    
    DWORD waitMask = 0;
    
    if(waitMode & Selector::WaitInput)
        waitMask |= EV_RXCHAR;
        
    if(waitMode & Selector::WaitOutput)
        waitMask |= EV_TXEMPTY;

    SetCommMask( _handle, waitMask );
    
    handles.push_back( _ovStatus.hEvent );    

    resetEvent( _ovStatus.hEvent );            
}
        
IODeviceImpl::WaitResult SerialDeviceImpl::waitResult( HANDLE handle )
{           
   if( handle != _ovStatus.hEvent )
        throw std::logic_error("Unknown event handle ");
        
    if( (_eventMask & EV_TXEMPTY) == EV_TXEMPTY )
        return ReadyWrite;    

/*    if( (_eventMask & EV_RXFLAG) == EV_RXFLAG)
        return _readEvent;       *//*

    if( (_eventMask & EV_RXCHAR) == EV_RXCHAR)
        return ReadyRead;
    
    throw IOError("Unknow event", PT_SOURCEINFO);
    return ReadyRead;
}*/

}//namespace System
}//namespace Pt
