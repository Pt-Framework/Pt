/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
#ifndef PT_SYSTEM_SHAREDMEMORYIMPL_H
#define PT_SYSTEM_SHAREDMEMORYIMPL_H

#include "Pt/Api.h"
#include "Pt/System/SharedMemory.h"
#include "Pt/System/SystemError.h"
#include <windows.h>

namespace Pt {

namespace System {

class PT_API SharedMemoryImpl {
	public:

		//! @brief Constructor
		//  @throw SystemError
		SharedMemoryImpl(const char* name, size_t sz, SharedMemory::OpenMode omode);

		//! @brief Destructor
		~SharedMemoryImpl();

		//! @brief MS Windows specific implementation of unlink()
		/**
			@see SharedMemory#unlink()
			@throw SystemError
		*/
		void unlink();

		//! @brief MS Windows specific implementation of map()
		/**
			@see SharedMemory#map()

			@param addr ignored
			@throw SystemError
		*/
		void* map(const void* addr);

		//! @brief MS Windows specific implementation of unmap()
		/**
			@see SharedMemory#unmap()
			@throw SystemError
		*/
		void unmap(void* addr);

	private:
		LPCSTR _name;
		DWORD  _mode;
		DWORD  _size;
		HANDLE _handle;
};

} // !namespace System

} // !namespace Pt

#endif
