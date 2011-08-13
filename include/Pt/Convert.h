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
#include <Pt/ConversionError.h>
#include <Pt/StringStream.h>
#include <sstream>
#include <string>
#include <typeinfo>

namespace Pt {

// nothrow
PT_API const Pt::Char* format(Pt::Char* s, size_t n, long long value);

// nothrow
PT_API const Pt::Char* format(Pt::Char* s, size_t n, unsigned long long value);

// nothrow
PT_API const Pt::Char* parse(long long& n, const Pt::Char* str);

// nothrow
PT_API const Pt::Char* parse(unsigned long long& n, const Pt::Char* str);

//
// Conversions to Pt::String
//

inline void convert(String& s, const String& str)
{
    s = str;
}

inline void convert(String& s, const std::string& value)
{
    s = String::widen(value);
}

PT_API void convert(String& s, bool value);
PT_API void convert(String& s, char value);
PT_API void convert(String& s, unsigned char value);
PT_API void convert(String& s, signed char value);
PT_API void convert(String& s, int value);
PT_API void convert(String& s, unsigned int value);
PT_API void convert(String& s, float value);
PT_API void convert(String& s, double value);

template <typename T>
inline void convert(String& s, const T& value)
{
    OStringStream os;
    os << value;
    s = os.str();
}

//
// Conversions from Pt::String
//

PT_API void convert(bool& n, const String& str);
PT_API void convert(char& n, const String& str);
PT_API void convert(unsigned char& n, const String& str);
PT_API void convert(signed char& n, const String& str);
PT_API void convert(int& n, const String& str);
PT_API void convert(unsigned int& n, const String& str);
PT_API void convert(float& n, const String& str);
PT_API void convert(double& n, const String& str);

template <typename T>
inline void convert(T& t, const String& str)
{
    IStringStream is(str);
    Char ch;
    is >> t;
    if (is.fail() || !(is >> ch).eof())
        ConversionError::doThrow("T", "Pt::String");
}



//
// Conversions to std::string
//

inline void convert(std::string& s, const std::string& str)
{
    s = str;
}

inline void convert(std::string& s,const String& str)
{
    s = str.narrow();
}

PT_API void convert(std::string& s, int value);
PT_API void convert(std::string& s, unsigned int value);
PT_API void convert(std::string& s, float value);
PT_API void convert(std::string& s, double value);

template <typename T>
inline void convert(std::string& s, const T& value)
{
    std::ostringstream os;
    os << value;
    s = os.str();
}

//
// Conversions from std::string
//

PT_API void convert(bool& n, const std::string& str);
PT_API void convert(float& n, const std::string& str);
PT_API void convert(double& n, const std::string& str);

template <typename T>
inline void convert(T& t, const std::string& str)
{
    std::istringstream is(str);
    char ch;
    is >> t;
    if (is.fail() || !(is >> ch).eof())
        ConversionError::doThrow("T", "std::string");
}


inline void convert(float& n, const char* str)
{ convert(n, std::string(str)); }

inline void convert(double& n, const char* str)
{ convert(n, std::string(str)); }

//
// Conversions from wide character strings
//

inline void convert(float& n, const wchar_t* str)
{ convert(n, String(str)); }

inline void convert(double& n, const wchar_t* str)
{ convert(n, String(str)); }


//
// Generic stream-based conversions
//

template<typename T, typename S>
void convert(T& to, const S& from)
{
    StringStream ss;
    if( !(ss << from && ss >> to) )
        ConversionError::doThrow(typeid(T).name(), typeid(S).name());
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
