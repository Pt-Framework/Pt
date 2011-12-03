/*
 * Copyright (C) 2004-2008 Marc Boris Duerner
 * Copyright (C) 2011      Aloysius Indayanto        
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

#include "Pt/System/StreamBuffer2.h"
#include <algorithm>
#include <stdexcept>
#include <cstring>

namespace Pt {

namespace System {

    StreamBuffer2::StreamBufferImpl::StreamBufferImpl(size_t bufferSize, bool extend)
    : _ioDevice   (0),
      _ibufferSize(0),
      _ibuffer    (0),
      _obufferSize(0),
      _obuffer    (0),
      _pbmax      (4),
      _oextend    (extend)
    {}

    void StreamBuffer2::StreamBufferImpl::streamBufferInit(StreamBuffer2& sb, size_t bufferSize, bool extend)
    {
        _ibufferSize = bufferSize + 4;
        _ibuffer = 0;
        _obufferSize = bufferSize;
        _obuffer = 0;
        _oextend = extend;

        if( sb.gptr() )
            sb.setg(_ibuffer, _ibuffer + _ibufferSize, _ibuffer + _ibufferSize);

        if( sb.pptr() )
            sb.setp(_obuffer, _obuffer + _obufferSize);
    }

    void StreamBuffer2::StreamBufferImpl::streamBufferAttach(StreamBuffer2& sb, IODevice& ioDevice)
    {
        if(ioDevice.busy())
            throw IOPending( PT_ERROR_MSG("IODevice in use") );

        if(_ioDevice)
        {
            if(_ioDevice->busy())
                throw IOPending( PT_ERROR_MSG("IODevice in use") );

            disconnect(ioDevice.inputReady,  sb, &StreamBuffer2::onRead );
            disconnect(ioDevice.outputReady, sb, &StreamBuffer2::onWrite);
        }

        _ioDevice = &ioDevice;
        connect(ioDevice.inputReady,  sb, &StreamBuffer2::onRead );
        connect(ioDevice.outputReady, sb, &StreamBuffer2::onWrite);
    }

}

}
