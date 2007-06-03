#ifndef PT_CMA_ICOMPONENTBUILDER_H
#define PT_CMA_ICOMPONENTBUILDER_H

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/Cma/TypeId.h>
#include <Pt/Cma/IUnknown.h>


namespace Pt {

namespace Cma {

    /** Implements reference counting for interfaces.

    This is the Base class for ComponentBuilder and holds the reference
    counter and the typeId of the Interface.
    */
    class PT_EXPORT IComponentBuilder
    {
        public:
            IComponentBuilder(const TypeId& typeId)
            :_typeId(typeId)
            , _refCount(0)
            {

            }

            virtual ~IComponentBuilder()
            {}

            virtual  IUnknown* createComponent() = 0;

            TypeId typeId() const
            {
                return _typeId;
            }

            void release()
            {
                --_refCount;
            }

            size_t instances()
            {
                return _refCount;
            }

        protected:
            /** component type to build */
            TypeId _typeId;

            /** number of created components of this type */
            size_t _refCount;
    };

}

}

#endif
