
namespace Pt {

/** @brief An interface for all callable entities
    @ingroup sigslot

    The %Callable interface extends the %Invokable interface to handle
    return values. There are partial specializations of this class
    template for up to ten arguments.
*/
template <typename R, typename ARGUMENTS>
class Callable : public Invokable<ARGUMENTS>
{
    public:
        /** @brief Returns a copy of this instance

            A copy of the instance is created with new is returned. Ownership
            is transfered to the caller, who has to delete it.
        */
        virtual Callable* clone() const = 0;

        /** @brief Call the callable entity.

            Since this class template is partially specialized, the passed
            arguments \a ARGUMENTS must match the template parameters.
        */
        virtual R operator()(ARGUMENTS) const = 0;

        /** @brief Same as operator().
        */
        R call(ARGUMENTS) const;

        /** @brief Invoke the callable entity.

            Inherited from Invokable. Ignores the return value of the %Callable.
            Since this class template is partially specialized, the passed
            arguments \a ARGUMENTS must match the template parameters.
        */
        void invoke(ARGUMENTS) const;
};

}
