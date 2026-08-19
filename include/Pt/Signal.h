/*
  Copyright (C) 2005-2013 by Dr. Marc Boris Duerner

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

#ifndef Pt_Signal_h
#define Pt_Signal_h

#include <Pt/Api.h>
#include <Pt/Void.h>
#include <Pt/Event.h>
#include <Pt/Slot.h>
#include <Pt/Function.h>
#include <Pt/Method.h>
#include <Pt/ConstMethod.h>
#include <Pt/Connectable.h>
#include <map>

namespace Pt {

class PT_API SignalBase : public Connectable
{
    public:
        struct PT_API Sentry
        {
            Sentry(SignalBase* signal);

            ~Sentry();

            void detachAll();

            void detach();

            bool operator!() const
            { return _signal == 0; }

            SignalBase* _signal;
            Sentry*     _next;
        };

        SignalBase();

        ~SignalBase();

        SignalBase& operator=(const SignalBase& other);

        virtual void onConnectionOpen(const Connection& c);

        virtual void onConnectionClose(const Connection& c);

    protected:
        void disconnectSlots();

        void disconnectSlot(const Slot&);

    private:
        Sentry* _sentry;
        bool _sending;
        bool _dirty;
};

/** @brief Multicast %Signal to call multiple slots.

    Signals are normally members of objects and are being sent e.g. When the
    object state changes or some event occurs. When a signal is sent, it calls
    all slots it is connected to. Callable entities, like functions or member
    functions can serve as slots for signals.
    The template parameter list of the Pt::Signal class template determines
    the signature of the signal. If a signal does not have any arguments the
    parameter list is left empty:

    @code
    Pt::Signal<> sig0;         // Signal without arguments
    Pt::Signal<int> sig1;      // Signal with one argument
    Pt::Signal<int, int> sig2; // Signal with two arguments
    @endcode

    @copydetails Pt::Slot

    A signal can be connected to a slot if the signatures are compatible.
    One important feature of Pt::Signal is that the return value of a slot is
    ignored and therefore a slot is compatible to a signal no matter what type
    it returns. The following code example shows how a signal is connected to
    a function and a member function:

    @code
    class Callee : public Pt::Connectable
    {
    public:
        void slot()
        { std::cout << "Callee::slot() called" << std::endl; }
    };

    void slot()
    { std::cout << "slot() called." << std::endl; }

    int main()
    {
        Callee callee;

        Pt::Signal<> signal;
        signal += Pt::slot(slot);
        signal += Pt::slot(callee, &Callee::slot);

        return 0;
    }
    @endcode

    Two slots are constructed, one from a function pointer and another one from
    a member function pointer and the object instance to be called. The signal
    is connected to both slots. Signals can only be connected to objects that
    derive from Pt::Connectable, to ensure that all connections are closed when
    the object runs out of scope and no dangling connections are left.
    The += operator, to connect a signal with a slot, returns a connection object,
    which can be used to disconnect signals from slots manually. The following
    code illustrates this:

    @code
    void slot()
    { std::cout << "slot() called." << std::endl; }

    int main()
    {
        Pt::Signal<> signal;
        Connection c = signal += Pt::slot(slot);
        c.isValid() // returns true

        c.close();
        c.isValid() // returns false

        return 0;
    }
    @endcode

    A connection is reference counted and can not be duplicated as such,
    but always refers to the same shared connection data. If one peer of
    a connection is destroyed or the connection is closed manually, the
    connection becomes invalid.
    Once a connection has been established, signals can be send to invoke
    the connected slots. This happens by calling @link Pt::Signal::send()
    send()@endlink with the appropriate arguments, if any.

    @code
    void tellAge(int age)
    { std::cout << "I am " << age << " years old\n"; }

    int main ()
    {
        Pt::Signal<int> signal;
        signal += Pt::slot(tellAge);
        signal.send(26);
        return 0;
    }
    @endcode

    When the signal is send, the slot is called with the same value passed to
    Signal::send. Nothing will happen if the signal is not connected to any
    slots. When a signal is sent, the slot is called immediatly and directly
    and does not depend on an event loop. If multiple slots are connected to
    a signal, the slots will be called one after another.

    @ingroup Pt-Signals
*/
template <typename... As>
class Signal : public SignalBase
{
    public:
        typedef Invokable<As...> InvokableT;

