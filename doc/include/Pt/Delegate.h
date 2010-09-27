
namespace Pt {

    /** @brief Connects to one slot and handle return value.

        Delegates can only be connected to one slot, but have the advantage
        that they return the return value of the connected slot when called.
        There are partial specializations of this class template for up to 
        ten arguments.

        \ingroup sigslot
    */
    template < typename R, typename ARGUMENTS>
    class Delegate : Connectable
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


/** @brief Creates a %DelegateSlot object from an equivalent %Delegate.
    @ingroup sigslot
*/
template <typename R, typename ARGS>
DelegateSlot<R, ARGS> slot( Delegate<R, ARGS>& delegate );

/** @brief Connect a %Delegate to another %Delegate.
    @ingroup sigslot
*/
template <typename R, typename ARGS>
Connection connect(Delegate<R, ARGS>& delegate, Delegate<R, ARGS>& receiver);

/** @brief Connect a %Delegate to a function.
    @ingroup sigslot
*/
template <typename R, typename ARGS>
Connection connect(Delegate<R, ARGS>& delegate, R(*func)(ARGS));

/** @brief Connect a %Delegate to a member function.
    @ingroup sigslot
*/
template <typename R, class BaseT, class ClassT, typename ARGS>
Connection connect(Delegate<R, ARGS>& delegate, BaseT& object, R(ClassT::*memFunc)(ARGS));

/** @brief Connect a %Delegate to a const member function.
    @ingroup sigslot
*/
template <typename R, class BaseT, class ClassT, typename ARGS>
Connection connect(Delegate<R>& delegate, BaseT& object, R(ClassT::*memFunc)(ARGS) const);

} // !namespace Pt
