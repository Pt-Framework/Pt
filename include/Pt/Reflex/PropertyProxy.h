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
#ifndef PT_REFLEX_PROPERTYPROXY_H
#define PT_REFLEX_PROPERTYPROXY_H

#include <Pt/Reflex/PropertyInfo.h>
#include <Pt/Reflex/TypeManager.h>
#include <Pt/Reflex/ArgumentTraits.h>
#include <Pt/Reflex/Type.h>
#include <Pt/Any.h>
#include <string>

namespace Pt {

namespace Reflex {

template <typename C, typename T>
class ReadWritePropertyProxy : public PropertyInfo
{
    public:
        typedef T (*Getter)(C&);
        typedef void (*Setter)(C&, T);

        ReadWritePropertyProxy(TypeManager& ctx, const std::string& name, Getter getter, Setter setter )
        : _name(name)
        , _getter(getter)
        , _setter(setter)
        {
            _type = ctx.getType( typeid(T) );
        }

        ~ReadWritePropertyProxy()
        { }

        virtual Type& type()
        {
            return *_type;
        }

        virtual const char* name() const
        { return _name.c_str(); }

        virtual Pt::Any get(void* instance)
        {
            C* t = static_cast<C*>( instance );
            T r = _getter(*t);
            return ReturnTraits<T>::make(r);
        }

        virtual void set(void* instance, Any& value, Type& type)
        {
            C* t = static_cast<C*>( instance );
            T a = ArgumentTraits<T>::cast( *_type, type, value.get() );
            _setter( *t, a );
        }

    private:
        std::string _name;
        Getter _getter;
        Setter _setter;
        Type* _type;
};

}

}

#endif
