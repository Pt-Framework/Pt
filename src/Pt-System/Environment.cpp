/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
 *                                                                         *
 ***************************************************************************/

#include "Pt/System/Environment.h"
#include "EnvironmentImpl.h"

namespace Pt {
namespace System {


Environment::Environment()
: _impl(0)
{
	_impl = new EnvironmentImpl();
}

Environment::~Environment()
{
	delete _impl;
}

const std::string& Environment::sharedLibraryExtension()
{
    return EnvironmentImpl::sharedLibraryExtension();
}

const std::string& Environment::sharedLibraryPrefix()
{
    return EnvironmentImpl::sharedLibraryPrefix();
}

} // namespace Pt
} // namespace System
