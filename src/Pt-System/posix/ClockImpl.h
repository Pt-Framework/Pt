#if !defined(PT_ClockImpl_H)
#define PT_ClockImpl_H

#include <sys/time.h>
#include <time.h>

#include "Pt/DateTime.h"
#include "Pt/System/TimeValue.h"


namespace Pt {

namespace System {

class PT_API ClockImpl
{
	public:
		ClockImpl();
		~ClockImpl();

        void start ();
		TimeValue stop();

		static DateTime getCurrentTime();

	private:
		struct timeval  _startTime;
		struct timeval  _stopTime;
		struct timezone _timeZone;
};

} // namespace Pt

} // namespace System


#endif // PT_ClockImpl_H
