/*
 * Copyright (C) 2007-2016 Marc Boris Duerner
 * Copyright (C) 2007-2016 Laurentiu-Gheorghe Crisan
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
#include "Pt/System/Logger.h"

PT_LOG_DEFINE("Pt.System.SerialDevice")

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

    if( h == INVALID_HANDLE_VALUE )
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
, _eventMask(0)
{
}


SerialDeviceImpl::~SerialDeviceImpl()
{
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

    h = CreateFile( port.c_str(), openFlags, 0, NULL, 
                    OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

    if( h == INVALID_HANDLE_VALUE )
        throw AccessFailed(port_);

    try
    {
        if( ! GetCommState(h, &_orgCommState) )
            throw IOError("GetCommState");

        // Do not use timeouts, return read data immediately.
        COMMTIMEOUTS comTimeOut;
        comTimeOut.ReadIntervalTimeout          = MAXDWORD;
        comTimeOut.ReadTotalTimeoutMultiplier   = 0;
        comTimeOut.ReadTotalTimeoutConstant     = 0;
        comTimeOut.WriteTotalTimeoutMultiplier  = 0;
        comTimeOut.WriteTotalTimeoutConstant    = 0;

        if( ! SetCommTimeouts(h, &comTimeOut) )
            throw IOError("SetCommTimeouts");

        SetCommMask(h, 0);
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


size_t SerialDeviceImpl::beginRead(EventLoop& loop, char* buffer, size_t n, bool& eof)
{
    DWORD readBytes = 0;

    if(_readOv.hEvent == NULL)
    {
        loop.selector().enableOverlapped(_ioh);
        _readOv.hEvent = _ioh.handle();
        _writeOv.hEvent = _ioh.handle();
    }

    SetCommMask(handle(), EV_RXCHAR);

    bool avail = false;
    for(;;)
    {
        BOOL ret = ReadFile(handle(), (void*)buffer, n, &readBytes, &_readOv);
        if(ret == FALSE)
        {
            DWORD err = GetLastError();
            if(ERROR_HANDLE_EOF == err || ERROR_BROKEN_PIPE == err)
            {
                eof = true;
                PT_LOG_DEBUG("read: EOF");
                return 0;
            }

            loop.selector().disableOverlapped(_ioh);
            _readOv.hEvent = NULL;
            _writeOv.hEvent = NULL;

            throw IOError("read failed");
        }

        if(readBytes > 0)
        {
            PT_LOG_DEBUG("read: " << readBytes << " bytes");
            break;
        }
        
        if(avail)
            throw IOError("invalid comm event");

        _eventMask = 0;
        ret = ::WaitCommEvent(handle(), &_eventMask, &_readOv);
        if(ret == FALSE)
        {
            DWORD err = GetLastError();
            if( ERROR_HANDLE_EOF == err || ERROR_BROKEN_PIPE == err )
            {
                eof = true;
                PT_LOG_DEBUG("wait result: EOF");
                return 0;
            }
            else if( err == ERROR_IO_PENDING )
            {
                PT_LOG_DEBUG("wait: I/O pending");
                return 0;
            }

            loop.selector().disableOverlapped(_ioh);
            _readOv.hEvent = NULL;
            _writeOv.hEvent = NULL;

            throw IOError("read failed");
        }

        if(_eventMask & EV_RXCHAR != 0)
        {
            PT_LOG_DEBUG("characters immediately available");
            avail = true;
        }
    }

    return readBytes;
}


std::size_t SerialDeviceImpl::endRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    DWORD readBytes = 0;
    if( FALSE == GetOverlappedResult(handle(), &_readOv, &readBytes, TRUE) )
    {
        DWORD err = GetLastError();
        if( ERROR_BROKEN_PIPE == err )
        {
            eof = true;
            PT_LOG_DEBUG("wait result: EOF");
        }
        else
        {
            throw IOError("read failed");
        }
    }

    if(_eventMask & EV_RXCHAR != 0)
    {
        PT_LOG_DEBUG("characters available");
    }
    else
    {
        PT_LOG_INFO("invalid event received: " << _eventMask);
    }

    readBytes = 0;
    if( FALSE == ReadFile(handle(), (void*)buffer, n, &readBytes, &_readOv) )
    {
        DWORD err = GetLastError();
        if( ERROR_HANDLE_EOF == err || ERROR_BROKEN_PIPE == err )
        {
            eof = true;
            PT_LOG_DEBUG("read: EOF");
            return 0;
        }

        loop.selector().disableOverlapped(_ioh);
        _readOv.hEvent = NULL;
        _writeOv.hEvent = NULL;

        throw IOError("read failed");
    }

    PT_LOG_DEBUG("read:" << readBytes << " bytes");
    return readBytes;
}


bool SerialDeviceImpl::runRead(EventLoop& loop)
{  
    if( HasOverlappedIoCompleted(&_readOv) )
    {
        //COMSTAT stat;
        //ClearCommError(handle(), NULL, &stat);
        //if(stat.cbInQue > 0)
        //  return true;

        //// TODO:
        //// sometimes he overlapped handle is signalled,
        //// but no data is available, so keep waiting
        //::WaitCommEvent(handle(), &_eventMask, &_readOv);
        //return false;

        return true;
    }

    return false;
}


std::size_t SerialDeviceImpl::read(char* buffer, std::size_t n, bool& eof)
{
    if( ! _ioEvent)
    {
        _ioEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
        if( _ioEvent == NULL )
            throw SystemError("CreateEvent failed");
    }

    OVERLAPPED ov;
    ov.hEvent = _ioEvent;
    ov.Offset = _readOv.Offset;
    ov.OffsetHigh = _readOv.OffsetHigh;
    
    DWORD bufsize = n > std::numeric_limits<DWORD>::max() ? std::numeric_limits<DWORD>::max()
                                                          : static_cast<DWORD>(n);

    for(;;)
    {
        DWORD readBytes = 0;
        if( FALSE == ReadFile(handle(), (void*)buffer, bufsize, &readBytes, &ov) )
        {
            DWORD err = GetLastError();
            if( ERROR_HANDLE_EOF == err || ERROR_BROKEN_PIPE == err )
            {
                eof = true;
                return 0;
            }

            throw IOError("read failed");
        }

        if(readBytes > 0)
            return readBytes;

        SetCommMask(handle(), EV_RXCHAR);
    
        _eventMask = 0;
        BOOL ret = ::WaitCommEvent(handle(), &_eventMask, &ov);
        if(ret == FALSE)
        {
            DWORD err = GetLastError();
            if( ERROR_HANDLE_EOF == err || ERROR_BROKEN_PIPE == err )
            {
                eof = true;
                return 0;
            }
            else if( err != ERROR_IO_PENDING )
            {
                throw IOError("read failed");
            }
        }

        DWORD result = WaitForSingleObject(ov.hEvent, _timeout);

        if(result != WAIT_OBJECT_0)
        {
            throw IOError("ReadFile timeout");
        }
    
        if(FALSE == GetOverlappedResult(handle(), &ov, &readBytes, TRUE) )
        {
            throw IOError("GetOverlappedResult failed");
        }
    }

    return 0;
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

void SerialDeviceImpl::setRts(bool on)
{
    DWORD flag = on ? SETRTS : CLRRTS;    
    EscapeCommFunction(handle(), flag);
}

void SerialDeviceImpl::setDtr(bool on)
{
    DWORD flag = on ? SETDTR : CLRDTR;    
    EscapeCommFunction(handle(), flag);
}

void SerialDeviceImpl::setBreak(bool on)
{
    DWORD flag = on ? SETBREAK : CLRBREAK;    
    EscapeCommFunction(handle(), flag);
}

void SerialDeviceImpl::sendBreak(int duration)
{
    duration = duration == 0 ? 300 : duration;
    setBreak(true);
    ::Sleep(duration);
    setBreak(false);
}

bool SerialDeviceImpl::isCts() const
{
    DWORD flags = 0;
    GetCommModemStatus( handle() , &flags);
    return ((flags & MS_CTS_ON) == MS_CTS_ON);
}

bool SerialDeviceImpl::isDsr() const
{
    DWORD flags = 0;
    GetCommModemStatus( handle() , &flags);
    return ((flags & MS_DSR_ON) == MS_DSR_ON);    
}


void SerialDeviceImpl::clear()
{
  ::PurgeComm(handle(), PURGE_TXCLEAR|PURGE_RXCLEAR);
}


void SerialDeviceImpl::sync() const
{
#ifdef _WIN32_WCE
  Pt::System::IODeviceImpl::sync();
#else
  OverlappedIODeviceImpl::sync();
#endif
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

        case SerialDevice::FlowControlHard:
            commState.fInX = commState.fOutX = 1;
            commState.fOutxCtsFlow = 1;
            commState.fRtsControl = RTS_CONTROL_HANDSHAKE;
        break;

        case SerialDevice::FlowControlNone:
            commState.fRtsControl = RTS_CONTROL_DISABLE;
            commState.fOutxCtsFlow = 0;
            commState.fInX = commState.fOutX = 0;
        break;
    }

    writeCommState( commState );
}


SerialDevice::FlowControl SerialDeviceImpl::flowControl() const
{
    DCB commState;

    readCommState( commState );

    //Check for hardware flow control.
    if( commState.fOutxCtsFlow == 1 && commState.fRtsControl == RTS_CONTROL_HANDSHAKE )
        return  SerialDevice::FlowControlHard;

    //Check for software flow control.
    if( commState.fInX == commState.fOutX && commState.fInX == 1 )
       return SerialDevice::FlowControlSoft;


    return SerialDevice::FlowControlNone;
}

} //namespace System

} //namespace Pt
