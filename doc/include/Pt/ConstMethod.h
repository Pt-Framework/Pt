
namespace Pt {

/** @brief Adapter for const class methods

    The ConstMethod class wraps const member functions as Callable objects
    so that they can be used with the signals/slots framework. There are 
    partial specializations of this class template for up to ten arguments.
*/
template < typename R,typename ClassT, ARGUMENTS>
class ConstMethod : public Callable<R, ARGUMENTS>
{
    public:
        /** @brief The wrapped member function type.
        */
        typedef R (ClassT::*MemFuncT)(ARGUMENTS) const;

        /** @brief Wraps the given member function of the given object.
        */
        ConstMethod(ClassT& object, MemFuncT ptr);

        /** @brief Copy constructor 

            The created object refers to the same member function and
            object instance as the given one.
        */
        ConstMethod(const ConstMethod& rhs);

        //! @brief Returns a reference to this object's bound ClassT object.
        ClassT& object()
        { return *_object;}

        //! @brief Returns a const reference to this object's bound ClassT object.
        const ClassT& object() const;

        // docs inherited
        R operator()(ARGUMENTS) const;

        // docs inherited
        ConstMethod<R, ARGUMENTS>* clone() const;

    private:
        //! @internal
        ClassT* _object;

        //! @internal
        MemFuncT _method;
};

} //namespace Pt


