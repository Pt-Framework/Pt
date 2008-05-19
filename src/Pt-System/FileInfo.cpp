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
#include "Pt/System/FileInfo.h"

namespace Pt {

namespace System {

FileInfo::FileInfo()
: _node(0)
{}


FileInfo::FileInfo(const char* path)
: _node(0)
{
    FileSystemNode::Type type = FileSystemNode::stat( path );
    if(type == FileSystemNode::Directory)
    {
        _dir.setPath(path);
        _node = &_dir;
    }
    else if(type == FileSystemNode::File)
    {
        _file.setPath(path);
        _node = &_file;
    }
    else
    {
        throw SystemError("Unknown file system node " + std::string(path), PT_SOURCEINFO);
    }
}


FileInfo::~FileInfo()
{
}


FileInfo::FileInfo(const FileInfo& fi)
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
}


const char* FileInfo::name() const
{
    if(_node)
        return _node->name().c_str();

    return "";
}

const char* FileInfo::path() const
{
    if(_node)
        return _node->path().c_str();

    return "";
}


const FileSystemNode& FileInfo::node() const
{
    return *_node;
}

} // namespace System

} // namespace Pt
