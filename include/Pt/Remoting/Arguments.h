/*
  Copyright (C) 2009-2026 by Marc Duerner

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

#ifndef PT_REMOTING_ARGUMENTS_H
#define PT_REMOTING_ARGUMENTS_H

#include <Pt/Remoting/Api.h>
#include <Pt/Composer.h>
#include <Pt/SerializationContext.h>
#include <cstddef>

namespace Pt {

namespace Remoting {

template <typename T>
struct Argument
{
    T value;
    BasicComposer<T> composer;

    explicit Argument(SerializationContext* ctx)
    : composer(ctx)
    {}

    void begin()
    {
        composer.begin(value);
    }
};


template <typename... Ts>
class Arguments;


template <>
class Arguments<>
{
    public:
        explicit Arguments(SerializationContext* /*ctx*/)
        {}

        void begin()
        {}

        void fill(std::size_t i, Composer** args)
        {
            args[i] = 0;
        }
};


template <typename T, typename... Ts>
class Arguments<T, Ts...>
{
    public:
        explicit Arguments(SerializationContext* ctx)
        : _head(ctx)
        , _tail(ctx)
        {}

        void begin()
        {
            _head.begin();
            _tail.begin();
        }

        void fill(std::size_t i, Composer** args)
        {
            args[i] = &_head.composer;
            _tail.fill(i + 1, args);
        }

        T& head()
        {
            return _head.value;
        }

        const T& head() const
        {
            return _head.value;
        }

        Arguments<Ts...>& tail()
        {
            return _tail;
        }

        const Arguments<Ts...>& tail() const
        {
            return _tail;
        }

    private:
        Argument<T> _head;
        Arguments<Ts...> _tail;
};


template <std::size_t I>
struct At
{
    template <typename T, typename... Ts>
    static auto& get(Arguments<T, Ts...>& args)
    {
        return At<I - 1>::get(args.tail());
    }

    template <typename T, typename... Ts>
    static const auto& get(const Arguments<T, Ts...>& args)
    {
        return At<I - 1>::get(args.tail());
    }
};


template <>
struct At<0>
{
    template <typename T, typename... Ts>
    static T& get(Arguments<T, Ts...>& args)
    {
        return args.head();
    }

    template <typename T, typename... Ts>
    static const T& get(const Arguments<T, Ts...>& args)
    {
        return args.head();
    }
};


template <std::size_t I, typename... Ts>
auto& get(Arguments<Ts...>& args)
{
    return At<I>::get(args);
}


template <std::size_t I, typename... Ts>
const auto& get(const Arguments<Ts...>& args)
{
    return At<I>::get(args);
}

} // namespace

} // namespace

#endif // include guard
