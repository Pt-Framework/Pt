/***************************************************************************
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
#include "PipeIODevice.h"

namespace Pt {

namespace System {


PipeIODevice::PipeIODevice()
: _handle(INVALID_HANDLE_VALUE)
{
    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;
    _readOv.hEvent = NULL;
    _readOv.hEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
    _writeOv.hEvent = NULL;
    _writeOv.hEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
}

PipeIODevice::~PipeIODevice()
{
}



void PipeIODevice::open(HANDLE handle)
{
    _handle = handle;    
}

HANDLE PipeIODevice::deviceHandle() const
{
    return _handle;
}

IODeviceImpl::WaitResult PipeIODevice::waitResult( HANDLE handle )
{
    if( handle == _readOv.hEvent )
        return IODeviceImpl::ReadyRead;
    else if( handle == _writeOv.hEvent ) 
        return IODeviceImpl::ReadyWrite;
    
    throw std::logic_error( "Unkonw event handle" + PT_SOURCEINFO );
}


void PipeIODevice::eventHandles( std::vector<HANDLE>& handles, size_t waitMode )
{
    DWORD readBytes = 0;    
    bool eof;
    handles.clear();
    
    if( (waitMode & IODevice::WaitInput) == IODevice::WaitInput )
    {
        this->_read(0, 0, eof);
        //ReadFile(_handle, 0, 0, &readBytes, &_readOv);
        handles.push_back( _readOv.hEvent );
    }
    
    if( (waitMode & IODevice::WaitOutput) == IODevice::WaitOutput )
        handles.push_back( _writeOv.hEvent );          
}


void PipeIODevice::_close()
{
    if(_readOv.hEvent != NULL)
        ::CloseHandle(_readOv.hEvent);

    if(_writeOv.hEvent != NULL)
        ::CloseHandle(_writeOv.hEvent);

    if(_handle != INVALID_HANDLE_VALUE)
    {
        if( FALSE == ::CloseHandle(_handle) )
            throw IOError("Could not close file handle", PT_SOURCEINFO);

        _handle = INVALID_HANDLE_VALUE;
    }
}

size_t PipeIODevice::_read(char* buffer, size_t count, bool& eof)
{
     eof = false;
    DWORD readBytes = 0;    

    if( FALSE == ReadFile(_handle, (void*)buffer, count, &readBytes, &_readOv) )
    {
        if( ERROR_HANDLE_EOF == GetLastError() )
        {
            eof = true;
            readBytes = 0;
        }
        else if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IOError("Could not read from file handle", PT_SOURCEINFO);
        }
        else if (GetOverlappedResult(_handle, &_readOv, &readBytes, FALSE) == FALSE )
        {
            readBytes = 0;
        }

    }

    _readOv.Offset += readBytes;
    _writeOv.Offset += readBytes;
    return readBytes;
}


size_t PipeIODevice::_write(const char* buffer, size_t count)
{
    DWORD writtenBytes = 0;

    if( FALSE == WriteFile(_handle, (void*)buffer, count, &writtenBytes, &_writeOv) )
    {
        if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IOError("Could not write to file handle", PT_SOURCEINFO);
        }

        #ifndef _WIN32_WCE
        if(GetOverlappedResult(_handle, &_readOv, &writtenBytes, FALSE) == FALSE )
        {
            writtenBytes = 0;
        }
        #endif
    }

    _readOv.Offset += writtenBytes;
    _writeOv.Offset += writtenBytes;
    return writtenBytes;
}

    }
}
