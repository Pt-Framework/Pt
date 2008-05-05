/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Tobias Mueller                                *
 *   Copyright (C) 2006-2007 by PTV AG                                     *
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
#include "Pt/System/SharedLib.h"
#include "SharedLibImpl.h"

#include "Pt/System/Directory.h"
#include "Pt/System/Environment.h"

#include <string>
#include <sstream>
#include <iostream>


namespace Pt {

namespace System {


SharedLib::SharedLib()
: _impl(0)
{
    _impl = new SharedLibImpl();
}


SharedLib::SharedLib(const Directory& baseDirectory, const std::string& libraryName)
: _impl(0)
, _libraryFile(createLibraryFile(baseDirectory, libraryName))
{
    _impl = new SharedLibImpl(_libraryFile);
}


SharedLib::SharedLib(const File& libraryFile)
: _impl(0)
, _libraryFile(libraryFile)
{
    _impl = new SharedLibImpl(_libraryFile);
}


SharedLib::~SharedLib()
{
    delete _impl;
}


SharedLib& SharedLib::open(const File& libraryFile)
{
    if (!_libraryFile.name().empty())
    {
        throw SystemError("Shared library " + _libraryFile.path() + " is already opened.", PT_SOURCEINFO);
    }
    
    _impl->open(libraryFile);
    _libraryFile = libraryFile;
    
    return *this;
}

SharedLib& SharedLib::open(const Directory& baseDirectory, const std::string& libraryName)
{
    return this->open(createLibraryFile(baseDirectory, libraryName));
}


void* SharedLib::operator[](const char* symbol)
{
  return _impl->resolve(symbol);
}


void* SharedLib::resolve(const char* symbol)
{
  return _impl->resolve(symbol);
}


SharedLib::operator void*()
{
    return _impl->failed() ? 0 : this;
}


bool SharedLib::operator!()
{
    return _impl->failed() ? true : false;
}


const Pt::System::File& SharedLib::libraryFile() const
{
    return _libraryFile;
}

void* SharedLib::openResolve(const File& libraryFile, const char* symbol)
{
    return SharedLibImpl::openResolve(libraryFile, symbol);
}


void* SharedLib::openResolve(const Directory& baseDir, const std::string& libraryName, const char* symbol)
{
    return SharedLibImpl::openResolve(createLibraryFile(baseDir, libraryName), symbol);
}


File SharedLib::convertToPlatformSpecificLibraryFile(const File& file)
{
    std::string extension = file.extension();
    
    if (!extension.empty() && Pt::System::Environment::sharedLibraryPrefix().empty())
    {
        // Extension is set and no prefix has to be added. Just return the given file unchanged.
        return file;
    }
    
    if (extension.empty())
    {
        extension = Pt::System::Environment::sharedLibraryExtension();
    }
    else
    {
        extension = "." + extension;
    }
    
    return File(file.dirName(), Pt::System::Environment::sharedLibraryPrefix() + file.baseName() + extension);
}


File SharedLib::createLibraryFile(const Pt::System::Directory& baseDir, const std::string& libraryName)
{
    return Pt::System::File(baseDir,  Environment::sharedLibraryPrefix()
                                    + libraryName
                                    + Environment::sharedLibraryExtension());
}

} // namespace System

} // namespace Pt


void pt_system_testSharedLib()
{ std::cerr << "ptv_system_testSharedLib() called." << std::endl; }

