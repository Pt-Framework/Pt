/*
 * Copyright (C) 2007 Marc Boris D�rner
 * Copyright (C) 2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2007 Bjoern Oliver Streule
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
#include "../win32/win32.h"
#include "MainLoopImpl.h"
#include "SerialDeviceImpl.h"
#include "Pt/System/Thread.h"
#include <iostream>

namespace Pt{

namespace System{

SerialDeviceImpl::SerialDeviceImpl(SerialDevice& device)
: _device(device)
, _thread(0)
, _terminateThread(false)
, _beginWait(0)
, _rlen(0)
, _wlen(0)
, _event(0)
{ 
    _beginWait = CreateEvent(NULL, FALSE, FALSE, NULL);
}


SerialDeviceImpl::~SerialDeviceImpl()
{ 
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

    size_t err = GetLastError();

    if( h == 0  || h == INVALID_HANDLE_VALUE )
        throw AccessFailed(port_);

    this->setHandle(h);

    try
    {
        if( ! GetCommState( h, &_orgCommState ) )
            throw AccessFailed("Get port state failed" );       

        COMMTIMEOUTS comTimeOut;
        comTimeOut.ReadIntervalTimeout          = MAXDWORD;
        comTimeOut.ReadTotalTimeoutMultiplier   = MAXDWORD;
        comTimeOut.ReadTotalTimeoutConstant     = MAXDWORD;
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
    // Closing the com handle will end WaitCommEvent
    CloseHandle( handle() );
    CloseHandle( _beginWait );

    this->setHandle(INVALID_HANDLE_VALUE);
    _beginWait = 0;
}


void SerialDeviceImpl::cancel(EventLoop& loop)
{
    if( handle() == 0 || handle() == INVALID_HANDLE_VALUE )
        return;
    
    // Signalise the thread to terminate
    _terminateThread = true;
    
    // Reset the wait mask, to wake up the comm event thread
    SetCommMask( handle(), 0 );

    // Wake up the thread
    SetEvent(_beginWait);

    // Wait for comm event thread termination
    _thread->join();
    delete _thread;
    _thread = 0;
}


void SerialDeviceImpl::attach(EventLoop& mon)
{ 
}


void SerialDeviceImpl::detach(EventLoop& mon)
{ 
}


bool SerialDeviceImpl::setWaitHandle(HANDLE h, bool& avail)
{
    return false;
}


void SerialDeviceImpl::getWaitHandles(HandleMap& handles, bool& avail)
{ 
    //handles.add(_ioReady, &_device);
}


bool SerialDeviceImpl::runRead(EventLoop& loop)
{


    return false;
}

bool SerialDeviceImpl::runWrite(EventLoop& loop)
{


    return false;
}


/*bool SerialDeviceImpl::checkEvent()
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
}*/


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
    if ( ! _thread)
    {
        _thread = new AttachedThread( callable(*this, &SerialDeviceImpl::run) );
        _thread->start();
    }

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

	WaitForSingleObject(_ioReady, INFINITE);
    ResetEvent(_ioReady); 

    // might have read data before WaitCommEvent
    if( len > 0 )
        return len;

    // no data read previously, but data is available
    if( ! ReadFile( handle(), _device._rbuf, _device._rbuflen, &len, 0 ) )
        throw IOError(  PT_ERROR_MSG("ReadFile failed") );

    if( len == 0 )     
       eof = true;

    return len;
}


size_t SerialDeviceImpl::beginWrite(const char* buffer, size_t n)
{
    if ( ! _thread)
    {
        _thread = new AttachedThread( callable(*this, &SerialDeviceImpl::run) );
        _thread->start();
    }

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

	WaitForSingleObject(_ioReady, INFINITE);
    ResetEvent(_ioReady); 

    if ( len == 0 )
        len = this->write(_device._wbuf, _device._wbuflen);

    return len;
}   


void SerialDeviceImpl::cancel()
{
    throw std::logic_error("SerialDeviceImpl::cancel() not implemented");
}


size_t SerialDeviceImpl::read( char* buffer, size_t count, bool& eof )
{
    DWORD length = 0;

    if( ! ReadFile( handle(), buffer, count, &length, 0 ) )
        throw IOError( PT_ERROR_MSG("Read port failed") );

    if( length == 0 )     
       eof = true;

    return length;
}


size_t SerialDeviceImpl::write( const char* buffer, size_t count )
{
    DWORD length = 0;

    if( ! WriteFile( handle(), buffer, count, &length, 0 ) )
    {
        throw IOError( PT_ERROR_MSG("Could not write to file handle") );
    }

    return length;
}


void SerialDeviceImpl::writeCommState( DCB& commState )
{
    if( ! SetCommState( handle(), &commState ) )
        throw IOError( PT_ERROR_MSG("Changing port state failed") );
}


void SerialDeviceImpl::readCommState( DCB& commState ) const
{
    if( ! GetCommState( handle(), &commState ) )
        throw IOError( PT_ERROR_MSG("Get port state failed") );
}


void SerialDeviceImpl::setBaudRate( unsigned rate )
{
    DCB commState;
    
    readCommState( commState );
    commState.BaudRate = static_cast<DWORD>( rate );
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

    throw std::runtime_error( PT_ERROR_MSG("Unknown stop bits") );
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

    throw std::runtime_error(  PT_ERROR_MSG("Invalid parity") );
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

    //Check for hardware.
    if( commState.fOutxCtsFlow == 1 && commState.fRtsControl == RTS_CONTROL_HANDSHAKE )
        return  SerialDevice::FlowControlHard;

    //Check for software.
    if( commState.fInX == commState.fOutX && commState.fInX == 1 )
       return SerialDevice::FlowControlSoft;

    throw std::runtime_error( PT_ERROR_MSG("Unknown flow control") );
    return SerialDevice::FlowControlBoth;
}

bool SerialDeviceImpl::setSignal(SerialDevice::SerialLine signal)
{
    switch(signal)
    {
        case SerialDevice::CLR_BREAK:
            return EscapeCommFunction(handle(), CLRBREAK);
        break;
        case SerialDevice::CLR_DTR:
            return EscapeCommFunction(handle(), CLRDTR);
        break;
        case SerialDevice::CLR_RTS:
            return EscapeCommFunction(handle(), CLRRTS);
        break;
        case SerialDevice::SET_BREAK:
            return EscapeCommFunction(handle(), SETBREAK);
        break;
        case SerialDevice::SET_DTR:
            return EscapeCommFunction(handle(), SETDTR);
        break;
        case SerialDevice::SET_RTS:
            return EscapeCommFunction(handle(), SETRTS);
        break;
        case SerialDevice::SET_XOFF:
            return EscapeCommFunction(handle(), SETXOFF);
        break;
        case SerialDevice::SET_XON:
            return EscapeCommFunction(handle(), SETXON);
        break;
    }

    return false;
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