    public:
        /** @brief Default constructor */
        Signal()
        { }

        /** @brief Copy Constructor */
        Signal(const Signal& rhs)
        {
            SignalBase::operator=(rhs);
        }

        /** @brief Connects to a slot

            Connects slot to this signal, such that firing this signal
            will invoke slot.
        */
        template <typename R>
        Connection connect(const BasicSlot<R, As...>& slot)
        {
            return Connection(*this, slot.clone());
        }

        /** @brief Disconnects from current slots.
        */
        void disconnect()
        {
            this->disconnectSlots();
        }

        /** @brief Disconnects from a slot
        */
        template <typename R>
        void disconnect(const BasicSlot<R, As...>& slot)
        {
            this->disconnectSlot(slot);
        }

        /** @brief Invlokes all slots

            Invokes all slots connected to this signal, in an undefined
            order. Their return values are ignored. Calling of connected slots will
            be interrupted if a slot deletes this Signal object or throws an exception.
        */
        void send(As... args)
        {
            if( Connectable::connections().empty() )
                return;

            SignalBase::Sentry sentry(this);

            std::list<Connection>::const_iterator it = Connectable::connections().begin();
            std::list<Connection>::const_iterator last = --Connectable::connections().end();
            while(true)
            {
                if( it->isValid() && it->sender() == this )
                {
                    const InvokableT* invokable =
                        static_cast<const InvokableT*>( it->callable() );
                    invokable->invoke(args...);
                }

                if( ! sentry )
                    return;

                if(it == last)
                    break;

                ++it;
            }
        }

        /** @brief  Invlokes all slots
            @see send
        */
        void operator()(As... args)
        {
            this->send(args...);
        }
};

/** @brief  Wraps %Signal objects so that they can act as Slots.

    SignalSlot is a "slot wrapper" for %Signal objects. That is, it
    effectively converts a %Signal object into a Slot object, so that it
    can be used as the target of another %Signal. This allows chaining of
    %Signals.
    @ingroup Pt-Signals
*/
template <typename... As>
class SignalSlot : public BasicSlot<void, As...>
{
    public:
        /** @brief Constructs from signal
        */
        explicit SignalSlot(Signal<As...>& signal)
        : _method( signal, &Signal<As...>::send )
        {}

        // inherit doc
        Slot* clone() const
        {
            return new SignalSlot(*this);
        }

        // inherit doc
        virtual const Callback* callable() const
        {
            return &_method;
        }

        // inherit doc
        virtual void onConnect(const Connection& c)
        {
            _method.object().onConnectionOpen(c);
        }

        // inherit doc
        virtual void onDisconnect(const Connection& c)
        {
            _method.object().onConnectionClose(c);
        }

        // inherit doc
        virtual bool equals(const Slot& rhs) const
        {
            const SignalSlot* ss = dynamic_cast<const SignalSlot*>(&rhs);
            return ss ? (_method == ss->_method) : false;
        }

    private:
        Method<void, Signal<As...> , As...> _method;
};

/** @brief Returns a slot object for the given signal.
    @related SignalSlot
    @related Slot
*/
template <typename... As>
SignalSlot<As...> slot(Signal<As...>& signal)
{
    return SignalSlot<As...>(signal);
}

/** @brief  Connects a %Signal to a slot.

    @related Signal
*/
template <typename R, typename... As>
Connection operator+=(Signal<As...>& signal, const BasicSlot<R, As...>& slot)
{
    return signal.connect(slot);
}

/** @brief  Disconnects a %Signal fro a slot.

    @related Signal
*/
template <typename R, typename... As>
void operator-=(Signal<As...>& signal, const BasicSlot<R, As...>& slot)
{
    signal.disconnect(slot);
}

struct PT_API CompareEventTypeInfo
{
    bool operator()( const std::type_info* t1,
                     const std::type_info* t2 ) const;
};

