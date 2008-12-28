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
#ifndef Pt_Base64Codec_h
#define Pt_Base64Codec_h

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/TextCodec.h>

namespace Pt {

class PT_API Base64Codec : public TextCodec<char, char>
{
    public:
        explicit Base64Codec(size_t ref = 0);

        virtual ~Base64Codec();

        result do_in(MBState& s, 
                     const char* fromBegin,
                     const char* fromEnd, 
                     const char*& fromNext,
                     char* toBegin, 
                     char* toEnd, 
                     char*& toNext) const;

        result do_out(MBState& s, 
                      const char* fromBegin,
                      const char* fromEnd, 
                      const char*& fromNext,
                      char* toBegin, 
                      char* toEnd,
                      char*& toNext) const;

        result do_unshift(MBState& state, 
                          char* toBegin, 
                          char* toEnd, 
                          char*& toNext) const;
                                     

        bool do_always_noconv() const throw();

        int do_length(MBState& s, 
                      const char* fromBegin, 
                      const char* fromEnd, 
                      size_t max) const;

        int do_encoding() const throw();

        int do_max_length() const throw();
};

} //namespace Pt

#endif
