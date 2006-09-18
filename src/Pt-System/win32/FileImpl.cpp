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
#include <string>
#include <windows.h>


namespace Pt {

namespace System {


FileImpl::FileImpl() throw(SystemError)
{
}


FileImpl::~FileImpl() throw()
{
}


void FileImpl::open(const char* name) throw(SystemError)
{
	// make sure file exists
	WIN32_FIND_DATA data;
	HANDLE h = ::FindFirstFile(name, &data);
	if(h == INVALID_HANDLE_VALUE)
		throw SystemError("Could not get file size.", PT_SOURCEINFO);

	::FindClose(h);

	_path = name;
}


void FileImpl::close() throw(SystemError)
{
	_path.clear();
}


std::size_t FileImpl::size() const
{
	WIN32_FIND_DATA data;

	HANDLE h = FindFirstFile(_path.c_str(), &data);
	if(h == INVALID_HANDLE_VALUE)
		throw SystemError("Could not get file size.", PT_SOURCEINFO);

	FindClose(h);
	
	LARGE_INTEGER li;
	li.HighPart = data.nFileSizeHigh;
	li.LowPart = data.nFileSizeLow;
	return li.QuadPart;
}


void FileImpl::resize(std::size_t newSize)
{
	HANDLE fileHandle = ::CreateFile(_path.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(fileHandle == INVALID_HANDLE_VALUE) {
		throw SystemError("Could not open file.", PT_SOURCEINFO);
	}

	// under Win32 resizing is done by moving to the desired position
	// and then calling SetEndOfFile on the handle.
	DWORD ret = ::SetFilePointer(fileHandle, newSize, NULL, FILE_BEGIN);
	if(ret == INVALID_SET_FILE_POINTER) {
		::CloseHandle(fileHandle);
		throw SystemError("Could not set file pointer", PT_SOURCEINFO);
	}

	if( FALSE == ::SetEndOfFile(fileHandle) ) {
		::CloseHandle(fileHandle);
		throw SystemError("Could not truncate file", PT_SOURCEINFO);
	}
		
	if( FALSE == ::CloseHandle(fileHandle) )
		throw SystemError("Could not close file handle", PT_SOURCEINFO);
}


void FileImpl::remove()
{
	this->close();

	if(FALSE == ::DeleteFile( _path.c_str() ))
		throw SystemError("Could not unlink file", PT_SOURCEINFO);
}


void FileImpl::copy(const char* to) const
{
	if(FALSE == ::CopyFile( _path.c_str(), to, FALSE ))
		throw SystemError("Could not copy file", PT_SOURCEINFO);
}


void FileImpl::move(const char* to)
{
	this->close();

	if(FALSE == ::MoveFile( _path.c_str(), to ))
		throw SystemError("Could not move file", PT_SOURCEINFO);
}


} // namespace System

} // namespace Pt
