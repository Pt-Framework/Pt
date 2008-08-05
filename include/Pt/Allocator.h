#ifndef PT_ALLOCATOR_H
#define PT_ALLOCATOR_H

#include <cstddef>

namespace Pt {

    class Allocator
    {
        public:
            Allocator()
            {}

            virtual ~Allocator()
            {}

            virtual void* allocate(std::size_t size)
            {
                return operator new(size);
            }

            virtual void deallocate(void* p, std::size_t size)
            {
                operator delete(p);
            }
    };

}

#endif
