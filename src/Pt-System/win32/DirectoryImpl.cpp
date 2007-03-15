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
#include "Pt/System/FileSystem.h"

#include <vector>

#include <windows.h>


namespace Pt {

namespace System {

DirectoryIteratorImpl::DirectoryIteratorImpl()
: _refs(1),
  _node(0),
  _findHandle(INVALID_HANDLE_VALUE)
{
}


DirectoryIteratorImpl::DirectoryIteratorImpl(const char* path)
: _refs(1),
  _node(0),
  _findHandle(INVALID_HANDLE_VALUE)
{
    std::string firstFile = path;
    if( !firstFile.empty() && firstFile[firstFile.size()-1] != '\\' )
        firstFile += "\\";

    firstFile += '*';

    std::basic_string<TCHAR> tpath = win32::fromMultiByte( firstFile );
    _findHandle = FindFirstFile( tpath.c_str(), &_current );

    if(_findHandle == INVALID_HANDLE_VALUE)
        throw SystemError("Could not open find handle.", PT_SOURCEINFO);

    _path = path;
}


DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
    delete _node;

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


void DirectoryIteratorImpl::advance()
{
    // cannot advance an unintialised iterator
    if(_findHandle == INVALID_HANDLE_VALUE) {
        return;
    }

    // the current node becomes invalid now
    delete _node;
    _node = 0;

    // _findHandle = INVALID_HANDLE_VALUE means end
    if( FALSE == FindNextFile(_findHandle, &_current) )
    {
        ::FindClose(_findHandle);
        _findHandle = INVALID_HANDLE_VALUE;
    }
}


FileSystemNode& DirectoryIteratorImpl::node()
{
    // reuse previously created node
    if(_node)
        return *_node;

    // build complete path
    std::string path = _path;
    if( !path.empty() && path[path.size()-1] != '\\')
        path += '\\';
    path += this->name();

    // create file system node by full path
    _node = FileSystem::instance().create( path.c_str() );
    if(!_node)
        throw SystemError("Unknown file system node", PT_SOURCEINFO);

    return *_node;
}


std::string DirectoryIteratorImpl::name() const
{
    if(_findHandle != INVALID_HANDLE_VALUE)
        return win32::toMultiByte( _current.cFileName );

    return "";
}

bool DirectoryIteratorImpl::operator==(const DirectoryIteratorImpl& impl) const
{
    return _findHandle == impl._findHandle;
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


std::string DirectoryImpl::current()
{
    #ifdef _WIN32_WCE
    
        throw std::runtime_error("GetCurrentDirectory not supported." + PT_SOURCEINFO);
        
    #else
    
        char path[MAX_PATH+2];
        DWORD len = ::GetCurrentDirectory(MAX_PATH+2, path);
        return std::string(path, len);
        
    #endif
}


std::string DirectoryImpl::system()
{
    return "c:\\";
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
