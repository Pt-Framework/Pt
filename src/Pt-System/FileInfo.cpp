/***************************************************************************
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
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
#include "DirectoryImpl.h"
#include "Pt/System/FileInfo.h"
#include "Pt/System/Environment.h"

namespace Pt {

namespace System {

FileInfo::FileInfo()
: _type(FileSystemNode::Invalid)
{}


FileInfo::FileInfo(const char* path)
: _path(path)
{
    _type = FileSystemNode::stat( path );
}


FileInfo::~FileInfo()
{
}


std::string FileInfo::name() const
{
    std::string::size_type pos = _path.rfind(Environment::pathSeparator());

    if (pos != std::string::npos)
    {
        return _path.substr(pos + 1);
    }
    else
    {
        return _path;
    }
}


const char* FileInfo::path() const
{
    return _path.c_str();
}


std::string FileInfo::dirName() const
{
    // Find last slash. This separates the file name from the path.
    std::string::size_type pos = _path.find_last_of(Environment::pathSeparator());

    // If there is no separator, the file is relative to the current 
    // directory. So an empty path is returned.
    if (pos == std::string::npos)
    {
        return "";
    }

    // Include trailing separator to be able to distinguish between no 
    // path ("") and a path which is relative to the root ("/"), for example.
    return _path.substr(0, pos + 1);
}


std::size_t FileInfo::size() const
{
    if(_type == FileSystemNode::File)
    {
        return FileImpl::size( _path.c_str() );
    }

    return 0;
}


bool FileInfo::isDirectory() const
{
    return _type == FileSystemNode::Directory;
}


bool FileInfo::isFile() const
{
    return _type == FileSystemNode::File;
}


void FileInfo::remove()
{
    if(_type == FileSystemNode::File)
    {
        return FileImpl::remove( _path.c_str() );
    }

    return DirectoryImpl::remove( _path.c_str() );
}


void FileInfo::move(const std::string& newname)
{
    if(_type == FileSystemNode::File)
    {
        return FileImpl::move( _path.c_str(), newname.c_str() );
    }

    return DirectoryImpl::move( _path.c_str(), newname.c_str() );
}

} // namespace System

} // namespace Pt


/*FileInfo::FileInfo(const FileInfo& fi)
{
    this->operator=(fi);
}


FileInfo& FileInfo::operator=(const FileInfo& fi)
{
    if(fi._node == &fi._dir)
    {
        _dir = fi._dir;
        _node = &_dir;
    }
    else if(fi._node == &fi._file)
    {
        _file = fi._file;
        _node = &_file;
    }

    return *this;
}*/

