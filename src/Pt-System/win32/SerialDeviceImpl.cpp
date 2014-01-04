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

#ifdef _WIN32_WCE

SerialDeviceImpl::SerialDeviceImpl(SerialDevice& device)
: _device(device)
, _thread(0)
, _terminateThread(false)
, _beginWait(0)
, _event(0)
{ 
    _beginWait = CreateEvent(NULL, FALSE, FALSE, NULL);
}


SerialDeviceImpl::~SerialDeviceImpl()
{ 
    CloseHandle( _beginWait );
}


void SerialDeviceImpl::open( const std::string& port_, std::ios::openmode mode)
{
    std::basic_string<TCHAR> port;
    win32::fromMultiByte( port_, port );

    DWORD openFlags = 0;
    if( mode & std::ios::out )
        openFlags |= GENERIC_WRITE;

    if( mode & std::ios::in )
        openFlags |= GENERIC_READ;

    HANDLE h = CreateFile( port.c_str() , openFlags, 0, NULL, OPEN_EXISTING, 0, NULL);

    std::size_t err = GetLastError();

    if( h == 0  || h == INVALID_HANDLE_VALUE )
        throw AccessFailed(port_);

    this->setHandle(h);

    try
    {
        if( ! GetCommState( h, &_orgCommState ) )
            throw AccessFailed("Get port state failed" );       

        // ReadFile and WriteFile are non-blocking
        COMMTIMEOUTS comTimeOut;
        comTimeOut.ReadIntervalTimeout          = MAXDWORD;
        comTimeOut.ReadTotalTimeoutMultiplier   = MAXDWORD;
        comTimeOut.ReadTotalTimeoutConstant     = 1;
        comTimeOut.WriteTotalTimeoutMultiplier  = 0;
        comTimeOut.WriteTotalTimeoutConstant    = 1;

        if( ! SetCommTimeouts( h, &comTimeOut ) )
            throw AccessFailed("Set port time outs failed");

        _terminateThread = false;
    }
    catch( ... )
    {
        CloseHandle(h);
        h = 0;
        throw;
    }
}

void SerialDeviceImpl::close()
{
    assert(0 == _thread);
    CloseHandle( handle() );
    this->setHandle(INVALID_HANDLE_VALUE);
}


void SerialDeviceImpl::cancel(EventLoop& loop)
{
    if( handle() == 0 || handle() == INVALID_HANDLE_VALUE )
        return;
    
    // Signalise the thread to terminate
    _terminateThread = true;
    
    // Reset the wait mask, to wake up WaitCommEvent
    SetCommMask( handle(), 0 );

    // Wake up the thread
    SetEvent(_beginWait);

    // Wait for comm event thread termination
    _thread->join();
    delete _thread;
    _thread = 0;

    _event = 0;
}


bool SerialDeviceImpl::runRead(EventLoop& loop)
{
    return (_event & EV_RXCHAR) == EV_RXCHAR;
}


bool SerialDeviceImpl::runWrite(EventLoop& loop)
{
    return (_event & EV_TXEMPTY) == EV_TXEMPTY;
}


void SerialDeviceImpl::run()
{ 
    while( ! _terminateThread )
    {   
        WaitForSingleObject(_beginWait, INFINITE);

        if(_terminateThread)
            return;

        // NOTE: WaitCommEvent can be interrupted by calling SetCommMask
        // from another thread
        _event = 0;
        BOOL ret = WaitCommEvent(handle(), &_event, NULL); 

        //TODO: Handle comm errors
        DWORD error = 0;
        COMSTAT cs; 
        ClearCommError( handle(), &error, &cs );
        DWORD err = GetLastError();
        
        if( ret == TRUE && (_event & (EV_TXEMPTY|EV_RXCHAR)) )
        {         
            _device.loop()->setReady(_device);
        }        
        else
        {
            // TODO: Handling for unexpected events
            DebugBreak();
        }
    }
}


std::size_t SerialDeviceImpl::beginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof) 
{
    DWORD len = 0;
    if( ! ReadFile( handle(), buffer, n, &len, 0 ) )
    {
        throw IOError("ReadFile");
    }

    if( len > 0 )
        return len;

    DWORD mask = 0;
    GetCommMask( handle(), &mask );
    SetCommMask( handle(), mask | EV_RXCHAR );

    if ( ! _thread)
    {
        _thread = new AttachedThread( callable(*this, &SerialDeviceImpl::run) );
        _thread->start();
    }

    SetEvent(_beginWait); 
    return 0;
}


std::size_t SerialDeviceImpl::endRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    DWORD mask = 0;
    GetCommMask(handle(), &mask);
    SetCommMask(handle(), mask &~ EV_RXCHAR);
    _event &= ~EV_RXCHAR;

    return read(buffer, n, eof);
}


std::size_t SerialDeviceImpl::read( char* buffer, std::size_t count, bool& eof )
{
    DWORD length = 0;

    if( ! ReadFile( handle(), buffer, count, &length, 0 ) )
        throw IOError( PT_ERROR_MSG("Read port failed") );

    if(length > 0)
        return length;

    DWORD mask = 0;
    GetCommMask( handle(), &mask );
    SetCommMask( handle(), mask | EV_RXCHAR );

    DWORD event = 0;
    WaitCommEvent(handle(), &event, NULL); 

    SetCommMask( handle(), mask &~ EV_RXCHAR );

    if( ! ReadFile( handle(), buffer, count, &length, 0 ) )
        throw IOError( PT_ERROR_MSG("Read port failed") );

    if( length == 0 )     
       eof = true;

    return length;
}


