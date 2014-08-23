/*
 * Copyright (C) 2006-2014 Marc Boris Duerner
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

#include "win32.h"
#include <Pt/WinVer.h>
#include <Pt/String.h>
#include <string>

namespace Pt {

namespace System {

class PathImpl
{
    public:
        PathImpl()
        { }

        void clear()
        { _path.clear(); }

        void append(const PathImpl& p)
        {
            _path += p._path;
        }

        void append(const Pt::Char* s, std::size_t n)
        {
            Pt::String(s, n).toUtf16( std::back_inserter(_path) );
        }

        bool append(const char* from, std::size_t size)
        {
            return false;
        }

        void appendSlash()
        {
            _path += '\\';
        }

        Pt::String toString() const
        {
            return Pt::String::fromUtf16( _path.begin(), _path.end() );
        }

        std::string toLocal() const
        {
            return win32::toMultiByte( _path.c_str() );
        }

        static const char* slash() 
        { return "\\"; }

    private:
        std::wstring _path;
};

} // namespace System

} // namespace Pt
