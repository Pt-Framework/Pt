#ifndef PT_SYSTEM_APPLICATION_IMPL_H
#define PT_SYSTEM_APPLICATION_IMPL_H

#include <Pt/System/Api.h>

namespace Pt {

namespace System  {

class ApplicationImpl
{
	public:
		ApplicationImpl();

		virtual ~ApplicationImpl();

		void catchSystemSignal(int sig);

		int signalFd() const;
};

} // namespace System

} // namespace Pt

#endif
