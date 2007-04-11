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



namespace Pt {

namespace System {

PipeIODevice::PipeIODevice(Mode mode)
: _handle(INVALID_HANDLE_VALUE)
, _mode(mode)
, _msgSize(0)
, _isWaitable(true)
{    
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



void PipeIODevice::open(HANDLE handle)
{
    _handle = handle; 
    
    MSGQUEUEINFO info;
    if ( TRUE == GetMsgQueueInfo(_handle, &info) )
    {
        _msgSize    = info.cbMaxMessage;        
        _buffer.reserve(_msgSize);
        _tempBuffer.resize(_msgSize);
    }

    setValid(true);
}

HANDLE PipeIODevice::deviceHandle() const
{
    return _handle;
}

IODeviceImpl::WaitResult PipeIODevice::waitResult( HANDLE handle )
{
    if( Read == _mode ) {
        return IODeviceImpl::ReadyRead;
    }
    
    return IODeviceImpl::ReadyWrite;    
}

void PipeIODevice::eventHandles( std::vector<HANDLE>& handles, size_t waitMode )
{
    handles.clear();

    handles.push_back(_handle);    
}


void PipeIODevice::_close()
{
    if(_handle != INVALID_HANDLE_VALUE)
    {
        if( FALSE == ::CloseMsgQueue(_handle) )
            throw IOError("Could not close file handle", PT_SOURCEINFO);

        _handle = INVALID_HANDLE_VALUE;
    }
}

size_t PipeIODevice::_read(char* buffer, size_t count, bool& eof)
{
    if( Read != _mode ) {
        throw IOError("Could not read from write only pipe", PT_SOURCEINFO);
    }

    eof = false;
    DWORD readBytes = 0;   
    DWORD flags;    

    if (!_buffer.empty())
    {
        readBytes = _buffer.size();
        copy(_buffer.begin(), _buffer.end(), _tempBuffer.begin());
    }
    else if ( FALSE == ReadMsgQueue(_handle, &_tempBuffer[0], _msgSize, &readBytes, 0, &flags) )
    {
        throw IOError("Could not read from file handle", PT_SOURCEINFO);
    }
    
    memcpy(buffer, &_tempBuffer[0], count);

    _isWaitable = true;

    _buffer.clear();

    if (count >= readBytes)
        return readBytes;

    // external buffer is too small, copy overlapping bytes to internal buffer
    // and set the device to non waitable to signal that more data is available
    
    _isWaitable = false;
    
    std::vector<char>::iterator it = (_tempBuffer.begin() + count);        
                   
    _buffer.assign(it, _tempBuffer.end());        
    _buffer.resize(readBytes - count); // get the real size of bytes readed    
    
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



PipeImpl::PipeImpl()
: _inputDevice(PipeIODevice::Read)
, _outputDevice(PipeIODevice::Write)
{    
    MSGQUEUEOPTIONS writeOpts, readOpts;

    memset(&writeOpts, 0, sizeof(writeOpts));
    memset(&readOpts,  0, sizeof(readOpts));

    writeOpts.dwSize          = sizeof(MSGQUEUEOPTIONS);
    writeOpts.dwFlags         = MSGQUEUE_ALLOW_BROKEN;
    writeOpts.dwMaxMessages   = 10; 
    writeOpts.cbMaxMessage    = 20;
    writeOpts.bReadAccess     = FALSE;
    
    readOpts = writeOpts;
    readOpts.bReadAccess     = TRUE;

    HANDLE outputHandle= CreateMsgQueue(L"test", &writeOpts);
    if (outputHandle == INVALID_HANDLE_VALUE)
        throw IOError("Could not create message queue handle", PT_SOURCEINFO);

    HANDLE inputHandle  = OpenMsgQueue(::GetCurrentProcess(), outputHandle, &readOpts);
    if (inputHandle == INVALID_HANDLE_VALUE)
        throw IOError("Could not open message queue handle", PT_SOURCEINFO);

    _inputDevice.open(inputHandle);
    _outputDevice.open(outputHandle);
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

Pt::uint32_t  PipeImpl::_nameId = 0;

} // namespace System

} // namespace Pt
