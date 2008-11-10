#ifndef Pt_Signal_h
#define Pt_Signal_h

#include <Pt/Api.h>
#include <Pt/Void.h>
#include <Pt/Event.h>
#include <Pt/Function.h>
#include <Pt/Method.h>
#include <Pt/ConstMethod.h>
#include <Pt/Connectable.h>

#include <map>
#include <list>
#include <algorithm>

namespace Pt {

/** @brief Multicast %Signal
    @ingroup sigslot

    A %Signal can be connected to multiple slots. The return value of the
    target method is ignored when the slots are invoked.
*/
template <typename ARGUMENTS>
class Signal : public SignalBase {
    public:
        typedef Invokable<ARGUMENTS> InvokableT;

    public:
        /** @brief Default contructor */
        Signal();

        /** @brief Copy Constructor */
        Signal(const Signal& rhs);

        /** @brief Connects to a slot

            Connects slot to this signal, such that firing this signal
            will invoke slot.
        */
        template <typename R>
        Connection connect(const BasicSlot<R, ARGUMENTS>& slot);

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
    @ingroup sigslot

    SignalSlot is a "slot wrapper" for %Signal objects. That is, it
    effectively converts a %Signal object into a Slot object, so that it
    can be used as the target of another %Signal. This allows chaining of
    %Signals.
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


/** @brief  Creates a %SignalSlot object from an equivalent %Signal.
    @ingroup sigslot
*/
template <typename ARGS>
SignalSlot<ARGS> slot( Signal<ARGS> & signal );

/** @brief  Connects a %Signal to a function.
    @ingroup sigslot
*/
template <typename R, typename ARGS>
Connection connect(Signal<ARGS>& signal, R(*func)(ARGS));

/** @brief  Connects a %Signal to a member function.
    @ingroup sigslot
*/
template <typename R, class BaseT, class ClassT, class ARGS>
Connection connect(Signal<ARGS>& signal, BaseT& object, R(ClassT::*memFunc)(ARGS));

/** @brief  Connects a %Signal to a const member function.
    @ingroup sigslot
*/
template <typename R, class BaseT, class ClassT, class ARGS>
Connection connect(Signal<ARGS>& signal, BaseT& object, R(ClassT::*memFunc)(ARGS) const);

/** @brief  Connects a %Signal to another %Signal.
    @ingroup sigslot
*/
template <class ARGS>
Connection connect(Signal<ARGS>& sender, Signal<ARGS>& receiver);

/** @brief Disconnects a %Signal from a function
    @ingroup sigslot
*/
template <typename R,class ARGS>
void disconnect(Signal<ARGS>& signal, R(*func)(ARGS));

/** @brief Disconnects a %Signal from a member function
    @ingroup sigslot
*/
template <typename R, class BaseT, class ClassT,class ARGS>
void disconnect(Signal<ARGS>& signal, BaseT& object, R(ClassT::*memFunc)(ARGS));

/** @brief Disconnects a %Signal from a const member function
    @ingroup sigslot
*/
template <typename R, class BaseT, class ClassT, class ARGS>
void disconnect(Signal<ARGS>& signal, BaseT& object, R(ClassT::*memFunc)(ARGS) const);

/** @brief Disconnects a %Signal from another %Signal
    @ingroup sigslot
*/
template <class ARGS>
void disconnect(Signal<ARGS>& sender, Signal<ARGS>& receiver);

} // !namespace Pt

#endif
