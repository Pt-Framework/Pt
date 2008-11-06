#ifndef Pt_Slot_h
#define Pt_Slot_h

#include <Pt/Api.h>
#include <Pt/Void.h>

namespace Pt {

class Connection;

/** @brief Endpoint of a signal/slot connection
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
        virtual void opened(const Connection& c) = 0;

        //! @brief Notifies of disconnects
        virtual void closed(const Connection& c) = 0;

        //! @brief Returns true if two slots are equal in value
        virtual bool equals(const Slot& slot) const = 0;
};

/** @brief Base type for various "slot" types.
    @ingroup sigslot
*/
template < typename R, typename ARGUMENTS>
class BasicSlot : public Slot
{
    public:
        /** @brief Returns a copy of this objects created by new
        */
        virtual Slot* clone() const = 0;
};

}

#endif

