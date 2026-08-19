/*
   Copyright (C) 2005 by Dr. Marc Boris Duerner

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

#ifndef Pt_Delegate_h
#define Pt_Delegate_h

#include <Pt/Void.h>
#include <Pt/Slot.h>
#include <Pt/Function.h>
#include <Pt/Method.h>
#include <Pt/ConstMethod.h>
#include <Pt/Connectable.h>
#include <stdexcept>

namespace Pt {

class DelegateBase : public Connectable
{
    public:
        DelegateBase()
        { }

        DelegateBase(const DelegateBase& rhs)
        { DelegateBase::operator=(rhs); }

        DelegateBase& operator=(const DelegateBase& other)
        {
            _target.close();

            if( other._target.isValid() )
            {
                const Slot* slot = other._target.slot();
                _target = Connection( *this, slot->clone()  );
            }

            return *this;
        }

        /** @brief Returns true if connected to a target.
        */
        bool isConnected() const
        {
            return _target.isValid();
        }

        virtual void onConnectionOpen(const Connection& c)
        {
            const Connectable* sender = c.sender();

            if( sender == this )
            {
                _target.close();
                _target = c;
            }

            Connectable::onConnectionOpen(c);
        }

        virtual void onConnectionClose(const Connection& c)
        {
            Connectable::onConnectionClose(c);
        }

    protected:
        void disconnectSlot()
        {
            _target.close();
        }

        void disconnectSlot(const Slot& slot)
        {
            if( _target.isValid() && _target.slot()->equals(slot) )
            {
                _target.close();
            }
        }

    protected:
        Connection _target;
};

/** @brief Delegates an action to a slot.

    The Pt::Delegate is an alternative to the Pt::Signal and differs from it
    in two ways. Firstly, delegates forward the return value of the slot and
    secondly, delegates can only be connected to one slot at a time. The same
    types of slots can be used for signals and delegates. The template
    parameter list of the Delegate determines its signature, where the first
    parameter represents the return type:

    @code
    Pt::Delegate<int> del0;           // Delegate only returns int
    Pt::Delegate<int, int> del1;      // Delegate with one argument
    Pt::Delegate<int, int, int> del2; // Delegate with two arguments
    @endcode

    The syntax for connecting delegates is identical to how signals are connected
    to slots. However, since a delegate forwards the return value of its slot, not
    only the arguments passed to the slot must be compatible, but also the return
    value. Furthermore, when an already connected delegate is connected again,
    the current connection will be closed and the delegate is connected to its
    new target.

    @code
    int slotA()
    { return 5; }

    int slotB()
    { return 6; }

    int main()
    {
        Pt::Delegate<int> delegate;
        delegate += Pt::slot(slotA);
        delegate += Pt::slot(slotB); // disconnects from slotA

        return 0;
    }
    @endcode

    The example above constructs a delegate which can be connected to any slot
    that returns an int. It is first connected to a slot of the function slotA.
    When it is connected for the second time to a slot of the function slotB,
    the old connection will be closed and only slotB is called when the delegate
    is called.

    There are two possibilities how a delegate can call its slot. The member
    function @link Pt::Delegate::call() call()@endlink will return the return
    value of the slot. If the delegate is not connected to a slot, an exception
    is thrown. The second method is through @link Pt::Delegate::invoke()
    invoke()@endlink, where the return value is ignored, but if the delegate
    is not connected, no exception will be thrown.

    @code
    int slot()
    { return 5; }

    int main()
    {
        try
        {
            Pt::Delegate<int> delegate;
            Pt::Connection connection = delegate += Pt::slot(slot);

            int i = delegate.call(); // i is 5 now

            connection.close();
            delegate.invoke() // does not throw
            delegate.call();  // will throw because not connected
        }
        catch(const std::logic_error& ex)
        {
            std::cerr << "could not call delegate" << std::endl;
        }

        return 0;
    }
    @endcode

    The program above connects a delegate to a slot and then calls it. After the
    connection was closed, the delegate is invoked, which has no effect. Finally,
    when the delegate is called again, an exception is thrown and catched.

    \ingroup Pt-Signals
*/
template <typename R, typename... As>
class Delegate : public DelegateBase
{
    public:
        typedef Callable<R, As...> CallableT;

