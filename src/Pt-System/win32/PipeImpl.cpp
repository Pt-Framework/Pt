/*
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 Bjoern Oliver Streule
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
#include "PipeImpl.h"
#include "Pt/System/SystemError.h"
#include <sstream>
#include <iostream>
#include <cassert>
#include <windows.h>

#ifdef _WIN32_WCE
#include "MainLoopImpl.h"
#include "Pt/System/EventLoop.h"
#include <Msgqueue.h>
#endif

namespace Pt {

namespace System {

#ifdef _WIN32_WCE

PipeIODevice::PipeIODevice(Mode mode)
: _timeout(System::EventLoop::WaitInfinite)
, _mode(mode)
, _msgSize(0)
, _bufferSize(0)
{
    _ioh.sel = this;
}


PipeIODevice::~PipeIODevice()
{   
    try
    {
        IODevice::close();
    }
    catch(...)
    {
    }
}


void PipeIODevice::open(HANDLE h)
{
    this->setHandle(h);

    _ioh.setHandle(h);

    MSGQUEUEINFO info;
    info.dwSize = sizeof(MSGQUEUEINFO);

    if ( TRUE == GetMsgQueueInfo(handle(), &info) )
    {
        _msgSize = info.cbMaxMessage;
        _buffer.resize(_msgSize);
    }
}


void PipeIODevice::onSetTimeout(std::size_t timeout)
{
    _timeout = timeout;
}


void PipeIODevice::onClose()
{
    if(handle() != INVALID_HANDLE_VALUE)
    {
        if( FALSE == ::CloseMsgQueue(handle()) )
        {
            throw IOError( "CloseMsgQueue failed" );
        }

        this->setHandle(INVALID_HANDLE_VALUE);
        _ioh.setHandle(INVALID_HANDLE_VALUE);
    }
}


void PipeIODevice::onCancel()
{
    if( loop() )
        loop()->selector().disable(_ioh);
}


bool PipeIODevice::onRun()
{
    bool avail = false;

    if( _wbuf )
    {
        outputReady().send(*this);
        avail = true;
    }
    
    if( _rbuf )
    {
        inputReady().send(*this);
        avail = true;
    }

    return avail;
}


std::size_t PipeIODevice::onBeginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    if( Read != _mode )
        throw IOError( PT_ERROR_MSG("Could not read from write only pipe") );
    
    if(_bufferSize)
        return std::min(_bufferSize, n);

    loop.selector().enable(_ioh);
    return 0;
}


std::size_t PipeIODevice::onEndRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    loop.selector().disable(_ioh);

    DWORD readBytes = 0;
    DWORD flags     = 0;
    eof = false;

    // TODO: can we receive EOF?

    if (_bufferSize)
    {
        readBytes = _bufferSize;
    }
    else if ( FALSE == ReadMsgQueue(handle(), &_buffer[0], _msgSize, &readBytes, INFINITE, &flags) )
    {
        throw IOError( PT_ERROR_MSG("Could not read from message queue handle") );
    }

    DWORD bytesToCopy = std::min<DWORD>(_rbuflen, readBytes);

    memcpy(_rbuf, &_buffer[0], bytesToCopy);
    _bufferSize = 0;

    if (_rbuflen >= readBytes)
        return readBytes;

    std::vector<char>::iterator beginData = (_buffer.begin() + bytesToCopy);
    std::vector<char>::iterator endData   = (_buffer.begin() + readBytes);
    std::copy(beginData, endData, _buffer.begin());

    _bufferSize = (readBytes - bytesToCopy);
    return bytesToCopy;
}


std::size_t PipeIODevice::onBeginWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    if( Write != _mode )
    {
        throw IOError( PT_ERROR_MSG("Could not write on a read only pipe") );
    }
    
    loop.selector().enable(_ioh);
    return 0;
}


std::size_t PipeIODevice::onEndWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    loop.selector().disable(_ioh);

    DWORD bytesToWrite = std::min<DWORD>(_wbuflen, _msgSize);

    if ( FALSE == WriteMsgQueue(handle(), (LPVOID) _wbuf, bytesToWrite, 0, 0))
    {
        throw IOError( PT_ERROR_MSG("WriteMsgQueue failed") );
    }
    
    return bytesToWrite;
}


std::size_t PipeIODevice::onRead(char* buffer, std::size_t count, bool& eof)
{
    if( Read != _mode )
        throw IOError( PT_ERROR_MSG("Could not read from write only pipe") );

    DWORD readBytes = 0;
    DWORD flags     = 0;
    eof = false;

    DWORD timeout = _timeout == EventLoop::WaitInfinite ? INFINITE 
                                                        : static_cast<std::size_t>(_timeout);

    if(_bufferSize) 
    {
        readBytes = _bufferSize;
    }
    else if ( FALSE == ReadMsgQueue(handle(), &_buffer[0], _msgSize, &readBytes, timeout, &flags) ) 
    {
        throw IOError("ReadMsgQueue failed");
    }

    memcpy(buffer, &_buffer[0], count);
    _bufferSize = 0;

    if (count >= readBytes)
        return readBytes;

    std::vector<char>::iterator beginData = (_buffer.begin() + count);
    std::vector<char>::iterator endData   = (_buffer.begin() + readBytes);
    std::copy(beginData, endData, _buffer.begin());
    _bufferSize = (readBytes - count);

    return count;
}


void PipeIODevice::writeMessage(const char* buffer, std::size_t count)
{
    DWORD timeout = _timeout == EventLoop::WaitInfinite ? INFINITE 
                                                        : static_cast<std::size_t>(_timeout);

    if( FALSE == WriteMsgQueue(handle(), (LPVOID) buffer, count, timeout, 0) )
        throw IOError("WriteMsgQueue failed");
}


std::size_t PipeIODevice::onWrite(const char* buffer, std::size_t count)
{
    if( Write != _mode )
        throw IOError( PT_ERROR_MSG("Could not write on a read only pipe") );

    
    DWORD timeout = _timeout == EventLoop::WaitInfinite ? INFINITE 
                                                        : static_cast<std::size_t>(_timeout);

    DWORD bytesToWrite = std::min<DWORD>(count, _msgSize);
    if ( FALSE == WriteMsgQueue(handle(), (LPVOID) buffer, bytesToWrite, timeout, 0))
    {
        throw IOError("WriteMsgQueue failed");
    }

    return bytesToWrite;
}


void PipeIODevice::onSync() const
{
}


/*bool PipeIODevice::onWait(std::size_t msecs)
{
    if(_bufferSize)
    {
        return true;
    }

    DWORD result = WaitForSingleObject(handle(), msecs);

    if(result == WAIT_OBJECT_0)
    {
        this->checkEvent();
    }
    else if(result == WAIT_FAILED)
    {
        throw IOError( PT_ERROR_MSG("WaitForSingleObject failed") );
    }

    return result == WAIT_OBJECT_0;
}
*/


