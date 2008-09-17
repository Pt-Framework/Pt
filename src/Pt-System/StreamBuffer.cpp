/***************************************************************************
 *   Copyright (C) 2004-2008 Marc Boris Duerner                            *
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

#include "Pt/System/StreamBuffer.h"
#include <algorithm>
#include <stdexcept>
#include <cstring>

namespace Pt {

namespace System {

StreamBuffer::StreamBuffer(IODevice& ioDevice, size_t bufferSize)
: _ioDevice(&ioDevice),
  _ibuffer(0),
  _obuffer(0),
  _bufferSize(bufferSize+4),
  _pbmax(4),
  _syncing(false),
  _flushing(false)
{
    //_ibuffer = new char[_bufferSize];

    this->setg(0, 0, 0);
    this->setp(0, 0);

    this->attach(ioDevice);
}


StreamBuffer::StreamBuffer(size_t bufferSize)
: _ioDevice(0),
  _ibuffer(0),
  _obuffer(0),
  _bufferSize(bufferSize+4),
  _pbmax(4),
  _syncing(false),
  _flushing(false)
{
    //_ibuffer = new char[_bufferSize];

    this->setg(0, 0, 0);
    this->setp(0, 0);
}


StreamBuffer::~StreamBuffer()
{
    delete[] _ibuffer;
    delete[] _obuffer;
}


void StreamBuffer::attach(IODevice& ioDevice)
{
    if( ioDevice.busy() )
        throw IOPending("IODevice in use", PT_SOURCEINFO);

    if(_ioDevice)
    {
        if( _ioDevice->busy() )
            throw IOPending("IODevice in use", PT_SOURCEINFO);

        disconnect(ioDevice.inputReady, *this, &StreamBuffer::onSync);
        disconnect(ioDevice.outputReady, *this, &StreamBuffer::onFlush);
    }

    _ioDevice = &ioDevice;
    connect(ioDevice.inputReady, *this, &StreamBuffer::onSync);
    connect(ioDevice.outputReady, *this, &StreamBuffer::onFlush);
}


IODevice* StreamBuffer::device()
{
    return _ioDevice;
}


void StreamBuffer::beginSync()
{
    if(_syncing || _ioDevice == 0)
        return;

    if( ! _ibuffer )
    {
        _ibuffer = new char[_bufferSize];
    }

    size_t putback = _pbmax;
    size_t leftover = 0;

    // keep chars for putback
    if( this->gptr() )
    {
        putback = std::min<size_t>( gptr() - eback(), _pbmax);
        char* to = _ibuffer + _pbmax - putback;
        char* from = this->gptr() - putback;

        if(to == from)
            throw std::logic_error(PT_SOURCEINFO + "StreamBuffer is full");

        leftover = egptr() - gptr();
        std::memmove( to, from, putback + leftover );
    }

    size_t used = _pbmax + leftover;
    _ioDevice->beginRead( _ibuffer + used, _bufferSize - used );
    _syncing = true;

    this->setg( _ibuffer + (_pbmax - putback), // start of get area
                _ibuffer + used, // gptr position
                _ibuffer + used ); // end of get area
}


void StreamBuffer::onSync(IODevice& dev)
{
    this->endSync();
    inputReady.send(*this);
}


void StreamBuffer::endSync()
{
    size_t readSize = _ioDevice->endRead();
    _syncing = false;

    this->setg( this->eback(), // start of get area
                this->gptr(), // gptr position
                this->egptr() + readSize ); // end of get area
}


StreamBuffer::int_type StreamBuffer::underflow()
{
    if( ! _ioDevice )
        return traits_type::eof();

    if(_syncing)
        this->endSync();

    if( this->gptr() < this->egptr() )
        return traits_type::to_int_type( *(this->gptr()) );

    if( _ioDevice->eof() )
        return traits_type::eof();

    if( ! _ibuffer )
    {
        _ibuffer = new char[_bufferSize];
    }

    size_t putback = _pbmax;

    if( this->gptr() )
    {
        putback = std::min<size_t>(this->gptr() - this->eback(), _pbmax);
        std::memmove( _ibuffer + (_pbmax - putback),
                      this->gptr() - putback,
                      putback );
    }

    size_t readSize = _ioDevice->read( _ibuffer + _pbmax, _bufferSize - _pbmax );

    this->setg( _ibuffer + _pbmax - putback,    // start of get area
                _ibuffer + _pbmax,              // gptr position
                _ibuffer + _pbmax + readSize ); // end of get area

    if( _ioDevice->eof() )
        return traits_type::eof();

    return traits_type::to_int_type( *(this->gptr()) );
}


std::streamsize StreamBuffer::showmanyp()
{
    return _bufferSize;
}


void StreamBuffer::beginFlush()
{
    if(_flushing || _ioDevice == 0 )
        return;

    if( this->pptr() )
    {
        size_t avail = this->pptr() - this->pbase();
        if(avail > 0)
        {
            _ioDevice->beginWrite(_obuffer, avail);
            _flushing = true;
        }
    }
}


void StreamBuffer::onFlush(IODevice& dev)
{
    this->endFlush();
    outputReady.send(*this);
}


void StreamBuffer::endFlush()
{
    _flushing = false;
    size_t leftover = 0;

    if( this->pptr() )
    {
        size_t avail = this->pptr() - this->pbase();
        size_t written = _ioDevice->endWrite();

        leftover = avail - written;
        if(leftover > 0)
        {
            traits_type::move(_obuffer, _obuffer + written, leftover);
        }
    }

    this->setp(_obuffer + leftover, _obuffer + _bufferSize);
}


StreamBuffer::int_type StreamBuffer::overflow(int_type ch)
{
    if( ! _ioDevice )
        return traits_type::eof();

    if( ! _obuffer )
    {
        _obuffer = new char[_bufferSize];
        this->setp(_obuffer, _obuffer + _bufferSize);
    }

    if(_flushing)
    {
        this->endFlush();
    }
    else if( this->pptr() > this->pbase() )
    {
        size_t avail = this->pptr() - this->pbase();
        size_t written = _ioDevice->write(_obuffer, avail);
        size_t leftover = avail - written;

        if(leftover > 0)
        {
            traits_type::move(_obuffer, _obuffer + written, leftover);
        }
        this->setp(_obuffer + leftover, _obuffer + _bufferSize);
    }

    // if the overflow char is not EOF put it in buffer
    if( traits_type::eq_int_type(ch, traits_type::eof()) ==  false )
    {
        *this->pptr() = traits_type::to_char_type(ch);
        this->pbump(1);
    }

    return traits_type::not_eof(ch);
}


StreamBuffer::int_type StreamBuffer::pbackfail(StreamBuffer::int_type)
{
    return traits_type::eof();
}


int StreamBuffer::sync()
{
    if( ! _ioDevice )
        return 0;

    if( pptr() )
    {
        while( this->pptr() > this->pbase() )
        {
            const int_type ch = this->overflow( traits_type::eof() );
            if( ch == traits_type::eof() )
            {
                return -1;
            }

            _ioDevice->sync();
        }
    }

    return 0;
}


std::streamsize StreamBuffer::xspeekn(char* buffer, std::streamsize size)
{
    if( traits_type::eof() == this->underflow() )
        return 0;

    const std::streamsize avail = this->egptr() - this->gptr();
    size = std::min(avail, size);
    if(size == 0) {
        return 0;
    }

    std::memcpy(buffer, this->gptr(), sizeof(char) * size);
    return size;
}


StreamBuffer::pos_type
StreamBuffer::seekoff(off_type off, std::ios::seekdir dir, std::ios::openmode)
{
    pos_type ret =  pos_type( off_type(-1) );

    if ( ! _ioDevice || ! _ioDevice->enabled() ||
         ! _ioDevice->seekable() || off == 0)
    {
        return ret;
    }

    if(_flushing)
    {
        this->endFlush();
    }

    if(_syncing)
    {
        this->endSync();
    }

    ret = _ioDevice->seek(off, dir);

    // eliminate currently buffered sequence
    this->setg(0, 0, 0);
    this->setp(0, 0);

    return ret;
}


StreamBuffer::pos_type
StreamBuffer::seekpos(pos_type p, std::ios::openmode mode)
{
    return this->seekoff(p, std::ios::beg, mode);
}

}

}
