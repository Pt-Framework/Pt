
namespace Pt {

/** @brief Adapter for const class methods
    @ingroup sigslot

    The %ConstMethod class wraps const member functions as Callable objects
    so that they can be used with the signals/slots framework. There are 
    partial specializations of this class template for up to ten arguments.
*/
template < typename R,typename ClassT, typename ARGUMENTS>
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

        //! @brief Returns true if both use the same object and function pointer
        bool operator==(const ConstMethod& rhs) const;

    private:
        //! @internal
        ClassT* _object;

        //! @internal
        MemFuncT _method;
};


/** @brief Wraps %ConstMethod objects so that they can act as Slots.
    @ingroup sigslot
*/
template < typename R, typename ClassT,class ARGUMENTS>
class ConstMethodSlot : public BasicSlot<R, ARGUMENTS>
{
    public:
        //! @brief Constructs from callable
        ConstMethodSlot(const ConstMethod<R, ClassT, ARGUMENTS>& method);

        // inherit doc
        Slot* clone() const;

        // inherit doc
        virtual const void* callable() const;

        // inherit doc
        virtual void opened(const Connection& c);

        // inherit doc
        virtual void closed(const Connection& c);

        // inherit doc
        virtual bool equals(const Slot& slot) const;

    private:
        //! @internal
        ConstMethod<R, ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> _method;
};


/** @brief Returns a %ConstMethod object for the given object/method pair.
    @ingroup sigslot
*/
template <class R, class BaseT, class C, typename ARGS>
ConstMethod<R, C, ARGS> callable( C & obj, R (BaseT::*ptr)(ARGS) const );


/** @brief Returns a slot object for the given object/member pair.
    @ingroup sigslot
*/
template <class R, class BaseT, class C, typename ARGS>
ConstMethodSlot<R, C, ARGS> slot( C & obj, R (BaseT::*memFunc)(ARGS) const );

} //namespace Pt


