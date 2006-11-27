#ifndef PTV_SYSTEM_CLOCK_H
#define PTV_SYSTEM_CLOCK_H

#include <Pt/Api.h>
#include <Pt/DateTime.h>
#include <Pt/Types.h>
#include <Pt/System/TimeValue.h>


namespace Pt {

namespace System {

class PT_API Clock
{
	public:
		/** @brief Contructs a Clock
		*/
		Clock();

		/** @brief Destructor
		*/
		~Clock();

        /** @brief Start the clock.
            @return Return the micro seconds part.
        */
        void start();

        /** @brief Stop the clock.
            @return Return the time diference.
        */
		TimeValue stop();

		/** @brief Gets the current time
		    @return Returns the current time
		*/
		static DateTime getCurrentTime();

	private:
		class ClockImpl *_impl;
};

} //namespace Pt

} //namespace System


#endif // PTV_SysTime_H
