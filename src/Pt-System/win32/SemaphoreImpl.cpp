/*
 * Copyright (C) 2006-2013 Marc Boris Dürner
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

#include "SemaphoreImpl.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/Semaphore.h"
#include <climits>

namespace Pt {

namespace System {

SemaphoreImpl::SemaphoreImpl(unsigned int initial)
{
#ifdef __cplusplus_winrt
    _handle = CreateSemaphoreExW(NULL, initial, LONG_MAX, 0, 0, SEMAPHORE_ALL_ACCESS);
#else
    _handle = CreateSemaphore(NULL, initial, LONG_MAX, 0);
#endif

    if( !_handle )
        throw SystemError("CreateSemaphore");
}


SemaphoreImpl::~SemaphoreImpl()
{
    CloseHandle(_handle);
}


void SemaphoreImpl::wait()
{
#ifdef __cplusplus_winrt
    DWORD ret = WaitForSingleObjectEx(_handle, INFINITE, FALSE);
#else
    DWORD ret = WaitForSingleObject(_handle, INFINITE);
#endif

    if(ret == WAIT_FAILED)
        throw SystemError("WaitForSingleObject");
}


bool SemaphoreImpl::tryWait()
{
#ifdef __cplusplus_winrt
    DWORD ret = WaitForSingleObjectEx(_handle, 0, FALSE);
#else
    DWORD ret = WaitForSingleObject(_handle, 0);
#endif

    if(ret == WAIT_FAILED) {
        throw SystemError("WaitForSingleObject");
    }
    else if(ret == WAIT_OBJECT_0) {
        return true;
    }

    return false;
}


void SemaphoreImpl::post()
{
    if( 0 == ReleaseSemaphore(_handle, 1, NULL) )
        throw SystemError("ReleaseSemaphore");
}

} // namespace System;

} // namespace Pt
