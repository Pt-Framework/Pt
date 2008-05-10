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
#include "SharedLibImpl.h"
#include "Pt/System/SharedLib.h"
#include "Pt/System/File.h"
#include "Pt/System/Environment.h"
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

namespace Pt {

namespace System {

SharedLib::SharedLib()
: _impl(0)
{
    _impl = new SharedLibImpl();
}


SharedLib::SharedLib(const std::string& path)
: _impl(0)
{
    _path = find(path);
    _impl = new SharedLibImpl(_path);
}


SharedLib::~SharedLib()
{
    delete _impl;
}


SharedLib& SharedLib::open(const std::string& path)
{
    if( ! _path.empty() )
    {
        throw SystemError("Shared library " + _path + " is already opened.", PT_SOURCEINFO);
    }

    _path = find(path);
    _impl->open(path);
    return *this;
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


const std::string& SharedLib::path() const
{
    return _path;
}


std::string SharedLib::find(const std::string& path_)
{
    std::string path = path_;

    if( FileSystemNode::exists( path.c_str() ) )
        return path;

    char sep = Environment::pathSeparator();
    std::string::size_type idx = path.find(sep);

    if(++idx == path.length())
    {
        throw SystemError("Invalid file name " + path , PT_SOURCEINFO);
    }

    path += Environment::sharedLibraryExtension();
    if( FileSystemNode::exists( path.c_str() ) )
        return path;

    if(idx == std::string::npos)
    {
        idx = 0;
    }
    path.insert( idx, Environment::sharedLibraryPrefix() );

    if( ! FileSystemNode::exists( path.c_str() ) )
    {
        // TODO FileNotFound
        //throw SystemError("Shared library not found " + path , PT_SOURCEINFO);
        return "";
    }

    return path;
}

/*
File SharedLib::createLibraryFile(const Pt::System::Directory& baseDir, const std::string& libraryName)
{
    return Pt::System::File(baseDir,  Environment::sharedLibraryPrefix()
                                    + libraryName
                                    + Environment::sharedLibraryExtension());
}
*/
} // namespace System

} // namespace Pt


void pt_system_testSharedLib()
{ std::cerr << "ptv_system_testSharedLib() called." << std::endl; }
