/*
 * Copyright (C) 2004 Marc Boris Duerner
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
#ifndef Pt_Utf32Codec_h
#define Pt_Utf32Codec_h

#include <Pt/Api.h>
#include <Pt/Char.h>
#include <Pt/TextCodec.h>

namespace Pt {

    class PT_API Utf32Codec : public TextCodec<Char, char> {
        public:
            /**
             * @brief Constructs a new Utf32Codec object.
             *
             * The internal type is Pt::Char and external type is $char$
             *
             * @param ref This optional parameter is passed to std::codecvt. When ref == 0 the locale takes
             * care of deleting the facet. If ref == 1 the locale does not destroy the facet. Default value is 0.
             */
            Utf32Codec(size_t ref = 0);

            //! Empty destructor
            virtual ~Utf32Codec();

            virtual Utf32Codec::result do_in(std::mbstate_t& s, const char* fromBegin,
                                             const char* fromEnd, const char*& fromNext,
                                             Char* toBegin, Char* toEnd, Char*& toNext) const;

            virtual Utf32Codec::result do_out(std::mbstate_t& s, const Char* fromBegin, const Char* fromEnd, const Char*& fromNext,
                                             char* toBegin, char* toEnd, char*& toNext) const;

            // inheritdoc
            virtual int do_length(std::mbstate_t& s, const char* fromBegin, const char* fromEnd, size_t max) const;

            // inheritdoc
            virtual int do_max_length() const throw();

            // inheritdoc
            virtual bool do_always_no_conv() const throw();
    };

} //namespace Pt

#endif

