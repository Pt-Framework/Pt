
namespace Pt {

    /** @brief Connects to one slot and handle return value.
        @ingroup sigslot

        Delegates can only be connected to one slot, but have the advantage
        that they return the return value of the connected slot when called.
        There are partial specializations of this class template for up to 
        ten arguments.
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

            //! @brief Same as %Delegate::call()
            R operator()(ARGUMENTS) const;
    };

} // !namespace Pt
