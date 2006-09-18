#include "Pt/Api.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/SharedLib.h"

#include <windows.h>


namespace Pt {

namespace System {

class PT_EXPORT SharedLibImpl {
	public:
		SharedLibImpl()
		: _handle(0)
		{ }

		SharedLibImpl(const char* path, SharedLib::BindMode mode)
			: _handle(0)
		{
			_handle = ::LoadLibrary( path );
			if(_handle == 0)
				throw SystemError("Could not open shared library", PT_SOURCEINFO);
		}

		~SharedLibImpl() throw()
		{
			if(_handle != 0) {
				::FreeLibrary(_handle);
			}
		}

		void open(const char* path, SharedLib::BindMode mode)
		{
			if(_handle != 0) {
				return;
			}

			_handle = ::LoadLibrary( path );
			if(_handle == 0)
				throw SystemError("Could not open shared library", PT_SOURCEINFO);
		}

		void* resolve(const char* symbol)
		{
			if(_handle == 0) {
				return 0;
			}

			return ::GetProcAddress(_handle, symbol);
		}

		bool failed()
		{ return _handle == 0; }

	public:
		static void* openResolve(const char* path, const char* symbol)
		{
			HMODULE handle = ::LoadLibrary( path );

			// throw different exception when path does not exist
			if(handle == 0)
				throw SystemError("Could not open shared library", PT_SOURCEINFO);

			return ::GetProcAddress(handle, symbol);
		}

	private:
		HMODULE _handle;
};

} // namespace System

} // namespace Pt