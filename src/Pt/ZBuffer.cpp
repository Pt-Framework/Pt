/*
 * Copyright (C) 2015 by Marc Boris Duerner
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

#include <Pt/ZBuffer.h>
#include <Pt/IOError.h>
#include <cassert>
#include <cstring>
#include <zlib.h>

namespace Pt {

ZBuffer::ZBuffer(Format fmt)
: _target(0)
, _zstr(0)
, _format(fmt)
, _zbufsize(0)
{
    this->setg(0, 0, 0);
    this->setp(0, 0);

    _zstr = new z_stream;
    _zstr->zalloc = (alloc_func)0;
    _zstr->zfree = (free_func)0;
    _zstr->opaque = (voidpf)0;
}


ZBuffer::ZBuffer(std::ios& ios, Format fmt)
: _target(&ios)
, _zstr(0)
, _format(fmt)
, _zbufsize(0)
{
    this->setg(0, 0, 0);
    this->setp(0, 0);

    _zstr = new z_stream;
    _zstr->zalloc = (alloc_func)0;
    _zstr->zfree = (free_func)0;
    _zstr->opaque = (voidpf)0;
}


ZBuffer::~ZBuffer()
{
    try
    {
        reset();
    }
    catch(...)
    {}

    delete _zstr;
}


void ZBuffer::attach(std::ios& target)
{
    _target = &target;
}


void ZBuffer::detach()
{
    _target = 0;
}


void ZBuffer::discard()
{
    if( this->pptr() )
    {
        deflateEnd(_zstr);
        setp(0, 0);
    }

    if( this->gptr() )
    {
      inflateEnd(_zstr);
      setg(0, 0, 0);
    }

    _zbufsize = 0;
}


void ZBuffer::reset()
{
    discard();
    detach();
}


void ZBuffer::reset(std::ios& target)
{
    discard();
    attach(target);
}


void ZBuffer::finish()
{
    if( this->pptr() )
    {
        _zstr->next_in  = reinterpret_cast<Bytef*>(_buf);
        _zstr->avail_in = this->pptr() - this->pbase();

        for(int err = Z_OK; err != Z_STREAM_END; )
        {
          _zstr->next_out = reinterpret_cast<Bytef*>(_zbuf);
          _zstr->avail_out = _zbufmax;

          err = deflate(_zstr, Z_FINISH);
          if(err != Z_STREAM_END && err != Z_OK)
              throw IOError("deflate failed");

          std::streamsize avail = _zbufmax - _zstr->avail_out;
          avail -= _target->rdbuf()->sputn(_zbuf, avail);
          if(avail > 0)
              throw IOError("deflate failed");
        }

        deflateReset(_zstr);
    }
}


std::streamsize ZBuffer::import(std::streamsize maxImport)
{
    if( ! this->gptr() )
    {
        // discard ends deflating for put area
        discard();

        const int windowBits = (_format == Gzip) ? MAX_WBITS + 32 : MAX_WBITS;
        int err = inflateInit2(_zstr, windowBits);
        if (err != Z_OK)
            throw IOError("inflateInit failed");

        this->setg(_buf, _buf, _buf);
    }

    if( ! _target || ! _target->rdbuf() )
    {
        inflateBuffer();
        return 0;
    }

    std::streamsize n = 0;

    // special case: import all available input
    if(maxImport == 0)
        maxImport = _target->rdbuf()->in_avail();

    // not more than available zbuffer size
    const std::streamsize zbufavail = _zbufmax - _zbufsize;
    maxImport = zbufavail < maxImport ? zbufavail : maxImport;

    if(maxImport > 0)
    {
        n = _target->rdbuf()->sgetn( _zbuf + _zbufsize,  maxImport );
        if(n > 0)
            _zbufsize += static_cast<int>(n);
    }

    inflateBuffer();

    return n;
}


std::streamsize ZBuffer::import(const char* data, std::streamsize size)
{
    if( ! this->gptr() )
    {
        // discard ends deflating for put area
        discard();

        const int windowBits = (_format == Gzip) ? MAX_WBITS + 32 : MAX_WBITS;
        int err = inflateInit2(_zstr, windowBits);
        if (err != Z_OK)
            throw IOError("inflateInit failed");

        this->setg(_buf, _buf, _buf);
    }

    // not more than available zbuffer size
    const std::streamsize zbufavail = _zbufmax - _zbufsize;
    std::streamsize n = zbufavail < size ? zbufavail : size;

    if(n > 0)
    {
        std::memcpy(_zbuf + _zbufsize, data, static_cast<std::size_t>(n));
        _zbufsize += static_cast<int>(n);
    }

    inflateBuffer();

    return n;
}


void ZBuffer::inflateBuffer()
{
    // make room for decompressed data
    if( this->gptr() - this->eback() > _pbmax)
    {
        std::streamsize movelen = this->egptr() - this->gptr() + _pbmax;
        std::char_traits<char_type>::move( _buf,
                                            this->gptr() - _pbmax,
                                            static_cast<std::size_t>(movelen));
        this->setg(_buf, _buf + _pbmax, _buf + movelen);
    }

    // inflate to end of input buffer area
    const std::streamsize used = this->egptr() - this->eback();
    const std::streamsize unused = _bufmax - used;
    assert(used + unused == _bufmax);

    _zstr->next_in = reinterpret_cast<Bytef*>( const_cast<char*>(_zbuf) );
    _zstr->avail_in = static_cast<uLong>(_zbufsize);

    _zstr->next_out = reinterpret_cast<Bytef*>( this->egptr() );
    _zstr->avail_out = static_cast<uLong>(unused);

    while( _zstr->avail_in > 0 && _zstr->avail_out > 0)
    {
      int err = inflate(_zstr, Z_NO_FLUSH);

      if(err == Z_STREAM_END)
          break;

      if(err != Z_OK)
          throw IOError("inflate failed");
    }

    // move leftover compressed data to front
    std::memmove(_zbuf, _zstr->next_in, _zstr->avail_in);
    _zbufsize = _zstr->avail_in;

    std::streamsize generated = unused - _zstr->avail_out;
    if(generated)
    {
        this->setg(this->eback(),               // start of read buffer
                   this->gptr(),                // gptr position
                   this->egptr() + generated ); // end of read buffer
    }
}


std::streamsize ZBuffer::showmanyc()
{
    // Return 0, because we can not predict how many characters
    // can be decompressed. If we returned a number > 0, the next
    // call to underflow() must exactly read this number of bytes
    // without blocking.

    return _target && _target->rdbuf() ? 0 : -1;
}


std::streamsize ZBuffer::showfull()
{
    return 0;
}


int ZBuffer::sync()
{
    if( this->pptr() )
    {
        while( this->pptr() > this->pbase() )
        {
            const int_type ch = this->overflow( traits_type::eof() );
            if( ch == traits_type::eof() )
            {
                return -1;
            }
        }
    }

    return 0;
}


ZBuffer::int_type ZBuffer::underflow()
{
    if( this->gptr() < this->egptr() )
        return traits_type::to_int_type( *this->gptr() );

    import(_bufmax);

    return this->gptr() < this->egptr() ? traits_type::to_int_type( *this->gptr() )
                                        : traits_type::eof();
}


ZBuffer::int_type ZBuffer::overflow(int_type ch)
{
    if( ! _target || ! _target->rdbuf() )
        return traits_type::eof();

    if( ! this->pptr() )
    {
        // discard ends inflating for get area
        discard();

        const int windowBits = (_format == Gzip) ? MAX_WBITS + 16 : MAX_WBITS;
        int err = deflateInit2(_zstr, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY);
        if (err != Z_OK)
          throw IOError("deflateInit failed");

        this->setp( _buf, _buf + _bufmax );
    }
    else
    {
        _zstr->next_in  = reinterpret_cast<Bytef*>(_buf);
        _zstr->avail_in = this->pptr() - this->pbase();

        while(_zstr->avail_in > 0)
        {
            _zstr->next_out = reinterpret_cast<Bytef*>(_zbuf);
            _zstr->avail_out = _zbufmax;

            int err = deflate(_zstr, Z_NO_FLUSH);
            if (err != Z_OK)
                throw IOError("deflate failed");

            std::streamsize avail = _zbufmax - _zstr->avail_out;
            avail -= _target->rdbuf()->sputn(_zbuf, avail);
            if(avail > 0)
                return traits_type::eof();
        }

        this->setp(_buf, _buf + _bufmax);
    }

    if( ! traits_type::eq_int_type(ch, traits_type::eof()) )
    {
        *( this->pptr() ) = traits_type::to_char_type(ch);
        this->pbump(1);
    }

    return traits_type::not_eof(ch);
}

} // namespace Pt
