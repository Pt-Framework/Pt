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
#include "win32.h"
#include "DirectoryImpl.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/Directory.h"
#include "Pt/System/Process.h"
#include <vector>
#include <windows.h>

namespace Pt {

namespace System {

void throwError(DWORD error, const std::string& path, const Pt::SourceInfo& si);


void throwDirError(const std::string& path, const Pt::SourceInfo& si)
{
    DWORD error = GetLastError();
    switch(error)
    {
        case ERROR_BAD_PATHNAME:
        case ERROR_PATH_NOT_FOUND:
        case ERROR_OPEN_FAILED:
            throw DirectoryNotFound(path, si);

        default:
            throwError(error, path, si);
    }
}


DirectoryIteratorImpl::DirectoryIteratorImpl(const std::string& path)
: _refs(1),
  _path(path),
  _findHandle(INVALID_HANDLE_VALUE),
  _dirty(true)
{
    std::string firstFile = path;
    if( ! firstFile.empty() && firstFile[firstFile.size()-1] != '\\' )
        firstFile += '\\';

    firstFile += '*';

    std::basic_string<TCHAR> tpath;
    win32::fromMultiByte( firstFile, tpath );
    _findHandle = FindFirstFile( tpath.c_str(), &_current );

    if(_findHandle == INVALID_HANDLE_VALUE)
        throwDirError(path, PT_SOURCEINFO);

    _path = path;
    if( ! _path.empty() && _path[_path.size()-1] != '\\')
        _path += '\\';
}


DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
    if(_findHandle != INVALID_HANDLE_VALUE)
        ::FindClose(_findHandle);
}


bool DirectoryIteratorImpl::advance()
{
    // the current node becomes invalid now
    _dirty  = true;

    // _findHandle set to INVALID_HANDLE_VALUE means end
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


const std::string& DirectoryIteratorImpl::path() const
{
    if(_dirty)
    {
        // replace substring after last slash with the new file-name or
        // append the file-name if we have a trailing slash. Ctor makes
        // sure we have a trailing slash.
        std::string::size_type idx = _path.rfind('\\');
        if(idx != std::string::npos && ++idx < _path.size() )
        {
            _path.replace(idx, _path.size(), win32::toMultiByte( _current.cFileName ) );
        }
        else
        {
            _path += win32::toMultiByte( _current.cFileName );
        }
    }

    return _path;
}


void DirectoryImpl::create(const std::string& path)
{
    std::basic_string<TCHAR> str;
    win32::fromMultiByte( path, str );

    if( FALSE == ::CreateDirectory(str.c_str(), NULL) )
        throwDirError(path, PT_SOURCEINFO);
}


void DirectoryImpl::move(const std::string& oldName, const std::string& newName)
{
    std::basic_string<TCHAR> from;
    win32::fromMultiByte( oldName, from );
    std::basic_string<TCHAR> to;
    win32::fromMultiByte( newName, to );

    #ifdef _WIN32_WCE

        if( FALSE == ::MoveFile( from.c_str(), to.c_str() ) )
            throwDirError(oldName, PT_SOURCEINFO);

    #else

        if( FALSE == ::MoveFileEx( from.c_str(), to.c_str(), MOVEFILE_COPY_ALLOWED) )
            throwDirError(oldName, PT_SOURCEINFO);

    #endif
}


void DirectoryImpl::remove(const std::string& path)
{
    std::basic_string<TCHAR> str;
    win32::fromMultiByte( path, str );

    if( FALSE == ::RemoveDirectory( str.c_str() ) )
        throwDirError(path, PT_SOURCEINFO);
}


void DirectoryImpl::chdir(const std::string& path)
{
    #ifdef _WIN32_WCE

        throw std::runtime_error( PT_ERROR_MSG("SetCurrentDirectory not supported.") );

    #else

        if( FALSE == ::SetCurrentDirectory( path.c_str() ) )
            throwDirError(path, PT_SOURCEINFO);

    #endif
}


std::string DirectoryImpl::cwd()
{
#ifdef _WIN32_WCE

    throw std::runtime_error( PT_ERROR_MSG("DirectoryImpl::cwd not supported.") );

#else

    char path[MAX_PATH+2];
    DWORD len = ::GetCurrentDirectory(MAX_PATH+2, path);
    return std::string(path, len);

#endif
}


std::string DirectoryImpl::tmpdir()
{
    std::string tmpDir = Process::getEnvVar("TEMP");
    if (tmpDir.length() == 0)
    {
        tmpDir = Process::getEnvVar("TMP");
    }

    return tmpDir;
}

} // namespace System

} // namespace Pt
