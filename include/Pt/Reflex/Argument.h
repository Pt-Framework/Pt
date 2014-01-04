/*
 * Copyright (C) 2004-2010 by Marc Boris Duerner
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
#ifndef PT_REFLEX_ARGUMENT_H
#define PT_REFLEX_ARGUMENT_H

#include <Pt/Reflex/Api.h>
#include <Pt/Any.h>
#include <cstddef>

namespace Pt {

namespace Reflex {

class Type;

class Argument
{
    public:
        Argument(const Pt::Any& a, Pt::Reflex::Type& type)
        : _v(a)
        , _type(&type)
        { }

        Argument& operator=(const Argument& other)
        {
            _v = other._v;
            _type = other._type;
            return *this;
        }

        void* get()
        { return _v.get(); }

        const void* get() const
        { return _v.get(); }

        Pt::Reflex::Type& type()
        { return *_type; }

        const Pt::Reflex::Type& type() const
        { return *_type; }

        Pt::Any& toAny()
        { return _v; }

        const Pt::Any& toAny() const
        { return _v; }

    protected:
        Argument(Pt::Reflex::Type& type)
        : _type(&type)
        { }

        void setType(Pt::Reflex::Type& type)
        { _type = &type; }

    private:
        Pt::Any _v;
        Pt::Reflex::Type* _type;
};


class ArgumentIterator
{
    public:
        ArgumentIterator(Argument* args, std::size_t stride)
        : _arg(args)
        , _stride(stride)
        {}

        ArgumentIterator& operator++()
        {
            char* p = (char*)(_arg);
            p += _stride;
            _arg = (Argument*)(p);
            return *this;
        }

        ArgumentIterator operator+(std::size_t offset) const
        {
            char* p = (char*)(_arg);
            p += (_stride * offset);
            Argument* pos = (Argument*)(p);
            return ArgumentIterator(pos, _stride);
        }

        Argument& operator*() const
        { return *_arg; }

        Argument* operator->() const
        { return _arg; }

        std::size_t stride() const
        { return _stride; }

        Argument* get() const
        { return _arg; }

    private:
        Argument* _arg;
        const std::size_t _stride;
};


class ArgumentList
{
    public:
        ArgumentList()
        : _begin( 0, sizeof(Argument) )
        , _length(0)
        {}

        template <typename T>
        ArgumentList(T* args, std::size_t length)
        : _begin( args, sizeof(T) )
        , _length(length)
        {}

        ArgumentList(Argument* args, std::size_t length, std::size_t stride)
        : _begin(args, stride)
        , _length(length)
        {}

        const ArgumentIterator& begin() const
        { return _begin; }

        ArgumentIterator end() const
        { return _begin + _length; }

        std::size_t size() const
        { return _length; }

    private:
        ArgumentIterator _begin;
        std::size_t _length;
};

}

}

#endif
