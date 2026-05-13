#ifndef PT_API_SIGNAL_H
#define PT_API_SIGNAL_H

namespace Pt {

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

    @headerfile Signal.h <Pt/Signal.h>
    @ingroup sigslot
*/
template <typename ARGUMENTS>
class Signal : public Connectable 
{
    public:
        typedef Invokable<ARGUMENTS> InvokableT;

    public:
        /** @brief Default constructor */
        Signal();

        /** @brief Copy Constructor */
        Signal(const Signal& rhs);

        /** @brief Connects to a slot

            Connects slot to this signal, such that firing this signal
            will invoke slot.
        */
        template <typename R>
        Connection connect(const BasicSlot<R, ARGUMENTS>& slot);

        /** @brief Disconnects from current slots.
        */
        void disconnect();

        /** @brief Disconnects from a slot
        */
        template <typename R>
        void disconnect(const BasicSlot<R, ARGUMENTS>& slot);

        /** @brief Invlokes all slots

            Invokes all slots connected to this signal, in an undefined
            order. Their return values are ignored. Calling of connected slots will
            be interrupted if a slot deletes this Signal object or throws an exception.
        */
        inline void send(ARGUMENTS args) const;

        /** @brief  Invlokes all slots
            @see send
        */
        inline void operator()(ARGUMENTS args) const;
};

/** @brief  Wraps %Signal objects so that they can act as Slots.

    SignalSlot is a "slot wrapper" for %Signal objects. That is, it
    effectively converts a %Signal object into a Slot object, so that it
    can be used as the target of another %Signal. This allows chaining of
    %Signals.

    @headerfile Signal.h <Pt/Signal.h>
    @ingroup sigslot
*/
template <typename ARGUMENTS>
class SignalSlot : public BasicSlot<R, ARGUMENTS>
{
    public:
        //! @brief Constructs from signal
        SignalSlot(Signal<ARGUMENTS>& signal);

        // inherit doc
        Slot* clone() const;

        // inherit doc
        virtual const void* callable() const;

        // inherit doc
        virtual void onConnect(const Connection& c);

        // inherit doc
        virtual void onDisconnect(const Connection& c);

        // inherit doc
        virtual bool equals(const Slot& slot) const;

    private:
        //! @internal
        mutable ConstMethod<void, Signal<ARGUMENTS>, ARGUMENTS > _method;
};

/** @brief  Connects a %Signal to a slot.

    @related Signal
*/
template <typename R, typename ARGS>
Connection operator+=(Signal<ARGS>& signal, const BasicSlot<R, ARGS>& slot);

/** @brief  Disconnects a %Signal fro a slot.

    @related Signal
*/
template <typename R, typename ARGS>
void operator-=(Signal<ARGS>& signal, const BasicSlot<R, ARGS>& slot);

} // namespace Pt

#endif
