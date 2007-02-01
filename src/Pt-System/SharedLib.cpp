/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2006 by PTV AG                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "Pt/System/SharedLib.h"
#include "SharedLibImpl.h"

#include "Pt/System/Directory.h"
#include "Pt/System/Environment.h"

#include <string>
#include <sstream>
#include <iostream>


namespace {

std::string addSharedLibraryExtension(const std::string& path)
{
	Pt::ssize_t separatorPos = path.rfind( Pt::System::Directory::separator() );

	std::string fileName((separatorPos != -1) ? path.substr(separatorPos + 1) : path);
	std::string onlyPath((separatorPos != -1) ? path.substr(0, separatorPos + 1)  : "");

	Pt::ssize_t extensionPos = fileName.rfind('.');

	std::string extension((extensionPos != -1) ? fileName.substr(extensionPos + 1) : "");
	std::string fileNameWithoutExtension((extensionPos != -1)
	                                ? fileName.substr(0, fileName.length() - extension.length() - 1)
	                                : fileName);

	std::stringstream result;

	result << onlyPath
	       << Pt::System::Environment::sharedLibraryPrefix()
	       << fileNameWithoutExtension;

	if (extension.empty())
	{
		result << Pt::System::Environment::sharedLibraryExtension();
	}
	else
	{
		result << "." << extension;
	}

	return result.str();
}

}


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


void pt_system_testSharedLib()
{ std::cerr << "ptv_system_testSharedLib() called." << std::endl; }
