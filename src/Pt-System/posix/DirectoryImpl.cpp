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
  _node(0),
  _handle(0),
  _current(0),
  _dirty(true)
{
}


DirectoryIteratorImpl::DirectoryIteratorImpl(const char* path)
: _refs(1),
  _path(path),
  _node(0),
  _handle(0),
  _current(0),
  _dirty(true)
{
    _handle = ::opendir( path );

    if( !_handle )
    {
        throw SystemError("Could not open directory", PT_SOURCEINFO);
    }

    // build complete path
    if( ! _path.empty() && _path[_path.size()-1] != '/')
        _path += "/\0";

    this->advance();
}


DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
    if(_handle)
        ::closedir(_handle);
}


const char* DirectoryIteratorImpl::name() const
{
    return _current->d_name;
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
    _node = 0;
    _dirty  = true;

    // _current == 0 means end
    _current = ::readdir( _handle );
    return _current != 0;
}


FileSystemNode& DirectoryIteratorImpl::node()
{
    // reuse previously created node
    if(_node)
        return *_node;

    // build complete path, ctor makes sure there is always a trailing 
    // slash and one character following it so idx+1 works out
    std::string::size_type idx = _path.rfind('/') + 1;
    _path.replace(idx, _path.size(), _current->d_name);

    FileSystemNode::Type type = FileSystemNode::stat(_path.c_str());
    if(type == FileSystemNode::Directory)
    {
        _dir.setPath(_path);
        _node = &_dir;
    }
    else if(type == FileSystemNode::File)
    {
        _file.setPath(_path);
        _node = &_file;
    }
    else
    {
        throw SystemError("Unknown file system node " + _path, PT_SOURCEINFO);
    }

    return *_node;
}


DirectoryEntry& DirectoryIteratorImpl::entry()
{
    if(_dirty)
    {
        // build complete path, ctor makes sure there is always a trailing 
        // slash and one character following it so idx+1 works out
        std::string::size_type idx = _path.rfind('/') + 1;
        _path.replace(idx, _path.size(), _current->d_name);
        _entry.setPath(_path);
    }

    return _entry;
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
