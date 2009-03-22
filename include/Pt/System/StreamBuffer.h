/*
 * Copyright (C) 2005 Marc Boris Duerner
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
#ifndef Pt_System_StreamBuffer_h
#define Pt_System_StreamBuffer_h

#include <ios>
#include <streambuf>
#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <cstring>

namespace Pt {

namespace System {

//! @brief A stream buffer for IODevices with linear buffer area
class StreamBuffer : public std::basic_streambuf<char>
                   , public Connectable
{
    PT_SYSTEM_API static void Attach(StreamBuffer& sb, IODevice& ioDevice);
    PT_SYSTEM_API static void BeginRead(StreamBuffer& sb);

    public:
        //! @brief Contructs an IOBuffer for an IODevice
        explicit StreamBuffer(IODevice& ioDevice, size_t bufferSize = 8192, bool extend = false);

        //! @brief Default constructor
        explicit StreamBuffer(size_t bufferSize = 8192, bool extend = false);

        ~StreamBuffer();

        void attach(IODevice& device)
		{ Attach(*this, device); }

        IODevice* device()
		{
		    return _ioDevice;
		}

		void beginRead()
		{ BeginRead(*this); }

        void beginWrite();

        void discard();

        std::streamsize speekn(char* buffer, std::streamsize size)
        { 
		    return this->xspeekn(buffer, size); 
		}

        std::streamsize out_avail()
        {
            if( this->pptr() )
                return this->pptr() - this->pbase();

             return this->showfull();
        }
        
		Signal<StreamBuffer&> inputReady;

        Signal<StreamBuffer&> outputReady;

    protected:
        virtual int sync();

        virtual std::streamsize showfull();

        virtual std::streamsize xspeekn(char* buffer, std::streamsize size);

        virtual int_type underflow();

        virtual int_type overflow(int_type ch);

        virtual int_type pbackfail(int_type c);

        /** @brief  Alters the stream positions
        */
        virtual pos_type seekoff(off_type offset, std::ios::seekdir sd, std::ios::openmode mode);

        /** @brief  Alters the stream positions
        */
        virtual pos_type seekpos(pos_type p, std::ios::openmode mode );

    private:
        void onRead(IODevice& dev);

        void endRead();

        void onWrite(IODevice& dev);

        void endWrite();

    private:
        IODevice* _ioDevice;
        size_t _bufferSize;
        char* _ibuffer;
        std::size_t _obufferSize;
        char* _obuffer;
        bool _oextend;
        const size_t _pbmax;
        bool _reading;
        bool _flushing;
};


inline StreamBuffer::StreamBuffer(IODevice& ioDevice, size_t bufferSize, bool extend)
: _ioDevice(&ioDevice),
  _bufferSize(bufferSize+4),
  _ibuffer(0),
  _obufferSize(bufferSize),
  _obuffer(0),
  _oextend(extend),
  _pbmax(4),
  _reading(false),
  _flushing(false)
{
    this->setg(0, 0, 0);
    this->setp(0, 0);

    this->attach(ioDevice);
}


inline StreamBuffer::StreamBuffer(size_t bufferSize, bool extend)
: _ioDevice(0),
  _bufferSize(bufferSize+4),
  _ibuffer(0),
  _obufferSize(bufferSize),
  _obuffer(0),
  _oextend(extend),
  _pbmax(4),
  _reading(false),
  _flushing(false)
{
    this->setg(0, 0, 0);
    this->setp(0, 0);
}


inline StreamBuffer::~StreamBuffer()
{
    delete[] _ibuffer;
    delete[] _obuffer;
}


/*inline void StreamBuffer::attach(IODevice& ioDevice)
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
}*/


