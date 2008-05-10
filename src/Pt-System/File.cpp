/***************************************************************************
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
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
#include "FileImpl.h"
#include "Pt/System/File.h"
#include "Pt/System/Directory.h"
#include "Pt/System/Environment.h"

namespace Pt {

namespace System {

File::File(const std::string& path)
{
    _impl = new FileImpl(path);
}


File::File(const File& file)
{
    _impl = new FileImpl( *file._impl );
}


File::~File()
{
    delete _impl;
}


File& File::operator=(const File& file)
{
    *_impl = *(file._impl);
    return *this;
}


const std::string& File::path() const
{
    return _impl->path();
}


std::size_t File::size() const
{
    return _impl->size();
}


void File::resize(std::size_t newSize)
{
    _impl->resize(newSize);
}


FileSystemNode::Type File::type() const
{
    return FileSystemNode::File;
}


void File::remove()
{
    _impl->remove();
}

/*
void File::copy(const std::string& to) const
{
    return _impl->copy(to);
}
*/

void File::move(const std::string& newPath)
{
    _impl->move(newPath);
}

// TODO This should be done on a file system basis. If we'd have a relative file here,
// with no path, and try to determine the parent, an empty string would be returned,
// though a parent is available.
// TODO This is identical to Directory::parentPath(). Maybe this should be moved into
// the common base class FileSystemNode.
std::string File::dirName() const
{
    // Find last slash. This separates the file name from the path.
    std::string::size_type separatorPos = path().find_last_of(Environment::pathSeparator());

    // If there is no separator, the file is relative to the current directory. So an empty path is returned.
    if (separatorPos == std::string::npos)
    {
        return "";
    }

    // Include trailing separator to be able to distinguish between no path ("") and a path
    // which is relative to the root ("/"), for example.
    return path().substr(0, separatorPos + 1);
}


// TODO This is identical to Directory::name(). Maybe this should be moved into
// the common base class FileSystemNode.
std::string File::name() const
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


bool File::create(const char* path)
{
    if( FileSystemNode::exists(path) )
    {
        return false;
    }

    FileImpl::create(path);
    return true;
}

} // namespace System

} // namespace Pt
