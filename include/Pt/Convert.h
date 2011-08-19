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
#include <limits>
#include <iterator>
#include <cctype>
#include <cmath>

namespace Pt {

//
// Conversions to Pt::String
//

inline void convert(String& s, const String& str)
{
    s = str;
}

PT_API void convert(String& s, const std::string& value);

PT_API void convert(String& s, bool value);

PT_API void convert(String& s, char value);
PT_API void convert(String& s, unsigned char value);
PT_API void convert(String& s, signed char value);

PT_API void convert(String& s, short value);
PT_API void convert(String& s, unsigned short value);
PT_API void convert(String& s, int value);
PT_API void convert(String& s, unsigned int value);
PT_API void convert(String& s, long value);
PT_API void convert(String& s, unsigned long value);

PT_API void convert(String& s, float value);
PT_API void convert(String& s, double value);
PT_API void convert(String& s, long double value);

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

PT_API void convert(short& n, const String& str);
PT_API void convert(unsigned short& n, const String& str);
PT_API void convert(int& n, const String& str);
PT_API void convert(unsigned int& n, const String& str);
PT_API void convert(long& n, const String& str);
PT_API void convert(unsigned long& n, const String& str);

PT_API void convert(float& n, const String& str);
PT_API void convert(double& n, const String& str);
PT_API void convert(long double& n, const String& str);

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
// Conversions from const Pt::Char* (null-terminated)
//

PT_API void convert(int& n, const Pt::Char* str);

//
// Conversions to std::string
//

inline void convert(std::string& s, const std::string& str)
{
    s = str;
}

PT_API void convert(std::string& s, const String& str);

PT_API void convert(std::string& s, bool value);

PT_API void convert(std::string& s, char value);
PT_API void convert(std::string& s, signed char value);
PT_API void convert(std::string& s, unsigned char value);

PT_API void convert(std::string& s, short value);
PT_API void convert(std::string& s, unsigned short value);
PT_API void convert(std::string& s, int value);
PT_API void convert(std::string& s, unsigned int value);
PT_API void convert(std::string& s, long value);
PT_API void convert(std::string& s, unsigned long value);

PT_API void convert(std::string& s, float value);
PT_API void convert(std::string& s, double value);
PT_API void convert(std::string& s, long double value);

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

PT_API void convert(char& n, const std::string& str);
PT_API void convert(signed char& n, const std::string& str);
PT_API void convert(unsigned char& n, const std::string& str);

PT_API void convert(short& n, const std::string& str);
PT_API void convert(unsigned short& n, const std::string& str);
PT_API void convert(int& n, const std::string& str);
PT_API void convert(unsigned int& n, const std::string& str);
PT_API void convert(long& n, const std::string& str);
PT_API void convert(unsigned long& n, const std::string& str);

PT_API void convert(float& n, const std::string& str);
PT_API void convert(double& n, const std::string& str);
PT_API void convert(long double& n, const std::string& str);

template <typename T>
inline void convert(T& t, const std::string& str)
{
    std::istringstream is(str);
    char ch;
    is >> t;
    if (is.fail() || !(is >> ch).eof())
        ConversionError::doThrow("T", "std::string");
}

//
// Conversions from const char* (null-terminated)
//

PT_API void convert(int& n, const char* str);

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

//
// parsing and formating of numbers
//

template <typename InIterT, typename T>
InIterT getSigned(InIterT it, InIterT end, bool& ok, T& n)
{
    n = 0;
    bool neg = false;
    ok = false;
    
    // strip leading whitespace, parse sign
    for( ; it != end; ++it)
    {
        if( ! Pt::isspace(*it) )
        {
			switch(*it)
			{
				case '-':
					neg = true; 
					// fall through intended
					
				case '+':
					++it; 
					break;
			}
			
			break;
        }
    }

	// parse number
	T d = 0;
    bool done = false;
    while(it != end)
    {
        switch(*it)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if ( n != 0 && 10 > std::numeric_limits<T>::max() / n )
					return it;

                n *= 10;

                d = *it - '0';
                if(d > std::numeric_limits<T>::max() - n)
                    return it;
                
                n += d;
                break;

            default:
                done = true;
                break;
        }

        if(done)
            break;

        ++it;
    }

    if(neg) 
        n *= -1;

	ok = true;
    return it;
}


template <typename OutIterT, typename T>
inline OutIterT putSigned(OutIterT it, T i)
{
    const bool negative = i < 0;
    if(negative)
    {
		i = -i;
		*it = '-'; ++it;
    }
   
    // large enough for packed hex, dec, oct numbers
    const std::size_t buflen = (sizeof(T) * 4) + 1;
	char buf[buflen];
    
    char* end = buf + buflen;
    char* cur = end;

    do
    {
        T lsd = i % 10;
        i /= 10;
        --cur;
        *cur = '0' + int(lsd);
    } 
    while(i != 0 && cur != buf);

	return std::copy(cur, end, it);
}


template <typename InIterT, typename T>
InIterT getUnsigned(InIterT it, InIterT end, bool& ok, T& n)
{
    n = 0;
    ok = false;
    
    // strip leading whitespace, parse sign
    for( ; it != end; ++it)
    {
        if( ! Pt::isspace(*it) )
        {
			switch(*it)
			{				
				case '+':
					++it; 
					break;
			}
			
			break;
        }
    }

	// parse number
	T d = 0;
    bool done = false;
    while(it != end)
    {
        switch(*it)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if ( n != 0 && 10 > std::numeric_limits<T>::max() / n )
					return it;

                n *= 10;

                d = *it - '0';
                if(d > std::numeric_limits<T>::max() - n)
                    return it;
                
                n += d;
                break;

            default:
                done = true;
                break;
        }

        if(done)
            break;

        ++it;
    }

	ok = true;
    return it;
}


