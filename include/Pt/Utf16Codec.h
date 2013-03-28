/*
 * Copyright (C) 2005 by Marc Boris Duerner
 * Copyright (C) 2005 by Aloysius Indrayanto
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
#ifndef Pt_Utf16Codec_h
#define Pt_Utf16Codec_h

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/TextCodec.h>
#include <Pt/String.h>

namespace Pt {

class Utf16Codec : public TextCodec<Char, char> 
{
    public:
        explicit Utf16Codec(size_t ref = 0)
        {}

        //! Empty destructor
        virtual ~Utf16Codec()
        {}

        //! @brief Decodes UTF-8 to UTF-32.
        virtual result do_in(MBState& s, 
                             const char* from, const char* fromEnd, const char*& fromNext,
                             Char* to, Char* toEnd, Char*& toNext) const
        {         
            // return ok, error, partial
            // set fromNext and fromNext where we stopped
            // convert from -> to
            // handle replacement chars
            // assume BE encoding for now
            // work wih unsigned chars !!!

            for( ; from != fromEnd; ++from)
            {
                unsigned ch = *from;

                // high surrogate
                if (ch >= 0xD800 && ch <= 0xDBFF) 
                {
                    // invalid or missing low surrogate
                    if(++from == fromEnd || *from < 0xDC00 || *from > 0xDFFF) 
                    {
                        if(to < toEnd)
                            *to++ = Pt::Char(0xFFFD);
                        break;
                    }

                    const unsigned lo = *from;
                    ch = ((ch - 0xD800) << 10) + (lo - 0xDC00) + 0x0010000U;
                    if(to < toEnd)
                        *to++ = Pt::Char(ch);
                }
                // not a surrogate
                else if(ch < 0xDC00 || ch > 0xDFFF)
                {
                    if(to < toEnd)
                        *to++ = Pt::Char(ch);
                }
                // not a valid unicode point
                else
                {
                    if(to < toEnd)
                        *to++ = Pt::Char(0xFFFD);
                }
            }

            toNext = to;
            fromNext = from;
        
            return ok;
        }

        //! @brief Encodes UTF-32 to UTF-8.
        virtual result do_out(MBState& s, const Char* fromBegin,
                                            const Char* fromEnd, const Char*& fromNext,
                                            char* toBegin, char* toEnd, char*& toNext) const
        { return error; }

        // inheritdoc
        virtual bool do_always_noconv() const throw() 
        { return false; }

        // inheritdoc
        virtual int do_length(MBState& s, const char* fromBegin, const char* fromEnd, size_t max) const
        {
          return 0;
        }
        // inheritdoc
        virtual int do_max_length() const throw() 
        { return 4; }

        // inheritdoc
        std::codecvt_base::result do_unshift(Pt::MBState&, char*, char*, char*&) const
        { return std::codecvt_base::noconv; }

        // inheritdoc
        int do_encoding() const throw()
        { return 0; }
};

} //namespace Pt

#endif

