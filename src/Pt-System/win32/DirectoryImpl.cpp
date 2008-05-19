/***************************************************************************
 *   Copyright (C) 2005-2007 by Marc Boris Duerner                         *
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
#include "win32.h"
#include "Pt/System/SystemError.h"
#include <vector>
#include <windows.h>


namespace Pt {

namespace System {

DirectoryIteratorImpl::DirectoryIteratorImpl()
: _refs(1),
  _findHandle(INVALID_HANDLE_VALUE),
  _dirty(true)
{
}


DirectoryIteratorImpl::DirectoryIteratorImpl(const char* path)
: _refs(1),
  _path(path),
  _findHandle(INVALID_HANDLE_VALUE),
  _dirty(true)
{
    std::string firstFile = path;
    if( ! firstFile.empty() && firstFile[firstFile.size()-1] != '\\' )
        firstFile += '\\';

    firstFile += '*';

    std::basic_string<TCHAR> tpath = win32::fromMultiByte( firstFile );
    _findHandle = FindFirstFile( tpath.c_str(), &_current );

    if(_findHandle == INVALID_HANDLE_VALUE)
        throw SystemError("Could not open find handle.", PT_SOURCEINFO);

    _path = path;
    if( ! _path.empty() && _path[_path.size()-1] != '\\')
        _path += "\\\0";
}


DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
    if(_findHandle == INVALID_HANDLE_VALUE)
        ::FindClose(_findHandle);
}


int DirectoryIteratorImpl::ref()
{
    return ++_refs;
}


int DirectoryIteratorImpl::deref()
{
    return --_refs;
}


bool DirectoryIteratorImpl::advance()
{
    // cannot advance an unintialised iterator
    if(_findHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    // the current node becomes invalid now
    _dirty  = true;

    // _findHandle = INVALID_HANDLE_VALUE means end
    if( FALSE == FindNextFile(_findHandle, &_current) )
    {
        ::FindClose(_findHandle);
        _findHandle = INVALID_HANDLE_VALUE;
        _name.clear();
        return false;
    }

    _name = win32::toMultiByte( _current.cFileName );
    return true;
}


const char* DirectoryIteratorImpl::name() const
{
    if(_findHandle != INVALID_HANDLE_VALUE)
    {
        return _name.c_str();
    }

    return "";
}


const char* DirectoryIteratorImpl::path() const
{
    if(_findHandle != INVALID_HANDLE_VALUE)
    {
        if(_dirty)
        {
            // build complete path, ctor makes sure there is always a trailing 
            // slash and one character following it so idx+1 works out
            std::string::size_type idx = _path.rfind('\\') + 1;
            _path.replace(idx, _path.size(), win32::toMultiByte( _current.cFileName ) );
        }

        return _path.c_str();
    }
    
    return "";
}



bool DirectoryImpl::exists(const std::string& path)
{
    std::basic_string<TCHAR> str = win32::fromMultiByte( path );

    DWORD file_attr = ::GetFileAttributes( str.c_str() );
    
    return (file_attr != 0xffffffff) && (file_attr & FILE_ATTRIBUTE_DIRECTORY);
}


void DirectoryImpl::create(const std::string& path)
{
    std::basic_string<TCHAR> str = win32::fromMultiByte( path );

    if( FALSE == ::CreateDirectory(str.c_str(), NULL) )
    {
        throw SystemError("Could not create directory '" + path + "'", PT_SOURCEINFO);
    }
}


void DirectoryImpl::move(const std::string& oldName, const std::string& newName)
{
    std::basic_string<TCHAR> from = win32::fromMultiByte( oldName );
    std::basic_string<TCHAR> to   = win32::fromMultiByte( newName );

    #ifdef _WIN32_WCE
    
        if( FALSE == ::MoveFile( from.c_str(), to.c_str() ) )
        {
            throw SystemError("Could not move directory" , PT_SOURCEINFO);
        }
        
    #else
    
        if( FALSE == ::MoveFileEx( from.c_str(), to.c_str(), MOVEFILE_COPY_ALLOWED) )
        {
            throw SystemError("Could not move/rename directory '" + from + "' to '" + to + "'", PT_SOURCEINFO);
        }
            
    #endif
}


void DirectoryImpl::remove(const std::string& path)
{
    std::basic_string<TCHAR> str = win32::fromMultiByte( path );

    if( FALSE == ::RemoveDirectory( str.c_str() ) )
    {
        throw SystemError("Could not remove directory '" + path + "'" , PT_SOURCEINFO);
    }
}


void DirectoryImpl::changeCurrent(const std::string& path)
{
    #ifdef _WIN32_WCE
    
        throw std::runtime_error("SetCurrentDirectory not supported." + PT_SOURCEINFO);
        
    #else
    
        if (FALSE == ::SetCurrentDirectory(path.c_str()))
        {
            throw SystemError("Could not change current directory to '" + path + "'", PT_SOURCEINFO);
        }
        
    #endif
}

} // namespace System
} // namespace Pt
