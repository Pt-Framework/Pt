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
: _device(device)
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
    if(mode & IODevice::Async)
    {
        h = CreateFile( port.c_str() , openFlags, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    }
    else
    {
        h = CreateFile( port.c_str() , openFlags, 0, NULL, OPEN_EXISTING, 0, NULL);
    }

    if( h == 0  || h == INVALID_HANDLE_VALUE )
        throw DeviceNotFound("Could not open port" , PT_SOURCEINFO);

    this->setHandle(h);

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
}


void SerialDeviceImpl::close()
{
    //Restore the port state.
    SetCommState( handle(), &_orgCommState );
    
    if(handle() != INVALID_HANDLE_VALUE)
    {
        if( FALSE == ::CloseHandle( handle() ) )
            throw IOError("Could not close file handle", PT_SOURCEINFO);

        this->setHandle(INVALID_HANDLE_VALUE);
    }
}


void SerialDeviceImpl::attach(EventLoop& loop)
{
    HANDLE h = loop.impl().beginWait(_device);

    bool active = false;
    this->setWaitHandle(h, active);

    // TODO: use this->setAvail() ?
    if(active)
        loop.impl().setAvail(_device);
}


void SerialDeviceImpl::detach(EventLoop& loop)
{
    // handle the case when we were added to a EventLoop and beginRead
    // was called with data possibly available. setWaitHandle() will
    // cancel the overlapped operation or set the active flag in which
    // case we set Avail so the next waiter knows data is available
    bool active = false;
    this->setWaitHandle(_waitHandle, active);

    if(active)
        _device.setAvail(true);

    loop.impl().endWait(_device);
}


void SerialDeviceImpl::enable(EventLoop& loop)
{
}


void SerialDeviceImpl::disable(EventLoop& loop)
{
}


bool SerialDeviceImpl::avail()
{
    bool avail = false;

    if( _device._wbuf && HasOverlappedIoCompleted(&_writeOv) )
    {
        _device.outputReady.send(_device);
        avail = true;
    }

    if( _device._rbuf && HasOverlappedIoCompleted(&_readOv) )
    {
        _device.inputReady.send(_device);
        avail = true;
    }

    return avail;
}


bool SerialDeviceImpl::wait(std::size_t msecs)
{
    if( _device.avail() )
    {
        this->checkEvent();
        return true;
    }

    if(_readOv.hEvent == NULL)
    {
        bool active = false;
        this->setWaitHandle(_waitHandle, active);
        if( active )
        {
            this->checkEvent();
            return true;
        }
    }

    DWORD result = WaitForSingleObject(_readOv.hEvent, msecs);
    
    if(result == WAIT_OBJECT_0)
    {
        this->checkEvent();
        return true;
    }

    if(result != WAIT_TIMEOUT)
        throw IOError("WAIT_FAILED on pipe", PT_SOURCEINFO);
          
    return false;
}


bool SerialDeviceImpl::setWaitHandle(HANDLE h, bool& avail)
{
    HANDLE prevHandle = _readOv.hEvent;

    if( prevHandle != h )
    {
        if(_device._rbuf && _readOv.hEvent)
            HasOverlappedIoCompleted(&_readOv) ? avail = true
                                               : CancelIo( handle() );

        if(_device._wbuf && _writeOv.hEvent)
            HasOverlappedIoCompleted(&_writeOv)  ? avail = true
                                                 : CancelIo( handle() );

        _readOv.hEvent = h;
        _writeOv.hEvent = h;
    }
    
    if( ! prevHandle && _device._rbuf )
    {
        bool eof = false;
        size_t n = this->beginRead(_device._rbuf, _device._rbuflen, eof);
        if(eof || n > 0)
            avail = true;

        _device.setEof(eof);
    }

    if( ! prevHandle && _device._wbuf)
    {
        size_t n = this->beginWrite(_device._wbuf, _device._wbuflen);
        if(n > 0)
            avail = true;
    }
 
    return true;
}
        

bool SerialDeviceImpl::checkEvent()
{
    bool avail = false;

    if( _device._wbuf && HasOverlappedIoCompleted(&_writeOv) )
    {
        _device.outputReady.send( _device );
        avail = true;
    }
    
    if( _device._rbuf && HasOverlappedIoCompleted(&_readOv) )
    {
        _device.inputReady.send( _device );
        avail = true;
    }

    return avail;
}