/*inline void StreamBuffer::beginRead()
{
    if(_reading || _ioDevice == 0)
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
            throw std::logic_error( PT_ERROR_MSG("StreamBuffer is full") );

        leftover = egptr() - gptr();
        std::memmove( to, from, putback + leftover );
    }

    size_t used = _pbmax + leftover;
    _ioDevice->beginRead( _ibuffer + used, _bufferSize - used );
    _reading = true;

    this->setg( _ibuffer + (_pbmax - putback), // start of get area
                _ibuffer + used, // gptr position
                _ibuffer + used ); // end of get area
}
*/

inline void StreamBuffer::onRead(IODevice& dev)
{
    this->endRead();
    inputReady.send(*this);
}


inline void StreamBuffer::endRead()
{
    size_t readSize = _ioDevice->endRead();
    _reading = false;

    this->setg( this->eback(), // start of get area
                this->gptr(), // gptr position
                this->egptr() + readSize ); // end of get area
}


inline StreamBuffer::int_type StreamBuffer::underflow()
{
    if( ! _ioDevice )
        return traits_type::eof();

    if(_reading)
        this->endRead();

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


inline std::streamsize StreamBuffer::showfull()
{
    return 0;
}


inline void StreamBuffer::beginWrite()
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


inline void StreamBuffer::discard()
{
    if (_reading || _flushing)
        throw IOPending( PT_ERROR_MSG("discard failed - streambuffer is in use") );

    setg(0, 0, 0);
    setp(0, 0);
}


inline void StreamBuffer::onWrite(IODevice& dev)
{
    this->endWrite();
    outputReady.send(*this);
}


inline void StreamBuffer::endWrite()
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

    this->setp(_obuffer + leftover, _obuffer + _obufferSize);
}


inline StreamBuffer::int_type StreamBuffer::overflow(int_type ch)
{
    if( ! _ioDevice )
        return traits_type::eof();

    if( ! _obuffer )
    {
        _obuffer = new char[_obufferSize];
        this->setp(_obuffer, _obuffer + _obufferSize);
    }

    if(_flushing) // beginWrite is unfinished
    {
        this->endWrite();
    }
    else if (traits_type::eq_int_type( ch, traits_type::eof() ) || !_oextend)
    {
        // normal blocking overflow case
        size_t avail = this->pptr() - this->pbase();
        size_t written = _ioDevice->write(_obuffer, avail);
        size_t leftover = avail - written;

        if(leftover > 0)
        {
            traits_type::move(_obuffer, _obuffer + written, leftover);
        }
        this->setp(_obuffer + leftover, _obuffer + _obufferSize);
    }
    else
    {
        // if the buffer area is extensible and overflow is not called by
        // sync/flush we copy the output buffer to a larger one
        size_t bufsize = _obufferSize + (_obufferSize/2);
        char* buf = new char[ bufsize ];
        traits_type::move(buf, _obuffer, _obufferSize);
        std::swap(_obuffer, buf);
        _obufferSize = bufsize;
        delete [] buf;
    }

    // if the overflow char is not EOF put it in buffer
    if( traits_type::eq_int_type(ch, traits_type::eof()) ==  false )
    {
        *this->pptr() = traits_type::to_char_type(ch);
        this->pbump(1);
    }

    return traits_type::not_eof(ch);
}


inline StreamBuffer::int_type StreamBuffer::pbackfail(StreamBuffer::int_type)
{
    return traits_type::eof();
}


inline int StreamBuffer::sync()
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


inline std::streamsize StreamBuffer::xspeekn(char* buffer, std::streamsize size)
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


inline StreamBuffer::pos_type
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
        this->endWrite();
    }

    if(_reading)
    {
        this->endRead();
    }

    ret = _ioDevice->seek(off, dir);

    // eliminate currently buffered sequence
    this->setg(0, 0, 0);
    this->setp(0, 0);

    return ret;
}


inline StreamBuffer::pos_type
StreamBuffer::seekpos(pos_type p, std::ios::openmode mode)
{
    return this->seekoff(p, std::ios::beg, mode);
}

} // namespace System

} // namespace Pt

#endif
