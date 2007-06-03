/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
 *                                                                         *
 ***************************************************************************/
#ifndef PT_CMA_IUNKNOWN_H
#define PT_CMA_IUNKNOWN_H

#include <Pt/Api.h>
#include <string>

#include <Pt/Cma/TypeId.h>

namespace Pt
{
    class Version;
}

namespace Pt {

namespace Cma {

    /**
     *  Base interface for all services of a component.
     *
     *  This is the base interface for any service provided by a component.
     *  All services need to subclass this interface.
     */
    class IUnknown
    {
        template <typename IfaceT>
        friend IfaceT* queryInterface(IUnknown* unknown);

        public:
            /**
            * Creates a concrete interface type.
            *
            * Converts the initially received IUnknown interface type into a
            * concrete service interface provided by the component.
            *
            * @return pointer to an interface type
            */
            template <typename IfaceT>
            IfaceT* queryInterface()
            {
                return (IfaceT*) this->queryInterface( IfaceT::typeId() );
            }

            virtual IUnknown* queryInterface(const TypeId& typeId) = 0;

            /**
            * Decrement the reference count of this component.
            *
            * The components are reference counted and each time an interface
            * is queried, the reference counter is increased. If all interfaces
            * are released, the component is not referenced anymore and deletes
            * itself.
            */
            virtual void release() = 0;

            /**
            * @brief Get information on version and creation date for this component
            *
            * During the build process information on version and creation date
            * are compiled into the component. That information can be aquired
            * by class Version. As any component should export its version information
            * this method has to be implemented by every component.
            *
            * @return The version-information object
            */
            virtual const Pt::Version& getVersionInformation() = 0;

        protected:
            //! Default destructor
            virtual ~IUnknown(void)
            {}
    };

}

}

#endif
