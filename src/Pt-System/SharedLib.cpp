#include "Pt/System/SharedLib.h"
#include "SharedLibImpl.h"

#include <string>
#include <iostream>
using namespace std;


namespace Pt {

namespace System {

SharedLib::SharedLib()
: _impl(0)
{
	_impl = new SharedLibImpl();
}


SharedLib::SharedLib(const char* name)
: _impl(0)
{
	_impl = new SharedLibImpl(name);
}


SharedLib::~SharedLib()
{
	delete _impl;
}


SharedLib& SharedLib::open(const char* path)
{
  _impl->open(path);
  return *this;
}


void* SharedLib::operator[](const char* symbol)
{
  return _impl->resolve(symbol);
}


void* SharedLib::resolve(const char* symbol)
{
  return _impl->resolve(symbol);
}


SharedLib::operator void*()
{
	return _impl->failed() ? 0 : this;
}


bool SharedLib::operator!()
{
	return _impl->failed() ? true : false;
}


void* SharedLib::openResolve(const char* path, const char* symbol)
{
	return SharedLibImpl::openResolve(path, symbol);
}

} // namespace System

} // namespace Pt


void ptv_system_testSharedLib()
{ cerr << "ptv_system_testSharedLib() called." << endl; }
