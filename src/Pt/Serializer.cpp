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

#include <Pt/Serializer.h>
#include <Pt/SerializationContext.h>
#include <Pt/Formatter.h>

namespace Pt {

Serializer::Serializer()
: _context(0)
, _formatter(0)
, _current(0)
{}


Serializer::~Serializer()
{
    std::vector<Decomposer*>::iterator it;
    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        (*it)->~Decomposer();
        this->deallocate(*it);
    }

    _stack.clear();
}


SerializationContext* Serializer::context()
{
    return _context;
}


void Serializer::reset(SerializationContext* context)
{
    this->clear();
    _context = context;
}


Formatter* Serializer::formatter()
{
    return _formatter;
}


void Serializer::setFormatter(Formatter& formatter)
{
    _formatter = &formatter;
}


void Serializer::clear()
{
    if(_context)
        _context->clear();

    std::vector<Decomposer*>::iterator it;
    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        (*it)->~Decomposer();
        this->deallocate(*it);
    }

    _stack.clear();
    _current = 0;
}


bool Serializer::advance()
{
    if(_current)
    {
        _current = _current->advanceFormat(*_formatter);
        if( _current )
            return false;

        _stack.front()->~Decomposer();
        this->deallocate( _stack.front() );
        _stack.erase( _stack.begin() );
    }

    if( _stack.empty() )
        return true;

    _current = _stack.front();
    _current->beginFormat(*_formatter);
    return false;
}


void Serializer::finish()
{
    if(_current)
    {
        while(_current)
        {
            _current = _current->advanceFormat(*_formatter);
        }

        _stack.front()->~Decomposer();
        this->deallocate( _stack.front() );
        _stack.erase( _stack.begin() );
    }

    std::vector<Decomposer*>::iterator it;

    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        (*it)->format(*_formatter);
        (*it)->~Decomposer();
        this->deallocate(*it);
    }

    _stack.clear();
    _current = 0;
}


void* Serializer::allocate(size_t n)
{
    return ::operator new( n );
}


void Serializer::deallocate(void* p)
{
    ::operator delete (p);
}

} // namespace Pt
