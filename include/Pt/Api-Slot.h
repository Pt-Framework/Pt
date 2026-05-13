#ifndef PT_API_SLOT_H
#define PT_API_SLOT_H

#include <Pt/Api.h>
#include <Pt/Void.h>

namespace Pt {

class Connection;

/** @brief Endpoint of a signal/slot connection

    Slots can be constructed with the @link Pt::Slot slot()@endlink function,
    which is overloaded for various types of callable entities, most notably
    functions or member functions. Slots are lightweight proxy-objects and one
    example is the Pt::MethodSlot, which allows to use a member function as a
    slot. 

    @headerfile Slot.h <Pt/Slot.h>
    @ingroup sigslot
*/
class Slot
{
    public:
        //! @brief Destructor
        virtual ~Slot() {}

        //! @brief Clone this object with new
        virtual Slot* clone() const = 0;

        //! @brief Returns a pointer to the contained callable
        virtual const void* callable() const = 0;

        //! @brief Notifies of connects
        virtual void onConnect(const Connection& c) = 0;

        //! @brief Notifies of disconnects
        virtual void onDisconnect(const Connection& c) = 0;

        //! @brief Returns true if two slots are equal in value
        virtual bool equals(const Slot& slot) const = 0;
};

/** @brief Base type for various "slot" types.

    @headerfile Slot.h <Pt/Slot.h>
    @ingroup sigslot
*/
template < typename R, typename ARGUMENTS>
class BasicSlot : public Slot
{
    public:
        //! @brief Returns a pointer to the contained callable
        virtual const void* callable() const = 0;

        //! @brief Notifies of connects
        virtual void onConnect(const Connection& c) = 0;

        //! @brief Notifies of disconnects
        virtual void onDisconnect(const Connection& c) = 0;

        //! @brief Returns true if two slots are equal in value
        virtual bool equals(const Slot& slot) const = 0;
};

}

#endif

