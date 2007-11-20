#ifndef Pt_Slot_h
#define Pt_Slot_h

#include <Pt/Api.h>
#include <Pt/Void.h>


//! \addtogroup Pt
namespace Pt {

    class Connection;


    class PT_EXPORT Slot {
        public:
            virtual ~Slot() {}

            virtual Slot* clone() const = 0;

            virtual const void* callable() const = 0;

            virtual bool opened(const Connection& c) = 0;

            virtual void closed(const Connection& c) = 0;
    };


#include <Pt/Slot.tpp>

}


#endif


