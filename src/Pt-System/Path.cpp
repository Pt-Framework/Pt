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
    std::size_t size = std::strlen(s);
    _impl = new PathImpl();
    _impl->append(s, size);
}


Path::~Path()
{
    delete _impl;
}


Path& Path::operator=(const Path& p)
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
    std::size_t size = std::strlen(s);
    _impl->clear();
    _impl->append(s, size);
    return *this;
}


Path& Path::append(const Path& p)
{
    _impl->appendSlash();
    _impl->append(*p._impl);
    return *this;
}


Path& Path::append(const Pt::String& s)
{
    _impl->appendSlash();
    _impl->append( s.c_str(), s.size() );
    return *this;
}


Path& Path::append(const char* from)
{
    std::size_t size = std::strlen(from);
    append(from, size);
    return *this;
}


Path& Path::append(const char* from, std::size_t size)
{
    _impl->appendSlash();

    if( _impl->append(from, size) )
    {
        return *this;
    }

    Pt::Char to[32];
    Pt::Char* toEnd = to + 32;
    const char* fromEnd = from + size;
    
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


Path& Path::concat(const Pt::String& s)
{
    _impl->append( s.c_str(), s.size() );
    return *this;
}


void Path::clear()
{
    _impl->clear();
}


Pt::String Path::toString() const
{
    return _impl->toString();
}


std::string Path::toLocal() const
{
    return _impl->toLocal();
}

} // namespace System

} // namespace Pt
