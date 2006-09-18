/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Drner                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "Pt/Api.h"
#include "Pt/System/Mutex.h"

#include <pthread.h>


namespace Pt {

namespace System {

	//! @brief POSIX specific implementation of the Condition class.
	class PT_EXPORT ConditionImpl {
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
			void wait(Mutex& mtx);

			//! @brief Wait until condition becomes signalled. Returns true if successful,
			//! @brief false if a timeout occurred.
			/**
				@see Condition#wait()
			 */
			bool wait(Mutex& mtx, unsigned int ms);

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
			pthread_cond_t _cond;
	};

}

}


