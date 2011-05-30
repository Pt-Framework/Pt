/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
 * Copyright (C) 2010-2010 by Marc Boris Duerner
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

#include <Pt/Ssl/CipherStreamBuf.h>

#include <iostream> // Remove this later!
#include <sstream>

#include <assert.h>
#include <string.h>

namespace Pt {
namespace Ssl {

CipherStreamBuf::CipherStreamBuf()
: _ios(0), _cipher(0)
{
    this->setg(0, 0, 0);
    this->setp(0, 0);
}

CipherStreamBuf::CipherStreamBuf(std::iostream& ios, BasicCipher& cipher)
: _ios(0), _cipher(0)
{
    setIOStream(ios);
    setCipher(cipher);

    this->setg(0, 0, 0);
    this->setp(0, 0);
}

CipherStreamBuf::~CipherStreamBuf()
{}

void CipherStreamBuf::setIOStream(std::iostream& ios)
{ _ios = &ios; }

void CipherStreamBuf::setCipher(BasicCipher& cipher)
{ _cipher = &cipher; }

std::streamsize CipherStreamBuf::import()
{
    const std::streamsize avail = _ios->rdbuf()->in_avail();
    if(!avail) return 0;

    if(underflow() == traits_type::eof()) return 0;

    return this->egptr() - this->gptr();
}

void CipherStreamBuf::finish()
{
    if( this->pptr() )
        overflow( traits_type::eof() );
    
    this->setg(0, 0, 0);
    this->setp(0, 0);
}

CipherStreamBuf::int_type CipherStreamBuf::sync()
{
    if( this->pptr() ) {
        while( this->pptr() > this->pbase() ) {
            const int_type ch = this->overflow( traits_type::eof() );
            if( ch == traits_type::eof() ) return -1;
        }
    }

    return 0;
}

CipherStreamBuf::int_type CipherStreamBuf::underflow()
{
    // Just return EOF if any of the conditions is not met
    if( !_ios || !_cipher || this->pptr() )
        return traits_type::eof();
    
    // Initialize the get pointer (if needed)
    if( ! this->gptr() ) {
        // Prepare the output buffer
        _ioBuf.resize(_cipher->outputBlockSize());
        // Set the get pointers
        this->setg(&_ioBuf[0], &_ioBuf[0] + _ioBuf.size(), &_ioBuf[0] + _ioBuf.size());
        // Resize the conversion buffer
        _cnvBuf.resize(_cipher->inputBlockSize());
    }

    // Check if we still have anything left if the get buffer
    if( this->gptr() && this->gptr() < this->egptr() )
        return traits_type::to_int_type( *this->gptr() );

    // Decode data
    for(;;) {
        // Read from the attached iostream
        _ios->read(&_cnvBuf[0], _cnvBuf.size());
        // Is there any data to be decoded?
        const size_t inAvail = _ios->gcount();
        if(!inAvail) break;
        // Decode the data
        const char* from      = &_cnvBuf[0];
        const char* from_end  = from + inAvail;
        const char* from_next = 0;
        char*       to_next   = 0;
        const int   ret       = _cipher->decode(from, from_end, from_next, &_ioBuf[0], &_ioBuf[0] + _ioBuf.size(), to_next);
        assert(ret == 1);
        // Reset the get pointers
        const size_t outAvail = to_next - &_ioBuf[0];
        this->setg(&_ioBuf[0], &_ioBuf[0], &_ioBuf[0] + outAvail);
        // Done
        return traits_type::to_int_type( *this->gptr() );
    }

    // EOF
    return traits_type::eof();
}

CipherStreamBuf::int_type CipherStreamBuf::overflow(int_type ch)
{
    // Just return EOF if any of the conditions is not met
    if( !_ios || !_cipher || this->gptr() )
        return traits_type::eof();

    // Initialize the put pointer (if needed)
    if( ! this->pptr() ) {
        // Prepare the input buffer
        _ioBuf.resize(_cipher->inputBlockSize());
        // Set the put pointers
        this->setp(&_ioBuf[0], &_ioBuf[0] + _ioBuf.size());
        // Resize the conversion buffer
        _cnvBuf.resize(_cipher->outputBlockSize());
    }

    // Is the overflow char is an EOF?
    const bool isEOFChar = traits_type::eq_int_type( ch, traits_type::eof() );
    
    // Encode data
    for(;;) {
        // Is there any data to be encoded?
        const char*  from     = this->pbase();
        const char*  from_end = this->pptr();
        const size_t inAvail  = from_end - from;
        if(!inAvail) break;
        // Is there enough data to be encoded?
        if(!isEOFChar && inAvail < _cipher->inputBlockSize()) break;
        // Encode the data
        const char* from_next = 0;
        char*       to_next   = 0;
        const int   ret       = _cipher->encode(from, from_end, from_next, &_cnvBuf[0], &_cnvBuf[0] + _cnvBuf.size(), to_next);
        assert(ret == 1);
        // Write the data to the output stream
        _ios->write((const char*) &_cnvBuf[0], to_next - &_cnvBuf[0]);
        // Reset the put pointers
        this->setp(&_ioBuf[0], &_ioBuf[0] + _ioBuf.size());
        // Move leftover data to the front
        const size_t leftOver = from_end - from_next;
        if(leftOver > 0) {
            traits_type::move(&_ioBuf[0], from_next, leftOver);
            this->pbump(leftOver);
        }
    }

    // If the overflow char is not EOF, put it in the buffer area
    if( ! isEOFChar ) {
        *(this->pptr()) = traits_type::to_char_type(ch);
        this->pbump(1);
    }

    return traits_type::not_eof(ch);
}

} // namespace Pt
} // namespace Ssl
