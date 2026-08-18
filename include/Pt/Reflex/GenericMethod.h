/*
  Copyright (C) 2004-2010 by Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
*/

#ifndef PT_REFLEX_GENERICMETHOD_H
#define PT_REFLEX_GENERICMETHOD_H

#include <Pt/Reflex/MethodInfo.h>
#include <Pt/Reflex/Argument.h>
#include <Pt/Any.h>
#include <string>
#include <utility>

namespace Pt {

namespace Reflex {

//! @cond PT_DOXYGEN_HIDDEN

template <typename C, typename Seq>
class GenericMethodBase;

template <typename C, std::size_t... Is>
class GenericMethodBase<C, std::index_sequence<Is...>> : public MethodInfo
{
    template <std::size_t>
    using ArgumentN = Argument;

    public:
        template <typename... Ts>
        GenericMethodBase(const std::string& name, Any (*proxy)(C&, ArgumentN<Is>&...),
                          Type& rtype, Ts&... ts)
        : MethodInfo(name)
        , _proxy(proxy)
        , _params{ &ts..., static_cast<Type*>(0) }
        {
            static_assert(sizeof...(Ts) == sizeof...(Is),
                          "method arity mismatch");
            this->init(rtype, sizeof...(Is) ? _params : 0, sizeof...(Is));
        }

        Any call(void* instance, const ArgumentList& args) override
        {
            C* obj = static_cast<C*>(instance);
            ArgumentIterator arg = args.begin();
            return _proxy(*obj, *(arg + Is)...);
        }

    private:
        Any (*_proxy)(C&, ArgumentN<Is>&...);
        Type* _params[sizeof...(Is) + 1];
};

//! @endcond

/** @brief Wraps a generic method proxy.

    The first template parameter is the class type. The second is the
    number of %Argument parameters accepted by the generic method.

*/
template <typename C, std::size_t N>
class GenericMethod : public GenericMethodBase<C, std::make_index_sequence<N>>
{
    public:
        /** @brief Construct from a generic method proxy.
        */
        template <typename... As, typename... Ts>
        GenericMethod(const std::string& name, Any (*proxy)(C&, As&...),
                      Type& rtype, Ts&... ts)
        : GenericMethodBase<C, std::make_index_sequence<N>>(name, proxy, rtype, ts...)
        { }
};

} // namespace Reflex

} // namespace Pt

#endif // PT_REFLEX_GENERICMETHOD_H
