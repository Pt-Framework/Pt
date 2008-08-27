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
#include "SelectorImpl.h"
#include "SerialDeviceImpl.h"
#include "Pt/System/Thread.h"
#include <iostream>

namespace Pt{

namespace System{

SerialDeviceImpl::SerialDeviceImpl(SerialDevice& device)
: _device(device)
, _eventThread( *self() )
, _terminateThread(false)
, _ioReady(0)
, _beginWait(0)
//, _rbuf(0)
//, _rbuflen(0)
, _rlen(0)
//, _wbuf(0)
//, _wbuflen(0)
, _wlen(0)
, _event(0)
{ 
    _ioReady  = CreateEvent(NULL, TRUE, FALSE, NULL);    
    _beginWait = CreateEvent(NULL, FALSE, FALSE, NULL);
}


SerialDeviceImpl::~SerialDeviceImpl()
{ 
}


void SerialDeviceImpl::open( const std::string& port_, std::ios_base::openmode mode, bool isAsync )
{
    std::basic_string<TCHAR> port = win32::fromMultiByte( port_.c_str() );

    DWORD openFlags = 0;
    if( mode & std::ios_base::out )
        openFlags |= GENERIC_WRITE;

    if( mode & std::ios_base::in )
        openFlags |= GENERIC_READ;

    HANDLE h = CreateFile( port.c_str() , openFlags, 0, NULL, OPEN_EXISTING, 0, NULL);

    size_t err = GetLastError();
    
    if( h == 0  || h == INVALID_HANDLE_VALUE )
        throw OpenFailed("Could not open port" , PT_SOURCEINFO);

    this->setHandle(h);

    try
    {
        if( ! GetCommState( h, &_orgCommState ) )
            throw OpenFailed("Get port state failed" , PT_SOURCEINFO);       

        COMMTIMEOUTS comTimeOut;
        comTimeOut.ReadIntervalTimeout          = MAXDWORD;
        comTimeOut.ReadTotalTimeoutMultiplier   = MAXDWORD;
        comTimeOut.ReadTotalTimeoutConstant     = MAXDWORD;
        comTimeOut.WriteTotalTimeoutMultiplier  = 0;
        comTimeOut.WriteTotalTimeoutConstant    = 1;
        
        if( ! SetCommTimeouts( h, &comTimeOut ) )
            throw OpenFailed("Set port time outs failed" , PT_SOURCEINFO);        
    }        
    catch( ... )
    {
        CloseHandle(h);
        h = 0;        
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
    if( handle() == 0 || handle() == INVALID_HANDLE_VALUE )
        return;
    
    // Signalise the thread to terminate
    _terminateThread = true;
    
    // Reset the wait mask, to wake up the comm event thread
    SetCommMask( handle(), 0 );

    // Wake up the thread
    SetEvent(_beginWait);
    
    // Closing the com handle will end WaitCommEvent
    CloseHandle( handle() );

    // Wait for comm event thread termination
    _eventThread.wait();    

    CloseHandle( _ioReady );
    CloseHandle( _beginWait );

    this->setHandle(INVALID_HANDLE_VALUE);
    _ioReady = 0;
    _beginWait = 0;
}


void SerialDeviceImpl::attach(SelectorBase& mon)
{ 
}


void SerialDeviceImpl::detach(SelectorBase& mon)
{ 
}


bool SerialDeviceImpl::wait(unsigned int msecs)
{
    return false;
}


bool SerialDeviceImpl::setWaitHandle(HANDLE h, bool& avail)
{
    return false;
}


bool SerialDeviceImpl::getWaitHandles(HandleMap& handles)
{ 
    handles.add(_ioReady, &_device);
    return true; 
}


bool SerialDeviceImpl::checkEvent()
{
    bool avail = false;

    if( _wlen || (_event & EV_TXEMPTY) )
    {
        _device.outputReady.send( _device );
        avail = true;
    }
    
    if( _rlen || (_event & EV_RXCHAR) )
    {
        _device.inputReady.send( _device );
        avail = true;
    }

    return avail;
}


void SerialDeviceImpl::run()
{ 
    while( ! _terminateThread )
    {   
        WaitForSingleObject(_beginWait, INFINITE);

        if(_terminateThread)
            return;

        // When reading and the kernel buffer is already full, WaitForCommEvent 
        // might block forever because the event type is EV_RXCHAR .
        // Therefore we check for data first before waiting for comm events
        if(_device._rbuf)
        {
            _rlen = 0;
            if( ! ReadFile( handle(), _device._rbuf, _device._rbuflen, &_rlen, 0 ) )
            {
                //TODO: Handle com errors
                if( ! _terminateThread )
                    std::cerr << "ReadFile failed" << std::endl;
            }
            if(_rlen)
            {
                SetEvent(_ioReady);
                continue;
            }
        }

        // NOTE: WaitCommEvent can be interrupted by calling SetCommMask
        // from another thread
        _event = 0;
        BOOL ret = WaitCommEvent(handle(), &_event, NULL); 

        //TODO: Handle com errors
        DWORD error = 0;
        COMSTAT cs; 
        ClearCommError( handle(), &error, &cs );
        DWORD err = GetLastError();
        
        if( ret == TRUE && (_event & (EV_TXEMPTY|EV_RXCHAR)) )
        {         
            SetEvent( _ioReady );
        }        
        else
        {
            // TODO: Handling for unexpected events
            DebugBreak();
        }
    }
}


size_t SerialDeviceImpl::beginRead(char* buffer, size_t n, bool& eof) 
{
    _rlen = 0;

    DWORD mask = 0;
    GetCommMask(handle(), &mask);
    SetCommMask( handle(), mask | EV_RXCHAR );

    SetEvent(_beginWait); 
	return 0;
}


size_t SerialDeviceImpl::endRead(bool& eof)
{
    DWORD len = _rlen;
    _rlen = 0;

    DWORD mask = 0;
    GetCommMask(handle(), &mask);
    SetCommMask( handle(), mask &~ EV_RXCHAR );

    ResetEvent(_ioReady); 

    // might have read data before WaitCommEvent
    if( len > 0 )
        return len;

    // no data read previously, but data is available
    if( ! ReadFile( handle(), _device._rbuf, _device._rbuflen, &len, 0 ) )
        throw IOError("ReadFile failed" , PT_SOURCEINFO);

    if( len == 0 )     
       eof = true;

    return len;
}


size_t SerialDeviceImpl::beginWrite(const char* buffer, size_t n)
{
    _wlen = this->write(buffer, n); 

    if(_wlen == 0)
    {
        DWORD mask = 0;
        GetCommMask(handle(), &mask);
        SetCommMask( handle(), mask | EV_TXEMPTY );

        SetEvent(_beginWait);
    }
    else
    {
        SetEvent(_ioReady);
    }
	
	return 0;
}


size_t SerialDeviceImpl::endWrite()
{
    DWORD len = _wlen;
    _wlen = 0;

    DWORD mask = 0;
    GetCommMask(handle(), &mask);
    SetCommMask( handle(), mask &~ EV_TXEMPTY );

    ResetEvent(_ioReady); 

    if ( len == 0 )
        len = this->write(_device._wbuf, _device._wbuflen);

    return len;
}   


size_t SerialDeviceImpl::read( char* buffer, size_t count, bool& eof )
{
    DWORD length = 0;

    if( ! ReadFile( handle(), buffer, count, &length, 0 ) )
        throw IOError("Read port failed" , PT_SOURCEINFO);

    if( length == 0 )     
       eof = true;

    return length;
}


size_t SerialDeviceImpl::write( const char* buffer, size_t count )
{
    DWORD length = 0;

    if( ! WriteFile( handle(), buffer, count, &length, 0 ) )
    {
        DWORD error = GetLastError();
        throw IOError("Could not write to file handle", PT_SOURCEINFO);
    }

    return length;
}


void SerialDeviceImpl::writeCommState( DCB& commState )
{
    if( ! SetCommState( handle(), &commState ) )
        throw IOError("Changing port state failed" , PT_SOURCEINFO);
}


void SerialDeviceImpl::readCommState( DCB& commState ) const
{
    if( ! GetCommState( handle(), &commState ) )
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
    FlushFileBuffers( handle() );
}


void SerialDeviceImpl::setTimeout( size_t msec )
{    
    COMMTIMEOUTS comTimeOut;
    comTimeOut.ReadIntervalTimeout          = MAXDWORD;
    comTimeOut.ReadTotalTimeoutMultiplier   = MAXDWORD;
    comTimeOut.ReadTotalTimeoutConstant     = msec;

    comTimeOut.WriteTotalTimeoutMultiplier  = 0;
    comTimeOut.WriteTotalTimeoutConstant    = msec;

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