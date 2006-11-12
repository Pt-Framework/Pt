#ifndef PT_SHAREDLIBIMPL_H
#define PT_SHAREDLIBIMPL_H

#include "Pt/Api.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/SharedLib.h"

#include "win32.h"

#include <vector>
#include <iostream>
#include <windows.h>


namespace Pt {

namespace System {

class PT_API SharedLibImpl {
	public:
		//! @brief default Constructor
		SharedLibImpl()
		: _handle(0)
		{ }

		//! @brief Constructor which takes the path to the shared library to load
		/**
			@see SharedLib#SharedLib()
			@param path the shared library to load implicitely
		*/
		SharedLibImpl(const char* path)
		: _handle(0)
		{
			this->open(path);
		}

		//! @brief Destructor
		~SharedLibImpl() throw()
		{
			if(_handle != 0) {
				::FreeLibrary(_handle);
			}
		}

		//! @brief Loads the shared library specified by path
		/**
			This method holds the operating system dependend code to actually
			load the shared library.

			@see SharedLib#open()
			@param path the shared library to load
		*/

		void open(const char* path)
		{
			if(_handle != 0) {
				return;
			}

			std::basic_string<TCHAR> tpath = win32::fromMultiByte(path);
			_handle = ::LoadLibrary( tpath.c_str() );

			if(_handle == 0)
				throw SystemError("Could not open shared library", PT_SOURCEINFO);
		}

		//! @brief Resolves the symbol specified by symbol
		/**
			This method holds the operating system dependend code to actually
			resolve the symbol within the shared library.

			@see SharedLib#resolve()
			@param symbol the symbol to resolve
			@return the resolved symbol or 0 if the symbol cannot be resolved
		*/

		void* resolve(const char* symbol)
		{
			if(_handle == 0) {
				return 0;
			}

			std::basic_string<TCHAR> tsymbol = win32::fromMultiByte(symbol);
			return (void*) ( ::GetProcAddress( _handle, tsymbol.c_str() ) );
		}

		//! @brief Returns if the loading of the shared library was successful or not
		/**
			@see SharedLib#failed()
			@return true if the loading of the shared library has failed,
			false otherwise
		*/
		bool failed()
		{ return _handle == 0; }

	public:

		//! @brief Implicitely loads the shared library specified by path and tries to resolve the symbol specified by symbol
		/**
			This method contains the operating system dependend code to load the
			shared library and to resolve the desired symbol.

			@see SharedLib#failed()
			@param path the shared library to load
			@param symbol the symbol to resolve within the loaded library
			@param the resolved symbol or 0 if the loading of the shared
			library has failed
		*/
		static void* openResolve(const char* path, const char* symbol)
		{
			std::basic_string<TCHAR> tpath = win32::fromMultiByte(path);
			HMODULE handle = ::LoadLibrary( tpath.c_str() );

			// TODO: throw different exception when path does not exist
			if(handle == 0)
				throw SystemError("Could not open shared library", PT_SOURCEINFO);

			std::basic_string<TCHAR> tsymbol = win32::fromMultiByte(symbol);
			return (void*) ::GetProcAddress( handle, tsymbol.c_str() );
		}

	private:
		HMODULE _handle;
};

} // namespace System

} // namespace Pt

#endif
