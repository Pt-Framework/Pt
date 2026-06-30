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

#include "PathImpl.h"
#include <Pt/System/Path.h>
#include <Pt/Utf8Codec.h>

namespace Pt {

namespace System {

Path::Path()
{
    _impl = new PathImpl();
}


Path::Path(const Path& p)
{
    _impl = new PathImpl(*p._impl);
}


Path::Path(const Pt::String& s)
{
    _impl = new PathImpl();
    _impl->append( s.c_str(), s.size() );
}


Path::Path(const char* s)
{
    std::size_t n = std::strlen(s);
    _impl = new PathImpl();
    concat(s, n);
}


Path::Path(const char* s, std::size_t n)
{
    _impl = new PathImpl();
    concat(s, n);
}


Path::~Path()
{
    delete _impl;
}


Path& Path::assign(const Path& p)
{
    *_impl = *p._impl;
    return *this;
}


Path& Path::assign(const Pt::String& s)
{
    _impl->clear();
    _impl->append( s.c_str(), s.size() );
    return *this;
}


Path& Path::assign(const char* s)
{
    std::size_t n = std::strlen(s);
    assign(s, n);
    return *this;
}


Path& Path::assign(const char* s, std::size_t n)
{
    _impl->clear();
    concat(s, n);
    return *this;
}


Path& Path::append(const Path& p)
{
    if( ! empty() && ! p.empty() &&
        _impl->back() != _impl->dirsep() &&
        p._impl->front() != _impl->dirsep() )
    {
        _impl->push_back( _impl->dirsep() );
    }

    _impl->append(*p._impl);
    return *this;
}


Path& Path::append(const Pt::String& s)
{
    if( ! empty() && ! s.empty() &&
        _impl->back() != _impl->dirsep() &&
        s[0] != Char(_impl->dirsep()) &&
        s[0] != Char('/') )
    {
        _impl->push_back( _impl->dirsep() );
    }

    _impl->append( s.c_str(), s.size() );
    return *this;
}


Path& Path::append(const char* s)
{
    std::size_t n = std::strlen(s);
    append(s, n);
    return *this;
}


Path& Path::append(const char* s, std::size_t n)
{
    if( ! empty() && n > 0 &&
        _impl->back() != _impl->dirsep() &&
        s[0] != _impl->dirsep() &&
        s[0] != '/' )
    {
        _impl->push_back( _impl->dirsep() );
    }

    concat(s, n);
    return *this;
}


Path& Path::concat(const Path& p)
{
    _impl->append(*p._impl);
    return *this;
}


Path& Path::concat(const Pt::String& s)
{
    _impl->append( s.c_str(), s.size() );
    return *this;
}


Path& Path::concat(const char* s)
{
    std::size_t size = std::strlen(s);
    concat(s, size);
    return *this;
}


Path& Path::concat(const char* s, std::size_t n)
{
    if( _impl->append(s, n) )
    {
        return *this;
    }

    Pt::Char to[32];
    Pt::Char* toEnd = to + 32;
    const char* from  = s;
    const char* fromEnd = s + n;

    MBState state;
    std::codecvt_base::result r;
    Pt::Utf8Codec codec;

    do
    {
        Pt::Char* toNext = to;
        r = codec.in(state, from, fromEnd, from, to, toEnd, toNext);

        if (r == std::codecvt_base::error)
            _impl->append("?????", 5);
        else
            _impl->append(to, toNext - to);
    }
    while(r == std::codecvt_base::partial);

    return *this;
}


void Path::clear()
{
    _impl->clear();
}


bool Path::empty() const
{
    return _impl->empty();
}


Pt::String Path::fileName() const
{
    PathImpl::size_type pos = _impl->rfind( _impl->dirsep() );
    if(pos == PathImpl::npos())
        return toString();

    std::size_t len = _impl->size();
    if( ++pos < len )
        return _impl->substr(pos, len - pos);

    return Pt::String();
}


Pt::String Path::dirName() const
{
    if( _impl->empty() || (_impl->size() == 1 && _impl->back() == _impl->dirsep()) )
        return Pt::String();

    PathImpl::size_type off = PathImpl::npos();

    if( _impl->back() == _impl->dirsep() )
    {
        off = _impl->size() - 2;
    }

    PathImpl::size_type pos = _impl->rfind( _impl->dirsep(), off );
    if (pos != PathImpl::npos())
        return _impl->substr(0, pos + 1);

    return Pt::String();
}


Pt::String Path::baseName() const
{
    Pt::String file = fileName();

    Pt::String::size_type extPos = file.rfind('.');
    if(extPos != Pt::String::npos)
    {
        file.erase(extPos);
    }

    return file;
}


Pt::String Path::extension() const
{
    Pt::String s;

    PathImpl::size_type sepPos = _impl->rfind( _impl->dirsep() );
    if(sepPos == PathImpl::npos())
        sepPos = 0;

    PathImpl::size_type extPos = _impl->rfind( _impl->extsep() );

    std::size_t len = _impl->size();
    if(extPos != PathImpl::npos() && sepPos < extPos && ++extPos < len )
    {
        s = _impl->substr(extPos, len - extPos);
    }

    return s;
}


int Path::compare(const Path& p) const
{
    return _impl->compare(*p._impl);
}


Pt::String Path::toString() const
{
    return _impl->toString();
}


std::string Path::toLocal() const
{
    return _impl->toLocal();
}


Pt::String Path::toGeneric() const
{
    return _impl->toGeneric();
}


Pt::String Path::dirsep()
{
    return Pt::String(1, PathImpl::dirsep());
}


Pt::String Path::curdir()
{
    return ".";
}


Pt::String Path::updir()
{
    return "..";
}

} // namespace System

} // namespace Pt