template <typename OutIterT, typename T>
inline OutIterT putUnsigned(OutIterT it, T i)
{
    // large enough for packed hex, dec, oct numbers with a sign
    const std::size_t buflen = (sizeof(T) * 4) + 1;
	char buf[buflen];
    
    char* end = buf + buflen;
    char* cur = end;

    do
    {
        T lsd = i % 10;
        i /= 10;
        --cur;
        *cur = '0' + int(lsd);
    } 
    while(i != 0 && cur != buf);

	return std::copy(cur, end, it);
}


template <typename InIterT, typename T>
InIterT getFloat(InIterT it, InIterT end, bool& ok, T& n)
{
    bool pos = false;
    bool neg = false;
    n = 0.0;
    ok = false;

    // leading whitespace, sign, NaN, infinity
    bool done = false;
    while(it != end)
    {
        if( Pt::isspace(*it) )
            continue;

        switch(*it)
        {
            case '+':
                if(pos || neg)
                    return it;

                pos = true; 
                break;

            case '-': 
                if(neg || pos) 
                    return it;

                neg = true; 
                break;

            case 'n':
            case 'N':
                if(++it == end)
                    return it;

                if(*it != 'a' && *it != 'A')
                    return it;

                if(++it == end)
                    return it;

                if(*it != 'n' && *it != 'N')
                    return it;

                n = std::numeric_limits<T>::quiet_NaN();
                ok = true;
                return ++it;
                break;

            case 'i':
            case 'I':
                if(++it == end)
                    return it;

                if(*it != 'n' && *it != 'N')
                    return it;

                if(++it == end)
                    return it;

                if(*it != 'f' && *it != 'F')
                    return it;

                if( ++it != end )
                {
                    if(*it != 'i' && *it != 'I')
                        return it;

                    if(++it == end)
                        return it;

                    if(*it != 'n' && *it != 'N')
                        return it;

                    if(++it == end)
                        return it;

                    if(*it != 'i' && *it != 'I')
                        return it;

                    if(++it == end)
                        return it;

                    if(*it != 't' && *it != 'T')
                        return it;

                    if(++it == end)
                        return it;

                    if(*it != 'y' && *it != 'Y')
                        return it;

                    ++it;
                }

                n = std::numeric_limits<T>::infinity();
                if(neg)
                    n *= -1;

                ok = true;
                return it;
                break;

            default:
                done = true;
                break;
        }

        if(done)
            break;

        ++it;
    }

    // integral part
    while(it != end)
    {
        if( *it == '.' )
        {
            ++it;
            break;
        }

        switch(*it)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                n *= 10;
                n += static_cast<int>(*it) - 48;
                break;

            default:
                return it;
                break;
        }

        ++it;
    }
    
    if(it == end)
        return it;

    // fractional part
    unsigned short digits = 0;
    size_t maxDigits = std::numeric_limits<unsigned short>::max() - std::numeric_limits<T>::digits10;
    while(it != end && *it == '0')
    {
        if( digits > maxDigits )
            return it;

		++digits;
		++it;
    }
 
    unsigned short significants = 0;
    T fraction = 0.0;
    done = false;
    while(it != end)
    {
        switch(*it)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if( significants <= std::numeric_limits<T>::digits10 )
                {
                    fraction *= 10;
                    fraction += static_cast<int>(*it) - 48;

                    ++digits;
				    ++significants;
                }

                break;

            default:
                done = true;
                break;
        }

		if(done)
			break;

        ++it;
    }

    T base = 10.0;
    T exp = digits;
    fraction /= std::pow(base, exp);
    n += fraction;

    if(neg)
        n *= -1;

	ok = true;
    return it;
}


template <typename IterT, typename T>
inline IterT putFloat(IterT it, T d)
{
    // 1. Test for not-a-number with d != d
    if( d != d ) 
    {
        *it = 'n'; ++it;
        *it = 'a'; ++it;
        *it = 'n'; ++it;
        return it;
    }

    // 2. check sign
    if(d < 0.0)
    {
        *it = '-';
        ++it;
    }

    int digit = 0;
    T num = std::fabs(d);

    // 3. Test for infinity
    if( num == std::numeric_limits<T>::infinity() ) 
    {
        *it = 'i'; ++it;
        *it = 'n'; ++it;
        *it = 'f'; ++it;
        return it;
    }

	// 4. integral part
	int m = static_cast<int>( std::log10(num) );
	int places = std::numeric_limits<T>::digits10;
    
    if(num == 0.0 || m < 0)
    {
		*it = '0';
		++it;
    }
    else
    {
		while(m >= 0)
		{
			T weight = std::pow( T(10.0), m);
			digit = static_cast<int>( floor(num / weight) );
			num -= (digit * weight);

			*it = '0' + digit;
			++it;

			--m;
			--places;
		}
    }
    
    // 5. fractional part
	T fract = num;

    *it = '.';
    ++it;

    do
    {
        fract *= 10;
        digit = static_cast<int>( std::floor(fract) );
        fract -= digit;
        char c = '0' + digit;

        *it = c;
        ++it;
        
        // count significant digits from first non-null digit
        if(places != std::numeric_limits<T>::digits10 || digit != 0)
        {
            --places;
        }
    } 
    while(places != 0 && fract != 0.0);

    return it;
}

} // namespace Pt

#endif
