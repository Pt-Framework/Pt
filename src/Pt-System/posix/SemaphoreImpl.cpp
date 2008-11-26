/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
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
#include "SemaphoreImpl.h"
#include "Pt/System/SystemError.h"
#include <cerrno>

namespace Pt {

namespace System {

SemaphoreImpl::SemaphoreImpl(unsigned int initial)
{
    int ret = sem_init(&_handle, 0, initial);
    if( ret == -1 )
        throw SystemError( PT_ERROR_MSG("sem_init failed") );
}


SemaphoreImpl::~SemaphoreImpl()
{
    sem_destroy( &_handle );
}


void SemaphoreImpl::wait()
{
    int ret = sem_wait(&_handle);
    if(ret == -1)
        throw SystemError( PT_ERROR_MSG("sem_wait failed") );
}


bool SemaphoreImpl::tryWait()
{
    int ret = sem_trywait( &_handle );
    if(ret == -1)
    {
        if(errno == EAGAIN)
            return false;

        throw SystemError( PT_ERROR_MSG("sem_trywait failed") );
    }

    return true;
}


void SemaphoreImpl::post()
{
    again:
    if( 0 != sem_post(&_handle) )
    {
        if(errno == EINTR)
            goto again;

        throw SystemError( PT_ERROR_MSG("sem_post failed") );
    }
}

} // namespace System;

} // namespace Pt
