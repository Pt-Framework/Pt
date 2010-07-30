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
#include "Pt/System/Directory.h"

namespace Pt {

namespace System {

DirectoryIterator::DirectoryIterator(const std::string& path)
{
    _impl = new DirectoryIteratorImpl( path );
}


DirectoryIterator::DirectoryIterator(const DirectoryIterator& it)
: _impl(0)
{
    _impl = it._impl;

    if(_impl)
        _impl->ref();
}


DirectoryIterator::~DirectoryIterator()
{
    if( _impl && 0 == _impl->deref() )
    {
        delete _impl;
    }
}


DirectoryIterator& DirectoryIterator::operator++()
{
    if( _impl && _impl->advance() )
    {
        return *this;
    }

    if( _impl && 0 == _impl->deref() )
    {
        delete _impl;
    }

    _impl = 0;
    return *this;
}


DirectoryIterator& DirectoryIterator::operator=(const DirectoryIterator& it)
{
    if (_impl == it._impl)
        return *this;

    if( _impl && 0 == _impl->deref() )
    {
        delete _impl;
    }

    _impl = it._impl;

    if(_impl)
        _impl->ref();

    return *this;
}


const std::string& DirectoryIterator::path() const
{
    return _impl->path();
}


const std::string& DirectoryIterator::operator*() const
{
    return _impl->name();
}


const std::string* DirectoryIterator::operator->() const
{
    return &_impl->name();
}


Directory::Directory()
: _impl(0)
{
}


Directory::Directory(const std::string& path)
: _path(path)
, _impl(0)
{
    if( ! Directory::exists( path.c_str() ) )
        throw DirectoryNotFound(path, PT_SOURCEINFO);
}


Directory::Directory(const FileInfo& fi)
: _path( fi.path() )
, _impl(0)
{
    if( ! fi.isDirectory() )
        throw DirectoryNotFound(fi.path(), PT_SOURCEINFO);
}


Directory::Directory(const Directory& dir)
: _path(dir._path)
{
}


Directory::~Directory()
{
    // delete impl
}


Directory& Directory::operator=(const Directory& dir)
{
	_path = dir._path;
	return *this;
}


Directory::Iterator Directory::begin() const
{
    return DirectoryIterator( path().c_str() );
}


Directory::Iterator Directory::end() const
{
    return DirectoryIterator();
}


void Directory::remove()
{
    DirectoryImpl::remove( path() );
}


void Directory::move(const std::string& to)
{
    DirectoryImpl::move(path(), to);
    _path = to;
}


Directory Directory::create(const std::string& path)
{
    DirectoryImpl::create( path.c_str() );
    return Directory(path);
}


bool Directory::exists(const std::string& path)
{
    return FileInfo::getType( path.c_str() ) == FileInfo::Directory;
}


void Directory::chdir(const std::string& path)
{
    DirectoryImpl::chdir(path);
}


std::string Directory::cwd()
{
    return DirectoryImpl::cwd();
}


std::string Directory::curdir()
{
    return DirectoryImpl::curdir();
}


std::string Directory::updir()
{
    return DirectoryImpl::updir();
}


std::string Directory::rootdir()
{
    return DirectoryImpl::rootdir();
}


std::string Directory::tmpdir()
{
    return DirectoryImpl::tmpdir();
}


std::string Directory::sep()
{
    return DirectoryImpl::sep();
}

} // namespace System

} // namespace Pt
