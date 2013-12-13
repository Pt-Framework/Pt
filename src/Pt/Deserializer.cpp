/*
 * Copyright (C) 2008 by Marc Boris Duerner
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

#include <Pt/Deserializer.h>
#include <cassert>

namespace Pt {

Deserializer::Deserializer()
: _context(0)
, _fmt(0)
, _current(0)
, _mem(0)
, _memsize(0)
{}


Deserializer::~Deserializer()
{
    if(_current)
    {
        _current->~Composer();
    }

    this->deallocate(_mem);
}


SerializationContext* Deserializer::context()
{
    return _context;
}


void Deserializer::reset(SerializationContext* context)
{
    this->clear();
    _context = context;
}


Formatter* Deserializer::formatter()
{
    return _fmt;
}


void Deserializer::setFormatter(Formatter& formatter)
{
    _fmt = &formatter;
}


void Deserializer::clear()
{
    if(_context)
        _context->clear();

    if(_current)
    {
        _current->~Composer();
        _current = 0;
    }
}


bool Deserializer::advance()
{
    if( ! _current || ! _fmt )
        return false;

    bool finished = _fmt->parseSome();
    if(finished)
    {
        _current->~Composer();
        _current = 0;
    }

    return finished;
}


void Deserializer::finish()
{
    if(_current)
    {
        if(_fmt)
            _fmt->parse();
        
        _current->~Composer();
        _current = 0;
    }
}


void Deserializer::fixup()
{
    assert(_current == 0);

    if(_context)
        _context->fixup();
}


void* Deserializer::allocate(size_t n)
{
    if(_current)
    {
        _current->~Composer();
        _current = 0;
    }

    if(n < _memsize)
        return _mem;

    if(_mem)
        this->deallocate(_mem);

    _mem = ::operator new( n );
    return _mem;
}


void Deserializer::deallocate(void* p)
{
    ::operator delete (p);
}

} // namespace Pt
