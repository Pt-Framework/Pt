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
#include "MainLoopImpl.h"
#include "Pt/System/EventLoop.h"
#include "Pt/System/SystemError.h"
#include <windows.h>
#include <sstream>
#include <Msgqueue.h>
#include <sstream>

namespace Pt {

namespace System {

PipeIODevice::PipeIODevice(Mode mode)
: _mode(mode)
, _msgSize(0)
, _bufferSize(0)
{
}


PipeIODevice::~PipeIODevice()
{   
    try
    {
        Selectable::close();
    }
    catch(...)
    {
    }
}


void PipeIODevice::open(HANDLE h, bool isAsync)
{
    this->setHandle(h);

    MSGQUEUEINFO info;
    info.dwSize = sizeof(MSGQUEUEINFO);

    if ( TRUE == GetMsgQueueInfo(handle(), &info) )
    {
        _msgSize = info.cbMaxMessage;
        _buffer.resize(_msgSize);
    }

    this->setEnabled(true);
    this->setAsync(isAsync);
    this->setEof(false);
}


bool PipeIODevice::setWaitHandle(HANDLE h, bool& avail)
{
    return false;
}


void PipeIODevice::getWaitHandles(HandleMap& handles, bool& avail)
{ 
    handles.add(handle(), this);
	
	if(_bufferSize > 0)
		avail = true;
}


bool PipeIODevice::checkEvent()
{
    bool avail = false;

    if( _wbuf )
    {
        outputReady.send(*this);
        avail = true;
    }
    
    if( _rbuf )
    {
        inputReady.send(*this);
        avail = true;
    }

    return avail;
}


bool PipeIODevice::onWait(std::size_t msecs)
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


size_t PipeIODevice::onBeginRead(char* buffer, size_t n, bool& eof)
{
    if( Read != _mode )
        throw IOError( PT_ERROR_MSG("Could not read from write only pipe") );
    
	if(_bufferSize)
		return std::min(_bufferSize, n);

	return 0;
}


size_t PipeIODevice::onEndRead(bool& eof)
{
    DWORD readBytes = 0;
    DWORD flags     = 0;
    eof = false;

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


size_t PipeIODevice::onBeginWrite(const char* buffer, size_t n)
{
    if( Write != _mode )
    {
        throw IOError( PT_ERROR_MSG("Could not write on a read only pipe") );
    }
	
	return 0;
}


size_t PipeIODevice::onEndWrite()
{
    DWORD bytesToWrite = std::min<DWORD>(_wbuflen, _msgSize);

    if ( FALSE == WriteMsgQueue(handle(), (LPVOID) _wbuf, bytesToWrite, 0, 0))
    {
        throw IOError( PT_ERROR_MSG("WriteMsgQueue failed") );
    }
    
    return bytesToWrite;
}


void PipeIODevice::onClose()
{
    if(handle() != INVALID_HANDLE_VALUE)
    {
        if( FALSE == ::CloseMsgQueue(handle()) )
        {
            throw IOError( "CloseMsgQueue failed", PT_SOURCEINFO );
        }

        this->setHandle(INVALID_HANDLE_VALUE);
    }
}


size_t PipeIODevice::onRead(char* buffer, size_t count, bool& eof)
{
    if( Read != _mode )
        throw IOError( PT_ERROR_MSG("Could not read from write only pipe") );

    DWORD readBytes = 0;
    DWORD flags     = 0;
    eof = false;

    if(_bufferSize) 
    {
        readBytes = _bufferSize;
    }
    else if ( FALSE == ReadMsgQueue(handle(), &_buffer[0], _msgSize, &readBytes, INFINITE, &flags) ) 
    {
        throw IOError( PT_ERROR_MSG("ReadMsgQueue failed") );
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


void PipeIODevice::writeMessage(const char* buffer, size_t count)
{
    if( FALSE == WriteMsgQueue(handle(), (LPVOID) buffer, count, INFINITE, 0) )
        throw IOError( PT_ERROR_MSG("WriteMsgQueue failed") );
}


size_t PipeIODevice::onWrite(const char* buffer, size_t count)
{
    if( Write != _mode )
        throw IOError( PT_ERROR_MSG("Could not write on a read only pipe") );

    size_t offset = 0;
    for(size_t n = count; ; n -= _msgSize )
    {
        if (n <= _msgSize)
        {
            writeMessage( (buffer + offset), n );
            break;
        }

        writeMessage( (buffer + offset), _msgSize );
        offset += _msgSize;
    }

    return count;
}


void PipeIODevice::onSync() const
{
}


void PipeIODevice::onCancel()
{
}


PipeImpl::PipeImpl(bool isAsync)
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

    _out.open(inputHandle, isAsync);
    _in.open(outputHandle, isAsync);
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

} // namespace System

} // namespace Pt
