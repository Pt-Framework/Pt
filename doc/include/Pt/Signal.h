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

} // !namespace Pt

#endif
