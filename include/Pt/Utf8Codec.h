/*
 * Copyright (C) 2005-2013 by Marc Boris Duerner
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

#ifndef Pt_Utf8Codec_h
#define Pt_Utf8Codec_h

#include <Pt/Api.h>
#include <Pt/TextCodec.h>
#include <Pt/String.h>
#include <string>

namespace Pt {

/** @brief Convert between unicode and UTF-8.

    @ingroup Unicode
*/
class PT_API Utf8Codec : public TextCodec<Char, char> 
{
    public:
        /** @brief Constructor.
        */
        explicit Utf8Codec(std::size_t ref = 0);

        //! @brief Destructor.
        virtual ~Utf8Codec()
        {}

        // inheritdoc
        virtual result do_in(MBState& s, const char* fromBegin,
                                         const char* fromEnd, const char*& fromNext,
                                         Char* toBegin, Char* toEnd, Char*& toNext) const;

        // inheritdoc
        virtual result do_out(MBState& s, const Char* fromBegin,
                                          const Char* fromEnd, const Char*& fromNext,
                                          char* toBegin, char* toEnd, char*& toNext) const;

        // inheritdoc
        virtual bool do_always_noconv() const throw();

        // inheritdoc
        virtual int do_length(MBState& s, const char* fromBegin, const char* fromEnd, std::size_t max) const;

        // inheritdoc
        virtual int do_max_length() const throw();

        // inheritdoc
        virtual result do_unshift(Pt::MBState&, char*, char*, char*&) const;

        // inheritdoc
        virtual int do_encoding() const throw();

        //! @brief Decode to a unicode string.
        static String decode(const char* data, std::size_t size);
        
        //! @brief Decode to a unicode string.
        static String decode(const std::string& data)
        { return decode(data.data(), data.size()); }

        //! @brief Encode to a UTF-8 string.
        static std::string encode(const Char* data, std::size_t size);
        
        //! @brief Encode to a UTF-8 string.
        static std::string encode(const String& data)
        { return encode(data.data(), data.size()); }
};

} //namespace Pt

#endif // Pt_Utf8Codec_h
