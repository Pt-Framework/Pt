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

void StreamBufferInit(StreamBuffer& sb, size_t bufferSize, bool extend)
{
    sb._ibufferSize = bufferSize + 4;
    sb._ibuffer = 0;
    sb._obufferSize = bufferSize;
    sb._obuffer = 0;
    //sb._pbmax = 4;
    sb._oextend = extend;

    if( sb.gptr() )
        sb.setg(sb._ibuffer, sb._ibuffer + sb._ibufferSize, sb._ibuffer + sb._ibufferSize);

    if( sb.pptr() )
        sb.setp(sb._obuffer, sb._obuffer + sb._obufferSize);
}


void StreamBufferAttach(StreamBuffer& sb, IODevice& ioDevice)
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


void StreamBufferBeginRead(StreamBuffer& sb)
{
    if(sb._ioDevice == 0 || sb._ioDevice->reading())
        return;

    if( ! sb._ibuffer )
    {
        sb._ibuffer = new char[sb._ibufferSize];
    }

    size_t putback = sb._pbmax;
    size_t leftover = 0;

    // keep chars for putback
    if( sb.gptr() )
    {
        putback = std::min<size_t>( sb.gptr() - sb.eback(), sb._pbmax);
        char* to = sb._ibuffer + sb._pbmax - putback;
        char* from = sb.gptr() - putback;

        leftover = sb.egptr() - sb.gptr();
        std::memmove( to, from, putback + leftover );
    }

    size_t used = sb._pbmax + leftover;

    if( sb._ibufferSize == used )
        throw std::logic_error( PT_ERROR_MSG("StreamBuffer is full") );

    sb._ioDevice->beginRead( sb._ibuffer + used, sb._ibufferSize - used );

    sb.setg( sb._ibuffer + (sb._pbmax - putback), // start of get area
             sb._ibuffer + used, // gptr position
             sb._ibuffer + used ); // end of get area
}


void StreamBufferEndRead(StreamBuffer& sb)
{
    size_t readSize = sb._ioDevice->endRead();

    sb.setg( sb.eback(), // start of get area
             sb.gptr(), // gptr position
             sb.egptr() + readSize ); // end of get area
}


size_t StreamBufferBeginWrite(StreamBuffer& sb)
{
    if(sb._ioDevice == 0 || sb._ioDevice->writing())
        return 0;

    if( sb.pptr() )
    {
        size_t avail = sb.pptr() - sb.pbase();
        if(avail > 0)
        {
            return sb._ioDevice->beginWrite(sb._obuffer, avail);
        }
    }

    return 0;
}


size_t StreamBufferEndWrite(StreamBuffer& sb)
{
    typedef StreamBuffer::traits_type traits_type;

    size_t leftover = 0;
    size_t written = 0;

    if( sb.pptr() )
    {
        size_t avail = sb.pptr() - sb.pbase();
        written = sb._ioDevice->endWrite();

        leftover = avail - written;
        if(leftover > 0)
        {
            traits_type::move(sb._obuffer, sb._obuffer + written, leftover);
        }
    }

    sb.setp(sb._obuffer, sb._obuffer + sb._obufferSize);
    sb.pbump( leftover );

    return written;
}


void StreamBufferDiscard(StreamBuffer& sb)
{
    if (sb._ioDevice && (sb._ioDevice->reading() || sb._ioDevice->writing()))
        throw IOPending( PT_ERROR_MSG("discard failed - streambuffer is in use") );

    sb.setg(0, 0, 0);
    sb.setp(0, 0);
}


int StreamBufferSync(StreamBuffer& sb)
{
    typedef StreamBuffer::traits_type traits_type;

    if( ! sb._ioDevice )
        return 0;

    if( sb.pptr() )
    {
        while( sb.pptr() > sb.pbase() )
        {
            const StreamBuffer::int_type ch = sb.overflow( traits_type::eof() );
            if( ch == traits_type::eof() )
            {
                return -1;
            }

            sb._ioDevice->sync();
        }
    }

    return 0;
}


