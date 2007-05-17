/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
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
#include "IODeviceImpl.h"

namespace Pt{ 
namespace System{

IODeviceImpl::IODeviceImpl()
{
    _writeOvl.Offset = 0;
    _writeOvl.OffsetHigh = 0;
    _writeOvl.hEvent = NULL;
    _writeOvl.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}


IODeviceImpl::~IODeviceImpl()
{ 
    if(_writeOvl.hEvent != NULL)
        ::CloseHandle(_writeOvl.hEvent);
}


IOResult& IODeviceImpl::beginWrite(const char* buffer, size_t n)
{
	DWORD writtenBytes = 0;

	HANDLE h = deviceHandle();

	if( FALSE == WriteFile( h, (void*)buffer, n, &writtenBytes, &_writeOvl) )
	{
		DWORD err = GetLastError();
		if( ERROR_IO_PENDING != GetLastError() )
		{
			throw IOError("Could not read from file handle", PT_SOURCEINFO);
		}
	}

	_writeResult.setHandle(_writeOvl.hEvent);
	return _writeResult;
}


size_t IODeviceImpl::endWrite(IOResult& result)
{
	assert(&result == &_writeResult);
	
	DWORD writtenBytes = 0;

#ifndef _WIN32_WCE
	if (GetOverlappedResult( deviceHandle(), &_writeOvl, &writtenBytes, FALSE) == FALSE )
	{
		DWORD err = GetLastError();
		throw IOError("Could not read from file handle", PT_SOURCEINFO);
	}
#else
	throw std::runtime_error("endRead not implemented for WinCe" + PT_SOURCEINFO);
#endif

	_writeOvl.Offset += writtenBytes;
	return writtenBytes;
}	
			
}//namespaec System
}//namespace Pt
