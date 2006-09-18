#include "Pt/System/SharedLib.h"

#include "Pt/System/SystemError.h"

#include <string>
#include <iostream>

#include <dlfcn.h>


namespace Pt {

namespace System {


class SharedLibImpl {
	public:
		SharedLibImpl()
		: _handle(0)
		{ }

		SharedLibImpl(const char* path, SharedLib::BindMode mode)
		: _handle(0)
		{
			this->open(path, mode);
		}

		~SharedLibImpl() throw()
		{
			if(_handle)
				::dlclose(_handle);
		}

		void open(const char* path, SharedLib::BindMode mode)
		{
			if(_handle)
				return;

			int flags = 0;
			switch(mode)
			{
				case SharedLib::BindLazy:
					flags = RTLD_LAZY;
					break;
				case SharedLib::BindNow:
					flags = RTLD_NOW;
					break;
			}
			//flags |= RTLD_GLOBAL; this causes a SIGSEGV when loading multiple plugins with same exported vars

			_handle = ::dlopen(path, flags);
			if( !_handle ) {
				throw SystemError(dlerror(), PT_SOURCEINFO);
			}
		}

		void* resolve(const char* symbol)
		{
			if(_handle)
				return ::dlsym(_handle, symbol);
				
			return 0;
		}

		bool failed()
		{ return _handle == 0; }

	public:
		static void* openResolve(const char* path, const char* symbol)
		{
			void* handle = ::dlopen(path, RTLD_NOW);
			if(handle)
				return ::dlsym(handle, symbol);
				
			return 0;
		}

	private:
		void* _handle;
};

} // namespace System

} // namespace Pt
