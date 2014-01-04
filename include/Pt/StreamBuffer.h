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
    template class PT_EXPORT std::basic_streambuf<char>; 
#endif

#if defined(_MSC_VER)
	template class PT_EXPORT std::basic_streambuf<Pt::Char>;
#endif

namespace Pt {

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
        ~BasicStreamBuffer()
        { }

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

        std::streamsize out_avail()
        {
            if( this->pptr() )
                return this->pptr() - this->pbase();

            return showfull();
        }

    protected:
        BasicStreamBuffer()
        { }

        virtual std::streamsize showfull()
        { return 0; }
};

} // namespace Pt

#endif
