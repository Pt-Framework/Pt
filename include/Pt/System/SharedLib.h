/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef PT_SYSTEM_SHAREDLIB_H
#define PT_SYSTEM_SHAREDLIB_H

#include <Pt/Api.h>
#include <Pt/NonCopyable.h>

#include <string>


namespace Pt {

namespace System {

//! @ingroup ptv-system
//! @brief Shared library loader
/**
  A class that can be used to dynamically load shared libraries
  to resolve symbols from it. The example below shows how to retrieve
  the address of the myProcedure function in the MySharedLib library:

  @code
    typedef int (*MyProcType)();
	SharedLib shlib("MySharedLib.dll");
	void* procAddr = shlib["myProcedure"];
	MyProcType proc;
	int result = -1;

	proc = (MyProcType)procAddr;
    result = proc();
  @endcode
*/
class PT_API SharedLib : private NonCopyable {
	public:

		//! @brief Default Constructor
		SharedLib();

		//! @brief Constructor
		/**
			Constructs the object and loads the shared library specified
			in the path argument.

			@param path the path to the shared library
		*/
		SharedLib(const char* path);

		//! @brief Destructor
		/**
			The destructor unloads the shared library from memory.
		*/
		~SharedLib();

		//! @brief Loads a shared library.
		/**
			Loads a shared library.

			@param path the path to the shared library
		*/
		SharedLib& open(const char* path);

		//! @brief Resolve symbol from shared library
		/**
			Resolves a symbol from the shared library.

			@param symbol the name of the symbol to be resolved.
			@return the address of the symbol or NULL if it was not found
		*/
		void* operator[](const char* symbol);

		//! @brief Resolve symbol from shared library
		/**
			Resolves a symbol from the shared library.

			@param symbol the name of the symbol to be resolved.
			@return the address of the symbol or NULL if it was not found
		*/
		void* resolve(const char* symbol);

		//! @brief Object status checking
		/**
			@return NULL if object is in a failed state, otherwise non-NULL
		*/
		operator void*();

		//! @brief Object status checking
		/**
			@return true if object is in a failed state
		*/
		bool operator!();

	public:
		//! @brief Resolve symbol from a shared library
		/**
			Loads and resolves a symbol from the shared library.

			@param path the path to the shared library
			@param symbol the name of the symbol to be resolved.
			@return the address of the symbol or NULL if it was not found
		*/
		static void* openResolve(const char* path, const char* symbol);

	private:
		class SharedLibImpl* _impl;
};

}

}

extern "C" {

	/// \exclude
	PT_API void pt_system_testSharedLib();

}

#endif
