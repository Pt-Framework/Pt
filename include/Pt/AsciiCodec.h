/*
 * Copyright (C) 2004 Marc Boris Duerner
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
#ifndef PT_AsciiCodec_h
#define PT_AsciiCodec_h

#include <Pt/Api.h>
#include <Pt/Char.h>
#include <Pt/TextCodec.h>

namespace Pt {

    /**
     * @brief This simple Codec class is able to convert from ASCII to UTF-32 and from UTF-32 to ASCII.
     *
     * The method do_in() converts an array of char containing ACSII-data into an array of
     * Pt::Char which is UTF-32-encoded, which means that the data is a direct readable
     * 32-bit representation of the character.
     *
     * The method do_out() converts an array of Pt::Char objects (UTF-32/Unicode) into an
     * array of char which contains the same sequence of characters in ASCII-encoding.
     */
    class PT_API AsciiCodec : public TextCodec<Char, char> {
        public:
            /**
             * @brief Constructs a new AsciiCodec object with internal type of Pt::Char and
             * external type of $char$.
             *
             * @param ref This parameter is passed to TextCodec. When ref == 0 the locale takes care
             * of deleting the facet. If ref == 1 the locale does not destroy the facet.
             */
            explicit AsciiCodec(size_t ref = 0);

            //! Empty desctructor
            virtual ~AsciiCodec();

            //! @brief Decodes ASCII to UTF-32.
            virtual result do_in(MBState& s, const char* fromBegin, const char* fromEnd, const char*& fromNext,
                                             Char* toBegin, Char* toEnd, Char*& toNext) const;

            //! @brief Encodes UTF-32 to ASCII.
            virtual result do_out(MBState& s, const Char* fromBegin, const Char* fromEnd, const Char*& fromNext,
                                             char* toBegin, char* toEnd, char*& toNext) const;

            // interhitdoc
            virtual bool do_always_no_conv() const throw();

            // interhitdoc
            virtual int do_length(MBState& s, const char* fromBegin, const char* fromEnd, size_t max) const;

            // interhitdoc
            virtual int do_max_length() const throw();
    };

} //namespace Pt

#endif
