/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
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
#include "Pt/System/Directory.h"
#include "Pt/System/Environment.h"

namespace Pt {

namespace System {

DirectoryNotFound::DirectoryNotFound(const Directory& dir, const SourceInfo& si)
: SystemError("Directory not found: " + dir.path(), si)
{
}


DirectoryNotFound::~DirectoryNotFound() throw()
{
}


Directory::Directory()
: FileSystemNode()
, _impl(0)
{
}


Directory::Directory(const std::string& path)
: FileSystemNode(path)
, _impl(0)
{
    if( ! Directory::exists( path.c_str() ) )
        throw DirectoryNotFound(*this, PT_SOURCEINFO);
}


Directory::~Directory()
{
}


std::size_t Directory::size() const
{
    return 0;
}


DirectoryIterator Directory::begin() const
{
    return DirectoryIterator( path().c_str() );
}


DirectoryIterator Directory::end() const
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
    this->setPath(to);
}


// TODO This is identical to File::parentPath(). Maybe this should be moved into
// the common base class FileSystemNode.
std::string Directory::dirName() const
{
    // Find last slash. This separates the last path segment from the rest of the path
    std::string::size_type separatorPos = path().find_last_of(Environment::pathSeparator());

    // If there is no separator, this directory is relative to the current current directory.
    // So an empty path is returned.
    if (separatorPos == std::string::npos)
    {
        return "";
    }

    // Include trailing separator to be able to distinguish between no path ("") and a path
    // which is relative to the root ("/"), for example.
    return path().substr(0, separatorPos + 1);
}


// TODO This is identical to File::name(). Maybe this should be moved into
// the common base class FileSystemNode.
std::string Directory::name() const
{
    std::string::size_type separatorPos = path().rfind(Environment::pathSeparator());

    if (separatorPos != std::string::npos)
    {
        return path().substr(separatorPos + 1);
    }
    else
    {
        return path();
    }
}


FileSystemNode::Type Directory::type() const
{
    return FileSystemNode::Directory;
}


void Directory::create(const char* path)
{
    DirectoryImpl::create(path);
}


const FileSystemNode& DirectoryEntry::node() const
{
    if(_node)
        return *_node;

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


const FileSystemNode& FileInfo::node() const
{
    if(_node)
        return *_node;

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


DirectoryIterator::DirectoryIterator()
: _impl(0)
{ }


DirectoryIterator::DirectoryIterator(const char* path)
{
    _impl = new DirectoryIteratorImpl(path);
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
    if( _impl && 0 == _impl->deref() ) {
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
        _impl = 0;
    }

    return *this;
}


DirectoryIterator& DirectoryIterator::operator=(const DirectoryIterator& it)
{
    if (*this == it )
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


const char* DirectoryIterator::name() const
{
    if(_impl)
        return _impl->name();

    return "";
}


FileSystemNode& DirectoryIterator::operator*() const
{
    if(_impl == 0)
        throw std::out_of_range("directory iterator out of range");

    return _impl->node();
}


FileSystemNode* DirectoryIterator::operator->() const
{
    if(_impl == 0)
        throw std::out_of_range("directory iterator out of range");

    return &_impl->node();
}


DirectoryEntry& DirectoryIterator::entry()
{
    return _impl->entry();
}


const DirectoryEntry& DirectoryIterator::entry() const
{
    return _impl->entry();
}

} // namespace System

} // namespace Pt
