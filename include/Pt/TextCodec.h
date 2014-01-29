/*
 * Copyright (C) 2004-2011 Marc Boris Duerner
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
#ifndef Pt_TextCodec_h
#define Pt_TextCodec_h

#include <Pt/Api.h>
#include <Pt/Locale.h>
#include <Pt/String.h>

namespace Pt {

//! @cond HIDDEN_DOCS
template <typename CharT, typename ByteT>
class TextCodec : public std::codecvt<CharT, ByteT, Pt::MBState>
{   
    public:
        TextCodec(std::size_t ref = 0)
        : std::codecvt<CharT, ByteT, MBState>(ref)
        , _refs(ref)
        {}

    public:
        virtual ~TextCodec()
        {}

        std::size_t refs() const
        { return _refs; }

    private:
        std::size_t _refs;
};
//! @endcond

}

#endif
