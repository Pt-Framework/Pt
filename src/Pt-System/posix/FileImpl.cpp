/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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
#include "FileImpl.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

namespace Pt {

namespace System {

FileImpl::FileImpl() throw(SystemError)
: _fd(-1)
{
}


FileImpl::~FileImpl() throw()
{
}


void FileImpl::open(const char* name) throw(SystemError)
{
	if(_fd != -1)
		this->close();

	// try to open read/write...
	_fd = ::open(name, O_RDWR);

	// try to open for reading...
	if(_fd == -1)
		_fd = ::open(name, O_RDONLY);

	// ... Houston, we got a problem.
	if(_fd == -1) {
		throw SystemError("Could not open file handle", PT_SOURCEINFO);
	}

	_path = name;
}


void FileImpl::close() throw(SystemError)
{
	if(_fd != -1)
	{
		if( ::close(_fd) != 0 )
			throw SystemError("Could not close file handle", PT_SOURCEINFO);

		_fd = -1;
	}
}


std::size_t FileImpl::size() const
{
	struct stat buff;
	if( 0 != fstat(_fd, &buff) )
		throw SystemError("Could not stat file", PT_SOURCEINFO);

	return buff.st_size;
}


void FileImpl::resize(std::size_t newSize)
{
	int ret = ftruncate(_fd, newSize);
	if(ret != 0)
		throw SystemError("Could not truncate file", PT_SOURCEINFO);
}


void FileImpl::remove()
{
	this->close();

	if(0 != ::remove(_path.c_str()) == -1)
		throw SystemError("Could not remove file", PT_SOURCEINFO);
}


void FileImpl::copy(const char* to) const
{
	throw SystemError("Could not copy file", PT_SOURCEINFO);
}


void FileImpl::move(const char* to)
{
	//this->close();

	throw SystemError("Could not move file", PT_SOURCEINFO);
}

}

}
