/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "SemaphoreImpl.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/Semaphore.h"


namespace Pt {

namespace System {

SemaphoreImpl::SemaphoreImpl(unsigned int initial)
{
    _handle = CreateSemaphore(NULL, initial, LONG_MAX, 0);

    if( !_handle )
        throw SystemError( PT_ERROR_MSG("Could not create semaphore.") );
}


SemaphoreImpl::~SemaphoreImpl()
{
    CloseHandle(_handle);
}


void SemaphoreImpl::wait()
{
    DWORD ret = WaitForSingleObject(_handle, INFINITE);
    if(ret == WAIT_FAILED)
        throw SystemError( PT_ERROR_MSG("Could not wait on semaphore"));
}


bool SemaphoreImpl::tryWait()
{
    DWORD ret = WaitForSingleObject(_handle, 0);
    if(ret == WAIT_FAILED) {
        throw SystemError( PT_ERROR_MSG("Could not wait on semaphore") );
    }
    else if(ret == WAIT_OBJECT_0) {
        return true;
    }

    return false;
}


void SemaphoreImpl::post()
{
    if( 0 == ReleaseSemaphore(_handle, 1, NULL) )
        throw SystemError( PT_ERROR_MSG("Could not post semaphore") );
}

} // namespace System;

} // namespace Pt
