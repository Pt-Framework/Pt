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
#include "Pt/System/FileInfo.h"
#include "Pt/System/File.h"
#include "Pt/System/Directory.h"
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

namespace Pt {

namespace System {

SymbolNotFound::SymbolNotFound(const std::string& sym, const Pt::SourceInfo& si)
: SystemError("symbol not found: " + sym, si)
, _symbol(sym)
{ }


SymbolNotFound::~SymbolNotFound() throw()
{ }


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


SharedLib::SharedLib(const SharedLib& other)
{
    _path = other._path;
    _impl = other._impl;
    _impl->ref();
}


SharedLib& SharedLib::operator=(const SharedLib& other)
{
    if(_impl == other._impl)
        return *this;

    _path = other._path;

    other._impl->ref();

    if( ! _impl->unref() )
        delete _impl;

    _impl = other._impl;

    return *this;
}


SharedLib::~SharedLib()
{
    if ( ! _impl->unref() )
        delete _impl;
}


void SharedLib::detach()
{
    if ( _impl->refs() == 1 )
        return;

    _path.clear();

    SharedLibImpl* x = _impl;
    _impl = new SharedLibImpl();

    if( ! x->unref() )
        delete x;
}

SharedLib& SharedLib::open(const std::string& path)
{
    this->detach();

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


Symbol SharedLib::getSymbol(const char* symbol)
{
    void* sym = this->resolve(symbol);
    if (sym == 0)
    {
        throw SymbolNotFound(symbol, PT_SOURCEINFO);
    }

    return Symbol(*this, sym);
}

SharedLib::operator const void*() const
{
    return _impl->failed() ? 0 : this;
}


bool SharedLib::operator!() const
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

    if( FileInfo::exists( path.c_str() ) )
        return path;

    std::string::size_type idx = path.find( Directory::sep() );

    if(++idx == path.length())
    {
        throw SystemError("Invalid file name " + path , PT_SOURCEINFO);
    }

    path += suffix();
    if( FileInfo::exists( path.c_str() ) )
        return path;

    if(idx == std::string::npos)
    {
        idx = 0;
    }
    path.insert( idx, prefix() );

    if( ! FileInfo::exists( path.c_str() ) )
    {
        // TODO FileNotFound
        throw FileNotFound(path_ , PT_SOURCEINFO);
    }

    return path;
}


std::string SharedLib::suffix()
{
    return SharedLibImpl::suffix();
}


std::string SharedLib::prefix()
{
    return SharedLibImpl::prefix();
}

} // namespace System

} // namespace Pt


void pt_system_testSharedLib()
{ std::cerr << "ptv_system_testSharedLib() called." << std::endl; }
