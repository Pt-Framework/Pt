
namespace Pt {

/** @brief Connects to one slot and handle return value.

    Delegates can only be connected to one slot, but have the advantage
    that they return the return value of the connected slot when called.
    There are partial specializations of this class template for up to 
    ten arguments.

    \ingroup sigslot
*/
template < typename R, typename ARGUMENTS>
class Delegate : public Connectable
{
    public:
        //! @brief Default Constructor.
        Delegate();

        /** @brief Deeply copies the other %Delegate. 
        */
        Delegate(const Delegate& rhs);

        /** @brief Connects this object to the given slot and returns that Connection. 
        */
        Connection connect(const BasicSlot<R,ARGUMENTS>& slot);

        /** @brief Disconnects from current target.
        */
        void disconnect();

        /** @brief Disconnects from the target.
        */
        void disconnect(const BasicSlot<R,ARGUMENTS>& slot);

        /** @brief Returns true if connected to a target.
        */
        bool isConnected() const;

        /** @brief Calls the slot connected to the %Delegate

            Passes on all arguments to the connected slot and returns the return value
            of that slot. If no slot is connect then an exception is thrown.
        */
        inline R call(ARGUMENTS) const;

        /** @brief Invoke the slot connected to the %Delegate

            Passes on all arguments to the connected slot and ignores the return value. If
            No slot is connected, the call is silently ignored.
        */
        inline void invoke(ARGUMENTS) const;

        //! @brief Same as call()
        R operator()(ARGUMENTS) const;
};

/** @brief  Wraps %Delegate objects so that they can act as Slots.

    DelegateSlot is a "slot wrapper" for %Delegate objects. That is, it
    effectively converts a %Delegate object into a Slot object, so that it
    can be used as the target of another %Delegate. This allows chaining of
    delegates.

    @ingroup sigslot
*/
template < typename R, typename ARGUMENTS>
class DelegateSlot : public BasicSlot<R, ARGUMENTS>
{
    public:
        //! @brief Constructs from delegate
        DelegateSlot(Delegate<R, ARGUMENTS>& delegate);

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
        mutable ConstMethod<R, Delegate<R, ARGUMENTS>, ARGUMENTS > _method;
};

/** @brief Connect a %Delegate to a slot.

    @related Delegate
*/
template <typename R, typename ARGS>
Connection operator+=(Delegate<R, ARGS>& delegate, const BasicSlot<R, ARGS>& slot);

} // namespace Pt
