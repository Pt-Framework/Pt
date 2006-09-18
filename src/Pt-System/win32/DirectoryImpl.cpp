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
#include "DirectoryImpl.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/FileSystem.h"

#include <windows.h>


namespace Pt {

namespace System {

DirectoryIteratorImpl::DirectoryIteratorImpl()
: _refs(1),
  _node(0),
  _findHandle(INVALID_HANDLE_VALUE)
{
}


DirectoryIteratorImpl::DirectoryIteratorImpl(const char* path)
: _refs(1),
  _node(0),
  _findHandle(INVALID_HANDLE_VALUE)
{
	std::string firstFile = path;
	if( !firstFile.empty() && firstFile[firstFile.size()-1] != '\\' )
		firstFile += "\\";
	
	firstFile += '*'; 

	_findHandle = FindFirstFile(firstFile.c_str(), &_current);
	if(_findHandle == INVALID_HANDLE_VALUE)
		throw SystemError("Could not open find handle.", PT_SOURCEINFO);
		
	_path = path;
}


DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
	delete _node;

	if(_findHandle == INVALID_HANDLE_VALUE)
		::FindClose(_findHandle);
}


int DirectoryIteratorImpl::ref()
{ 
	return ++_refs; 
}


int DirectoryIteratorImpl::deref()
{ 
	return --_refs; 
}


void DirectoryIteratorImpl::advance()
{
	// cannot advance an unintialised iterator
	if(_findHandle == INVALID_HANDLE_VALUE) {
		return;
	}

	// the current node becomes invalid now
	delete _node;
	_node = 0;

	// _findHandle = INVALID_HANDLE_VALUE means end
	if( FALSE == FindNextFile(_findHandle, &_current) ) 
	{
		::FindClose(_findHandle);
		_findHandle = INVALID_HANDLE_VALUE;
	}
}


FileSystemNode& DirectoryIteratorImpl::node()
{
	// reuse previously created node
	if(_node)
		return *_node;

	// build complete path
	std::string path = _path;
	if( !path.empty() && path[path.size()] != '\\') 
		path += '\\';
	path += this->name();

	// create file system node by full path
	_node = FileSystem::instance().create( path.c_str() );
	if(!_node)
		throw SystemError("Unknown file system node", PT_SOURCEINFO);

	return *_node;
}


std::string DirectoryIteratorImpl::name() const
{
	if(_findHandle != INVALID_HANDLE_VALUE)
		return _current.cFileName;
		
	return "";
}


bool DirectoryIteratorImpl::operator==(const DirectoryIteratorImpl& impl) const
{ 
	return _findHandle == impl._findHandle; 
}




void DirectoryImpl::create(const char* dirpath)
{
	if( FALSE == ::CreateDirectory(dirpath, NULL) )
		throw SystemError("Could not create directory" , PT_SOURCEINFO);
}


void DirectoryImpl::remove(const char* dirpath)
{
	if( FALSE == ::RemoveDirectory(dirpath) )
		throw SystemError("Could not remove directory" , PT_SOURCEINFO);
}


std::string DirectoryImpl::current()
{
	char path[MAX_PATH+2];
	DWORD len = ::GetCurrentDirectory(MAX_PATH+2, path);
	return std::string(path, len);
}


std::string DirectoryImpl::system()
{
	return "c:\\";
}


void DirectoryImpl::changeCurrent(const char* dirpath)
{
	if(FALSE == ::SetCurrentDirectory(dirpath) )
		throw SystemError("Could not change current directory" , PT_SOURCEINFO);
}

}

}