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

namespace {

bool isDotEntry(const char* name)
{
    return name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0'));
}

}


DirectoryIteratorImpl::DirectoryIteratorImpl(const Path& path)
: _refs(1)
, _dir(path)
, _handle(0)
, _current(0)
{
    init( path.impl()->c_str() );
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

    this->advance();
}


bool DirectoryIteratorImpl::advance()
{
    for(;;)
    {
        _current = ::readdir(_handle);
        if( ! _current )
        {
            _finfo.clear();
            return false;
        }

        if( ! isDotEntry(_current->d_name) )
            break;
    }

    assignEntry(_current->d_name);
    return true;
}


void DirectoryIteratorImpl::assignEntry(const char* name)
{
    if(_finfo.path().empty())
        _finfo.setPath(_dir, name);
    else
        _finfo.setFileName(name);
}

} // namespace System

} // namespace Pt
