/*
 * Copyright (C) 2013 Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "../win32/win32.h"
#include "DirectoryImpl.h"
#include "FileInfoImpl.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/IOError.h"
#include <windows.h>

//using namespace Platform;
//using namespace Windows::Storage;

namespace Pt {

namespace System {

DirectoryIteratorImpl::DirectoryIteratorImpl(const std::string& path)
: _refs(1)
, _findHandle(INVALID_HANDLE_VALUE)
{
    init(path.c_str(), path.size());
}


DirectoryIteratorImpl::DirectoryIteratorImpl(const char* path)
: _refs(1)
, _findHandle(INVALID_HANDLE_VALUE)
{
    init(path, std::strlen(path));
}


DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
    if(_findHandle != INVALID_HANDLE_VALUE)
        ::FindClose(_findHandle);
}


void DirectoryIteratorImpl::init(const char* path, std::size_t pathlen)
{
    std::wstring wpath;
    win32::fromMultiByte( path, wpath );

    if( pathlen > 0 && path[pathlen-1] != '\\' )
        wpath += '\\';

    wpath += L'*';

    _findHandle = FindFirstFileExW( wpath.c_str(), 
                                    FindExInfoStandard,
                                    &_current,
                                    FindExSearchNameMatch,
                                    NULL,
                                    0 );

    if(_findHandle == INVALID_HANDLE_VALUE)
        throw AccessFailed(path);
}


bool DirectoryIteratorImpl::advance()
{
    if( FALSE == FindNextFileW(_findHandle, &_current) )
    {
        ::FindClose(_findHandle);

        // INVALID_HANDLE_VALUE means end iterator
        _findHandle = INVALID_HANDLE_VALUE;
        _finfo.clear();
        return false;
    }

    _finfo.path() = win32::toMultiByte( _current.cFileName );
    return true;
}


//const std::string& DirectoryIteratorImpl::path() const
//{
//    if(_dirty)
//    {
//        // replace substring after last slash with the new file-name or
//        // append the file-name if we have a trailing slash. Ctor makes
//        // sure we have a trailing slash.
//        std::string::size_type idx = _path.rfind('\\');
//        if(idx != std::string::npos && ++idx < _path.size() )
//        {
//            _path.replace(idx, _path.size(), win32::toMultiByte( _current.cFileName ) );
//        }
//        else
//        {
//            _path += win32::toMultiByte( _current.cFileName );
//        }
//    }
//
//    return _path;
//}
//
//
//void DirectoryImpl::create(const std::string& path)
//{
//    std::wstring wpath;
//    win32::fromMultiByte( path, wpath );
//
//    if( FALSE == ::CreateDirectoryW(wpath.c_str(), NULL) )
//        throw AccessFailed(path);
//}
//
//
//void DirectoryImpl::move(const std::string& from, const std::string& to)
//{
//    std::wstring wfrom;
//    win32::fromMultiByte( from, wfrom );
//    
//    std::wstring wto;
//    win32::fromMultiByte( to, wto );
//
//    if( FALSE == ::MoveFileExW( wfrom.c_str(), wto.c_str(), MOVEFILE_COPY_ALLOWED) )
//        throw AccessFailed(from);
//}
//
//
//void DirectoryImpl::remove(const std::string& path)
//{
//    std::wstring wpath;
//    win32::fromMultiByte( path, wpath );
//
//    if( FALSE == ::RemoveDirectoryW( wpath.c_str() ) )
//        throw AccessFailed(path);
//}
//
//
//void DirectoryImpl::chdir(const std::string& path)
//{
//    throw AccessFailed("chdir not supported");
//}
//
//
//std::string DirectoryImpl::cwd()
//{
//    // Windows.Storage
//
//    // might want RoamingFolder?
//    String^ path = ApplicationData::Current->LocalFolder->Path;
//    return win32::toMultiByte( path->Data() );
//}
//
//
//std::string DirectoryImpl::tmpdir()
//{
//    // Windows.Storage
//    String^ path = ApplicationData::Current->TemporaryFolder->Path;
//    return win32::toMultiByte( path->Data() );
//}

} // namespace System

} // namespace Pt
