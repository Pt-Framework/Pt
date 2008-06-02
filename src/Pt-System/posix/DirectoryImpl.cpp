/***************************************************************************
 *   Copyright (C) 2005-2007 by Marc Boris Duerner                         *
 *   Copyright (C) 2006-2007 Tobias Mueller                                *
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
#include "DirectoryImpl.h"
#include "Pt/System/SystemError.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

namespace Pt {

namespace System {

DirectoryIteratorImpl::DirectoryIteratorImpl()
: _refs(1),
  _handle(0),
  _current(0),
  _dirty(true)
{
}


DirectoryIteratorImpl::DirectoryIteratorImpl(const char* path)
: _refs(1),
  _path(path),
  _handle(0),
  _current(0),
  _dirty(true)
{
    _handle = ::opendir( path );

    if( !_handle )
    {
        throw SystemError("Could not open directory", PT_SOURCEINFO);
    }

    // append a trailing slash if not empty, so we can add the
    // directory entry name easily
    if( ! _path.empty() && _path[_path.size()-1] != '/')
        _path += '/';

    this->advance();
}


DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
    if(_handle)
        ::closedir(_handle);
}


const std::string& DirectoryIteratorImpl::name() const
{
    return _name;
}


const std::string& DirectoryIteratorImpl::path() const
{
    if(_dirty)
    {
        // replace substring after last slash with the new file-name or
        // append the file-name if we have a trailing slash. Ctor makes
        // sure we have a trailing slash.
        std::string::size_type idx = _path.rfind('/');
        if(idx != std::string::npos && ++idx < _path.size() )
        {
        	_path.replace(idx, _path.size(), _current->d_name);
        }
        else
        {
        	_path += _current->d_name;
        }
    }

    return _path;
}


int DirectoryIteratorImpl::ref()
{
    return ++_refs;
}


int DirectoryIteratorImpl::deref()
{
    return --_refs;
}


bool DirectoryIteratorImpl::advance()
{
    _dirty  = true;

    // _current == 0 means end
    _current = ::readdir( _handle );
    if(_current)
    	_name = _current->d_name;
    
    return _current != 0;
}




void DirectoryImpl::create(const std::string& path)
{
    if( -1 == ::mkdir(path.c_str(), 0777) )
    {
        throw SystemError("Could not create directory '" + path + "'" , PT_SOURCEINFO);
    }
}

bool DirectoryImpl::exists(const std::string& path)
{
    struct stat buff;
    int err = stat(path.c_str(), &buff);

    if (err == -1 )
    {
        if (errno == ENOENT || errno == ENOTDIR)
        {
            return false;
        }

        throw SystemError("Could not stat file '" + path + "'", PT_SOURCEINFO);
    }

    return true;
}


void DirectoryImpl::remove(const std::string& path)
{
    if( -1 == ::rmdir(path.c_str()) )
    {
        throw SystemError("Could not remove directory '" + path + "'", PT_SOURCEINFO);
    }
}


void DirectoryImpl::move(const std::string& oldName, const std::string& newName)
{
    if (0 != ::rename(oldName.c_str(), newName.c_str()))
    {
        throw SystemError("Could not move directory '" + oldName + "' to '" + newName + "'", PT_SOURCEINFO);
    }
}


void DirectoryImpl::changeCurrent(const std::string& path)
{
    if( -1 == ::chdir(path.c_str()) )
    {
        throw SystemError("Could not change working directory to '" + path + "'", PT_SOURCEINFO);
    }
}

} // namespace System

} // namespace Pt
