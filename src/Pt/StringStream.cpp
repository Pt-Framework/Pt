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
#include "Pt/StringStream.h"

namespace Pt {

BasicStringStreamBuffer::BasicStringStreamBuffer(std::ios::openmode mode)
: std::basic_stringbuf<Pt::Char>(mode)
{
// When building a DLL under Visual studio, we need to imbue here
#ifndef PT_WITHOUT_STD_LOCALE
    //if( false == std::has_facet< std::ctype<Pt::Char> >( std::locale() ) )
    //{
    //    std::locale::global( std::locale(std::locale(), new std::ctype<Pt::Char>) );
    //    std::locale::global( std::locale(std::locale(), new std::numpunct<Pt::Char>) );
    //    std::locale::global( std::locale(std::locale(), new std::num_get<Pt::Char>) );
    //    std::locale::global( std::locale(std::locale(), new std::num_put<Pt::Char>) );
    //}
#endif
}

BasicStringStreamBuffer::BasicStringStreamBuffer(const Pt::String& str, std::ios::openmode mode)
: std::basic_stringbuf<Pt::Char>(str, mode)
{
// When building a DLL under Visual studio, we need to imbue here
#ifndef PT_WITHOUT_STD_LOCALE
//    if( false == std::has_facet< std::ctype<Pt::Char> >( std::locale() ) )
//    {
//        std::locale::global( std::locale(std::locale(), new std::ctype<Pt::Char>) );
//        std::locale::global( std::locale(std::locale(), new std::numpunct<Pt::Char>) );
//        std::locale::global( std::locale(std::locale(), new std::num_get<Pt::Char>) );
//        std::locale::global( std::locale(std::locale(), new std::num_put<Pt::Char>) );
//    }
#endif
}

} // namespace Pt


namespace std {

basic_stringstream<Pt::Char>::~basic_stringstream()
{
    delete _buffer;
}

basic_stringbuf<Pt::Char>* basic_stringstream<Pt::Char>::rdbuf() const
{
    return (basic_stringbuf<Pt::Char>*)_buffer;
}

Pt::String basic_stringstream<Pt::Char>::str() const
{
    return (_buffer->str());
}

void basic_stringstream<Pt::Char>::str(const Pt::String& newStr)
{
    _buffer->str(newStr);
}

} // namespace std
