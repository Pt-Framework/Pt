/***************************************************************************
 *   Copyright (C) 2004 by Christian Prochnow                              *
 *   cproch@seculogix.de                                                   *
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
 
#include "Pt/Pt-config.h"
#include "Pt/System/SharedMemory.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
 
namespace Pt {
namespace System {

struct SharedMemory::Handle {
	int shmid;
};

SharedMemory::SharedMemory(const char* name, size_t sz, OpenMode omode,
	AccessMode /*amode*/) throw(OutOfMemory, SystemError)
{
	key_t key = ftok(name, 0);
	int flags;
	
	switch(omode)
	{
		case OpenCreate:
			flags = IPC_CREAT;
			break;
			
		case CreateFail:
			flags = IPC_CREAT|IPC_EXCL;
			break;
			
		case OpenFail:
			flags = 0;
			break;
	}
	
	int shmid = shmget(key, sz, flags);
	if(shmid == -1)
		throw SystemError(errno, "Could not get SYSV shared-memory segment",
			P_SOURCEINFO);

	_handle = new Handle;
	_handle->shmid = shmid;
}

SharedMemory::~SharedMemory()
{
	delete _handle;
}

void SharedMemory::unlink() throw(SystemError)
{
	if(shmctl(_handle->shmid, IPC_RMID, 0) == -1)
		throw SystemError(errno, "Could not remove SYSV shared-memory segment",
			P_SOURCEINFO);
}

void* SharedMemory::map(const void* addr, AccessMode mode) throw(SystemError)
{
	int flags = SHM_RND;
	if(mode == ReadOnly)
		flags |= SHM_RDONLY;

	void* mapaddr = shmat(_handle->shmid, addr, flags);
	if((int)mapaddr == -1)
		throw SystemError(errno, "Could not map SYSV shared-memory segment",
			P_SOURCEINFO);

	return mapaddr;
}

void SharedMemory::unmap(void* addr) throw(SystemError)
{
	if(shmdt(addr) == -1)
		throw SystemError(errno, "Could not unmap SYSV shared-memory segment",
			P_SOURCEINFO);
}

} // !namespace System
} // !namespace P
