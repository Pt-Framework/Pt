/*
 * Copyright (C) 2005-2010 by Dr. Marc Boris Duerner
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

#include "FileChannel.h"
#include <Pt/System/Uri.h>
#include <Pt/System/FileInfo.h>
#include <sstream>
#include <cctype>

namespace Pt {

namespace System {

FileChannel::FileChannel()
: LogChannel()
, _maxSize(1*1024*1024)
, _numBackup(1)
, _curSize(0)
{
}


FileChannel::~FileChannel()
{
}


void FileChannel::onOpen(const std::string& urlstring)
{
    _maxSize = 1*1024*1024;
    _numBackup = 1;
    Pt::System::Uri url(urlstring);

    enum {
      Key, Value
    } state = Key;

    // adding a '&' to the end makes the parser simpler
    std::string query = url.query();
    if( ! query.empty() )
        query += '&';

    std::string key, value;
    for(std::string::iterator it = query.begin(); it != query.end(); ++it)
    {
        char ch = *it;
        switch(state)
        {
            case Key:
                if(std::isalpha(ch))
                    key += ch;
                else if(ch == '=')
                    state = Value;
                else
                    throw std::invalid_argument("invalid query string");

                break;

            case Value:
                if(std::isalnum(ch))
                    value += ch;
                else if(ch == '&')
                {
                    if(key == "size")
                    {
                        std::istringstream ss(value);
                        ss >> _maxSize;
                    }
                    else if(key == "files")
                    {
                        unsigned maxFile = 1;
                        std::istringstream ss(value);
                        ss >> maxFile;
                        _numBackup = maxFile - 1;
                    }
                    key.clear();
                    value.clear();
                    state = Key;
                }
                else
                    throw std::invalid_argument("invalid query string");

                break;

            default:
                throw std::invalid_argument("invalid query string");
                break;
        }
    }

    _file = url.path();

    _fs.open(_file.c_str(), std::ios_base::out | std::ios_base::app);
    std::streamoff pos = _fs.tellp();

    if( !_fs || pos < 0)
        throw std::invalid_argument("invalid file name: " + _file);

    _curSize = static_cast<std::size_t>(pos);

    if(_curSize > _maxSize)
    {
        this->rotate();
    }
}


void FileChannel::onClose()
{
    _fs.clear();
    _fs.close();
    _curSize = 0;
}


void FileChannel::onWrite(const char* msg, std::size_t msglen)
{
    if(_curSize + msglen > _maxSize)
    {
        this->rotate();
    }

    _fs.write(msg, msglen) << std::flush;
    _curSize += msglen;
}


void FileChannel::rotate()
{
    _fs.clear();
    _fs.close();

    if(_numBackup > 0)
    {
        std::string to = makePath(_numBackup);

        if( Pt::System::FileInfo::exists(to) ) 
            Pt::System::FileInfo::remove(to);

        for(std::size_t n = _numBackup; n > 0 ; --n)
        {
            std::string from = makePath(n-1);
            
            if( Pt::System::FileInfo::exists(from) ) 
                Pt::System::FileInfo::move(from, to);
            
            to = from;
        }
    }

    _fs.open(_file.c_str(), std::ios_base::out | std::ios_base::trunc);
    _curSize = 0;
}


std::string FileChannel::makePath(std::size_t n)
{
    if(n == 0)
        return _file;

    std::ostringstream oss;
    oss << _file << '.' << n;
    return oss.str();
}

} // namespace System

} // namespace Pt
