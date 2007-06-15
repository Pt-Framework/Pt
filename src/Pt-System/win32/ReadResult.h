#ifndef Pt_System_ReadResult_h
#define Pt_System_ReadResult_h

#include "IOResultImpl.h"

namespace Pt {
namespace System {

class ReadResult : public IOResultImpl
{
    public:
        ReadResult()
        {}

		virtual void onComplete()
		{
			this->device()->inputReady(*this);
		}
};

}
}

#endif
