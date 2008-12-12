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
#include "FileImpl.h"
#include "Pt/System/File.h"
#include "Pt/System/Directory.h"

namespace Pt {

namespace System {

File::File()
: _impl(0)
{
}


File::File(const std::string& path)
: _path(path)
, _impl(0)
{
    if( ! File::exists( _path) )
        throw FileNotFound(path, PT_SOURCEINFO);
}


File::File(const FileInfo& fi)
: _path( fi.path() )
, _impl(0)
{
    if( ! fi.isFile() )
        throw FileNotFound(fi.path(), PT_SOURCEINFO);
}


File::File(const File& file)
: _path( file.path() )
, _impl(0)
{
}


File::~File()
{
    // delete _impl;
}


File& File::operator=(const File& file)
{
    _path = file.path();
    return *this;
}


std::size_t File::size() const
{
    return FileImpl::size( path() );
}


void File::resize(std::size_t newSize)
{
    FileImpl::resize(path(), newSize);
}


void File::remove()
{
    FileImpl::remove( path() );
}


void File::move(const std::string& to)
{
    FileImpl::move( _path, to );
    _path = to;
}


std::string File::baseName() const
{
    std::string fileName = this->name();

    std::string::size_type extensionPointPos = fileName.rfind('.');

    if (extensionPointPos != std::string::npos)
    {
        return fileName.substr(0, extensionPointPos);
    }
    else
    {
        return fileName;
    }
}


std::string File::extension() const
{
    std::string fileName = this->name();

    std::string::size_type extensionPointPos = fileName.rfind('.');

    if (extensionPointPos != std::string::npos)
    {
        return fileName.substr(extensionPointPos + 1);
    }
    else
    {
        return "";
    }
}


File File::create(const std::string& path)
{
    FileImpl::create(path);
    return File(path);
}


bool File::exists(const std::string& path)
{
    return FileInfo::getType(path) == FileInfo::File;
}

} // namespace System

} // namespace Pt
