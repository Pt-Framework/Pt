/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef PT_SEMAPHOREIMPL_H
#define PT_SEMAPHOREIMPL_H

#include <Pt/Api.h>
#include <windows.h>


namespace Pt {

namespace System {

	//! @brief MS Windows specific semaphore class
	/**
		This class represents the MS Windows specific implementation
		of the Semaphore class. It is used as delegate from the common
		Semaphore class.
	*/
	class PT_API SemaphoreImpl {
		public:
			//! @brief Default Constructor
			SemaphoreImpl(unsigned int initial = 0);

			//! @brief Destructor
			~SemaphoreImpl();

			//! @brief MS Windows specific implementation of wait()
			/**
				@see Semaphore#wait()
			*/
			void wait();

			//! @brief MS Windows specific implementation of tryWait()
			/**
				@see Semaphore#tryWait()
			*/
			bool tryWait();

			//! @brief MS Windows specific implementation of post()
			/**
				@see Semaphore#post()
			*/
			void post();

		private:
			HANDLE _handle;
	};

} // !namespace System

} // !namespace Pt

#endif
