/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef PT_MUTEXIMPL_H
#define PT_MUTEXIMPL_H

#include "Pt/Api.h"
#include "Pt/System/Mutex.h"
#include <windows.h>


namespace Pt {

namespace System {

	//! @brief MS Windows specific mutex class
	/**
		This class represents the MS Windows specific implementation
		of the Mutex class. It is used as delegate from the common
		Mutex class.
	*/
	class MutexImpl {
		public:
			//! @brief Default Constructor
			MutexImpl(Mutex& mutex);

			//! @brief Destructor
			~MutexImpl();

			//! @brief MS Windows specific implementation of lock()
			/**
				@see Mutex#lock()
			*/
			void lock();

			//! @brief MS Windows specific implementation of tryLock()
			/**
				@see Mutex#tryLock()
			*/
			bool tryLock(unsigned int msec);

			//! @brief MS Windows specific implementation of unlock()
			/**
				@see Mutex#unlock()
			*/
			void unlock();

		private:
			Mutex& _mutex;
			HANDLE _handle;
	};

} // namespace System

} // namespace Pt

#endif