template <>
class PT_API Signal<const Pt::Event&> : public Connectable
                                      , protected NonCopyable
{
    struct PT_API Sentry
    {
        Sentry(Signal* signal);

        ~Sentry();

        void detach();

        void detachAll();

        bool operator!() const
        { return _signal == 0; }

        Signal*  _signal;
        Sentry*  _next;
    };

    class IEventRoute
    {
        public:
            IEventRoute(Connection& target)
            : _target(target)
            { }

            virtual ~IEventRoute() {}

            virtual void route(const Pt::Event& ev)
            {
                typedef Invokable<const Pt::Event&> InvokableT;
                const InvokableT* invokable = static_cast<const InvokableT*>( _target.callable() );
                invokable->invoke(ev);
            }

            Connection& connection()
            { return _target; }

            bool isValid() const
            { return _target.isValid(); }

        private:
            Connection _target;
    };

    template <typename EventT>
    class EventRoute : public IEventRoute
    {
        public:
            EventRoute(Connection& target)
            : IEventRoute(target)
            { }

            virtual void route(const Pt::Event& ev)
            {
                typedef Invokable<const Pt::Event&> InvokableT;
                const InvokableT* invokable = static_cast<const InvokableT*>( connection().callable() );

                const EventT& event = static_cast<const EventT&>(ev);
                invokable->invoke(event);
            }
    };

    typedef std::multimap< const std::type_info*,
                           IEventRoute*,
                           CompareEventTypeInfo > RouteMap;

    public:
        Signal();

        ~Signal();

        void send(const Pt::Event& ev);

        template <typename EventT>
        Connection connect( const BasicSlot<void, const EventT&>& slot )
        {
            Connection conn( *this, slot.clone() );
            EventT* selectAddRouteOverload = 0;
            this->addRoute(conn, selectAddRouteOverload);
            return conn;
        }

        void disconnect();

        template <typename R, typename EventT>
        void disconnect(const BasicSlot<R, const EventT&>& slot)
        {
            this->removeRoute(slot);
        }

        virtual void onConnectionOpen(const Connection& c);

        virtual void onConnectionClose(const Connection& c);

    protected:
        void addRoute(Connection& conn, const Pt::Event*)
        {
            this->addRoute( 0, new IEventRoute(conn) );
        }

        template <typename EventT>
        void addRoute(Connection& conn, const EventT*)
        {
            const std::type_info& ti = typeid(EventT);
            this->addRoute( &ti, new EventRoute<EventT>(conn) );
        }

        void addRoute(const std::type_info* ti, IEventRoute* route);

        void removeRoute(const Slot& slot);

        // TODO: Pt 2.0 (better performance)

        //template <typename R>
        //void removeRoute(const BasicSlot<R, const Event&>& slot)
        //{
        //    this->removeRoute(0, slot);
        //}

        //template <typename R, typename EventT>
        //void removeRoute(const BasicSlot<R, const EventT&>& slot)
        //{
        //    const std::type_info& ti = typeid(EventT);
        //    this->removeRoute(&ti, slot);
        //}

        void removeRoute(const std::type_info* ti, const Slot& slot);

    private:
        RouteMap _routes;
        Sentry* _sentry;
        bool _sending;
        bool _dirty;
};


template <typename R, class EventT>
Connection operator +=(Signal<const Pt::Event&>& signal, const BasicSlot<R, EventT>& slot)
{
    return signal.connect( slot );
}

template <typename R>
Connection operator +=(Signal<const Pt::Event&>& signal, const BasicSlot<R, const Pt::Event&>& slot)
{
    return signal.connect( slot );
}


template <typename R, class EventT>
void operator -=(Signal<const Pt::Event&>& signal, const BasicSlot<R, EventT>& slot)
{
    signal.disconnect( slot );
}

template <typename R>
void operator -=(Signal<const Pt::Event&>& signal, const BasicSlot<R, const Pt::Event&>& slot)
{
    signal.disconnect( slot );
}

} // namespace Pt

#endif