    public:
        //! @brief Default Constructor.
        Delegate()
        { }

        /** @brief Deeply copies the other %Delegate.
        */
        Delegate(const Delegate& rhs)
        {
            DelegateBase::operator=(rhs);
        }

        /** @brief Connects this object to the given slot and returns that Connection.
        */
        Connection connect(const BasicSlot<R, As...>& slot)
        {
            return Connection(*this, slot.clone());
        }

        /** @brief Disconnects from current target.
        */
        void disconnect()
        {
            DelegateBase::disconnectSlot();
        }

        /** @brief Disconnects from the target.
        */
        void disconnect(const BasicSlot<R, As...>& slot)
        {
            DelegateBase::disconnectSlot(slot);
        }

        /** @brief Calls the slot connected to the %Delegate

            Passes on all arguments to the connected slot and returns the return value
            of that slot. If no slot is connect then an exception is thrown.
        */
        R call(As... args)
        {
            if( ! _target.isValid() )
                throw std::logic_error("Delegate::call(): Delegate not connected");

            const CallableT* callable =
                static_cast<const CallableT*>( _target.callable() );
            return callable->call(args...);
        }

        /** @brief Invoke the slot connected to the %Delegate

            Passes on all arguments to the connected slot and ignores the return value. If
            No slot is connected, the call is silently ignored.
        */
        void invoke(As... args)
        {
            if( ! _target.isValid() )
                return;

            const CallableT* callable =
                static_cast<const CallableT*>( _target.callable() );
            callable->call(args...);
        }

        /** @brief Same as call()
        */
        R operator()(As... args)
        {
            return this->call(args...);
        }
};

/** @brief  Wraps %Delegate objects so that they can act as Slots.

    DelegateSlot is a "slot wrapper" for %Delegate objects. That is, it
    effectively converts a %Delegate object into a Slot object, so that it
    can be used as the target of another %Delegate. This allows chaining of
    delegates.

    @ingroup Pt-Signals
*/
template <typename R, typename... As>
class DelegateSlot : public BasicSlot<R, As...>
{
    public:
        /** @brief Constructs from delegate
        */
        explicit DelegateSlot(Delegate<R, As...>& delegate)
        : _method(delegate, &Delegate<R, As...>::call)
        { }

        // inherit doc
        Slot* clone() const
        {
            return new DelegateSlot(*this);
        }

        // inherit doc
        virtual const Callback* callable() const
        {
            return &_method;
        }

        // inherit doc
        virtual void onConnect(const Connection& connection)
        {
            _method.object().onConnectionOpen(connection);
        }

        // inherit doc
        virtual void onDisconnect(const Connection& connection)
        {
            _method.object().onConnectionClose(connection);
        }

        // inherit doc
        virtual bool equals(const Slot& slot) const
        {
            const DelegateSlot* other = dynamic_cast<const DelegateSlot*>(&slot);
            return other && _method == other->_method;
        }

    private:
        Method<R, Delegate<R, As...>, As...> _method;
};

/** @brief Returns a slot object for the given delegate.
    @related DelegateSlot
    @related Slot
*/
template <typename R, typename... As>
DelegateSlot<R, As...> slot(Delegate<R, As...>& delegate)
{
    return DelegateSlot<R, As...>(delegate);
}

/** @brief Connect a %Delegate to a slot.

    @related Delegate
*/
template <typename R, typename... As>
Connection operator+=(Delegate<R, As...>& delegate, const BasicSlot<R, As...>& slot)
{
    return delegate.connect(slot);
}

} // namespace Pt

#endif
