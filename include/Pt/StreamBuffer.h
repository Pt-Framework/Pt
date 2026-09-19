/*
 * Copyright (C) 2012 Marc Boris Duerner
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

#ifndef Pt_StreamBuffer_h
#define Pt_StreamBuffer_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <streambuf>
#include <cstddef>

#if defined(_MSC_VER) && defined(_WIN32_WCE) 
    // alternatively compile with /FORCE:multiple
    template class PT_API std::basic_streambuf<char>; 
#endif

#if defined(_MSC_VER)
	  template class PT_API std::basic_streambuf<Pt::Char>;
#endif

namespace Pt {

/** @brief Stream buffer with peek and available output.

    %BasicStreamBuffer is the buffer in the @ref Pt-Streams model. It is
    a @c std::basic_streambuf. Derived buffers implement underflow and
    overflow as they would for any streambuf. This type adds two
    queries that those derived buffers can serve from the get and put
    areas without going through the iostream formatting layer.

    %speekn() copies up to @a size characters from the get area into
    @a buffer and does not consume them. If the get area is empty, it
    calls underflow once. An unbuffered buffer, where underflow
    produced a character but no get area, yields that one character.
    A short count means fewer characters were available, not that the
    stream has failed.

    %out_avail() is the number of characters already in the put area.
    When there is no put pointer, it calls %showfull(), which is zero
    unless a derived unbuffered buffer overrides it.

    The stream types %BasicIStream, %BasicOStream and %BasicIOStream
    hold a pointer to this buffer. They do not own it. A derived
    buffer must outlive every stream that still uses it.

    @ingroup Pt-Streams
*/
template <typename CharT, typename TraitsT = std::char_traits<CharT> >
class BasicStreamBuffer : public std::basic_streambuf<CharT, TraitsT>
{
    public:
        typedef CharT char_type;
        typedef TraitsT traits_type;
        typedef typename TraitsT::int_type int_type;
        typedef typename TraitsT::pos_type pos_type;
        typedef typename TraitsT::off_type off_type;

    public:
        /** @brief Destructor.
        */
        ~BasicStreamBuffer()
        { }

        /** @brief Peeks characters in the stream buffer.

            The number of characters that can be peeked depends on the
            current get area and may be less than requested, similar to
            istream::readsome(). The characters are not consumed.
        */
        std::streamsize speekn(CharT* buffer, std::streamsize size)
        {
            if(size <= 0)
                return 0;

            int_type next = 0;
            if( ! this->gptr() || this->gptr() == this->egptr() )
            {
                next = this->underflow();
                
                if( traits_type::eof() == next)
                    return 0;
            }

            std::size_t avail = this->gptr() ? this->egptr() - this->gptr() : 0;

            // unbuffered streambufs
            if(avail == 0)
            {
                *buffer = traits_type::to_char_type(next);
                return 1;
            }

            std::size_t n = static_cast<std::size_t>(size);
            if(avail < n) 
                n = avail;

            traits_type::copy(buffer, this->gptr(), n);
            return size;
        }

        /** @brief Returns the number of characters buffered for output.
        */
        std::streamsize out_avail()
        {
            if( this->pptr() )
                return this->pptr() - this->pbase();

            return showfull();
        }

    protected:
        /** @brief Default Constructor.
        */
        BasicStreamBuffer()
        { }

        /** @brief Returns the number of characters buffered for output.
        */
        virtual std::streamsize showfull()
        { return 0; }
};

#if defined(_MSC_VER)
    template class PT_API BasicStreamBuffer<char>;
    template class PT_API BasicStreamBuffer<Char>;
#endif

} // namespace Pt

#endif
