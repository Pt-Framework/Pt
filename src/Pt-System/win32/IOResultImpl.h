#ifndef Pt_System_IOResultImpl_h
#define Pt_System_IOResultImpl_h

#include "Pt/System/IOResult.h"
#include <windows.h>

namespace Pt {
namespace System {

class IOResultImpl : public IOResult
{
    public:
        virtual IOResultImpl* impl()
        { return this; }

        void setHandle(HANDLE h)
        { _handle = h; }

        HANDLE handle() const
        { return _handle; }

		virtual void onComplete() = 0;

        virtual bool _wait(unsigned int msecs)
        {
           const DWORD result = WaitForSingleObject(_handle, msecs);
           return (result == WAIT_OBJECT_0);
        }

	protected:
        IOResultImpl()
        : IOResult()
        , _handle(INVALID_HANDLE_VALUE)
        {}

	private:
        HANDLE _handle;
};

}
}

#endif 