std::streambuf::int_type StreamBufferUnderflow(StreamBuffer& sb)
{
    typedef StreamBuffer::traits_type traits_type;

    if( ! sb._ioDevice )
        return traits_type::eof();

    if(sb._ioDevice->reading())
        sb.endRead();

    if( sb.gptr() < sb.egptr() )
        return traits_type::to_int_type( *(sb.gptr()) );

    if( sb._ioDevice->eof() )
        return traits_type::eof();

    if( ! sb._ibuffer )
    {
        sb._ibuffer = new char[sb._ibufferSize];
    }

    size_t putback = sb._pbmax;

    if( sb.gptr() )
    {
        putback = std::min<size_t>(sb.gptr() - sb.eback(), sb._pbmax);
        std::memmove( sb._ibuffer + (sb._pbmax - putback),
                      sb.gptr() - putback,
                      putback );
    }

    size_t readSize = sb._ioDevice->read( sb._ibuffer + sb._pbmax, sb._ibufferSize - sb._pbmax );

    sb.setg( sb._ibuffer + sb._pbmax - putback,    // start of get area
             sb._ibuffer + sb._pbmax,              // gptr position
             sb._ibuffer + sb._pbmax + readSize ); // end of get area

    if( sb._ioDevice->eof() )
        return traits_type::eof();

    return traits_type::to_int_type( *(sb.gptr()) );
}


std::streambuf::int_type StreamBufferOverflow(StreamBuffer& sb, std::streambuf::int_type ch)
{
    typedef StreamBuffer::traits_type traits_type;

    if( ! sb._ioDevice )
        return traits_type::eof();

    if( ! sb._obuffer )
    {
        sb._obuffer = new char[sb._obufferSize];
        sb.setp(sb._obuffer, sb._obuffer + sb._obufferSize);
    }
    else if(sb._ioDevice->writing()) // beginWrite is unfinished
    {
        sb.endWrite();
    }
    else if (traits_type::eq_int_type( ch, traits_type::eof() ) || ! sb._oextend)
    {
        // normal blocking overflow case
        size_t avail = sb.pptr() - sb._obuffer;
        size_t written = sb._ioDevice->write(sb._obuffer, avail);
        size_t leftover = avail - written;

        if(leftover > 0)
        {
            traits_type::move(sb._obuffer, sb._obuffer + written, leftover);
        }
        sb.setp(sb._obuffer, sb._obuffer + sb._obufferSize);
        sb.pbump( leftover );
    }
    else
    {
        // if the buffer area is extensible and overflow is not called by
        // sync/flush we copy the output buffer to a larger one
        size_t bufsize = sb._obufferSize + (sb._obufferSize / 2);
        char* buf = new char[ bufsize ];
        traits_type::copy(buf, sb._obuffer, sb._obufferSize);
        std::swap(sb._obuffer, buf);
        sb.setp(sb._obuffer, sb._obuffer + bufsize);
        sb.pbump( sb._obufferSize );
        sb._obufferSize = bufsize;
        delete [] buf;
    }

    // if the overflow char is not EOF put it in buffer
    if( traits_type::eq_int_type(ch, traits_type::eof()) ==  false )
    {
        *sb.pptr() = traits_type::to_char_type(ch);
        sb.pbump(1);
    }

    return traits_type::not_eof(ch);
}


std::streamsize StreamBufferXspeekn(StreamBuffer& sb, char* buffer, std::streamsize size)
{
    typedef StreamBuffer::traits_type traits_type;

    if( traits_type::eof() == sb.underflow() )
        return 0;

    const std::streamsize avail = sb.egptr() - sb.gptr();
    size = std::min(avail, size);
    if(size == 0) {
        return 0;
    }

    std::memcpy(buffer, sb.gptr(), sizeof(char) * size);
    return size;
}


std::streambuf::pos_type StreamBufferSeekoff(StreamBuffer& sb, 
                                             std::streambuf::off_type off, 
                                             std::ios::seekdir dir, 
                                             std::ios::openmode)
{
    typedef StreamBuffer::pos_type pos_type;
    typedef StreamBuffer::off_type off_type;

    pos_type ret = pos_type( off_type(-1) );

    if ( ! sb._ioDevice || 
         ! sb._ioDevice->enabled() ||
         ! sb._ioDevice->seekable() || 
         off == 0)
    {
        return ret;
    }

    if( sb._ioDevice->writing() )
    {
        sb.endWrite();
    }

    if( sb._ioDevice->reading() )
    {
        sb.endRead();
    }

    ret = sb._ioDevice->seek(off, dir);

    // eliminate currently buffered sequence
    sb.discard();

    return ret;
}


std::streambuf::pos_type StreamBufferSeekpos(StreamBuffer& sb, 
                                             std::streambuf::pos_type p, 
                                             std::ios::openmode mode)
{
    return sb.seekoff(p, std::ios::beg, mode);
}


std::streamsize StreamBufferShowfull(StreamBuffer& sb)
{
    return 0;
}

