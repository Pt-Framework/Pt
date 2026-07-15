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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301  USA
 */
#ifndef PT_REFLEX_GENERICMETHOD_H
#define PT_REFLEX_GENERICMETHOD_H

#include <Pt/Reflex/MethodInfo.h>
#include <Pt/Reflex/Argument.h>
#include <Pt/Any.h>
#include <string>

namespace Pt {

namespace Reflex {

template <typename T>
class GenericMethod0 : public Pt::Reflex::MethodInfo
{
    public:
        typedef Pt::Any (*ProxyFunc)(T&);

    public:
        GenericMethod0(ProxyFunc proxy, const std::string& name,
                       Pt::Reflex::Type& rtype)
        : Pt::Reflex::MethodInfo(name)
        , _proxy(proxy)
        {
            this->init(rtype, 0, 0);
        }

        Pt::Any call(void* instance, const Pt::Reflex::ArgumentList& args)
        {
            T* obj = static_cast<T*>(instance);
            return _proxy(*obj);
        }

    private:
        ProxyFunc _proxy;
};


template <typename T>
class GenericMethod1 : public Pt::Reflex::MethodInfo
{
    public:
        typedef Pt::Any (*ProxyFunc)(T&, Pt::Reflex::Argument&);

    public:
        GenericMethod1(ProxyFunc proxy, const std::string& name,
                       Pt::Reflex::Type& rtype, Pt::Reflex::Type& t1)
        : Pt::Reflex::MethodInfo(name)
        , _proxy(proxy)
        {
            _params[0] = &t1;
            this->init(rtype, _params, 1);
        }

        Pt::Any call(void* instance, const Pt::Reflex::ArgumentList& args)
        {
            T* obj = static_cast<T*>(instance);

            Pt::Reflex::ArgumentIterator arg = args.begin();

            return _proxy(*obj, *arg);
        }

    private:
        ProxyFunc _proxy;
        Pt::Reflex::Type* _params[1];
};


template <typename T>
class GenericMethod2 : public Pt::Reflex::MethodInfo
{
    public:
        typedef Pt::Any (*ProxyFunc)(T&, Pt::Reflex::Argument&, Pt::Reflex::Argument&);

    public:
        GenericMethod2(ProxyFunc proxy, const std::string& name,
                       Pt::Reflex::Type& rtype, Pt::Reflex::Type& t1, Pt::Reflex::Type& t2)
        : Pt::Reflex::MethodInfo(name)
        , _proxy(proxy)
        {
            _params[0] = &t1;
            _params[1] = &t2;
            this->init(rtype, _params, 2);
        }

        Pt::Any call(void* instance, const Pt::Reflex::ArgumentList& args)
        {
            T* obj = static_cast<T*>(instance);

            Pt::Reflex::ArgumentIterator arg = args.begin();
            Pt::Reflex::Argument& a1 = *arg;

            ++arg;
            Pt::Reflex::Argument& a2 = *arg;

            return _proxy(*obj, a1, a2);
        }

    private:
        ProxyFunc _proxy;
        Pt::Reflex::Type* _params[2];
};

} // namespace Reflex

} // namespace Pt

#endif // PT_REFLEX_GENERICMETHOD_H
