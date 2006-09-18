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

#include <windows.h>


namespace Pt {

namespace System {


SharedMemoryImpl::SharedMemoryImpl(const char* name, size_t sz, SharedMemory::OpenMode omode) throw(SystemError)
: _mode(omode), _size(sz)
{
	throw SystemError("SharedMemory not implemented", PT_SOURCEINFO);
}


SharedMemoryImpl::~SharedMemoryImpl()
{
}


void SharedMemoryImpl::unlink() throw(SystemError)
{
}


void* SharedMemoryImpl::map(const void* addr) throw(SystemError)
{
	return 0;
}


void SharedMemoryImpl::unmap(void* addr) throw(SystemError)
{
}


} // !namespace System

} // !namespace Pt
