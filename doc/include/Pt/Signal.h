#ifndef Pt_Signal_h
#define Pt_Signal_h

namespace Pt {

/** @brief Multicast %Signal to call multiple slots.

    A %Signal can be connected to multiple slots. The return value of the
    target method is ignored when the slots are invoked.

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
    @ingroup sigslot
*/
template <typename R, typename ARGS>
Connection operator+=(Signal<ARGS>& signal, const BasicSlot<R, ARGS>& slot);

/** @brief  Disconnects a %Signal fro a slot.
    @ingroup sigslot
*/
template <typename R, typename ARGS>
void operator-=(Signal<ARGS>& signal, const BasicSlot<R, ARGS>& slot);

} // namespace Pt

#endif
