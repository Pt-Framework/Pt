
namespace Pt {

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
