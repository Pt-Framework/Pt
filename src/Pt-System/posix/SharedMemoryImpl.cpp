/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Duerner                              *
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
#include "SharedMemoryImpl.h"

#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


namespace Pt {

namespace System {


SharedMemoryImpl::SharedMemoryImpl(const char* name, size_t sz, SharedMemory::OpenMode omode) throw(SystemError)
: _mode(omode), _size(sz), _fd(-1)
{
	int flags = O_RDONLY;

	if(omode & SharedMemory::Write) {
		flags |= O_RDWR;
	}

	flags |= O_CREAT;

	// open the shared memory segment
	_fd = ::shm_open(name, flags, 0700);
	if(_fd == -1)
	{
		throw SystemError("Could not open POSIX shared-memory segment", PT_SOURCEINFO);
	}

	// truncate segment to given size
	if( -1 == ::ftruncate(_fd, sz) )
	{
		::close(_fd);
		_fd = -1;
		throw SystemError("Could not truncate POSIX shared-memory segment", PT_SOURCEINFO);
	}

	_name = name;
}


SharedMemoryImpl::~SharedMemoryImpl()
{
	if(_fd != -1)
		::close(_fd);
}


void SharedMemoryImpl::unlink() throw(SystemError)
{
	if(-1 == ::shm_unlink( _name.c_str() ) )
		throw SystemError("Could not unlink POSIX shared-memory segment", PT_SOURCEINFO);
}


void* SharedMemoryImpl::map(const void* addr) throw(SystemError)
{
	int prot= PROT_READ;

	if(_mode & SharedMemory::Write)
		prot = PROT_WRITE;

	void* mapaddr = ::mmap((void*)addr, _size, prot, MAP_SHARED, _fd, 0);

	if(mapaddr == MAP_FAILED)
		throw SystemError("Could not map POSIX shared-memory segment", PT_SOURCEINFO);

	return mapaddr;
}


void SharedMemoryImpl::unmap(void* addr) throw(SystemError)
{
	if(-1 == ::munmap(addr, _size) )
		throw SystemError("Could not unmap POSIX shared-memory segment", PT_SOURCEINFO);
}


} // !namespace System

} // !namespace Pt
