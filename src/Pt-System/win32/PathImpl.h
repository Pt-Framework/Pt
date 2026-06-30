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
#include <cwchar>

namespace Pt {

namespace System {

class PathImpl
{
    public:
        typedef std::wstring::size_type size_type;

        static const std::string::size_type nullSize = sizeof(wchar_t) - 1;

    public:
        PathImpl()
        : _path(nullSize, '\0')
        { }

        void clear()
        {
            _path.clear();
            _path.assign(nullSize, '\0');
        }

        bool empty() const
        { return _path.size() <= nullSize; }

        std::size_t size() const
        {
            // null terminator is part of path data
            return (_path.size() - nullSize) / sizeof(wchar_t);
        }

        const wchar_t* c_str() const
        { return reinterpret_cast<const wchar_t*>( _path.c_str() ); }

        wchar_t front() const
        {
            return *c_str();
        }

        wchar_t back() const
        {
            std::size_t n = size() - 1;
            return *(c_str() + n);
        }

        void assign(const wchar_t* p)
        {
            //_path = p;

            const char* bytes = reinterpret_cast<const char*>(p);
            std::size_t bytesLen = std::wcslen(p) * sizeof(wchar_t);
            _path.assign(bytes, bytesLen);

            // append null terminator part
            _path.append(nullSize, '\0');

            makeNative();
        }

        void append(const PathImpl& p)
        {
            //_path += p._path;

            // remove null terminator part
            if( ! _path.empty() )
                _path.resize( _path.size() - nullSize );

            // also appends null terminator part
            _path += p._path;
        }

        void append(const Pt::Char* s, std::size_t n)
        {
            //Pt::String(s, n).toUtf16( std::back_inserter(_path) );

            // remove null terminator part
            if( ! _path.empty() )
                _path.resize( _path.size() - nullSize );

            Pt::String tmp(s, n);
            std::wstring wpath;
            tmp.toUtf16( std::back_inserter(wpath) );

            const char* bytes = reinterpret_cast<const char*>( wpath.c_str() );
            std::size_t bytesLen = wpath.size() * sizeof(wchar_t);
            _path.append(bytes, bytesLen);

            // append null terminator part
            _path.append(nullSize, '\0');

            makeNative();
        }

        bool append(const char*, std::size_t)
        {
            // no direct assign from UTF-8 possible
            return false;
        }

        void push_back(wchar_t p)
        {
            //_path.push_back(c);

            if(p == L'/')
                p = L'\\';

            // remove null terminator part
            if( ! _path.empty() )
                _path.resize( _path.size() - nullSize );

            const char* bytes = reinterpret_cast<const char*>(&p);
            _path.append(bytes, sizeof(wchar_t));

            // append null terminator part
            _path.append(nullSize, '\0');
        }

        static size_type npos()
        { return std::string::npos; }

        size_type rfind(wchar_t ch, size_type pos = npos()) const
        {
            //return _path.rfind(ch);

            std::size_t n = _path.size() - nullSize;

            if( pos != npos() )
                n = (pos + 1) * sizeof(wchar_t);

            while(n != 0)
            {
                n -= sizeof(wchar_t);

                const char* c = &_path[n];
                const wchar_t* w = reinterpret_cast<const wchar_t*>(c);
                if(*w == ch)
                    return n / sizeof(wchar_t);
            }

            return npos();
        }

        Pt::String substr(size_type pos, size_type n)
        {
            //std::wstring::iterator from = _path.begin() + pos;
            //Pt::String tmp = Pt::String::fromUtf16(from, from + n);
            //return tmp;

            const wchar_t* from = c_str() + pos;
            Pt::String tmp = Pt::String::fromUtf16(from, from + n);
            return tmp;
        }

        int compare(const PathImpl& p) const
        {
            // return _path.compare(p._path);

            return std::wcscmp( c_str(), p.c_str() );
        }

        Pt::String toString() const
        {
            //return Pt::String::fromUtf16( _path.begin(), _path.end() );

            const wchar_t* from = c_str();
            std::size_t n = size();
            return Pt::String::fromUtf16(from, from + n);
        }

        std::string toLocal() const
        {
            return win32::toMultiByte( c_str() );
        }

        Pt::String toGeneric() const
        {
            const wchar_t* from = c_str();
            std::size_t n = size();
            Pt::String str = Pt::String::fromUtf16(from, from + n);

            for(std::size_t i = 0; i < str.size(); ++i)
            {
                if(str[i] == Pt::Char('\\'))
                    str[i] = Pt::Char('/');
            }

            return str;
        }

        static char dirsep()
        { return '\\'; }

        static char extsep()
        { return '.'; }

    private:
        void makeNative()
        {
            std::size_t n = _path.size() - nullSize;

            for(std::size_t i = 0; i < n; i += sizeof(wchar_t))
            {
                wchar_t* w = reinterpret_cast<wchar_t*>(&_path[i]);
                if(*w == L'/')
                    *w = L'\\';
            }
        }

        std::string _path;
};

} // namespace System

} // namespace Pt
