/*
 * Copyright (C) 2006-2013 Marc Boris Duerner
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

#include "FileInfoImpl.h"
#include <Pt/System/FileInfo.h>

namespace Pt {

namespace System {

FileInfo::FileInfo(const std::string& path)
: _path(path)
{
    FileStatus::Type type = FileInfoImpl::getType( _path );

    std::size_t size = (type == FileStatus::File) ? FileInfoImpl::size(_path)
                                                  : 0;

    _status = FileStatus(type, size);
}


FileInfo::FileInfo(const char* path)
: _path(path)
{
    FileStatus::Type type = FileInfoImpl::getType( _path );

    std::size_t size = (type == FileStatus::File) ? FileInfoImpl::size(_path)
                                                  : 0;

    _status = FileStatus(type, size);
}


void FileInfo::clear()
{
	  _path.clear();
    _status = FileStatus(FileStatus::Invalid, 0);
}


std::string& FileInfo::init(FileStatus::Type type, std::size_t n)
{
    _status = FileStatus(type, n);
    return _path;
}


FileStatus::Type FileInfo::type(const std::string& path)
{
    return FileInfoImpl::getType(path);
}


std::size_t FileInfo::size(const std::string& path)
{
    return FileInfoImpl::size(path);
}


std::string FileInfo::name(const std::string& path)
{
    std::string::size_type pos = path.rfind( FileInfoImpl::sep() );

    if (pos != std::string::npos)
    {
        return path.substr(pos + 1);
    }

    return path;
}


std::string FileInfo::dirName(const std::string& path)
{
    // Find last slash. This separates the file name from the path.
    std::string::size_type pos = path.find_last_of( FileInfoImpl::sep() );

    // If there is no separator, the file is relative to the current 
    // directory. So an empty path is returned.
    if (pos == std::string::npos)
    {
        return std::string();
    }

    // Include trailing separator to be able to distinguish between no 
    // path ("") and a path which is relative to the root ("/"), for example.
    return path.substr(0, pos + 1);
}


std::string FileInfo::baseName(const std::string& path)
{
    std::string::size_type sepPos = path.rfind( FileInfoImpl::sep() );

    if(sepPos == std::string::npos)
    {
        sepPos = 0;
    }

    std::string::size_type extPos = path.rfind('.');

    if(extPos == std::string::npos || sepPos >= extPos)
    {
        return path.substr(sepPos);
    }

    return path.substr(sepPos, extPos - sepPos);
}


std::string FileInfo::extension(const std::string& path)
{
    std::string::size_type sepPos = path.rfind( FileInfoImpl::sep() );

    if(sepPos == std::string::npos)
    {
        sepPos = 0;
    }

    std::string::size_type extPos = path.rfind('.');

    if(extPos == std::string::npos || sepPos >= extPos)
    {
        return std::string();
    }

    return path.substr(extPos + 1);
}


void FileInfo::createFile(const std::string& path)
{
    FileInfoImpl::createFile(path);
}


void FileInfo::createDirectory(const std::string& path)
{
    FileInfoImpl::createDirectory(path);
}


void FileInfo::resize(const std::string& path, std::size_t n)
{
    FileInfoImpl::resize(path, n);
}


void FileInfo::remove(const std::string& path)
{
    FileInfoImpl::remove(path);
}


void FileInfo::move(const std::string& path, const std::string& to)
{
    FileInfoImpl::move(path, to);
}


const char* FileInfo::sep()
{
    return FileInfoImpl::sep();
}


const char* FileInfo::curdir()
{
    return FileInfoImpl::curdir();
}


const char* FileInfo::updir()
{
    return FileInfoImpl::updir();
}

} // namespace System

} // namespace Pt
