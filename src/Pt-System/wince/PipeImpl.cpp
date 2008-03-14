/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Bjoern Oliver Streule                         *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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
: _handle(INVALID_HANDLE_VALUE)
, _mode(mode)
, _msgSize(0)
, _isWaitable(true)
, _bufferSize(0)
{
    _internalBufferWaitHandle = CreateEvent(NULL, TRUE, TRUE, NULL);
}

PipeIODevice::~PipeIODevice()
{
    try
    {
        close();
    }
    catch(...)
    {
    }
}



void PipeIODevice::open(HANDLE handle, bool isAsync)
{
    _handle = handle;

    MSGQUEUEINFO info;
    info.dwSize = sizeof(MSGQUEUEINFO);

    if ( TRUE == GetMsgQueueInfo(_handle, &info) )
    {
        _msgSize    = info.cbMaxMessage;
        _buffer.resize(_msgSize);
    }

    this->setValid(true);
    this->setAsync(isAsync);
}

IOResult& PipeIODevice::_beginRead(char* buffer, size_t n, bool& eof)
{
    if( Read != _mode )
    {
        throw IOError("Could not read from write only pipe", PT_SOURCEINFO);
    }

    if (_bufferSize)
    {
        _readResult.setHandle(_internalBufferWaitHandle);
    }
    else
    {
        _readResult.setHandle(_handle);
    }

    _readResult.attach(buffer, n);
    return _readResult;

}

size_t PipeIODevice::_endRead(IOResult& result, bool& eof)
{
    DWORD readBytes = 0;
    DWORD flags     = 0;

    eof = false;

    if (_bufferSize)
    {
        readBytes = _bufferSize;
    }
    else if ( FALSE == ReadMsgQueue(_handle, &_buffer[0], _msgSize, &readBytes, 0, &flags) )
    {
        throw IOError("Could not read from message queue handle", PT_SOURCEINFO);
    }

    DWORD bytesToCopy = std::min(_readResult.bufferSize(), readBytes);

    memcpy(_readResult.buffer(), &_buffer[0], bytesToCopy);

    _bufferSize = 0;

    if (_readResult.bufferSize() >= readBytes)
        return readBytes;

    // external buffer is too small, copy overlapping bytes to internal buffer
    // and set the device to non waitable to signal that more data is available
    std::vector<char>::iterator beginData = (_buffer.begin() + bytesToCopy);
    std::vector<char>::iterator endData   = (_buffer.begin() + readBytes);

    copy(beginData, endData, _buffer.begin());

    _bufferSize = (readBytes - bytesToCopy);

    return bytesToCopy;
}

IOResult& PipeIODevice::_beginWrite(const char* buffer, size_t n)
{
    if( Write != _mode )
    {
        throw IOError("Could not write on a read only pipe", PT_SOURCEINFO);
    }

    _writeResult.setHandle(_handle);
    _writeResult.attach(buffer, n);

    return _writeResult;
}

size_t PipeIODevice::_endWrite(IOResult& result)
{
    DWORD bytesToWrite = std::min(_writeResult.bufferSize(), _msgSize);

    if ( FALSE == WriteMsgQueue(_handle, (LPVOID) _writeResult.buffer(), bytesToWrite, 0, 0))
    {
        std::ostringstream errorMsg;
        errorMsg << "system error code: " << GetLastError() << std::endl;
        throw IOError("WriteMsgQueue failed, " + errorMsg.str(), PT_SOURCEINFO);
    }
    return bytesToWrite;
}


HANDLE PipeIODevice::deviceHandle() const
{
    return _handle;
}

void PipeIODevice::_close()
{
    if(_handle != INVALID_HANDLE_VALUE)
    {
        if( FALSE == ::CloseMsgQueue(_handle) )
        {
            std::ostringstream errorMsg;
            errorMsg << "system error code: " << GetLastError() << std::endl;
            throw IOError( "Could not close file handle, " + errorMsg.str(), PT_SOURCEINFO);
        }

        _handle = INVALID_HANDLE_VALUE;
    }
}

size_t PipeIODevice::_read(char* buffer, size_t count, bool& eof)
{
    if( Read != _mode ) {
        throw IOError("Could not read from write only pipe", PT_SOURCEINFO);
    }

    DWORD readBytes = 0;
    DWORD flags     = 0;

    eof = false;

    if (_bufferSize) {
        readBytes = _bufferSize;
    }
    else if ( FALSE == ReadMsgQueue(_handle, &_buffer[0], _msgSize, &readBytes, 0, &flags) ) {
        throw IOError("Could not read from message queue handle", PT_SOURCEINFO);
    }

    memcpy(buffer, &_buffer[0], count);

    _isWaitable = true;

    _bufferSize = 0;

    if (count >= readBytes)
        return readBytes;

    // external buffer is too small, copy overlapping bytes to internal buffer
    // and set the device to non waitable to signal that more data is available

    _isWaitable = false;

    std::vector<char>::iterator beginData = (_buffer.begin() + count);
    std::vector<char>::iterator endData   = (_buffer.begin() + readBytes);

    copy(beginData, endData, _buffer.begin());

    _bufferSize = (readBytes - count);

    return count;
}

void PipeIODevice::writeMessage(const char* buffer, size_t count)
{
    if( Write != _mode ) {
        throw IOError("Could not write on a read only pipe", PT_SOURCEINFO);
    }

    if ( TRUE == WriteMsgQueue(_handle, (LPVOID) buffer, count, 0, 0)) {
        return;
    }

    WaitForSingleObject(_handle, INFINITE);

    throw std::logic_error("WriteMsgQueue failed" + PT_SOURCEINFO);


}
size_t PipeIODevice::_write(const char* buffer, size_t count)
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

void PipeIODevice::_sync() const
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

    HANDLE outputHandle= CreateMsgQueue(NULL, &writeOpts);
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
