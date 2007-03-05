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

#ifndef PTV_TEXT_STRINGSTREAM_H
#define PTV_TEXT_STRINGSTREAM_H

#include <Pt/Text/Api.h>
#include <Pt/Char_ctype.h>
#include <Pt/Char_numpunct.h>

#include <Pt/Char.h>
#include <Pt/String.h>
#include <sstream>

namespace Pt {
namespace Text {

class PT_TEXT_API BasicStringStreamBuffer : public std::basic_stringbuf<Pt::Char>
{
    public:
        explicit BasicStringStreamBuffer(std::ios::openmode mode = std::ios::in | std::ios::out);
        explicit BasicStringStreamBuffer(const Pt::String& str, std::ios::openmode mode = std::ios::in | std::ios::out);
};

} // namespace Text
} // namespace Pt


namespace std {

template<>
class PT_TEXT_API basic_stringstream<Pt::Char> : public basic_iostream<Pt::Char>
{
    public:
	    typedef Pt::Char char_type;
	    typedef std::char_traits<Pt::Char> traits_type;
	    typedef std::allocator<Pt::Char> allocator_type;
	    
	    typedef traits_type::int_type int_type;
	    typedef traits_type::pos_type pos_type;
	    typedef traits_type::off_type off_type;
    	
	    explicit basic_stringstream(ios_base::openmode mode = ios_base::in | ios_base::out);

	    explicit basic_stringstream(const Pt::String& str, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);

	    virtual ~basic_stringstream();

	    basic_stringbuf<Pt::Char>* rdbuf() const;

	    Pt::String str() const;

	    void str(const Pt::String& newStr);

    private:
        Pt::Text::BasicStringStreamBuffer* _buffer;
};

} // namespace std


namespace Pt {
namespace Text {
    typedef std::basic_stringstream<Pt::Char> StringStream;
} // namespace Text
} // namespace Pt

#endif
