#include "LibraryImpl.h"
#include "win32.h"

namespace Pt {

namespace System {

void LibraryImpl::open(const std::string& path)
{
	if(_handle != 0)
		return;

	std::basic_string<TCHAR> tpath;
    win32::fromMultiByte(path, tpath);
	_handle = ::LoadLibrary( tpath.c_str() );

	if(_handle == 0)
	{
		throw OpenLibraryFailed( path, PT_SOURCEINFO );
	}
}


void LibraryImpl::close()
{
    if(_handle != 0)
        ::FreeLibrary(_handle);
}


void* LibraryImpl::resolve(const char* symbol) const
{
	if(_handle == 0)
		return 0;

	std::basic_string<TCHAR> tsymbol;
    win32::fromMultiByte(symbol, tsymbol);
	return (void*) ( ::GetProcAddress( _handle, tsymbol.c_str() ) );
}

} // namespace System

} // namespace Pt