PipeImpl::PipeImpl()
: _out(PipeIODevice::Read)
, _in(PipeIODevice::Write)
{
    MSGQUEUEOPTIONS writeOpts, readOpts;

    memset(&writeOpts, 0, sizeof(writeOpts));
    memset(&readOpts,  0, sizeof(readOpts));

    writeOpts.dwSize          = sizeof(MSGQUEUEOPTIONS);
    writeOpts.dwFlags         = MSGQUEUE_ALLOW_BROKEN;
    writeOpts.dwMaxMessages   = 100;
    writeOpts.cbMaxMessage    = 1024;
    writeOpts.bReadAccess     = FALSE;

    readOpts = writeOpts;
    readOpts.bReadAccess     = TRUE;

    HANDLE outputHandle = CreateMsgQueue(NULL, &writeOpts);
    if (outputHandle == INVALID_HANDLE_VALUE)
        throw IOError( PT_ERROR_MSG("Could not create message queue handle") );

    HANDLE inputHandle  = OpenMsgQueue(::GetCurrentProcess(), outputHandle, &readOpts);
    if (inputHandle == INVALID_HANDLE_VALUE)
        throw IOError( PT_ERROR_MSG("Could not open message queue handle") );

    _out.open(inputHandle);
    _in.open(outputHandle);
}


PipeImpl::~PipeImpl()
{
}


IODevice& PipeImpl::out()
{
    return _out;
}

IODevice& PipeImpl::in()
{
    return _in;
}

#else  // normal WIN32

PipeIODevice::PipeIODevice()
: _impl()
{
    _impl.init(*this);
}


PipeIODevice::~PipeIODevice()
{
    try
    {
        IODevice::close();
    }
    catch(...)
    {
    }
}


void PipeIODevice::open(HANDLE handle)
{
    _impl.setHandle(handle);
}


void PipeIODevice::onSetTimeout(std::size_t timeout)
{
    _impl.setTimeout(timeout);
}


void PipeIODevice::onClose()
{
    _impl.close();
}


void PipeIODevice::onCancel()
{
    /*::CancelIoEx( handle(), &_readOv );
    ::CancelIoEx( handle(), &_writeOv );

    DWORD bytes = 0;

    if( this->reading() && ! HasOverlappedIoCompleted(&_readOv) )
    {
        GetOverlappedResult( handle(), &_readOv, &bytes, TRUE );
    }

    if( this->writing() && ! HasOverlappedIoCompleted(&_writeOv) )
    {
        GetOverlappedResult( handle(), &_writeOv, &bytes, TRUE );
    }*/

    EventLoop* loop = this->loop();
    if( loop )
        _impl.cancel( *loop );

    IODevice::onCancel();
}


bool PipeIODevice::onRun()
{
    if( this->isReading() )
    {
        if( _ravail || isEof() || _impl.runRead( *loop() ) )
        {
            inputReady().send(*this);
            return true;
        }
    }

    if( this->isWriting() )
    {
        if( _wavail || _impl.runWrite( *loop() ) )
        {
            outputReady().send(*this);
            return true;
        }
    }

    return false;
}