std::size_t SerialDeviceImpl::beginWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    DWORD len = 0;
    if( ! WriteFile( handle(), buffer, n, &len, 0 ) )
    {
        throw IOError( PT_ERROR_MSG("Could not write to file handle") );
    }

    if(len > 0)
        return len;

    DWORD mask = 0;
    GetCommMask(handle(), &mask);
    SetCommMask(handle(), mask | EV_TXEMPTY);

    if ( ! _thread)
    {
        _thread = new AttachedThread( callable(*this, &SerialDeviceImpl::run) );
        _thread->start();
    }

    SetEvent(_beginWait);

    return 0;
}


std::size_t SerialDeviceImpl::endWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    DWORD mask = 0;
    GetCommMask(handle(), &mask);
    SetCommMask( handle(), mask &~ EV_TXEMPTY );
    _event &= ~EV_TXEMPTY;

    return write(buffer, n);
}   


std::size_t SerialDeviceImpl::write( const char* buffer, std::size_t count )
{
    DWORD length = 0;

    if( ! WriteFile( handle(), buffer, count, &length, 0 ) )
    {
        throw IOError( PT_ERROR_MSG("Could not write to file handle") );
    }

    if(length > 0)
        return length;

    DWORD mask = 0;
    GetCommMask(handle(), &mask);
    SetCommMask(handle(), mask | EV_TXEMPTY);

    DWORD event = 0;
    WaitCommEvent(handle(), &event, NULL); 

    SetCommMask( handle(), mask &~ EV_TXEMPTY );

    if( ! WriteFile( handle(), buffer, count, &length, 0 ) )
    {
        throw IOError( PT_ERROR_MSG("Could not write to file handle") );
    }

    return length;
}


void SerialDeviceImpl::setTimeout( std::size_t msec )
{
    COMMTIMEOUTS comTimeOut;
    comTimeOut.ReadIntervalTimeout          = MAXDWORD;
    comTimeOut.ReadTotalTimeoutMultiplier   = MAXDWORD;
    comTimeOut.ReadTotalTimeoutConstant     = msec;

    comTimeOut.WriteTotalTimeoutMultiplier  = 0;
    comTimeOut.WriteTotalTimeoutConstant    = msec;
    if( !SetCommTimeouts( handle(), &comTimeOut ) )
        throw IOError("SetCommTimeouts");
}


std::size_t SerialDeviceImpl::timeout() const
{
    COMMTIMEOUTS comTimeOut;
    GetCommTimeouts( handle(), &comTimeOut );
    return  comTimeOut.ReadTotalTimeoutConstant;
}

#else // normal WIN32

SerialDeviceImpl::SerialDeviceImpl(SerialDevice& device)
: OverlappedIODeviceImpl(device)
, _device(device)
, _waitHandle(INVALID_HANDLE_VALUE)
{
    _waitHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
    if( _waitHandle == NULL )
        throw SystemError("CreateEvent");

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



void SerialDeviceImpl::open( const std::string& port_, std::ios::openmode mode)
{
    std::basic_string<TCHAR> port;
    win32::fromMultiByte( port_.c_str(), port );

    DWORD openFlags = 0;

    if( mode & std::ios::out )
        openFlags |= GENERIC_WRITE;

    if( mode & std::ios::in )
        openFlags |= GENERIC_READ;

    HANDLE h = INVALID_HANDLE_VALUE;

    h = CreateFile( port.c_str() , openFlags, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

    if( h == 0  || h == INVALID_HANDLE_VALUE )
        throw AccessFailed(port_);

    try
    {
        if( ! GetCommState( h, &_orgCommState ) )
            throw IOError("GetCommState");

        // Do not use timeouts, return read data immediately.
        COMMTIMEOUTS comTimeOut;
        comTimeOut.ReadIntervalTimeout          = 0;
        comTimeOut.ReadTotalTimeoutMultiplier   = 0;
        comTimeOut.ReadTotalTimeoutConstant     = 100;
        comTimeOut.WriteTotalTimeoutMultiplier  = 0;
        comTimeOut.WriteTotalTimeoutConstant    = 100;

        if( !SetCommTimeouts( h, &comTimeOut ) )
            throw IOError("SetCommTimeouts");

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

#endif // normal WIN32

void SerialDeviceImpl::writeCommState( DCB& commState )
{
    if( ! SetCommState( handle(), &commState ) )
        throw IOError("SetCommStated");
}


void SerialDeviceImpl::readCommState( DCB& commState ) const
{
    if( ! GetCommState( handle(), &commState ) )
        throw IOError("GetCommState");
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

bool SerialDeviceImpl::setSignal(SerialDevice::Signal signal)
{
    switch(signal)
    {
        case SerialDevice::ClearBreak:
            return EscapeCommFunction(handle(), CLRBREAK) > 0;
        break;
        case SerialDevice::ClearDtr:
            return EscapeCommFunction(handle(), CLRDTR) > 0;
        break;
        case SerialDevice::ClearRts:
            return EscapeCommFunction(handle(), CLRRTS) > 0;
        break;
        case SerialDevice::SetBreak:
            return EscapeCommFunction(handle(), SETBREAK) > 0;
        break;
        case SerialDevice::SetDtr:
            return EscapeCommFunction(handle(), SETDTR) > 0;
        break;
        case SerialDevice::SetRts:
            return EscapeCommFunction(handle(), SETRTS) > 0;
        break;
        case SerialDevice::SetXOff:
            return EscapeCommFunction(handle(), SETXOFF) > 0;
        break;
        case SerialDevice::SetXOn:
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

}//namespace System

}//namespace Pt
