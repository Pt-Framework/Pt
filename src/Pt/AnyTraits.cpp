/***************************************************************************
 *   Copyright (C) 2004-2007 by Marc Boris Duerner                         *
 *   Copyright (C) 2006-2007 by Tobias Mueller                             *
 *   Copyright (C) 2006-2007 by Laurentiu-Gheorghe Crisan				   *
 *   Copyright (C) 2006-2007 by PTV AG                                     *
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
#include "Pt/AnyTraits.h"
#include "Pt/SourceInfo.h"
#include "Pt/Byteorder.h"
#include "Pt/Unicode.h"

#include <iostream>
using namespace std;

namespace Pt {

void AnyTraits<bool>::output(std::ostream& os, const bool& value)
{
    os << std::boolalpha << value;
}

void AnyTraits<bool>::output(std::basic_ostream<Pt::Char>& os, const bool& value)
{
    os << std::boolalpha << value;
}

void AnyTraits<bool>::input(std::istream& is, bool& value)
{
    is >> std::boolalpha >> value;
}

void AnyTraits<bool>::input(std::basic_istream<Pt::Char>& is, bool& value)
{
    is >> std::boolalpha >> value;
}

void AnyTraits<char>::output(std::ostream& os, const char& value)
{
    os << value;
}

void AnyTraits<char>::output(std::basic_ostream<Pt::Char>& os, const char& value)
{
    os << static_cast<int>(value);
}

void AnyTraits<char>::input(std::istream& is, char& value)
{
    is >> value;
}

void AnyTraits<char>::input(std::basic_istream<Pt::Char>& is, char& value)
{
    Pt::Char ch;
    is >> ch;
    
    value = ch.narrow('*');
}

void AnyTraits<unsigned char>::output(std::ostream& os, const unsigned char& value)
{
    os << value;
}

void AnyTraits<unsigned char>::output(std::basic_ostream<Pt::Char>& os, const unsigned char& value)
{
    os << static_cast<unsigned int>(value);
}

void AnyTraits<unsigned char>::input(std::istream& is, unsigned char& value)
{
    is >> value;
}

void AnyTraits<unsigned char>::input(std::basic_istream<Pt::Char>& is, unsigned char& value)
{
    Pt::Char ch;
    is >> ch;
    
    value = ch.narrow('*');
}

void AnyTraits<signed char>::output(std::ostream& os, const signed char& value)
{
    os << value;
}

void AnyTraits<signed char>::output(std::basic_ostream<Pt::Char>& os, const signed char& value)
{
    os << static_cast<signed int>(value);
}

void AnyTraits<signed char>::input(std::istream& is, signed char& value)
{
    is >> value;
}

void AnyTraits<signed char>::input(std::basic_istream<Pt::Char>& is, signed char& value)
{
    Pt::Char ch;
    is >> ch;

    value = ch.narrow('*');
}


void AnyTraits<unsigned int>::output(std::ostream& os, unsigned int value)
{
    os << value;
}

void AnyTraits<unsigned int>::output(std::basic_ostream<Pt::Char>& os, unsigned int value)
{
    os << value;
}

void AnyTraits<unsigned int>::input(std::istream& is, unsigned int& value)
{
    is >> value;
}

void AnyTraits<unsigned int>::input(std::basic_istream<Pt::Char>& is, unsigned int& value)
{
    is >>  value;
}

void AnyTraits<double>::output(std::ostream& os, double value)
{
    os << value;
}

void AnyTraits<double>::output(std::basic_ostream<Pt::Char>& os, double value)
{
    os << value;
}

void AnyTraits<double>::input(std::istream& is, double& value)
{
    is >> value;
}

void AnyTraits<double>::input(std::basic_istream<Pt::Char>& is, double& value)
{
    is >>  value;
}

template <typename CharT, typename CharT2>
void output_std_string(std::basic_ostream<CharT>& os, const std::basic_string<CharT2>& value)
{
    //
    // same implementation as AnyTraits<String>::output
    //
    os << CharT('\"');

    typename std::basic_string<CharT2>::const_iterator it;
    for( it = value.begin(); it != value.end(); ++it)
    {
        const CharT ch = *it;

        if(ch == '"' || ch == '\\')
            os << '\\';

        os << ch;
    }

    os << CharT('\"');
}


template <typename CharT, typename CharT2>
void input_std_string(std::basic_istream<CharT>& is, std::basic_string<CharT2>& value)
{
    std::basic_string<CharT2> tmpValue;

    //
    // same implementation as AnyTraits<String>::input
    //
    CharT ch;
    is >> ch;

    if( ch != '"' )
        throw std::runtime_error("Could not read string value" + PT_SOURCEINFO);

    bool isEscaped = false;

    while( is )
    {
        ch = is.get();

        if( ch == '\\' && !isEscaped )
        {
            isEscaped = true;
            continue;
        }

        if(ch == '"' && !isEscaped)
            break;

        tmpValue.push_back(ch);
        isEscaped = false;
    }

    // if we are EOF but haven't read a " character,
    // we still accept the sequence as a valid value
    // so we don't check for EOF here

    std::swap(value, tmpValue);
}


void AnyTraits<std::string>::output(std::ostream& os, const std::string& value)
{
    output_std_string(os, value);
}


void AnyTraits<std::string>::output(std::basic_ostream<Pt::Char>& os, const std::string& value)
{
    output_std_string(os, value);
}


void AnyTraits<std::string>::input(std::istream& is, std::string& value)
{
    input_std_string(is, value);
}


void AnyTraits<std::string>::input(std::basic_istream<Pt::Char>& is, std::string& value)
{
    input_std_string(is, value);
}


void writeBe(std::ostream& os, const Char& ch)
{
    uint32_t beValue = hostToBe( ch.value() );
    os.write( reinterpret_cast<const char*>(beValue), sizeof(uint32_t) );
}


void AnyTraits<String>::output(std::ostream& os, const String& value)
{
    //
    // same implementation as output_std_string
    //
    writeBe( os, Char('\"') );

    String::const_iterator it;
    for( it = value.begin(); it != value.end(); ++it)
    {
        const Char ch = *it;

        if(ch == '"' || ch == '\\')
            writeBe( os, Char('\\') );

        writeBe( os, ch );
    }

    writeBe( os, Char('\"') );
}


void AnyTraits<String>::output(std::basic_ostream<Pt::Char>& os, const String& value)
{
    output_std_string(os, value);
}


void readBe(std::istream& is, Char& ch)
{
    uint32_t value = 0;
    is.read( reinterpret_cast<char*>(&value), sizeof(uint32_t) );
    ch = beToHost(value);
}
void AnyTraits<String>::input(std::istream& is, String& value)
{
    String tmpValue;
    
    //
    // same implementation as AnyTraits<String>::input
    //
    Char ch(' ');

    while( Pt::Unicode::isSpace(ch) && is.good() )
        readBe(is, ch);

    if( ch != '"' )
        throw std::runtime_error("Could not read string value" + PT_SOURCEINFO);

    bool isEscaped = false;

    while( is )
    {
        readBe(is, ch);

        if( ch == '\\' )
        {
            isEscaped = true;
            continue;
        }

        if(ch == '"' && !isEscaped)
            break;

        tmpValue.push_back(ch);
        isEscaped = false;
    }

    // if we are EOF but haven't read a " character,
    // we still accept the sequence as a valid value
    // so we don't check for EOF here
    
    std::swap(value, tmpValue);
}


void AnyTraits<String>::input(std::basic_istream<Pt::Char>& is, String& value)
{
    input_std_string(is, value);
}

} // namespace Pt
