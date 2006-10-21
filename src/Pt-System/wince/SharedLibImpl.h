#ifndef MGG_SharedLibImpl
#define MGG_SharedLibImpl

#include "SharedLib.h"
#include <windows.h>


namespace MGG
{


class SharedLibImpl 
{
	public:
		SharedLibImpl()
		: _handle(0)
		{ }

		SharedLibImpl(const char* path, SharedLib::BindMode mode)
        : _handle(0)
		{
            
            const  size_t   len = ::strlen(path);
            wchar_t         wpath[len];

            widen(path,wpath,len);

			_handle = ::LoadLibrary((LPCTSTR) wpath );
//            _handle = ::LoadLibrary(_T("MGNavigation.dll"));
			//if(_handle == 0)
			//	throw SystemError(GetLastError(), "Could not load shared library", PT_SOURCEINFO);
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

			_handle = ::LoadLibrary((LPCTSTR) path );
			//if(_handle == 0)
			//	throw SystemError(GetLastError(), "Could not load shared library", PT_SOURCEINFO);
		}

		void* resolve(const char* symbol)
		{
			if(_handle == 0) {
				return 0;
			}

            LPCWSTR p = (WCHAR*) symbol;

			return ::GetProcAddress(_handle,p);
		}

		bool failed()
		{ return _handle == 0; }

        static void widen(const char* in, wchar_t*& out, size_t outLen)
        {
            for(int i = 0; i < outLen; i++)
                out[i] = in[i];
        }

	public:
		static void* openResolve(const char* path, const char* symbol)
		{
			HMODULE handle = ::LoadLibrary((LPCTSTR) path );
			if(handle == 0) 
				return 0;

			return ::GetProcAddress(handle,(LPCWSTR) &symbol[0]);
		}

	private:
		HMODULE _handle;
};

}
#endif
