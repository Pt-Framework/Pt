#include "Pt/WinVer.h"
#include "Pt/System/Api.h"
#include "Pt/System/Mutex.h"
#include <windows.h>

namespace Pt {

namespace System {

        //! @brief MS Windows specific implementation of the Condition class.
        class ConditionImpl 
		{
        public:
            //! @brief Default Constructor
            /**
                @see Condition
             */
            ConditionImpl();

            //! @brief Default Constructor
            /**
                @see ~Condition
             */
            ~ConditionImpl();

            //! @brief Wait until condition becomes signalled.
            /**
                @see Condition#wait()
             */
			void wait(Mutex& mtx)
			{
			    ConditionImpl::wait(mtx, INFINITE);
			}
            
			//! @brief Wait until condition becomes signalled. Returns true if successful,
            //! @brief false if a timeout occurred.
            /**
                @see Condition#wait()
             */
            bool wait(Mutex& mtx, unsigned int ms );

            //! @brief Unblock a single blocked thread.
            /**
                @see Condition#signal()
             */
            void signal();

            //! @brief Unblock all blocked threads.
            /**
                @see Condition#broadcast()
             */
            void broadcast();

        private:
            HANDLE _event1;
            HANDLE _event2;
            unsigned int _blockCount;
            CRITICAL_SECTION _critSec;
    };

} //namespace System

} // namespace Pt


