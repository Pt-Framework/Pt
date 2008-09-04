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

#include "Pt/System/IOBuffer.h"
#include <algorithm>
#include <cstring> //memcpy/memmove()

namespace Pt {

namespace System {

IOBuffer::IOBuffer(IODevice& ioDevice, size_t bufferSize)
: _selector(0),
    _ioDevice(&ioDevice),
    _buffer(0),
    _bufferSize(bufferSize),
    _putbackMax(4)
{
    _buffer = new char[_bufferSize];

    this->setg(0, 0, 0);
    this->setp(0, 0);

    this->setDevice(ioDevice);
}


IOBuffer::IOBuffer(size_t bufferSize)
: _selector(0),
    _ioDevice(0),
    _buffer(0),
    _bufferSize(bufferSize),
    _putbackMax(4)
{
    _buffer = new char[_bufferSize];

    this->setg(0, 0, 0);
    this->setp(0, 0);
}


IOBuffer::~IOBuffer()
{
    delete[] _buffer;
}


void IOBuffer::setDevice(IODevice& ioDevice)
{
    if(_ioDevice)
    {
        //if(_selector)
        //    _selector->remove(*_ioDevice);

        disconnect(ioDevice.inputReady, *this, &IOBuffer::onRead);
        disconnect(ioDevice.outputReady, *this, &IOBuffer::onWrite);
    }

    _ioDevice = &ioDevice;

    //if(_selector)
    //    _selector->add(*_ioDevice);

    connect(ioDevice.inputReady, *this, &IOBuffer::onRead);
    connect(ioDevice.outputReady, *this, &IOBuffer::onWrite);
}


IODevice* IOBuffer::device()
{
    return _ioDevice;
}


/*void IOBuffer::setSelector(SelectorBase* selector)
{
    if( _ioDevice)
        _ioDevice->setSelector(selector);

    _selector = selector;
}*/


void IOBuffer::beginSync()
{
    size_t putbackSize = _putbackMax;

    // keep chars for putback if in reading mode
    if( this->gptr() )
    {
        putbackSize = std::min<size_t>(this->gptr() - this->eback(), _putbackMax);
        std::memmove(_buffer + (_putbackMax - putbackSize),
                        this->gptr() - putbackSize,
                        putbackSize * sizeof(char) );
    }

    _ioDevice->beginRead( _buffer + _putbackMax, _bufferSize - _putbackMax );

    // set get area, will also enter reading mode
    this->setg( _buffer + (_putbackMax - putbackSize), // start of get area
                _buffer + _putbackMax, // gptr position
                _buffer + _putbackMax ); // end of get area
}


void IOBuffer::onRead(IODevice& dev)
{
    size_t readSize = dev.endRead();

    if( _ioDevice->eof() )
    {
        this->setg(0, 0, 0);
        return;
    }

    // set get area, will also enter reading mode
    this->setg( this->eback(), // start of get area
                this->gptr(), // gptr position
                this->egptr() + readSize ); // end of get area

    inputReady.send(*this);
}


size_t IOBuffer::out_avail() const
{
    if( this->pptr() )
        return this->epptr() - this->pptr();

    return _bufferSize;
}


void IOBuffer::beginFlush()
{
    // if in writing mode pptr is valid, write out the buffer
    if( this->pptr() )
    {
        // write buffer to device
        const size_t avail = this->pptr() - this->pbase();
        _ioDevice->beginWrite(_buffer, avail);
    }
}


void IOBuffer::onWrite(IODevice& dev)
{
    size_t leftover = 0;

    // if in writing mode pptr is valid, flush out the buffer
    if( this->pptr() )
    {
        const size_t avail = this->pptr() - this->pbase();
        size_t written = dev.endWrite();

        // setup put buffer area
        leftover = avail - written;
        if(leftover != 0)
        {
            traits_type::move(_buffer, _buffer + written, leftover);
        }
    }

    // this will also enter writing mode if pptr is not valid
    this->setp(_buffer + leftover, _buffer + _bufferSize);

    outputReady.send(*this);
}


int IOBuffer::sync()
{
    if(!_ioDevice)
        return 0;

    // if in writing mode flush put area
    if( this->pptr() )
    {
        const int_type ch = this->overflow( traits_type::eof() );
        if( ch == traits_type::eof() )
        {
            return -1;
        }

        _ioDevice->sync();
    }

    return 0;
}


std::streamsize IOBuffer::_peek(char* buffer, std::streamsize size)
{
    // can not peek in writing mode
    if( this->pptr() )
        return 0;

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


IOBuffer::int_type
IOBuffer::underflow()
{
    // return EOF if in writing mode or no device set
    if( !_ioDevice || this->pptr() )
        return traits_type::eof();

    // buffer is not empty yet.
    if( this->gptr() < this->egptr() ) {
        return traits_type::to_int_type( *(this->gptr()) );
    }

    if( _ioDevice->eof() )
        return traits_type::eof();

    size_t putbackSize = _putbackMax;

    // keep chars for putback if in reading mode
    if( this->gptr() ) {
        putbackSize = std::min<size_t>(this->gptr() - this->eback(), _putbackMax);
        std::memmove(_buffer + (_putbackMax - putbackSize),
                        this->gptr() - putbackSize,
                        putbackSize * sizeof(char) );
    }

    size_t readSize = _ioDevice->read( _buffer + _putbackMax, _bufferSize - _putbackMax );

    // set get area, will also enter reading mode
    this->setg( _buffer + (_putbackMax - putbackSize), // start of get area
                _buffer + _putbackMax, // gptr position
                _buffer + _putbackMax + readSize ); // end of get area

    if( _ioDevice->eof() )
        return traits_type::eof();

    return traits_type::to_int_type( *(this->gptr()) );
}


IOBuffer::int_type
IOBuffer::overflow(int_type ch)
{
    // return EOF if we are in reading mode or no device is set
    if(!_ioDevice || this->gptr() )
        return traits_type::eof();

    size_t leftover = 0;

    // if in writing mode pptr is valid, flush out the buffer
    if( this->pptr() )
    {
        // write buffer to device
        const size_t avail = this->pptr() - this->pbase();
        size_t written = _ioDevice->write(_buffer, avail);
        // check EOF ???

        // setup put buffer area
        const size_t leftover = avail - written;
        if(leftover != 0) {
            traits_type::move(_buffer, _buffer + written, leftover);
        }
    }

    // this will also enter writing mode if pptr is not valid
    this->setp(_buffer + leftover, _buffer + _bufferSize);

    // if the overflow char is not EOF put it in buffer
    if( traits_type::eq_int_type(ch, traits_type::eof()) ==  false )
    {
        *this->pptr() = traits_type::to_char_type(ch);
        this->pbump(1);
    }

    return traits_type::not_eof(ch);
}


IOBuffer::pos_type
IOBuffer::seekoff(off_type offset, std::ios::seekdir sd, std::ios::openmode mode)
{
    if(mode == std::ios::out)
        return pos_type(-1);

    if(!_ioDevice || !_ioDevice->seekable() )
        return pos_type(-1);

    return pos_type(-1);
}

}

}
