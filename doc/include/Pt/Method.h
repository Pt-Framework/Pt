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
        ClassT* _object;
        MemFuncT _memFunc;
};

} // namespace Pt

#endif
