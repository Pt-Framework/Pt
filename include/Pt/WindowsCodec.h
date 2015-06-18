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

#ifndef Pt_WindowsCodec_h
#define Pt_WindowsCodec_h

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/TextCodec.h>

namespace Pt {

/** @brief A codec for windows 1252 encoding.
*/
class PT_API Windows1252Codec : public TextCodec<Char, char> 
{
    public:
        //! @brief Constructor.
        explicit Windows1252Codec(std::size_t ref = 0);

        //! @brief Destructor.
        virtual ~Windows1252Codec();

        // inheritdoc
        virtual result do_in(MBState& s, 
                             const char* from, const char* fromEnd, const char*& fromNext,
                             Char* to, Char* toEnd, Char*& toNext) const;
        
        // inheritdoc
        virtual result do_out(MBState& s, const Char* fromBegin,
                              const Char* fromEnd, const Char*& fromNext,
                              char* toBegin, char* toEnd, char*& toNext) const;

        // inheritdoc
        virtual bool do_always_noconv() const throw()
        {
            return false;
        }

        // inheritdoc
        virtual int do_length(MBState& s, const char* fromBegin, 
                              const char* fromEnd, std::size_t max) const
        {
            std::size_t avail = fromEnd - fromBegin;
            return avail < max ? avail : max;
        }

        // inheritdoc
        virtual int do_max_length() const throw()
        {
            return 1;
        }

        // inheritdoc
        std::codecvt_base::result do_unshift(Pt::MBState&, char*, char*, char*&) const
        {
            return std::codecvt_base::noconv;
        }

        // inheritdoc
        int do_encoding() const throw()
        { return 1; }
};

/** @brief A codec for windows 936 encoding.
*/
class PT_API Windows936Codec : public TextCodec<Char, char> 
{
    public:
        //! @brief Constructor.
        explicit Windows936Codec(std::size_t ref = 0);

        //! @brief Destructor.
        virtual ~Windows936Codec();

        // inheritdoc
        virtual result do_in(MBState& s, 
                             const char* from, const char* fromEnd, const char*& fromNext,
                             Char* to, Char* toEnd, Char*& toNext) const;
        
        // inheritdoc
        virtual result do_out(MBState& s, const Char* fromBegin,
                              const Char* fromEnd, const Char*& fromNext,
                              char* toBegin, char* toEnd, char*& toNext) const;

        // inheritdoc
        virtual bool do_always_noconv() const throw()
        {
            return false;
        }

        // inheritdoc
        virtual int do_length(MBState& s, const char* fromBegin, 
                              const char* fromEnd, std::size_t max) const;

        // inheritdoc
        virtual int do_max_length() const throw()
        {
            return 2;
        }

        // inheritdoc
        std::codecvt_base::result do_unshift(Pt::MBState&, char*, char*, char*&) const
        {
            return std::codecvt_base::noconv;
        }

        // inheritdoc
        int do_encoding() const throw()
        { return 0; }
};

} //namespace Pt

#endif
