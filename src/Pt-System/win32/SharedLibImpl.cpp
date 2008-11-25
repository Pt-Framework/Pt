#include "SharedLibImpl.h"

namespace Pt {

namespace System {

void SharedLibImpl::open(const std::string& path)
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


void* SharedLibImpl::resolve(const char* symbol)
{
	if(_handle == 0)
		return 0;

	std::basic_string<TCHAR> tsymbol;
    win32::fromMultiByte(symbol, tsymbol);
	return (void*) ( ::GetProcAddress( _handle, tsymbol.c_str() ) );
}

} // namespace System

} // namespace Pt
