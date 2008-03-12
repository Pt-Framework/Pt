#ifndef PT_EVENT_H
#define PT_EVENT_H

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <typeinfo>

namespace Pt {

    /** \brief Base class for all event types.

        Specific Event objects, subclass from Event and implement the clone()
        and typeInfo() methods. The first is used to deep copy event objects
        for example in an EventLoop and the latter one is used to dispatch
        events by type.
     */
    class PT_EXPORT Event {
        public:
            /** \brief Default Constructor.
             */
            Event()
            {}

            /** \brief Destructor.
             */
            virtual ~Event()
            {}

            //! \brief Clone method.
            virtual Event* clone() const = 0;

            //! \brief Clone method.
            virtual size_t clone(void* to, size_t available)
            { return 0; }

            /** \brief Returns the type info for this class of events.
            */
            virtual const std::type_info& typeInfo() const = 0;
    };

} // namespace Pt

#endif
