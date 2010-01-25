/*
 * Copyright (C) 2009-2010 by Dr. Marc Boris Drner
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
#ifndef PT_REFLEX_TYPE_H
#define PT_REFLEX_TYPE_H

#include <Pt/Reflex/Api.h>
#include <Pt/Reflex/Context.h>
#include <Pt/Reflex/MethodInfo.h>
#include <Pt/Any.h>
#include <Pt/TypeInfo.h>
#include <Pt/TypeTraits.h>
#include <map>
#include <string>

namespace Pt {

namespace Reflex {

class PT_REFLEX_API Type
{
    typedef std::map<std::string, CallableInfo*> MethodMap;
    typedef std::map<std::string, PropInfo*> PropertyMap;

    public:
        Type(const std::type_info& ti, Context& context);

        virtual ~Type();

        virtual void registerMethods() = 0;

        template <typename R, typename T, typename A>
        void registerMethod( const char* name, R (*method)(T&, A) )
        {
            MethodInfo<R, T, A>* mi = new MethodInfo<R, T, A>(*_context, name, method);
            _methods[name] = mi;
        }

        template <typename R, typename C, typename A>
        void registerProperty( const char* name, R (C::*getter)() const, void (C::*setter)(A) )
        {
            ReadWritePropInfo<R, C, A>* pi = new ReadWritePropInfo<R, C, A>(name, getter, setter);
            _properties[name] = pi;
        }

        CallableInfo& method(const char* name);

        PropInfo& property(const char* name);

    private:
        Context* _context;
        MethodMap _methods;
        PropertyMap _properties;
};

}

}

#endif
