#ifndef Pt_Method_h
#define Pt_Method_h

#include <Pt/Callable.h>
#include <Pt/Connectable.h>
#include <Pt/Slot.h>

namespace Pt {

/** @brief Adapter for class methods
    @ingroup sigslot

    The %Method class wraps member functions as Callable objects
    so that they can be used with the signals/slots framework. There are 
    partial specializations of this class template for up to ten arguments.
*/
template <typename R, class ClassT, typename ARGUMENTS>
class Method : public Callable<R, ARGUMENTS>
{
    public:
        /** @brief The wrapped member function signature. */
        typedef R (ClassT::*MemFuncT)(ARGUMENTS);

        /** @brief Wraps the given object/member pair. */
        explicit Method(ClassT& object, MemFuncT ptr);

        /** @brief Returns a reference to this object's wrapped ClassT object. */
        ClassT& object();

        /** @brief Returns a const reference to the wrapped ClassT object. */
        const ClassT& object() const;

        /** @brief Returns a reference to the wrapped member function. */
        const MemFuncT& method() const;

        // inherit doc
        inline R operator()(ARGUMENTS0) const;

        // inherit doc
        Method<R, ClassT, ARGUMENTS>* clone() const;

        //! @brief Returns true if both use the same object and function pointer
        bool operator==(const Method& rhs) const;

    private:
        //! @internal
        ClassT* _object;
        //! @internal
        MemFuncT _memFunc;
};


/** @brief Wraps %Method objects so that they can act as Slots.
    @ingroup sigslot
*/
template < typename R, typename ClassT,class ARGUMENTS>
class MethodSlot : public BasicSlot<R, ARGUMENTS>
{
    public:
        //! @brief Constructs from callable
        MethodSlot(const Method<R, ClassT, ARGUMENTS>& method);

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
        Method<R, ClassT, ARGUMENTS0> _method;
};


/** @brief Returns a %Method object for the given object/method pair.
    @ingroup sigslot
*/
template <class R, class BaseT, class ClassT, typename ARGS>
Method<R,ClassT, ARGS> callable( ClassT & obj, R (BaseT::*ptr)(ARGS));


/** @brief Returns a slot object for the given object/member pair.
    @ingroup sigslot
*/
template <class R, class BaseT, class ClassT, typename ARGS>
MethodSlot<R, ClassT, ARGS> slot( ClassT& obj, R (BaseT::*memFunc)(ARGS) );

} // namespace Pt

#endif