std::streambuf::int_type StreamBufferPbackfail(StreamBuffer& sb, std::streambuf::int_type)
{
    typedef StreamBuffer::traits_type traits_type;
    return traits_type::eof();
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StreamBufferBase::StreamBufferBase(size_t bufferSize, bool extend)
: _sb(0),
  _ioDevice(0),
  _ibufferSize(bufferSize+4),
  _ibuffer(0),
  _obufferSize(bufferSize),
  _obuffer(0),
  _pbmax(4),
  _oextend(extend)
{
}


StreamBufferBase::~StreamBufferBase()
{
}


void StreamBufferBase::init(StreamBuffer& sb)
{ 
    _sb = &sb; 

    if (_sb->gptr())
        _sb->setg(_ibuffer, _ibuffer + _ibufferSize, _ibuffer + _ibufferSize);

    if (_sb->pptr())
        _sb->setp(_obuffer, _obuffer + _obufferSize);
}


void StreamBufferBase::attach(IODevice& ioDevice)
{
    if( ioDevice.busy() )
        throw IOPending( PT_ERROR_MSG("IODevice in use") );

    if(_ioDevice)
    {
        if( _ioDevice->busy() )
            throw IOPending( PT_ERROR_MSG("IODevice in use") );

        disconnect(ioDevice.inputReady, *this, &StreamBuffer::onRead);
        disconnect(ioDevice.outputReady, *this, &StreamBuffer::onWrite);
    }

    _ioDevice = &ioDevice;
    connect(ioDevice.inputReady, *this, &StreamBuffer::onRead);
    connect(ioDevice.outputReady, *this, &StreamBuffer::onWrite);
}


void StreamBufferBase::beginRead()
{
    if(_ioDevice == 0 || _ioDevice->reading())
        return;

    if( ! _ibuffer )
    {
        _ibuffer = new char[_ibufferSize];
    }

    size_t putback = _pbmax;
    size_t leftover = 0;

    // keep chars for putback
    if( _sb->gptr() )
    {
        putback = std::min<size_t>( _sb->gptr() - _sb->eback(), _sb->_pbmax);
        char* to = _ibuffer + _pbmax - putback;
        char* from = _sb->gptr() - putback;

        leftover = _sb->egptr() - _sb->gptr();
        std::memmove( to, from, putback + leftover );
    }

    size_t used = _pbmax + leftover;

    if( _ibufferSize == used )
        throw std::logic_error( PT_ERROR_MSG("StreamBuffer is full") );

    _ioDevice->beginRead( _ibuffer + used, _ibufferSize - used );

    _sb->setg( _ibuffer + (_pbmax - putback), // start of get area
               _ibuffer + used, // gptr position
               _ibuffer + used ); // end of get area
}


void StreamBufferBase::onRead(IODevice& dev)
{
    inputReady.send(*_sb);
}


void StreamBufferBase::endRead()
{
    size_t readSize = _ioDevice->endRead();

    _sb->setg( _sb->eback(), // start of get area
               _sb->gptr(), // gptr position
               _sb->egptr() + readSize ); // end of get area
}


size_t StreamBufferBase::beginWrite()
{
    if(_ioDevice == 0 || _ioDevice->writing())
        return 0;

    if( _sb->pptr() )
    {
        size_t avail = _sb->pptr() - _sb->pbase();
        if(avail > 0)
        {
            return _ioDevice->beginWrite(_obuffer, avail);
        }
    }

    return 0;
}


void StreamBufferBase::onWrite(IODevice& dev)
{
    outputReady.send(*_sb);
}


size_t StreamBufferBase::endWrite()
{
    size_t leftover = 0;
    size_t written = 0;

    if( _sb->pptr() )
    {
        size_t avail = _sb->pptr() - _sb->pbase();
        written = _ioDevice->endWrite();

        leftover = avail - written;
        if(leftover > 0)
        {
            traits_type::move(_obuffer, _obuffer + written, leftover);
        }
    }

    _sb->setp(_obuffer, _obuffer + _obufferSize);
    _sb->pbump( leftover );

    return written;
}


void StreamBufferBase::discard()
{
    if (_ioDevice && (_ioDevice->reading() || _ioDevice->writing()))
        throw IOPending( PT_ERROR_MSG("discard failed - streambuffer is in use") );

    _sb->setg(0, 0, 0);
    _sb->setp(0, 0);
}


int StreamBufferBase::do_sync()
{
    if( ! _ioDevice )
        return 0;

    if( _sb->pptr() )
    {
        while( _sb->pptr() > _sb->pbase() )
        {
            const int_type ch = _sb->overflow( traits_type::eof() );
            if( ch == traits_type::eof() )
            {
                return -1;
            }

            _ioDevice->sync();
        }
    }

    return 0;
}


StreamBufferBase::int_type StreamBufferBase::do_underflow()
{
    if( ! _ioDevice )
        return traits_type::eof();

    if(_ioDevice->reading())
        this->endRead();

    if( _sb->gptr() < _sb->egptr() )
        return traits_type::to_int_type( *(_sb->gptr()) );

    if( _ioDevice->eof() )
        return traits_type::eof();

    if( ! _ibuffer )
    {
        _ibuffer = new char[_ibufferSize];
    }

    size_t putback = _pbmax;

    if( _sb->gptr() )
    {
        putback = std::min<size_t>(_sb->gptr() - _sb->eback(), _pbmax);
        std::memmove( _ibuffer + (_pbmax - putback),
                      _sb->gptr() - putback,
                      putback );
    }

    size_t readSize = _ioDevice->read( _ibuffer + _pbmax, _ibufferSize - _pbmax );

    _sb->setg( _ibuffer + _pbmax - putback,    // start of get area
               _ibuffer + _pbmax,              // gptr position
               _ibuffer + _pbmax + readSize ); // end of get area

    if( _ioDevice->eof() )
        return traits_type::eof();

    return traits_type::to_int_type( *(_sb->gptr()) );
}


StreamBufferBase::int_type StreamBufferBase::do_overflow(int_type ch)
{
    if( ! _ioDevice )
        return traits_type::eof();

    if( ! _obuffer )
    {
        _obuffer = new char[_obufferSize];
        _sb->setp(_obuffer, _obuffer + _obufferSize);
    }
    else if(_ioDevice->writing()) // beginWrite is unfinished
    {
        this->endWrite();
    }
    else if (traits_type::eq_int_type( ch, traits_type::eof() ) || !_oextend)
    {
        // normal blocking overflow case
        size_t avail = _sb->pptr() - _obuffer;
        size_t written = _ioDevice->write(_obuffer, avail);
        size_t leftover = avail - written;

        if(leftover > 0)
        {
            traits_type::move(_obuffer, _obuffer + written, leftover);
        }
        _sb->setp(_obuffer, _obuffer + _obufferSize);
        _sb->pbump( leftover );
    }
    else
    {
        // if the buffer area is extensible and overflow is not called by
        // sync/flush we copy the output buffer to a larger one
        size_t bufsize = _obufferSize + (_obufferSize/2);
        char* buf = new char[ bufsize ];
        traits_type::copy(buf, _obuffer, _obufferSize);
        std::swap(_obuffer, buf);
        _sb->setp(_obuffer, _obuffer + bufsize);
        _sb->pbump( _obufferSize );
        _obufferSize = bufsize;
        delete [] buf;
    }

    // if the overflow char is not EOF put it in buffer
    if( traits_type::eq_int_type(ch, traits_type::eof()) ==  false )
    {
        *_sb->pptr() = traits_type::to_char_type(ch);
        _sb->pbump(1);
    }

    return traits_type::not_eof(ch);
}


std::streamsize StreamBufferBase::do_xspeekn(char* buffer, std::streamsize size)
{
    if( traits_type::eof() == _sb->underflow() )
        return 0;

    const std::streamsize avail = _sb->egptr() - _sb->gptr();
    size = std::min(avail, size);
    if(size == 0) {
        return 0;
    }

    std::memcpy(buffer, _sb->gptr(), sizeof(char) * size);
    return size;
}


StreamBufferBase::pos_type
StreamBufferBase::do_seekoff(off_type off, std::ios::seekdir dir, std::ios::openmode)
{
    pos_type ret =  pos_type( off_type(-1) );

    if ( ! _ioDevice || ! _ioDevice->enabled() ||
         ! _ioDevice->seekable() || off == 0)
    {
        return ret;
    }

    if(_ioDevice->writing())
    {
        this->endWrite();
    }

    if(_ioDevice->reading())
    {
        this->endRead();
    }

    ret = _ioDevice->seek(off, dir);

    // eliminate currently buffered sequence
    discard();

    return ret;
}


StreamBufferBase::pos_type
StreamBufferBase::do_seekpos(pos_type p, std::ios::openmode mode)
{
    return _sb->seekoff(p, std::ios::beg, mode);
}


std::streamsize StreamBufferBase::do_showfull()
{
    return 0;
}

StreamBuffer::int_type StreamBufferBase::do_pbackfail(StreamBuffer::int_type)
{
    return traits_type::eof();
}

}

}
