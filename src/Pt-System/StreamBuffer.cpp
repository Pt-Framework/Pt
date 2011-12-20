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

#include "Pt/System/StreamBuffer.h"
#include <algorithm>
#include <stdexcept>
#include <cstring>

namespace Pt {

namespace System {

StreamBufferImpl::StreamBufferImpl(StreamBuffer& sb, size_t bufferSize, bool extend)
: _ioDevice   (0),
    _ibufferSize(0),
    _ibuffer    (0),
    _obufferSize(0),
    _obuffer    (0),
    _pbmax      (4),
    _oextend    (extend)
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

void StreamBufferImpl::attach(StreamBuffer& sb, IODevice& ioDevice)
{
    if( ioDevice.reading() || ioDevice.writing() )
        throw IOPending( PT_ERROR_MSG("IODevice in use") );

    if(_ioDevice)
    {
        if( ioDevice.reading() || ioDevice.writing() )
            throw IOPending( PT_ERROR_MSG("IODevice in use") );

        disconnect(ioDevice.inputReady,  sb, &StreamBuffer::onRead );
        disconnect(ioDevice.outputReady, sb, &StreamBuffer::onWrite);
    }

    _ioDevice = &ioDevice;
    connect(ioDevice.inputReady,  sb, &StreamBuffer::onRead );
    connect(ioDevice.outputReady, sb, &StreamBuffer::onWrite);
}

void StreamBufferImpl::beginRead(StreamBuffer& sb)
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

void StreamBufferImpl::onRead(StreamBuffer& sb)
{ _inputReady.send(sb); }

void StreamBufferImpl::endRead(StreamBuffer& sb)
{
    size_t readSize = _ioDevice->endRead();

    sb.setg(sb.eback(),             // start of get area
            sb.gptr(),              // gptr position
            sb.egptr() + readSize); // end of get area
}

size_t StreamBufferImpl::beginWrite(StreamBuffer& sb)
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

void StreamBufferImpl::onWrite(StreamBuffer& sb)
{ _outputReady.send(sb); }

size_t StreamBufferImpl::endWrite(StreamBuffer& sb)
{
    typedef StreamBuffer::traits_type traits_type;

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

void StreamBufferImpl::discard(StreamBuffer& sb)
{
    if(_ioDevice && (_ioDevice->reading() || _ioDevice->writing()))
        throw IOPending(PT_ERROR_MSG("discard failed - streambuffer is in use"));

    sb.setg(0, 0, 0);

    if(_obuffer)
        sb.setp(_obuffer, _obuffer + _obufferSize);
    else
        sb.setp(0, 0);
}

int StreamBufferImpl::sync(StreamBuffer& sb)
{
    typedef StreamBuffer::traits_type traits_type;

    if(!_ioDevice )
        return 0;

    if(sb.pptr())
    {
        while(sb.pptr() > sb.pbase())
        {
            const StreamBuffer::int_type ch = sb.overflow(traits_type::eof());
            if(ch == traits_type::eof())
                return -1;

            _ioDevice->sync();
        }
    }

    return 0;
}

std::streambuf::int_type StreamBufferImpl::underflow(StreamBuffer& sb)
{
    typedef StreamBuffer::traits_type traits_type;

    if(!_ioDevice)
        return traits_type::eof();

    if(_ioDevice->reading())
        sb.endRead();

    if(sb.gptr() < sb.egptr())
        return traits_type::to_int_type(*(sb.gptr()));

    if(_ioDevice->eof())
        return traits_type::eof();

    if(!_ibuffer)
        _ibuffer = new char[_ibufferSize];

    size_t putback = _pbmax;

    if(sb.gptr())
    {
        putback = std::min<size_t>(sb.gptr() - sb.eback(), _pbmax);
        std::memmove(_ibuffer + (_pbmax - putback),
                        sb.gptr() - putback,
                        putback );
    }

    size_t readSize = _ioDevice->read(_ibuffer + _pbmax, _ibufferSize - _pbmax);

    sb.setg(_ibuffer + _pbmax - putback,    // start of get area
            _ibuffer + _pbmax,              // gptr position
            _ibuffer + _pbmax + readSize ); // end of get area

    if(_ioDevice->eof())
        return traits_type::eof();

    return traits_type::to_int_type(*(sb.gptr()));
}

std::streambuf::int_type StreamBufferImpl::overflow(StreamBuffer& sb, std::streambuf::int_type ch)
{
    typedef StreamBuffer::traits_type traits_type;

    if(!_ioDevice)
        return traits_type::eof();

    if(!_obuffer)
    {
        _obuffer = new char[_obufferSize];
        sb.setp(_obuffer, _obuffer + _obufferSize);
    }
    else if(_ioDevice->writing()) // beginWrite is unfinished
    {
        sb.endWrite();
    }
    else if(traits_type::eq_int_type(ch, traits_type::eof()) || ! _oextend)
    {
        // normal blocking overflow case
        size_t avail    = sb.pptr() - _obuffer;
        size_t written  = _ioDevice->write(_obuffer, avail);
        size_t leftover = avail - written;

        if(leftover > 0)
            traits_type::move(_obuffer, _obuffer + written, leftover);

        sb.setp(_obuffer, _obuffer + _obufferSize);
        sb.pbump(leftover);
    }
    else
    {
        // if the buffer area is extensible and overflow is not called by
        // sync/flush we copy the output buffer to a larger one
        size_t bufsize = _obufferSize + (_obufferSize / 2);
        char* buf = new char[ bufsize ];
        traits_type::copy(buf, _obuffer, _obufferSize);
        std::swap(_obuffer, buf);
        sb.setp(_obuffer, _obuffer + bufsize);
        sb.pbump(_obufferSize);
        _obufferSize = bufsize;
        delete [] buf;
    }

    // if the overflow char is not EOF put it in buffer
    if(traits_type::eq_int_type(ch, traits_type::eof()) == false)
    {
        *sb.pptr() = traits_type::to_char_type(ch);
        sb.pbump(1);
    }

    return traits_type::not_eof(ch);
}

std::streamsize StreamBufferImpl::xspeekn(StreamBuffer& sb, char* buffer, std::streamsize size)
{
    typedef StreamBuffer::traits_type traits_type;

    if(traits_type::eof() == sb.underflow())
        return 0;

    const std::streamsize avail = sb.egptr() - sb.gptr();
    size = std::min(avail, size);
    if(size == 0)
        return 0;

    std::memcpy(buffer, sb.gptr(), sizeof(char) * size);
    return size;
}

std::streambuf::pos_type StreamBufferImpl::seekoff(StreamBuffer& sb, std::streambuf::off_type off, std::ios::seekdir dir, std::ios::openmode)
{
    typedef StreamBuffer::pos_type pos_type;
    typedef StreamBuffer::off_type off_type;

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

std::streambuf::pos_type StreamBufferImpl::seekpos(StreamBuffer& sb, std::streambuf::pos_type p, std::ios::openmode mode)
{ return sb.seekoff(p, std::ios::beg, mode); }

std::streamsize StreamBufferImpl::showfull(StreamBuffer& sb)
{ return 0; }

std::streambuf::int_type StreamBufferImpl::pbackfail(StreamBuffer& sb, std::streambuf::int_type)
{
    typedef StreamBuffer::traits_type traits_type;
    return traits_type::eof();
}

}

}
