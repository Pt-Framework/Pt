/***************************************************************************
 *   Copyright (C) 2007 Marc Boris Dürner                                  *
 *   Copyright (C) 2007 Laurentiu-Gheorghe Crisan                          *
 *   Copyright (C) 2007 Bjoern Oliver Streule                              *
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
#include "../win32/win32.h"
#include "SerialDeviceImpl.h"
#include "Pt/System/Thread.h"
#include "Pt/System/Selector.h"
#include <iostream>

namespace Pt{
namespace System{

SerialDeviceImpl::SerialDeviceImpl()
: _eventThread( *self() )
, _terminateThread( false )
, _comEvent( 0 )
, _waitForComEvent( 0 )
{ 
    _comEvent        = CreateEvent(NULL, TRUE, FALSE, NULL);    
    _waitForComEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

SerialDeviceImpl::~SerialDeviceImpl()
{ }

void SerialDeviceImpl::open( const std::string& port_, std::ios_base::openmode mode, bool isAsync )
{
    std::basic_string<TCHAR> port = win32::fromMultiByte( port_.c_str() );

    DWORD openFlags = 0;

    if( mode & std::ios_base::out )
        openFlags |= GENERIC_WRITE;

    if( mode & std::ios_base::in )
        openFlags |= GENERIC_READ;

    _handle = CreateFile( port.c_str() , openFlags, 0, NULL, OPEN_EXISTING, 0, NULL);

    size_t err = GetLastError();
    
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
        comTimeOut.WriteTotalTimeoutMultiplier  = 0;
        if ( isAsync )
        {            
            comTimeOut.WriteTotalTimeoutConstant    = 1;               
        }
        else
        {   
            comTimeOut.WriteTotalTimeoutConstant    = 0;
        }
        
        if( !SetCommTimeouts( _handle, &comTimeOut ) )
            throw OpenFailed("Set port time outs failed" , PT_SOURCEINFO);        
    }        
    catch( ... )
    {
        CloseHandle( _handle );

        _handle = 0;        
        throw;
    }    
    
    _terminateThread = false;
    if (isAsync)
    {
        _eventThread.start();    
    }
}

void SerialDeviceImpl::close()
{
    if( _handle == 0 || _handle == INVALID_HANDLE_VALUE )
        return;
    
    //Signalize the thread to terminate.
    _terminateThread = true;
    
    //Reset the wait mask, to wake up the comm event thread.
    SetCommMask( _handle, 0 );

    // Wake up the thread.
    SetEvent(_waitForComEvent);
    
    // Closing the com handle will end WaitComEvent.
    CloseHandle( _handle );

    //Wait of comm event thread termination.
    _eventThread.wait();    

    CloseHandle( _comEvent );

    CloseHandle( _waitForComEvent );

    _handle             = 0;
    _comEvent           = 0;
    _waitForComEvent    = 0;
}

size_t SerialDeviceImpl::read( char* buffer, size_t count, bool& eof )
{
    DWORD   length;

    if( !ReadFile( _handle, buffer, count, &length, 0 ) )
        throw IOError("Read port failed" , PT_SOURCEINFO);

    if( length == 0 )     
       eof = true;

    return length;
}

IOResult& SerialDeviceImpl::beginRead(char* buffer, size_t count, bool& eof) 
{
    SetCommMask( _handle, EV_RXCHAR );

    _readResult.attach(buffer, count);

    _readResult.setHandle(_comEvent);

    SetEvent(_waitForComEvent);

    return _readResult;    
}

size_t SerialDeviceImpl::endRead(IOResult& result, bool& eof)
{
    DWORD   length;    

    ResetEvent(_comEvent); 

    if( !ReadFile( _handle, _readResult.buffer(), _readResult.bufferSize(), &length, 0 ) )
        throw IOError("Read port failed" , PT_SOURCEINFO);

    if( length == 0 )     
       eof = true;

    return length;
}

IOResult& SerialDeviceImpl::beginWrite(const char* buffer, size_t n)
{
	DWORD writtenBytes = 0;

    _writeResult.attach((char*) buffer, n);

    _writeResult.setHandle(_comEvent);

    size_t bytes = this->write(buffer, n); 

    _writeResult.setWrittenBytes(bytes);

    if (bytes == 0)
    {
        SetCommMask( _handle, EV_TXEMPTY );

        SetEvent(_waitForComEvent);
    }
    else
    {
        SetEvent(_comEvent);
    }

	return _writeResult;
}

size_t SerialDeviceImpl::endWrite(IOResult& result)
{
	assert(&result == &_writeResult);

    ResetEvent(_comEvent); 

    size_t bytes = 0;
    if ( _writeResult.writtenBytes() == 0 )
    {
        bytes = this->write(_writeResult.buffer(), _writeResult.bufferSize());
    }   
	
	return _writeResult.writtenBytes() + bytes;
}	

size_t SerialDeviceImpl::write( const char* buffer, size_t count )
{
    DWORD length = 0;

    if( !WriteFile(  _handle,  buffer,  count, &length, 0 ) )
    {
        DWORD error = GetLastError();
        throw IOError("Could not write to file handle", PT_SOURCEINFO);
    }

    return length;
}

void SerialDeviceImpl::writeCommState( DCB& commState )
{
    if( !SetCommState( _handle, &commState ) )
        throw IOError("Changing port state failed" , PT_SOURCEINFO);
}

void SerialDeviceImpl::readCommState( DCB& commState ) const
{
    if( !GetCommState( _handle, &commState ) )
        throw IOError("Get port state failed" , PT_SOURCEINFO);
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

    //Check for hardware.
    if( commState.fOutxCtsFlow == 1 && commState.fRtsControl == RTS_CONTROL_HANDSHAKE )
        return  SerialDevice::FlowControlHard;

    //Check for software.
    if( commState.fInX == commState.fOutX && commState.fInX == 1 )
       return SerialDevice::FlowControlSoft;

    throw std::runtime_error( "Unknown flow control" + PT_SOURCEINFO );

    return SerialDevice::FlowControlBoth;
}

void SerialDeviceImpl::flush()
{
    FlushFileBuffers( _handle );
}

void SerialDeviceImpl::run()
{
    DWORD   eventMask = 0;   
    DWORD   error;
    COMSTAT cs;    
    
    while( !_terminateThread )
    {   
        eventMask = 0;

        WaitForSingleObject(_waitForComEvent,  INFINITE);

        if (_terminateThread)
            return;

        bool retVal = ( WaitCommEvent( _handle, &eventMask, NULL )  == TRUE ); 

        ClearCommError( _handle, &error, &cs );

        //ToDo: Handle com errors

        DWORD err = GetLastError();
        
        if( retVal && ( eventMask & (EV_TXEMPTY | EV_RXCHAR)) )
        {         
            SetEvent( _comEvent );
        }        
        else
        {
            // ToDo: Handling for unexpected events
            DebugBreak();
        }
    }
}

void SerialDeviceImpl::setTimeout( size_t msec )
{    
    COMMTIMEOUTS comTimeOut;
    comTimeOut.ReadIntervalTimeout          = MAXDWORD;
    comTimeOut.ReadTotalTimeoutMultiplier   = MAXDWORD;
    comTimeOut.ReadTotalTimeoutConstant     = msec;

    comTimeOut.WriteTotalTimeoutMultiplier  = 0;
    comTimeOut.WriteTotalTimeoutConstant    = msec;

    if( !SetCommTimeouts( _handle, &comTimeOut ) )
        throw IOError("Set port time outs failed" , PT_SOURCEINFO);
}

size_t SerialDeviceImpl::timeout() const
{
    COMMTIMEOUTS comTimeOut;
    GetCommTimeouts( _handle, &comTimeOut );
    return  comTimeOut.ReadTotalTimeoutConstant;    
}

}//namespace System
}//namespace Pt
