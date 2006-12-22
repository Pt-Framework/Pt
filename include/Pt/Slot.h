#ifndef Pt_Slot_h
#define Pt_Slot_h

#include <Pt/Api.h>
#include <Pt/Void.h>
#include <Pt/Clonable.h>


//! \addtogroup Pt
namespace Pt {

	class Connection;


	class PT_API Slot {
		public:
			virtual ~Slot() {}

			virtual Slot* clone() const = 0;

			virtual const void* callable() const = 0;

			virtual void opened(const Connection& c) = 0;

			virtual void closed(const Connection& c) = 0;
	};


	template < typename R,
	            typename A1 = Pt::Void,
	            typename A2 = Pt::Void,
	            typename A3 = Pt::Void,
	            typename A4 = Pt::Void,
	            typename A5 = Pt::Void >
	class PT_API BasicSlot : public Slot {
		public:
			virtual Slot* clone() const = 0;
	};

}


#endif


