/*
 * Copyright (C) 2006-2008 Marc Boris Duerner
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

#include "DirectoryImpl.h"
#include "FileInfoImpl.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/IOError.h"
#include <vector>
#include <cassert>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

namespace Pt {

namespace System {

DirectoryIteratorImpl::DirectoryIteratorImpl(const std::string& path)
: _refs(1)
, _pathlen(0)
, _handle(0)
, _current(0)
{
	_finfo.path() = path;
    init( path.c_str() );
}


DirectoryIteratorImpl::DirectoryIteratorImpl(const char* path)
: _refs(1)
, _pathlen(0)
, _handle(0)
, _current(0)
{
	_finfo.path() = path;
    init(path);
}


DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
    if(_handle)
        ::closedir(_handle);
}


void DirectoryIteratorImpl::init(const char* path)
{
    _handle = ::opendir(path);
    
    if( ! _handle )
    {
        throw AccessFailed(path);
    }

	std::string& strpath = _finfo.path();

    // append a trailing slash if not empty, so we can add the
    // directory entry name easily
    if( ! strpath.empty() && strpath[strpath.size() - 1] != '/')
        strpath += '/';

    _pathlen = strpath.size();

    this->advance();
}


bool DirectoryIteratorImpl::advance()
{
    // _current == 0 means end
    _current = ::readdir( _handle );

    if(_current)
    {
		std::string& path = _finfo.path();

        path.erase(_pathlen);
        path += _current->d_name;
    }

    return _current != 0;
}


//void DirectoryImpl::create(const std::string& path)
//{
//    if( -1 == ::mkdir(path.c_str(), 0777) )
//    {
//        throw AccessFailed(path);
//    }
//}
//
//
//void DirectoryImpl::remove(const std::string& path)
//{
//    if( -1 == ::rmdir(path.c_str()) )
//    {
//        throw AccessFailed(path);
//    }
//}
//
//
//void DirectoryImpl::move(const std::string& oldName, const std::string& newName)
//{
//    if (0 != ::rename(oldName.c_str(), newName.c_str()))
//    {
//        throw AccessFailed(oldName);
//    }
//}
//
//
//void DirectoryImpl::chdir(const std::string& path)
//{
//    if( FileInfoImpl::getType( path.c_str() ) != FileInfo::Directory )
//        throw AccessFailed(path);
//
//    if( -1 == ::chdir(path.c_str()) )
//    {
//        throw SystemError("chdir");
//    }
//}
//
//
//std::string DirectoryImpl::cwd()
//{
//    const long size = pathconf(".", _PC_PATH_MAX);
//    if(size == -1)
//        throw SystemError( PT_ERROR_MSG("pathconf() failed for .") );
//
//    std::vector<char> buffer(size);
//    if( ! getcwd(&buffer[0], size) )
//    {
//        throw SystemError( PT_ERROR_MSG("getcwd() failed") );
//    }
//
//    return std::string( &buffer[0] );
//}
//
//
//std::string DirectoryImpl::tmpdir()
//{
//    const char* tmpdir = getenv("TEMP");
//
//    if(tmpdir)
//    {
//        return tmpdir;
//    }
//
//    tmpdir = getenv("TMP");
//    if(tmpdir)
//    {
//        return tmpdir;
//    }
//
//    return FileInfoImpl::getType("/tmp") == FileInfo::Directory ? "/tmp" : curdir();
//}

} // namespace System

} // namespace Pt
