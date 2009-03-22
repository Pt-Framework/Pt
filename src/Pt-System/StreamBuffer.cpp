/*
 * Copyright (C) 2004-2008 Marc Boris Duerner
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

#include "Pt/System/StreamBuffer.h"
#include <algorithm>
#include <stdexcept>
#include <cstring>

namespace Pt {

namespace System {

void StreamBuffer::Attach(StreamBuffer& sb, IODevice& ioDevice)
{
    if( ioDevice.busy() )
        throw IOPending( PT_ERROR_MSG("IODevice in use") );

    if(sb._ioDevice)
    {
        if( sb._ioDevice->busy() )
            throw IOPending( PT_ERROR_MSG("IODevice in use") );

        disconnect(ioDevice.inputReady, sb, &StreamBuffer::onRead);
        disconnect(ioDevice.outputReady, sb, &StreamBuffer::onWrite);
    }

    sb._ioDevice = &ioDevice;
    connect(ioDevice.inputReady, sb, &StreamBuffer::onRead);
    connect(ioDevice.outputReady, sb, &StreamBuffer::onWrite);
}


void StreamBuffer::BeginRead(StreamBuffer& sb)
{
    if(sb._reading || sb._ioDevice == 0)
        return;

    if( ! sb._ibuffer )
    {
        sb._ibuffer = new char[sb._bufferSize];
    }

    size_t putback = sb._pbmax;
    size_t leftover = 0;

    // keep chars for putback
    if( sb.gptr() )
    {
        putback = std::min<size_t>( sb.gptr() - sb.eback(), sb._pbmax);
        char* to = sb._ibuffer + sb._pbmax - putback;
        char* from = sb.gptr() - putback;

        if(to == from)
            throw std::logic_error( PT_ERROR_MSG("StreamBuffer is full") );

        leftover = sb.egptr() - sb.gptr();
        std::memmove( to, from, putback + leftover );
    }

    size_t used = sb._pbmax + leftover;
    sb._ioDevice->beginRead( sb._ibuffer + used, sb._bufferSize - used );
    sb._reading = true;

    sb.setg( sb._ibuffer + (sb._pbmax - putback), // start of get area
                sb._ibuffer + used, // gptr position
                sb._ibuffer + used ); // end of get area
}

}

}
