#ifndef PT_EVENT_H
#define PT_EVENT_H

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <typeinfo>

namespace Pt {

	class Allocator;

    /** \brief Base class for all event types.

        Specific Event objects, subclass from Event and implement the clone()
        and typeInfo() methods. The first is used to deep copy event objects
        for example in an EventLoop and the latter one is used to dispatch
        events by type.
     */
    class PT_EXPORT Event 
    {
        public:
            /** \brief Destructor.
             */
            virtual ~Event()
            {}

            virtual Event& clone(Allocator& allocator) const = 0;

            virtual void destroy(Allocator& allocator)= 0;
            
            /** \brief Returns the type info for this class of events.
              */
            virtual const std::type_info& typeInfo() const = 0;
    };

} // namespace Pt

#endif
