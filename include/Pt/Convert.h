/*
 * Copyright (C) 2004-2007 by Marc Boris Duerner
 * Copyright (C) 2004-2007 by Stepan Beal
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
#ifndef Pt_Convert_h
#define Pt_Convert_h

#include <Pt/Api.h>
#include <Pt/String.h>
//#include <Pt/SourceInfo.h>
#include <Pt/ConversionError.h>
#include <Pt/StringStream.h>
#include <sstream>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <iostream>


namespace Pt {

template <typename T>
inline void convert(Pt::String& s, const T& value)
{
    Pt::StringStream os;
    os << value;
    s = os.str();
}


template <typename T>
inline void convert(T& t, const Pt::String& str)
{
    Pt::StringStream is(str);
    is >> t;
    const std::ios::iostate iostate = is.rdstate();
    if((iostate&std::ios::failbit) || (iostate&std::ios::badbit) || !(iostate&std::ios::eofbit))
    {
        throw Pt::ConversionError( PT_CONVERSIONERROR(T, Pt::String) );
    }
}


template <typename T>
inline void convert(std::string& s, const T& value)
{
    std::ostringstream os;
    os << value;
    s = os.str();
}


template <typename T>
inline void convert(T& t, const std::string& str)
{
    std::istringstream is(str);
    is >> t;
    const std::ios::iostate iostate = is.rdstate();
    if((iostate&std::ios::failbit) || (iostate&std::ios::badbit) || !(iostate&std::ios::eofbit))
    {
        throw Pt::ConversionError( PT_CONVERSIONERROR(T, std::string) );
    }
}


inline void convert(Pt::String& s, const Pt::String& str)
{
    s = str;
}


inline void convert(Pt::String& s, bool value)
{
    s = value ?
        Pt::String::widen("true") :
        Pt::String::widen("false");
}

inline void convert(bool& n, const Pt::String& str)
{
    if( ( str.size() == 4 ) &&
        ( str[0] == Pt::Char('t') ) &&
        ( str[1] == Pt::Char('r') ) &&
        ( str[2] == Pt::Char('u') ) &&
        ( str[3] == Pt::Char('e') ) )
    {
        n = true;
    }
    else if( ( str.size() == 5 ) &&
        ( str[0] == Pt::Char('f') ) &&
        ( str[1] == Pt::Char('a') ) &&
        ( str[2] == Pt::Char('l') ) &&
        ( str[3] == Pt::Char('s') ) &&
        ( str[4] == Pt::Char('e') ) )
    {
        n = false;
    }
    else if( ( str.size() == 1 ) &&
        ( str[0] == Pt::Char('1') ) )
    {
        n = true;
    }
    else if( ( str.size() == 1 ) &&
        ( str[0] == Pt::Char('0') ) )
    {
        n = false;
    }
    else
    {
        throw Pt::ConversionError( PT_CONVERSIONERROR(bool, Pt::String) );
    }
}

inline void convert(bool& n, const std::string& str)
{
    if( ( str.size() == 4 ) &&
        ( str[0] == 't' ) &&
        ( str[1] == 'r' ) &&
        ( str[2] == 'u' ) &&
        ( str[3] == 'e' ) )
    {
        n = true;
    }
    else if( ( str.size() == 5 ) &&
        ( str[0] == 'f' ) &&
        ( str[1] == 'a' ) &&
        ( str[2] == 'l' ) &&
        ( str[3] == 's' ) &&
        ( str[4] == 'e' ) )
    {
        n = false;
    }
    else if( ( str.size() == 1 ) &&
        ( str[0] == '1' ) )
    {
        n = true;
    }
    else if( ( str.size() == 1 ) &&
        ( str[0] == '0' ) )
    {
        n = false;
    }
    else
    {
        throw Pt::ConversionError( PT_CONVERSIONERROR(bool, std::string) );
    }
}

inline void convert(Pt::String& s, char value)
{
    s = Pt::String( 1, Pt::Char(value) );
}


inline void convert(char& n, const Pt::String& str)
{
    if( str.empty() )
        throw Pt::ConversionError( PT_CONVERSIONERROR(char, Pt::String) );

    n = str[0].narrow('*');
}


inline void convert(Pt::String& s, unsigned char value)
{
    Pt::StringStream ss;
    unsigned int i = static_cast<unsigned int>(value);
    ss << i;
    s = ss.str();
}


inline void convert(unsigned char& n, const Pt::String& str)
{
    if( str.empty() )
        throw Pt::ConversionError( PT_CONVERSIONERROR(unsigned char, Pt::String) );

    // interpret as numeric value
    Pt::StringStream ss(str);
    unsigned int i = 0;
    ss >> i;
    const std::ios::iostate iostate = ss.rdstate();
    if((iostate&std::ios::failbit) || (iostate&std::ios::badbit) || !(iostate&std::ios::eofbit))
    {
        throw Pt::ConversionError( PT_CONVERSIONERROR(unsigned char, Pt::String) );
    }
    n = static_cast<unsigned char>(i);
}


inline void convert(Pt::String& s, signed char value)
{
    Pt::StringStream ss;
    int i = static_cast<signed int>(value);
    ss << i;
    s = ss.str();
}


inline void convert(signed char& n, const Pt::String& str)
{
    if( str.empty() )
        throw Pt::ConversionError( PT_CONVERSIONERROR(signed char, Pt::String) );
        
    // interpret as numeric value
    Pt::StringStream ss(str);
    int i = 0;
    ss >> i;
    const std::ios::iostate iostate = ss.rdstate();
    if((iostate&std::ios::failbit) || (iostate&std::ios::badbit) || !(iostate&std::ios::eofbit))
    {
        throw Pt::ConversionError( PT_CONVERSIONERROR(signed char, Pt::String) );
    }
    n = static_cast<signed char>(i);
}


inline void convert(Pt::String& s, const std::string& value)
{
    s = Pt::String::widen(value);
}


inline void convert(std::string& s,const Pt::String& str)
{
    s = str.narrow();
}


inline void convert(Pt::String& s, float value)
{
    // not a number
    if(value != value)
    {
        s = L"NAN";
        return;
    }

    Pt::StringStream os;
    os << value;
    s = os.str();
}


inline void convert(float& n, const Pt::String& str)
{
    // not a number
    if(str == L"NAN")
    {
        n = std::numeric_limits<float>::quiet_NaN();
        return;
    }

    Pt::StringStream is(str);
    is >> n;

    const std::ios::iostate iostate = is.rdstate();
    if((iostate&std::ios::failbit) || (iostate&std::ios::badbit) || !(iostate&std::ios::eofbit))
    {
        throw Pt::ConversionError( PT_CONVERSIONERROR(float, Pt::String) );
    }
}


inline void convert(Pt::String& s, double value)
{
    // not a number
    if(value != value)
    {
        s = L"NAN";
        return;
    }

    Pt::StringStream os;
    os << std::fixed << std::setprecision(15) << value;
    s = os.str();
}


inline void convert(double& n, const Pt::String& str)
{
    // not a number
    if(str == L"NAN")
    {
        n = std::numeric_limits<float>::quiet_NaN();
        return;
    }

    Pt::StringStream is(str);
    is >> std::fixed >> std::setprecision(15) >> n;

    const std::ios::iostate iostate = is.rdstate();
    if((iostate&std::ios::failbit) || (iostate&std::ios::badbit) || !(iostate&std::ios::eofbit))
    {
        throw Pt::ConversionError( PT_CONVERSIONERROR(double, Pt::String) );
    }
}

inline void convert(std::string& s, float value)
{
    // not a number
    if(value != value)
    {
        s = "NAN";
        return;
    }

    std::ostringstream os;
    os << value;
    s = os.str();
}


inline void convert(float& n, const std::string& str)
{
    // not a number
    if(str == "NAN")
    {
        n = std::numeric_limits<float>::quiet_NaN();
        return;
    }

    std::istringstream is(str);
    is >> n;

    const std::ios::iostate iostate = is.rdstate();
    if((iostate&std::ios::failbit) || (iostate&std::ios::badbit) || !(iostate&std::ios::eofbit))
    {
        throw Pt::ConversionError( PT_CONVERSIONERROR(float, std::string) );
    }
}


inline void convert(std::string& s, double value)
{
    // not a number
    if(value != value)
    {
        s = "NAN";
        return;
    }

    std::ostringstream os;
    os << std::fixed << std::setprecision(15) << value;
    s = os.str();
}

inline void convert(double& n, const std::string& str)
{
    // not a number
    if(str == "NAN")
    {
        n = std::numeric_limits<float>::quiet_NaN();
        return;
    }

    std::stringstream is(str);
    is >> std::fixed >> std::setprecision(15) >> n;

    const std::ios::iostate iostate = is.rdstate();
    if((iostate&std::ios::failbit) || (iostate&std::ios::badbit) || !(iostate&std::ios::eofbit))
    {
        throw Pt::ConversionError( PT_CONVERSIONERROR(double, std::string) );
    }
}

template<typename T, typename S>
void convert(T& to, const S& from)
{
    Pt::StringStream ss;
    if( !(ss << from && ss >> to) )
        throw Pt::ConversionError( PT_CONVERSIONERROR(streamable, streamable) );
}


template<typename T, typename S>
struct Convert
{
    T operator()(const S& from) const
    {
        T value = T();
        convert(value, from);
        return value;
    }
};


template<typename T, typename S>
T convert(const S& from)
{
    T value = T();
    convert(value, from);
    return value;
}

} // namespace Pt

#endif