void SerialDeviceImpl::cancel()
{	
    ::CancelIo( handle() );
	::PurgeComm(handle(), PURGE_RXABORT | PURGE_TXABORT| PURGE_TXCLEAR | PURGE_RXCLEAR);
	/*
    DWORD bytes = 0;

    if( _device.reading() && ! HasOverlappedIoCompleted(&_readOv) )
    {
        GetOverlappedResult( handle(), &_readOv, &bytes, TRUE );
    }

    if( _device.writing() && ! HasOverlappedIoCompleted(&_writeOv) )
    {
        GetOverlappedResult( handle(), &_writeOv, &bytes, TRUE );
    }*/
}


size_t SerialDeviceImpl::beginRead(char* buffer, size_t n, bool& eof)
{
    if(_readOv.hEvent == NULL)
        return 0;

    DWORD readBytes = 0;
    if( FALSE == ReadFile(handle(), (void*)buffer, n, &readBytes, &_readOv) )
    {
        DWORD err = GetLastError();
        if( ERROR_HANDLE_EOF == err || ERROR_BROKEN_PIPE == err )
        {
            eof = true;
            return 0;
        }
        else if( err == ERROR_IO_PENDING )
        {
            return 0;
        }
        
        throw IOError("Could not begin read from file handle", PT_SOURCEINFO);
    }
    
    return readBytes;
}


size_t SerialDeviceImpl::endRead(bool& eof)
{
    if( _device.eof() )
    { 
        eof = true;
        return 0;
    }

    DWORD readBytes = 0;
    if( FALSE == GetOverlappedResult(handle(), &_readOv, &readBytes, TRUE) )
    {
        DWORD err = GetLastError();
        if( ERROR_BROKEN_PIPE == err || ERROR_BROKEN_PIPE == err )
        {
            eof = true;
        }
        else
        {
            throw IOError("Could not end read from file handle", PT_SOURCEINFO);
        }
    }

    _readOv.Offset += readBytes;
    _writeOv.Offset += readBytes;
    return readBytes;
}


size_t SerialDeviceImpl::beginWrite(const char* buffer, size_t n)
{
    if(_writeOv.hEvent == NULL)
        return 0;

    DWORD writtenBytes = 0;
    if( FALSE == WriteFile(handle(), (void*)buffer, n, &writtenBytes, &_writeOv) )
    {
        DWORD err = GetLastError();
        if( ERROR_IO_PENDING == err )
        {
            return 0;
        }
        
        throw IOError("Could not read from file handle", PT_SOURCEINFO);
    }

    return writtenBytes;
}


size_t SerialDeviceImpl::endWrite()
{
    if(_writeOv.hEvent == NULL)
    {
        return this->write(_device._wbuf, _device._wbuflen);
    }

    DWORD writtenBytes = 0;
    if (GetOverlappedResult( handle(), &_writeOv, &writtenBytes, FALSE) == FALSE )
    {
        throw IOError("GetOverlappedResult failed", PT_SOURCEINFO);
    }

    _writeOv.Offset += writtenBytes;
    return writtenBytes;
}


size_t SerialDeviceImpl::read( char* buffer, size_t count, bool& eof )
{
    eof = false;
    DWORD readBytes = 0;

    if( FALSE == ReadFile(handle(), (void*)buffer, count, &readBytes, &_readOv) )
    {
        if( ERROR_HANDLE_EOF == GetLastError() || 
            ERROR_BROKEN_PIPE == GetLastError() )
        {
            eof = true;
            readBytes = 0;
        }
        else if( ERROR_IO_PENDING == GetLastError() )
        {
            if(FALSE == GetOverlappedResult(handle(), &_readOv, &readBytes, TRUE) )
            {
                throw IOError("Could not read from file handle", PT_SOURCEINFO);
            }
        }
        else
        {
            throw IOError("Could not read from file handle", PT_SOURCEINFO);
        }
    }

    _readOv.Offset += readBytes;
    _writeOv.Offset += readBytes;
    return readBytes;
}


size_t SerialDeviceImpl::write( const char* buffer, size_t count )
{
    DWORD writtenBytes = 0;

    if( FALSE == WriteFile(handle(), (void*)buffer, count, &writtenBytes, NULL) )
    {
        if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IOError("Could not write to file handle", PT_SOURCEINFO);
        }
        if(GetOverlappedResult(handle(), &_readOv, &writtenBytes, FALSE) == FALSE )
        {
            writtenBytes = 0;
        }
    }

    _readOv.Offset += writtenBytes;
    _writeOv.Offset += writtenBytes;
    return writtenBytes;
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


void SerialDeviceImpl::flush()
{
    FlushFileBuffers( handle() );
}

}//namespace System

}//namespace Pt
