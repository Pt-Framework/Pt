/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Duerner                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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

basic_stringstream<Pt::Char>::basic_stringstream(ios_base::openmode mode)
: basic_iostream<Pt::Char>(_buffer = new Pt::BasicStringStreamBuffer(mode))
{ }

basic_stringstream<Pt::Char>::basic_stringstream(const Pt::String& str, std::ios_base::openmode mode)
: basic_iostream<Pt::Char>(_buffer = new Pt::BasicStringStreamBuffer(str, mode))
{ }

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
