/*
 * Copyright (C) 2006-2013 Marc Boris Duerner
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

#include "FileInfoImpl.h"
#include <Pt/System/FileInfo.h>
#include <Pt/System/Directory.h>
#include <Pt/System/Logger.h>

PT_LOG_DEFINE("Pt.System.FileInfo")

namespace Pt {

namespace System {

FileInfo::FileInfo(const Path& path)
: _path(path)
{
}


FileInfo::FileInfo(const Pt::String& path)
: _path(path)
{
}


FileInfo::FileInfo(const char* path)
: _path(path)
{
}


void FileInfo::clear()
{
    _path.clear();
}


FileInfo::Type FileInfo::type(const Path& path)
{
    return FileInfoImpl::getType(path);
}


bool FileInfo::isLink(const Path& path)
{
    return FileInfoImpl::linkStatus(path) == FileInfo::Link;
}


uint64_t FileInfo::size(const Path& path)
{
    return FileInfoImpl::size(path);
}


DateTime FileInfo::lastModified(const Path& path)
{
    return FileInfoImpl::lastModified(path);
}


void FileInfo::createFile(const Path& path)
{
    FileInfoImpl::createFile(path);
}


void FileInfo::createSymlink(const Path& target, const Path& link)
{
    FileInfoImpl::createSymlink(target, link);
}


void FileInfo::createHardlink(const Path& target, const Path& link)
{
    FileInfoImpl::createHardlink(target, link);
}


void FileInfo::createDirectory(const Path& path)
{
    PT_LOG_DEBUG("created: " << path.toLocal());
    FileInfoImpl::createDirectory(path);
}


void FileInfo::createDirectories(const Path& path)
{
    if( path.empty() || FileInfo::exists(path) )
      return;

    Pt::System::Path subDir( path.dirName() );
    createDirectories(subDir);

    if( ! FileInfo::exists(path) )
        createDirectory(path);
}


void FileInfo::resize(const Path& path, Pt::uint64_t n)
{
    FileInfoImpl::resize(path, n);
}


void FileInfo::remove(const Path& path)
{
    FileInfoImpl::remove(path);
}


void FileInfo::removeAll(const Pt::System::Path& path)
{
    Pt::System::DirectoryIterator end;

    for(Pt::System::DirectoryIterator it(path); it != end; ++it)
    {
        Pt::String fileName = it->path().fileName();

        if(fileName.empty() || fileName == "." || fileName == "..")
            continue;

        Pt::System::Path subPath = path / it->path();
        Pt::System::FileInfo::Type fileType = FileInfo::type(subPath);

        if(fileType == Pt::System::FileInfo::Link)
        {
            Pt::System::FileInfo::remove(subPath);
            PT_LOG_DEBUG( "removed link: " << subPath.toLocal() );
        }
        else if(fileType == Pt::System::FileInfo::Directory)
        {
            FileInfo::removeAll(subPath);
            PT_LOG_DEBUG( "removing dir: " << subPath.toLocal() );
        }
        else // Pt::System::FileInfo::File
        {
            Pt::System::FileInfo::remove(subPath);
            PT_LOG_DEBUG( "removed: " << subPath.toLocal() );
        }
    }

    Pt::System::FileInfo::remove(path);
}


void FileInfo::move(const Path& path, const Path& to)
{
    FileInfoImpl::move(path, to);
}


FileInfo::Perms FileInfo::permissions(const Path& path)
{
    return FileInfoImpl::permissions(path);
}


void FileInfo::permissions(const Path& path, Perms prms, PermOptions opts)
{
    FileInfoImpl::permissions(path, prms, opts);
}

} // namespace System

} // namespace Pt
