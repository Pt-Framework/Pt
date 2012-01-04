/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
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
#include "IODeviceImpl.h"
#include "Pt/System/IOError.h"

namespace Pt{ 

namespace System{

IODeviceImpl::IODeviceImpl()
: _handle(INVALID_HANDLE_VALUE)
{
}


IODeviceImpl::~IODeviceImpl()
{ 
}


void IODeviceImpl::setHandle(HANDLE h)
{
    _handle = h;
}


void IODeviceImpl::close(EventLoop* loop)
{
    if(_handle != INVALID_HANDLE_VALUE)
    {
        if( FALSE == ::CloseHandle(_handle) )
            throw IOError("Could not close file handle", PT_SOURCEINFO);

        _handle = INVALID_HANDLE_VALUE;
    }
}


/*OverlappedIODeviceImpl::OverlappedIODeviceImpl()
: _waitHandle(INVALID_HANDLE_VALUE)
{
    _waitHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
    if( _waitHandle == NULL )
        throw SystemError( PT_ERROR_MSG("CreateEvent failed") );

    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;
    _readOv.hEvent = NULL;

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
    _writeOv.hEvent = NULL;
}


OverlappedIODeviceImpl::~OverlappedIODeviceImpl()
{ 
    ::CloseHandle(_waitHandle);
}


void OverlappedIODeviceImpl::onCancel()
{
    ::CancelIo( handle() );

    DWORD bytes = 0;

    if( this->reading() && ! HasOverlappedIoCompleted(&_readOv) )
    {
        GetOverlappedResult( handle(), &_readOv, &bytes, TRUE );
    }

    if( this->writing() && ! HasOverlappedIoCompleted(&_writeOv) )
    {
        GetOverlappedResult( handle(), &_writeOv, &bytes, TRUE );
    }
}*/

}//namespaec System

}//namespace Pt
