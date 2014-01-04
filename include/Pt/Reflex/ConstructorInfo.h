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
#ifndef PT_REFLEX_CONSTRUCTORINFO_H
#define PT_REFLEX_CONSTRUCTORINFO_H

#include <Pt/Reflex/Api.h>
#include <Pt/Reflex/Argument.h>
#include <cstddef>

namespace Pt {

class Any;

namespace Reflex {

class Type;

class ConstructorInfo
{
    public:
        virtual ~ConstructorInfo()
        {}

        //virtual void declare(TypeManager& tm) = 0;

        //virtual void define(TypeManager& tm) = 0;

        std::size_t psize() const
        { return _nargs; }

        Pt::Reflex::Type** params()
        { return _ptypes; }

        virtual void call(void* instance, const ArgumentList& args) = 0;

        void call(void* instance, Pt::Reflex::Argument* args, std::size_t n)
        {
            ArgumentList alist(args, n);
            this->call(instance, alist);
        }

        unsigned ref()
        { return ++_refs; }

        unsigned unref()
        { return --_refs; }

        unsigned refs() const
        { return _refs; }

    protected:
        ConstructorInfo(unsigned refs)
        : _refs(refs)
        , _nargs(0)
        {}

        ConstructorInfo()
        : _refs(0)
        , _nargs(0)
        {}

        void init(Pt::Reflex::Type** atypes, std::size_t nargs)
        {
            _ptypes = atypes;
            _nargs = nargs;
        }

        ConstructorInfo(const ConstructorInfo&);
        ConstructorInfo& operator=(const ConstructorInfo&);

    private:
        unsigned _refs;
        Pt::Reflex::Type** _ptypes;
        std::size_t _nargs;
};

}

}

#endif
