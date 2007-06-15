#ifndef Pt_System_WriteResult_h
#define Pt_System_WriteResult_h

#include "IOResultImpl.h"
#include "Pt/System/IODevice.h"


namespace Pt {

namespace System {

class WriteResult : public IOResultImpl
{
    public:
        WriteResult()
        {}

		virtual void onComplete()
		{
			this->device()->outputReady(*this);
		}
};

}

}

#endif
