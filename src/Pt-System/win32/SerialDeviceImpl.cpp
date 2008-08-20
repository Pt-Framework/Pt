/***************************************************************************
 *   Copyright (C) 2007 Marc Boris D�rner                                  *
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
#include "Pt/System/IODevice.h"
#include "Pt/System/Selector.h"
#include "Pt/System/SystemError.h"
#include <iostream>

namespace Pt {

namespace System {

SerialDeviceImpl::SerialDeviceImpl()
: _waitHandle(INVALID_HANDLE_VALUE)
, _rbuf(0)
, _rbuflen(0)
, _wbuf(0)
, _wbuflen(0)
{
    _waitHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
    if( _waitHandle == NULL )
        throw SystemError("CreateEvent failed", PT_SOURCEINFO);

    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;
    _readOv.hEvent = NULL;
    _readOv.hEvent  = NULL;

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
    _writeOv.hEvent = NULL;
    _writeOv.hEvent = NULL;
}


SerialDeviceImpl::~SerialDeviceImpl()
{
    ::CloseHandle(_waitHandle);
}


void SerialDeviceImpl::open( const std::string& port_, std::ios_base::openmode mode, bool isAsync )
{
    std::basic_string<TCHAR> port = win32::fromMultiByte( port_.c_str() );

    DWORD openFlags = 0;

    if( mode & std::ios_base::out )
        openFlags |= GENERIC_WRITE;

    if( mode & std::ios_base::in )
        openFlags |= GENERIC_READ;

    HANDLE h = INVALID_HANDLE_VALUE;
    if(isAsync)
    {
        h = CreateFile( port.c_str() , openFlags, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    }
    else
    {
        h = CreateFile( port.c_str() , openFlags, 0, NULL, OPEN_EXISTING, 0, NULL);
    }


    if( h == 0  || h == INVALID_HANDLE_VALUE )
        throw OpenFailed("Could not open port" , PT_SOURCEINFO);

    this->setHandle(h);

    try
    {
        if( ! GetCommState( h, &_orgCommState ) )
            throw OpenFailed("Get port state failed" , PT_SOURCEINFO);

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


void SerialDeviceImpl::attach(SelectorBase& s)
{
}


void SerialDeviceImpl::detach(SelectorBase& s)
{
    this->setWaitHandle(_waitHandle, 0);
}


bool SerialDeviceImpl::wait(unsigned int msecs)
{
    HANDLE prevHandle = NULL;

    if(_readOv.hEvent != _waitHandle)
    {
        prevHandle = _readOv.hEvent;
        this->setWaitHandle(_waitHandle, 0);
    }

    DWORD result = WaitForSingleObject(_waitHandle, msecs);

    if(prevHandle)
         _readOv.hEvent = prevHandle;
    
    if(result == WAIT_FAILED)
        throw IOError("WAIT_FAILED on pipe", PT_SOURCEINFO);
    
    if(result == WAIT_TIMEOUT)
        return false;

    if(result == WAIT_OBJECT_0)
    {
        this->checkEvent();
        return true;
    }
        
    throw IOError("Unknown return from WaitForSingleObject", PT_SOURCEINFO);
    return false;
}


bool SerialDeviceImpl::setWaitHandle(HANDLE h, std::set<Selectable*>* actives)
{
    if(_rbuf)
    {
        if( _readOv.hEvent && HasOverlappedIoCompleted(&_readOv) )
        {
            _readOv.hEvent = h;
            SetEvent(h);
            return true;
        }
 
        if(_readOv.hEvent != h)
        {
            if(_readOv.hEvent)
                CancelIo(handle());

            _readOv.hEvent = h;
        }

        DWORD readBytes = 0;
        if( FALSE == ReadFile( handle(), (void*)_rbuf, _rbuflen, &readBytes, &_readOv) )
        {
            if( ERROR_HANDLE_EOF == GetLastError() )
            {
            }
            else if( ERROR_IO_PENDING != GetLastError() )
            {
                throw IOError("Could not read from file handle", PT_SOURCEINFO);
            }
        }
    }

    _readOv.hEvent = h;

    if(_wbuf)
    {
        if( _writeOv.hEvent && HasOverlappedIoCompleted(&_writeOv) )
        {
            _writeOv.hEvent = h;
            SetEvent(h);
            return true;
        }

        if(_writeOv.hEvent != h)
        {
            if(_writeOv.hEvent)
                CancelIo(handle());
                
            _writeOv.hEvent = h;
        }

        DWORD writtenBytes = 0;
        if( FALSE == WriteFile(handle(), (void*)_wbuf, _wbuflen, &writtenBytes, &_writeOv) )
        {
            DWORD err = GetLastError();
            if( ERROR_IO_PENDING != err )
            {
                throw IOError("Could not read from file handle", PT_SOURCEINFO);
            }
        }
    }

    _writeOv.hEvent = h;

    return true;
}
        

bool SerialDeviceImpl::checkEvent()
{
    bool avail = false;

    if( _wbuf && HasOverlappedIoCompleted(&_writeOv) )
    {
        this->parent().outputReady.send( this->parent() );
        avail = true;
    }
    
    if( _rbuf && HasOverlappedIoCompleted(&_readOv) )
    {
        this->parent().inputReady.send( this->parent() );
        avail = true;
    }

    return avail;
}


void SerialDeviceImpl::beginRead(char* buffer, size_t n, bool& eof)
{
    _rbuf = buffer;
    _rbuflen = n;
        
    if(_readOv.hEvent != NULL)
    {
        DWORD readBytes = 0;
        if( FALSE == ReadFile(handle(), (void*)_rbuf, _rbuflen, &readBytes, &_readOv) )
        {
            if( ERROR_HANDLE_EOF == GetLastError() )
            {
            }
            else if( ERROR_IO_PENDING != GetLastError() )
            {
                throw IOError("Could not read from file handle", PT_SOURCEINFO);
            }
        }
    }
}


size_t SerialDeviceImpl::endRead(bool& eof)
{
    _rbuf = 0;
    _rbuflen = 0;
    DWORD readBytes = 0;

    if (GetOverlappedResult(handle(), &_readOv, &readBytes, FALSE) == FALSE )
    {
        DWORD err = GetLastError();
        if( ERROR_HANDLE_EOF == err )
        {
            eof = true;
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


void SerialDeviceImpl::beginWrite(const char* buffer, size_t n)
{
    _wbuf = buffer;
    _wbuflen = n;

    if(_writeOv.hEvent != NULL)
    {
        DWORD writtenBytes = 0;
        if( FALSE == WriteFile(handle(), (void*)buffer, n, &writtenBytes, &_writeOv) )
        {
            DWORD err = GetLastError();
            if( ERROR_IO_PENDING != err )
            {
                throw IOError("Could not read from file handle", PT_SOURCEINFO);
            }
        }
    }
}


size_t SerialDeviceImpl::endWrite()
{	
    _wbuf = 0;
    _wbuflen = 0;
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
    DWORD length;

    if( ReadFile( handle(), buffer, count, &length, &_readOv ) )
        return length;

    if( ERROR_HANDLE_EOF == GetLastError() )
    {
        eof = true;
        length = 0;
    }
    else if( ERROR_IO_PENDING == GetLastError() )
    {
        if( S_OK != GetOverlappedResult(handle(), &_readOv, &length, FALSE) )
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

    if( WriteFile( handle(),  buffer,  count, &length, &_writeOv ) )
        return length;

    if( ERROR_IO_PENDING == GetLastError() )
    {
        if( S_OK != GetOverlappedResult(handle(), &_writeOv, &length, FALSE) )
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
    if( ! SetCommState( handle(), &commState ) )
        throw IOError( "Changing port state failed" , PT_SOURCEINFO );
}


void SerialDeviceImpl::readCommState( DCB& commState ) const
{
    if( ! GetCommState( handle(), &commState ) )
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
