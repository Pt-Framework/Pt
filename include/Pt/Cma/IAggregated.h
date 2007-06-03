#ifndef PT_CMA_IAGGREGATED_H
#define PT_CMA_IAGGREGATED_H

#include <string>

#include <Pt/Cma/TypeId.h>
#include <Pt/Cma/IUnknown.h>

namespace Pt {

namespace Cma {

    /**
     *  Base interface for all aggregated services.
     *
     *  Implementations of interfaces that are going to be aggregated need to
     *  be derived from the special IAggregated class instead of just IUnknown.
     *  IAggregated forwards the functions calls to its parent IUnknown.
     */
    class IAggregated : public IUnknown
    {
        public:
            IAggregated(IUnknown* parent)
            : _parent(parent)
            {
            }

            void release()
            {
                _parent->release();
            }

            IUnknown* queryInterface(const TypeId& typeId)
            {
                return _parent->queryInterface(typeId);
            }

    private:
        IUnknown* _parent;
    };

}

}

#endif