std::size_t PipeIODevice::onBeginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    return _impl.beginRead(loop, buffer, n, eof);
}


std::size_t PipeIODevice::onEndRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    return _impl.endRead(loop, buffer, n, eof);
}


std::size_t PipeIODevice::onRead(char* buffer, std::size_t count, bool& eof)
{
    return _impl.read(buffer, count, eof);
}


std::size_t PipeIODevice::onBeginWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    return _impl.beginWrite(loop, buffer, n);
}


std::size_t PipeIODevice::onEndWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    return _impl.endWrite(loop, buffer, n);
}


std::size_t PipeIODevice::onWrite(const char* buffer, std::size_t count)
{
    return _impl.write(buffer, count);
}


void PipeIODevice::onSync() const
{
    _impl.sync();
}


PipeImpl::PipeImpl()
{
    std::stringstream ss;
    ss<<"\\\\.\\pipe\\pt-" << GetCurrentProcessId() << '-' << _nameId;

    DWORD pflags = PIPE_ACCESS_DUPLEX;
    DWORD access = GENERIC_WRITE;
    DWORD share  = 0;
    DWORD create = OPEN_EXISTING;
    DWORD flags  = 0;
    
    flags  = FILE_FLAG_OVERLAPPED;
    pflags |= FILE_FLAG_OVERLAPPED;

    HANDLE inputHandle = ::CreateNamedPipe(ss.str().c_str(),
                                           pflags,
                                           PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                                           1,
                                           256,
                                           256,
                                           1000,
                                           NULL );
    if (inputHandle == INVALID_HANDLE_VALUE)
        throw SystemError("Could not create named pipe");

    HANDLE outputHandle = ::CreateFile(ss.str().c_str(), access, share, NULL, create, flags, NULL);
    if(outputHandle == INVALID_HANDLE_VALUE)
        throw SystemError("Could not open file handle");

    _out.open(inputHandle);
    _in.open(outputHandle);

    InterlockedIncrement(&_nameId);
}

PipeImpl::~PipeImpl()
{
    _nameId--;
}


PipeIODevice& PipeImpl::out()
{
    return _out;
}


PipeIODevice& PipeImpl::in()
{
    return _in;
}


LONG PipeImpl::_nameId = 0;

#endif

} // namespace System

} // namespace Pt




/*bool PipeIODevice::setWaitHandle(HANDLE h, bool& avail)
{
    // set avail to true if data is immediately available. This will
    // let the EventLoop check the other Selectables with a timeout of
    // 0 and call checkEvent on this object

    // the previous handle might be this objects event handle
    // or the one assigned by the EventLoop
    HANDLE prevHandle = _readOv.hEvent;

    // if the handle changes, we need to stop any previous I/O operation
    // unless it has already completed. _rbuf is set by an IODevice when
    // beginRead has been called. The event handle is NULL when this
    // method is called for the first time. If either one is not set, then
    // there were no I/O operations scheduled before.
    if( prevHandle != h )
    {
        if(_rbuf && _readOv.hEvent)
            HasOverlappedIoCompleted(&_readOv) ? avail = true
                                               : CancelIo( handle() );

        if(_wbuf && _writeOv.hEvent)
            HasOverlappedIoCompleted(&_writeOv)  ? avail = true
                                                 : CancelIo( handle() );

        _readOv.hEvent = h;
        _writeOv.hEvent = h;
    }

    // If _rbuf is set by IODevice::beginRead but the previous event used in
    // the overlapped structs is NULL, IODevice::beginRead was called before
    // the IODevice was added to a EventLoop or IODevice::wait was called for
    // the first time.
    if( ! prevHandle && _rbuf )
    {
        bool eof = false;
        std::size_t n = this->onBeginRead(_rbuf, _rbuflen, eof);
        if(eof || n > 0)
            avail = true;

        this->setEof(eof);
    }

    // see above. onBeginWrite could not do anything when it was called
    if( ! prevHandle && _wbuf)
    {
        std::size_t n = this->onBeginWrite(_wbuf, _wbuflen);
        if(n > 0)
            avail = true;
    }

    // we accept the HANDLE h
    return true;
}*/


/*bool PipeIODevice::checkEvent()
{
    bool avail = false;

    if( _wbuf && HasOverlappedIoCompleted(&_writeOv) )
    {
        outputReady.send(*this);
        avail = true;
    }

    if( _rbuf && HasOverlappedIoCompleted(&_readOv) )
    {
        inputReady.send(*this);
        avail = true;
    }

    return avail;
}*/


/*bool PipeIODevice::onWait(std::size_t msecs)
{
    if( this->avail() )
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
        throw IOError( PT_ERROR_MSG("WAIT_FAILED on pipe") );

    return false;
}*/

