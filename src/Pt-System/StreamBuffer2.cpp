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

    void StreamBuffer2::StreamBufferImpl::streamBufferBeginRead(StreamBuffer2& sb)
    {
        if(_ioDevice == 0 || _ioDevice->reading())
            return;

        if(!_ibuffer)
            _ibuffer = new char[_ibufferSize];

        size_t putback = _pbmax;
        size_t leftover = 0;

        // Keep chars for putback
        if(sb.gptr())
        {
            putback    = std::min<size_t>(sb.gptr() - sb.eback(), _pbmax);
            char* to   = _ibuffer + _pbmax - putback;
            char* from = sb.gptr() - putback;

            leftover = sb.egptr() - sb.gptr();
            std::memmove(to, from, putback + leftover);
        }

        size_t used = _pbmax + leftover;

        if(_ibufferSize == used)
            throw std::logic_error(PT_ERROR_MSG("StreamBuffer is full"));

        _ioDevice->beginRead(_ibuffer + used, _ibufferSize - used);

        sb.setg(_ibuffer + (_pbmax - putback), // start of get area
                _ibuffer + used,               // gptr position
                _ibuffer + used);              // end of get area
    }

    void StreamBuffer2::StreamBufferImpl::streamBufferOnRead(StreamBuffer2& sb)
    { _inputReady.send(sb); }

    void StreamBuffer2::StreamBufferImpl::streamBufferEndRead(StreamBuffer2& sb)
    {
        size_t readSize = _ioDevice->endRead();

        sb.setg(sb.eback(),             // start of get area
                sb.gptr(),              // gptr position
                sb.egptr() + readSize); // end of get area
    }

    size_t StreamBuffer2::StreamBufferImpl::streamBufferBeginWrite(StreamBuffer2& sb)
    {
        if(_ioDevice == 0 || _ioDevice->writing())
            return 0;

        if(sb.pptr())
        {
            size_t avail = sb.pptr() - sb.pbase();
            if(avail > 0)
                return _ioDevice->beginWrite(_obuffer, avail);
        }

        return 0;
    }

    void StreamBuffer2::StreamBufferImpl::streamBufferOnWrite(StreamBuffer2& sb)
    { _outputReady.send(sb); }

    size_t StreamBuffer2::StreamBufferImpl::streamBufferEndWrite(StreamBuffer2& sb)
    {
        typedef StreamBuffer2::traits_type traits_type;

        size_t leftover = 0;
        size_t written  = 0;

        if(sb.pptr())
        {
            size_t avail = sb.pptr() - sb.pbase();
            written      = _ioDevice->endWrite();

            leftover = avail - written;
            if(leftover > 0)
                traits_type::move(_obuffer, _obuffer + written, leftover);
        }

        sb.setp(_obuffer, _obuffer + _obufferSize);
        sb.pbump(leftover);
    
        return written;
    }
    
    void StreamBuffer2::StreamBufferImpl::streamBufferDiscard(StreamBuffer2& sb)
    {
        if(_ioDevice && (_ioDevice->reading() || _ioDevice->writing()))
            throw IOPending(PT_ERROR_MSG("discard failed - streambuffer is in use"));

        sb.setg(0, 0, 0);

        if(_obuffer)
            sb.setp(_obuffer, _obuffer + _obufferSize);
        else
            sb.setp(0, 0);
    }

    std::streamsize StreamBuffer2::StreamBufferImpl::streamBufferXspeekn(StreamBuffer2& sb, char* buffer, std::streamsize size)
    {
        typedef StreamBuffer2::traits_type traits_type;

        if(traits_type::eof() == sb.underflow())
            return 0;

        const std::streamsize avail = sb.egptr() - sb.gptr();
        size = std::min(avail, size);
        if(size == 0)
            return 0;

        std::memcpy(buffer, sb.gptr(), sizeof(char) * size);
        return size;
    }

    std::streambuf::pos_type StreamBuffer2::StreamBufferImpl::streamBufferSeekoff(StreamBuffer2& sb, std::streambuf::off_type off, std::ios::seekdir dir, std::ios::openmode)
    {
        typedef StreamBuffer2::pos_type pos_type;
        typedef StreamBuffer2::off_type off_type;

        pos_type ret = pos_type(off_type(-1));

        if(!_ioDevice || !_ioDevice->enabled() || !_ioDevice->seekable() || off == 0)
            return ret;

        if(_ioDevice->writing())
            sb.endWrite();

        if(_ioDevice->reading())
            sb.endRead();

        ret = _ioDevice->seek(off, dir);

        // Eliminate currently buffered sequence
        sb.discard();

        return ret;
    }

    std::streambuf::pos_type StreamBuffer2::StreamBufferImpl::streamBufferSeekpos(StreamBuffer2& sb, std::streambuf::pos_type p, std::ios::openmode mode)
    { return sb.seekoff(p, std::ios::beg, mode); }

    std::streamsize StreamBuffer2::StreamBufferImpl::streamBufferShowfull(StreamBuffer2& sb)
    { return 0; }

    std::streambuf::int_type StreamBuffer2::StreamBufferImpl::streamBufferPbackfail(StreamBuffer2& sb, std::streambuf::int_type)
    {
        typedef StreamBuffer2::traits_type traits_type;
        return traits_type::eof();
    }

}

}
