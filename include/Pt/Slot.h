/*
  Copyright (C) 2008 Marc Boris Duerner

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

#ifndef Pt_Slot_h
#define Pt_Slot_h

#include <Pt/Api.h>
#include <Pt/Void.h>
#include <Pt/Callable.h>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Pt {

class Connection;

/** @brief Endpoint of a signal/slot connection

    Slots can be constructed with the @link Pt::Slot slot()@endlink function,
    which is overloaded for various types of callable entities, most notably
    functions or member functions. Slots are lightweight proxy-objects and one
    example is the Pt::MethodSlot, which allows to use a member function as a
    slot.

    @ingroup sigslot
*/
class Slot
{
    public:
        /** @brief Destructor
        */
        virtual ~Slot() {}

        /** @brief Clone this object with new
        */
        virtual Slot* clone() const = 0;

        /** @brief Returns a pointer to the contained callable
        */
        virtual const void* callable() const = 0;

        /** @brief Notifies of connects
        */
        virtual void onConnect(const Connection& c) = 0;

        /** @brief Notifies of disconnects
        */
        virtual void onDisconnect(const Connection& c) = 0;

        /** @brief Returns true if two slots are equal in value
        */
        virtual bool equals(const Slot& slot) const = 0;
};

/** @brief Base type for various "slot" types.

    @ingroup sigslot
*/
template <typename R, typename... As>
class BasicSlot : public Slot
{
    public:
        virtual Slot* clone() const = 0;
};


template <typename T>
class BindAdaptorBase
{
    public:
        BindAdaptorBase(const Slot& s, const T& a)
        : _slot( s.clone() )
        , _a(a)
        { }

        BindAdaptorBase(const BindAdaptorBase& c)
        : _slot( c._slot->clone() )
        , _a(c._a)
        { }

        ~BindAdaptorBase()
        { delete _slot; }

        BindAdaptorBase& operator=(const BindAdaptorBase& b)
        {
            if(this == &b)
            return *this;

            Slot* s = b.slot().clone();
            delete _slot;
            _slot = s;

            _a = b._a;
            return *this;
        }

        Slot& slot()
        {
            return *_slot;
        }

        const Slot& slot() const
        {
            return *_slot;
        }

        const T& arg() const
        {
            return _a;
        }

    private:
        Slot* _slot;
        T _a;
};

namespace SigSlotDetail {

template <typename R, typename Tuple, typename Indices>
class BindAdaptor;

template <typename R, typename Tuple, std::size_t... Is>
class BindAdaptor<R, Tuple, std::index_sequence<Is...>>
: public Callable<R, typename std::tuple_element<Is, Tuple>::type...>
, public BindAdaptorBase<
      typename std::tuple_element<std::tuple_size<Tuple>::value - 1, Tuple>::type>
{
    public:
        typedef typename std::tuple_element<std::tuple_size<Tuple>::value - 1, Tuple>::type BoundT;
        typedef BasicSlot<R, typename std::tuple_element<Is, Tuple>::type...> SlotBase;
        typedef BasicSlot<R, typename std::tuple_element<Is, Tuple>::type..., BoundT> FullSlotBase;
        typedef Callable<R, typename std::tuple_element<Is, Tuple>::type...> CallableBase;
        typedef Callable<R, typename std::tuple_element<Is, Tuple>::type..., BoundT> FullCallable;

    public:
        BindAdaptor(const FullSlotBase& slot, const BoundT& arg)
        : BindAdaptorBase<BoundT>(slot, arg)
        { }

        CallableBase* clone() const
        {
            return new BindAdaptor(*this);
        }

        R operator()(typename std::tuple_element<Is, Tuple>::type... args) const
        {
            const FullCallable* callable =
                static_cast<const FullCallable*>( this->slot().callable() );
            return callable->call(args..., this->arg());
        }
};

} // namespace SigSlotDetail

/** @brief Slot produced by binding a final argument to another slot.

    The bound argument is copied. The resulting slot accepts all original
    arguments except the final one.

    @ingroup sigslot
*/
template <typename R, typename Tuple, typename Indices>
class BoundSlot
: public SigSlotDetail::BindAdaptor<R, Tuple, Indices>::SlotBase
{
    typedef SigSlotDetail::BindAdaptor<R, Tuple, Indices> AdaptorT;

    public:
        template <typename T>
        BoundSlot(const typename AdaptorT::FullSlotBase& slot, const T& arg)
        : _adaptor(slot, arg)
        { }

        Slot* clone() const
        {
            return new BoundSlot(*this);
        }

        virtual const void* callable() const
        {
            return &_adaptor;
        }

        virtual void onConnect(const Connection& connection)
        {
            _adaptor.slot().onConnect(connection);
        }

        virtual void onDisconnect(const Connection& connection)
        {
            _adaptor.slot().onDisconnect(connection);
        }

        virtual bool equals(const Slot& slot) const
        {
            return _adaptor.slot().equals(slot);
        }

    private:
        AdaptorT _adaptor;
};

namespace SigSlotDetail {

template <typename R, typename Tuple>
struct BoundSlotTraits;

template <typename R, typename A, typename... As>
struct BoundSlotTraits<R, std::tuple<A, As...>>
{
    typedef BoundSlot<R,
                      std::tuple<A, As...>,
                      std::make_index_sequence<sizeof...(As)>> Type;
};

} // namespace SigSlotDetail

/** @brief Binds the final argument of a slot to a copied value.

    Repeatedly binding the result produces a slot with one fewer argument each
    time, including a zero-argument slot.

    @related BoundSlot
*/
template <typename R, typename... As, typename T>
typename SigSlotDetail::BoundSlotTraits<R, std::tuple<As...>>::Type
slot(const BasicSlot<R, As...>& slot, const T& arg)
{
    typedef typename SigSlotDetail::BoundSlotTraits<R, std::tuple<As...>>::Type BoundSlotT;
    return BoundSlotT(slot, arg);
}

} // namespace Pt

#endif
