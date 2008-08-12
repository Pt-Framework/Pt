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

		bool catchSystemSignal(int sig);

		bool raiseSystemSignal(int sig);
};

} // namespace System

} // namespace Pt

#endif
