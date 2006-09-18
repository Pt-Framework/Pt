#ifndef Pt_SharedLib_h
#define Pt_SharedLib_h

#include <Pt/Api.h>
#include <Pt/NonCopyable.h>

#include <string>

namespace Pt {

namespace System {

//! Shared library loader
/*!
  A class that can be used to dynamically load shared libraries
  to resolve symbols from it. The example below shows how to retrieve
  the address of the cos function in the math library:

  \code
    SharedLib shlib("libm.so");
    double (*cosine)(double);
    cosine = shlib["cos"];
    (*cosine)(2.0);
  \endcode
*/
class PT_EXPORT SharedLib : private NonCopyable {
	public:
		//! Binding mode
		enum BindMode {
			BindNow,  /** < Bind now */
			BindLazy  /** < Bind lazy (on first call) */
		};

		//! Default Constructor
		SharedLib();

		//! Constructor
		/**
			Constructs the object and loads the shared library specified
			in the path argument.

			@param path the path to the shared library
			@param mode enum specifying the mode when loading the shlib
		*/
		SharedLib(const char* path, BindMode mode = BindNow);

		//! Destructor
		/**
			The destructor unloads the shared library from memory.
		*/
		~SharedLib();

		//! Loads a shared library.
		/**
			Loads a shared library.

			@param path the path to the shared library
			@param mode enum specifying the mode when loading the shlib
		*/
		SharedLib& open(const char* path, BindMode mode = BindNow);

		//! Resolve symbol from shared library
		/**
			Resolves a symbol from the shared library.

			@symbol the name of the symbol to be resolved.
			@return the address of the symbol or NULL if it was not found
		*/
		void* operator[](const char* symbol);

		//! Resolve symbol from shared library
		/**
			Resolves a symbol from the shared library.

			@symbol the name of the symbol to be resolved.
			@return the address of the symbol or NULL if it was not found
		*/
		void* resolve(const char* symbol);

		//! Object status checking
		/**
			@return NULL if object is in a failed state, otherwise non-NULL
		*/
		operator void*();

		//! Object status checking
		/**
			@return true if object is in a failed state
		*/
		bool operator!();

	public:
		//! Resolve symbol from a shared library
		/**
			Loads and resolves a symbol from the shared library.

			@param path the path to the shared library
			@symbol the name of the symbol to be resolved.
			@return the address of the symbol or NULL if it was not found
		*/
		static void* openResolve(const char* path, const char* symbol);

	private:
		class SharedLibImpl* _impl;
};

}

}

extern "C" {

	PT_EXPORT void Pt_System_testSharedLib();

}

#endif
