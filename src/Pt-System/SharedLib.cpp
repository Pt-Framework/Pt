#include "Pt/System/SharedLib.h"
#include "SharedLibImpl.h"

#include "Pt/System/Directory.h"
#include "Pt/System/Environment.h"

#include <string>
#include <sstream>
#include <iostream>
using namespace std;


namespace Pt {

namespace System {

SharedLib::SharedLib()
: _impl(0)
{
	_impl = new SharedLibImpl();
}


SharedLib::SharedLib(const std::string& path)
: _impl(0)
{
	std::string pathWithExtension = addSharedLibraryExtension(path);
	
	_impl = new SharedLibImpl(pathWithExtension);
}


SharedLib::~SharedLib()
{
	delete _impl;
}


SharedLib& SharedLib::open(const std::string& path)
{
	std::string pathWithExtension = addSharedLibraryExtension(path);
	
	_impl->open(pathWithExtension);
	return *this;
}


std::string SharedLib::addSharedLibraryExtension(const std::string& path)
{
	ssize_t separatorPos = path.rfind(Directory::separator());
	
	string fileName((separatorPos != -1) ? path.substr(separatorPos + 1) : path);
	string onlyPath((separatorPos != -1) ? path.substr(0, separatorPos + 1)  : "");
	
	ssize_t extensionPos = fileName.rfind('.');
	
	string extension((extensionPos != -1) ? fileName.substr(extensionPos + 1) : "");
	string fileNameWithoutExtension((extensionPos != -1)
	                                ? fileName.substr(0, fileName.length() - extension.length() - 1)
	                                : fileName);
	
	stringstream result;
	
	result << onlyPath << Environment::sharedLibraryPrefix() << fileNameWithoutExtension;

	if (extension.empty())
	{
		result << Environment::sharedLibraryExtension();
	}
	else
	{
		result << "." << extension;
	}
	
	return result.str();
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


void* SharedLib::openResolve(const std::string& path, const char* symbol)
{
	std::string pathWithExtension = addSharedLibraryExtension(path);

	return SharedLibImpl::openResolve(pathWithExtension, symbol);
}

} // namespace System

} // namespace Pt


void ptv_system_testSharedLib()
{ cerr << "ptv_system_testSharedLib() called." << endl; }
