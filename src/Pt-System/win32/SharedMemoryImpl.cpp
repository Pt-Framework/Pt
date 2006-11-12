/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
#include <windows.h>
#include <iostream>
#include "SharedMemoryImpl.h"
#include "Pt/SourceInfo.h"
#include "Pt/System/SharedMemory.h"

namespace Pt {

namespace System {


SharedMemoryImpl::SharedMemoryImpl(const char* name, size_t sz, SharedMemory::OpenMode omode)
: _name(name)
, _size(sz)
{
	_mode = omode==SharedMemory::Write ? PAGE_READWRITE : PAGE_READONLY;

  _handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, _mode, 0, _size, _name);

	if (_handle == NULL || _handle == INVALID_HANDLE_VALUE)
	{
		throw SystemError("Could not create Windows file mapping object", PT_SOURCEINFO);
	}
}


SharedMemoryImpl::~SharedMemoryImpl()
{
}


void SharedMemoryImpl::unlink()
{
	if ( CloseHandle(_handle)==0 )
	{
		throw SystemError("Could not unlink Windows shared-memory segment", PT_SOURCEINFO);
	}
	_handle = NULL;
}


void* SharedMemoryImpl::map(const void* addr)
{
	DWORD access = _mode==PAGE_READWRITE ? FILE_MAP_WRITE : FILE_MAP_READ;
	LPVOID mapAddr = MapViewOfFile(_handle, access, 0, 0, _size);
	if (mapAddr == NULL)
	{
		throw SystemError("Could not map view of file", PT_SOURCEINFO);
	}
	return mapAddr;
}


void SharedMemoryImpl::unmap(void* addr)
{
	UnmapViewOfFile(addr);
}


} // !namespace System

} // !namespace Pt
