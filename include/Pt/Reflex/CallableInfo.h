/*
 * Copyright (C) 2005 by Marc Boris Duerner
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

#ifndef Pt_Reflex_CallableInfo_h
#define Pt_Reflex_CallableInfo_h

#include <Pt/Reflex/Api.h>
#include <Pt/Reflex/MemberInfo.h>
#include <Pt/SerializationInfo.h>
#include <Pt/Types.h>
#include <Pt/Any.h>

namespace Pt {

class Slot;

namespace Reflex {

class Type;

/** @brief Reflection support for callable entities
    @ingroup Reflection
*/
class CallableInfo : public MemberInfo
{
    public:
        virtual size_t argSize() const = 0;

        virtual Type& argType(size_t index) const = 0;

        virtual Type& retType() const = 0;

        virtual Pt::Any call(void* instance, const Any* args, size_t argCount) = 0;

        // virtual Slot* createSlot()
        // { return 0; }

    protected:
        CallableInfo()
        {}
};


class PropInfo : public MemberInfo
{
    public:
        PropInfo()
        {}

        virtual bool isWritable() const = 0;

        virtual Pt::Any get(void* instance) const = 0;

        virtual void set(void* instance, const Pt::Any& value) = 0;

        virtual void serialize(void* instance, Pt::SerializationInfo& si) const = 0;

        virtual void deserialize(void* instance, const Pt::SerializationInfo& si) = 0;
};


template <typename R, typename C, typename A>
class ReadWritePropInfo : public PropInfo
{
    public:
        typedef R (C::*Getter)() const;
        typedef void (C::*Setter)(A);

        ReadWritePropInfo(const std::string& name, Getter getter, Setter setter)
        : _name(name)
        , _getter(getter)
        , _setter(setter)
        {}

        virtual const char* name() const
        { return _name.c_str(); }

        virtual bool isWritable() const
        { return true; }

        virtual Pt::Any get(void* instance) const
        {
            C* t = reinterpret_cast<C*>( instance );

            R r = (t->*_getter)();

            if( TypeTraits<R>::isReference == 1 )
            {
                return Any(&r);
            }

            return Any(r);
        }

        virtual void set(void* instance, const Pt::Any& a)
        {
            try
            {
                C* t = reinterpret_cast<C*>( instance );
                A val = Pt::any_cast<A>(a);
                (t->*_setter)(val);
            }
            catch(const std::bad_cast&)
            {
                std::cerr << "PropertyInfo: Type mismatch: " << _name << std::endl;
            }
        }

        // TODO: when being serializing reflect a method named >>= and <<= and fixup
        //       the virtual serialization methods should only be required for
        //       Properties added at runtime.
        virtual void serialize(void* instance, Pt::SerializationInfo& si) const
        {
            C* t = reinterpret_cast<C*>( instance );
            si <<= (t->*_getter)();
        }

        virtual void deserialize(void* instance, const Pt::SerializationInfo& si)
        {
            C* t = reinterpret_cast<C*>( instance );

            typedef typename Pt::TypeTraits<A>::Value ValueT;
            ValueT value;
            si >>= value;

            (t->*_setter)(value);
        }

    private:
        std::string _name;
        Getter _getter;
        Setter _setter;
};

} // namespace Reflex

} // namespace Pt

#endif
