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
#include <cassert>
#include <zlib.h>

namespace Pt {

ZBuffer::ZBuffer()
: _target(0)
, _deflate(0)
{
    this->setg(0, 0, 0);
    this->setp(0, 0);

    _deflate = new z_stream;
    _deflate->zalloc = (alloc_func)0;
    _deflate->zfree = (free_func)0;
    _deflate->opaque = (voidpf)0;
}


ZBuffer::ZBuffer(std::ios& ios)
: _target(&ios)
, _deflate(0)
{

    this->setg(0, 0, 0);
    this->setp(0, 0);

    _deflate = new z_stream;
    _deflate->zalloc = (alloc_func)0;
    _deflate->zfree = (free_func)0;
    _deflate->opaque = (voidpf)0;
}


ZBuffer::~ZBuffer()
{ 
    close();

    delete _deflate;
}


void ZBuffer::open(std::ios& ios)
{
    close();
    _target = &ios;
}


void ZBuffer::close()
{
    if( this->pptr() )
    {
        for(;;)
        {
          _deflate->next_out = reinterpret_cast<Bytef*>(_zbuf);
          _deflate->avail_out = _zbufmax;

          int err = deflate(_deflate, Z_FINISH);
          if(err == Z_STREAM_END)
              break;
            
          if(err != Z_OK)
              throw std::ios::failure("deflate failed");
                  
          std::streamsize avail = _zbufmax - _deflate->avail_out;
          avail -= _target->rdbuf()->sputn(_zbuf, avail);
          if(avail > 0)
              throw std::ios::failure("deflate failed");
        }

        deflateEnd(_deflate);
        setp(0, 0);
    }

    if( this->gptr() )
    {
      // TODO: inflateEnd()
      setg(0, 0, 0);
    }
}


void ZBuffer::import(std::streamsize maxImport)
{

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
    if( ! _target || ! _target->rdbuf() || this->gptr() )
        return traits_type::eof();

    if( ! this->pptr() )
    {
        close();

        int err = deflateInit(_deflate, Z_DEFAULT_COMPRESSION);
        if (err != Z_OK) 
          throw std::ios::failure("deflateInit failed");

        this->setp( _buf, _buf + _bufmax );
    }
    else
    {
        _deflate->next_in  = reinterpret_cast<Bytef*>(_buf);
        _deflate->avail_in = this->pptr() - this->pbase();

        while(_deflate->avail_in > 0)
        {
            _deflate->next_out = reinterpret_cast<Bytef*>(_zbuf);
            _deflate->avail_out = _zbufmax;

            int err = deflate(_deflate, Z_NO_FLUSH);
            if (err != Z_OK) 
                throw std::ios::failure("deflate failed");
             
            std::streamsize avail = _zbufmax - _deflate->avail_out;
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
