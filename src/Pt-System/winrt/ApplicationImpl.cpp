/*
 * Copyright (C) 2013 Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "../win32/win32.h"
#include "ApplicationImpl.h"
#include <Pt/System/IOError.h>

using namespace Platform;
using namespace Windows::Storage;

namespace Pt {

namespace System {

ApplicationImpl::ApplicationImpl()
{
}


ApplicationImpl::~ApplicationImpl()
{
}


void ApplicationImpl::init(EventLoop& s)
{
}


bool ApplicationImpl::ignoreSystemSignal(int sig)
{
    return false;
}


bool ApplicationImpl::catchSystemSignal(int sig)
{
    return false;
}


bool ApplicationImpl::raiseSystemSignal(int sig)
{
    return false;
}


void ApplicationImpl::chdir(const std::string& path)
{
    throw AccessFailed("chdir not supported");
}


std::string ApplicationImpl::cwd()
{
    // Windows.Storage

    // might want RoamingFolder?
    String^ path = ApplicationData::Current->LocalFolder->Path;
    return win32::toMultiByte( path->Data() );
}


std::string ApplicationImpl::tmpdir()
{
    // Windows.Storage
    String^ path = ApplicationData::Current->TemporaryFolder->Path;
    return win32::toMultiByte( path->Data() );
}


std::string ApplicationImpl::rootdir()
{
    return "c:\\";
}


unsigned long ApplicationImpl::usedMemory()
{
    return 0;
}


void ApplicationImpl::setEnvVar(const std::string& name, const std::string& value)
{
    // TODO: implement file based environment in app data dir
}


void ApplicationImpl::unsetEnvVar(const std::string& name)
{
    // TODO: implement file based environment in app data dir
}


std::string ApplicationImpl::getEnvVar(const std::string& name)
{
    return std::string();
}

} // namespace System

} // namespace Pt
