/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Bjoern Oliver Streule                         *
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
#include "PipeImpl.h"
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
    IODeviceImpl::setParent(*this);
    _internalBufferWaitHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
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
}


bool PipeIODevice::setWaitHandle(HANDLE h, HANDLE finished)
{
    return false;
}


bool PipeIODevice::getWaitHandles(HandleMap& handles)
{ 
    handles.add(handle(), this);
    handles.add(_internalBufferWaitHandle, this);
    return true; 
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


bool PipeIODevice::onWait(unsigned int msecs)
{
    if(_bufferSize)
    {
        return true;
    }

    DWORD result = WaitForSingleObject(handle(), msecs);
    
    if(result == WAIT_FAILED)
        throw IOError("WaitForSingleObject failed", PT_SOURCEINFO);
    
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


void PipeIODevice::onBeginRead(char* buffer, size_t n, bool& eof)
{
    if( Read != _mode )
        throw IOError("Could not read from write only pipe", PT_SOURCEINFO);
        
    if(_bufferSize)
    {
        SetEvent(_internalBufferWaitHandle);
    }
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
        throw IOError("Could not read from message queue handle", PT_SOURCEINFO);
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


void PipeIODevice::onBeginWrite(const char* buffer, size_t n)
{
    if( Write != _mode )
    {
        throw IOError("Could not write on a read only pipe", PT_SOURCEINFO);
    }
}


size_t PipeIODevice::onEndWrite()
{
    DWORD bytesToWrite = std::min<DWORD>(_wbuflen, _msgSize);

    if ( FALSE == WriteMsgQueue(handle(), (LPVOID) _wbuf, bytesToWrite, 0, 0))
    {
        std::ostringstream errorMsg;
        errorMsg << "system error code: " << GetLastError() << std::endl;
        throw IOError("WriteMsgQueue failed, " + errorMsg.str(), PT_SOURCEINFO);
    }
    
    return bytesToWrite;
}


void PipeIODevice::onClose()
{
    if(handle() != INVALID_HANDLE_VALUE)
    {
        if( FALSE == ::CloseMsgQueue(handle()) )
        {
            std::ostringstream errorMsg;
            errorMsg << "system error code: " << GetLastError() << std::endl;
            throw IOError( "Could not close file handle, " + errorMsg.str(), PT_SOURCEINFO);
        }

        this->setHandle(INVALID_HANDLE_VALUE);
    }
}


size_t PipeIODevice::onRead(char* buffer, size_t count, bool& eof)
{
    if( Read != _mode ) {
        throw IOError("Could not read from write only pipe", PT_SOURCEINFO);
    }

    DWORD readBytes = 0;
    DWORD flags     = 0;
    eof = false;

    if (_bufferSize) 
    {
        readBytes = _bufferSize;
    }
    else if ( FALSE == ReadMsgQueue(handle(), &_buffer[0], _msgSize, &readBytes, INFINITE, &flags) ) 
    {
        throw IOError("Could not read from message queue handle", PT_SOURCEINFO);
    }

    std::cerr << "onRead " << readBytes<< std::endl;
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
    if( Write != _mode ) {
        throw IOError("Could not write on a read only pipe", PT_SOURCEINFO);
    }

    if ( TRUE == WriteMsgQueue(handle(), (LPVOID) buffer, count, 0, 0)) {
        return;
    }

    WaitForSingleObject(handle(), INFINITE);

    throw std::logic_error("WriteMsgQueue failed" + PT_SOURCEINFO);
}


size_t PipeIODevice::onWrite(const char* buffer, size_t count)
{
    size_t offset = 0;
    for (int n = count; ; n -= _msgSize )
    {
        if (n <= (int) _msgSize)
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




PipeImpl::PipeImpl(bool isAsync)
: _inputDevice(PipeIODevice::Read)
, _outputDevice(PipeIODevice::Write)
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
    if (outputHandle == INVALID_HANDLE_VALUE) {
        throw IOError("Could not create message queue handle", PT_SOURCEINFO);
    }

    HANDLE inputHandle  = OpenMsgQueue(::GetCurrentProcess(), outputHandle, &readOpts);
    if (inputHandle == INVALID_HANDLE_VALUE) {
        throw IOError("Could not open message queue handle", PT_SOURCEINFO);
    }

    _inputDevice.open(inputHandle, isAsync);
    _outputDevice.open(outputHandle, isAsync);
}


PipeImpl::~PipeImpl()
{
}


IODevice& PipeImpl::input()
{
    return _inputDevice;
}

IODevice& PipeImpl::output()
{
    return _outputDevice;
}

} // namespace System

} // namespace Pt