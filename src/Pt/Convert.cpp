/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <Pt/Convert.h>
#include <limits>
#include <cctype>
#include <cmath>
#include <cassert>

namespace {

    template <typename CharT>
    bool _stricmpL(const CharT* p1, const CharT* p2)
    {
        unsigned n;
        for (n = 0; p1[n] && p2[n]; ++n)
            if (std::tolower(p1[n]) != std::tolower(p2[n]))
                return false;
        return p1[n] == p2[n];
    }

    bool _stricmpL(const Pt::String& s, const wchar_t* p)
    {
        Pt::String::size_type n = 0;
        for ( ; n < s.size(); ++n)
            if (Pt::tolower(s[n]) != p[n])
                return false;
        return p[n] == L'\0';
    }

    template <typename CharT, typename T>
    const CharT* parseSigned(const CharT* str, T& n)
    {
        n = 0;
        bool neg = false;

        while(*str != '\0')
        {
            if( Pt::isspace(*str) )
                continue;

            switch(*str)
            {
                case '+':
                    break;

                case '-': 
                    if(neg) 
                        return 0;

                    neg = true; 
                    break;

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
                    n += static_cast<int>(*str) - '0';
                    break;

                default:
                    return 0;
                    break;
            }

            ++str;
        }

        if(neg) 
            n *= -1;

        return str;
    }

    template <typename CharT, typename T>
    const CharT* parseUnsigned(const CharT* str, T& n)
    {
        n = 0;

        while(*str != '\0')
        {
            if( Pt::isspace(*str) )
                continue;

            switch(*str)
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
                    n += static_cast<int>(*str) - '0';
                    break;

                default:
                    return 0;
                    break;
            }

            ++str;
        }

        return str;
    }

    template <typename CharT, typename T>
    inline const CharT* formatSigned(CharT* buf, size_t n, T i)
    {
        const bool negative = i < 0;
        if(negative)
        {
			i = -i;
        }

        if(0 == n)
            return 0;

        CharT* psz = buf + n - 1;
        *psz = 0;

        do
        {
            if(psz == buf)
                return 0;

            T lsd = i % 10;
            i /= 10;
            --psz;
            *psz = '0' + int(lsd);

        } while(i != 0);

        if(negative)
        {
            if(psz == buf)
                return 0;

            --psz;
            *psz = '-';
        }

        return psz;
    }

    template <typename CharT, typename T>
    inline const CharT* formatUnsigned(CharT* buf, size_t n, T i)
    {
        if(0 == n)
            return 0;

        CharT* psz = buf + n - 1;
        *psz = 0;

        do
        {
            if(psz == buf)
                return 0;

            T lsd = i % 10;
            i /= 10;
            --psz;
            *psz = '0' + int(lsd);

        } while(i != 0);

        return psz;
    }
    
    template <typename CharT, typename T>
    inline bool convertSigned(std::basic_string<CharT>& to, T n)
    {
        const size_t bufsize = (sizeof(T) * 4) + 4;
        CharT buf[bufsize];
        const CharT* str = formatSigned(buf, bufsize, n);

        if(str)
            to.assign(str);

        return 0 != str;
    }

    template <typename CharT, typename T>
    inline bool convertUnsigned(std::basic_string<CharT>& to, T n)
    {
        const size_t bufsize = (sizeof(T) * 4) + 4;
        CharT buf[bufsize];
        const CharT* str = formatUnsigned(buf, bufsize, n);

        if(str)
            to.assign(str);

        return 0 != str;
    }

    template <typename CharT, typename T>
    const CharT* parseFloat(const CharT* str, T& n)
    {
        bool neg = false;
        n = 0.0;

        // TODO: nan, inf
        //       parse for -, n, N, i, I first

        while(*str != '\0')
        {
            if( Pt::isspace(*str) )
                continue;

            if( *str == '.' )
            {
                ++str;
                break;
            }

            switch(*str)
            {
                case '+':
                    break;

                case '-': 
                    if(neg) 
                        return 0;

                    neg = true; 
                    break;

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
                    n += static_cast<int>(*str) - 48;
                    break;

                default:
                    return 0;
                    break;
            }

            ++str;
        }

        unsigned short digits = 0;
        T fraction = 0.0;
        while(*str != '\0')
        {
            if( Pt::isspace(*str) )
                break;

            switch(*str)
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
                    fraction *= 10;
                    fraction += static_cast<int>(*str) - 48;
                    ++digits;
                    break;

                default:
                    return 0;
                    break;
            }

            ++str;
        }

        T base = 10.0;
        T exp = digits;
        fraction /= std::pow(base, exp);
        n += fraction;

        if(neg)
            n *= -1;

        return str;
    }

    template <typename CharT, typename T>
    inline const CharT* formatFloat(CharT* str, size_t n, T d)
    {
        CharT* end = str + n;
        CharT* psz = str;

        // 1. Test for not-a-number with x != x
        if( d != d ) 
        {
            if(n < 4)
                return 0;

            str[0] = 'n'; str[1] = 'a'; str[2] = 'n'; str[3] = '\0';
            return str;
        }

        //2. Test for infinity by comparing with max
        if( d == std::numeric_limits<T>::infinity() ) 
        {
            if(n < 4)
                return 0;

            str[0] = 'i'; str[1] = 'n'; str[2] = 'f'; str[3] = '\0';
            return str;
        }
        
        //3. intergal part
        if(d < 0.0)
        {
    		if(psz == end)
    			return 0;

    		*psz = '-';
    		++psz;
        }

    	int digit = 0;
    	T num = std::fabs(d);
    	int m = static_cast<int>( std::log10(num) );
    	size_t places = std::numeric_limits<T>::digits10;
        
        if(num == 0.0 || m < 0)
        {
    		if(psz == end)
    			return 0;

    		*psz = '0';
    		++psz;
        }
        else
        {
    		while(m >= 0)
    		{
    			T weight = std::pow( T(10.0), m);
    			digit = static_cast<int>( floor(num / weight) );
    			num -= (digit * weight);
    			
    			if(psz == end)
    				return 0;

    			*psz = '0' + digit;
    			++psz;
    	     
    			--m;
    			--places;
    		}
        }
        
        //4. fractional part
    	T fract = num;
        
        if(psz == end)
    		return 0;

        *psz = '.';
        ++psz;

        do
        {
            fract *= 10;
            digit = static_cast<int>( floor(fract) );
            fract -= digit;
            char c = '0' + digit;

            if(psz == end)
                return 0;
    	
            *psz = c;
            ++psz;
        } 
        while(--places != 0 && fract != 0.0);

    	//5. add null terminator
        if(psz == end)
             return 0;

        *psz = 0;
        return str;
    }
}

namespace Pt {

const Pt::Char* format(Pt::Char* s, size_t n, long long value)
{
    return formatSigned(s, n, value);
}


const Pt::Char* format(Pt::Char* s, size_t n, unsigned long long value)
{
    return formatUnsigned(s, n, value);
}


const Pt::Char* format(Pt::Char* s, size_t n, double value)
{
        return formatFloat(s, n, value);
}


const Pt::Char* parse(const Pt::Char* str, long long& n)
{
    return parseSigned(str, n);
}


const Pt::Char* parse(const Pt::Char* str, unsigned long long& n)
{
    return parseUnsigned(str, n);
}

const Pt::Char* parse(const Pt::Char* str, double& n)
{
    return parseFloat(str, n);
}

//
// Conversions to Pt::String
//

void convert(String& str, bool value)
{
    static const wchar_t* trueValue = L"true";
    static const wchar_t* falseValue = L"false";
    str = value ? trueValue : falseValue;
}

void convert(String& str, char value)
{
    str = String( 1, Char(value) );
}

void convert(String& str, unsigned char value)
{
    if( false == convertUnsigned(str, value) )
        ConversionError::doThrow("unsigned char", "Pt::String");
}


void convert(String& str, signed char value)
{
    if( false == convertSigned(str, value) )
        ConversionError::doThrow("signed char", "Pt::String");
}


void convert(Pt::String& str, int value)
{
    if( false == convertSigned(str, value) )
        ConversionError::doThrow("int", "Pt::String");
}


void convert(Pt::String& str, unsigned int value)
{
    if( false == convertUnsigned(str, value) )
        ConversionError::doThrow("unsigned int", "Pt::String");
}


void convert(String& s, float value)
{
    const size_t bufsize = 64;
    Pt::Char buf[bufsize];
    const Pt::Char* num = formatFloat(buf, bufsize, value);
    if( 0 == num  )
        ConversionError::doThrow("float", "Pt::String");

    s.assign(num);
}


void convert(String& s, double value)
{
    const size_t bufsize = 64;
    Pt::Char buf[bufsize];
    const Pt::Char* num = formatFloat(buf, bufsize, value);
    if( 0 == num  )
        ConversionError::doThrow("float", "Pt::String");

    s.assign(num);
}

//
// Conversions from Pt::String
//

void convert(bool& n, const String& str)
{
    if (str == L"true" || str == L"1")
        n = true;
    else if (str == L"false" || str == L"0")
        n = false;
    else
        ConversionError::doThrow("bool", "Pt::String");
}

void convert(char& c, const String& str)
{
    if ( str.empty() )
        ConversionError::doThrow("char", "Pt::String");

    int n = str[0];
    c = n;
}

void convert(unsigned char& n, const String& str)
{
    const Pt::Char* end = parseUnsigned( str.c_str(), n);

    if(0 == end || *end != '\0')
        ConversionError::doThrow("unsigned char", "Pt::String");
}


void convert(signed char& n, const String& str)
{
    const Pt::Char* end = parseSigned( str.c_str(), n );

    if(0 == end || *end != '\0')
        ConversionError::doThrow("signed char", "Pt::String");
}


void convert(int& n, const Pt::String& str)
{
    const Pt::Char* end = parseSigned( str.c_str(), n );

    if(0 == end || *end != '\0')
        ConversionError::doThrow("int", "Pt::String");
}


void convert(unsigned int& n, const Pt::String& str)
{
    const Pt::Char* end = parseUnsigned( str.c_str(), n );

    if(0 == end || *end != '\0')
        ConversionError::doThrow("int", "Pt::String");
}


void convert(float& n, const String& str)
{
    const Pt::Char* end = parseFloat( str.c_str(), n );

    if(0 == end || *end != '\0')
        ConversionError::doThrow("float", "Pt::String");
/*
    // not a number
    if (_stricmpL(str, L"nan"))
    {
        n = std::numeric_limits<float>::quiet_NaN();
        return;
    }

    // inf
    if (_stricmpL(str, L"inf") || _stricmpL(str.c_str(), L"infinity"))
    {
        n = std::numeric_limits<float>::infinity();
        return;
    }

    // -inf
    if (_stricmpL(str, L"-inf") || _stricmpL(str.c_str(), L"-infinity"))
    {
        n = -std::numeric_limits<float>::infinity();
        return;
    }

    IStringStream is(str);
    Char ch;
    is >> n;

    if (is.fail() || !(is >> ch).eof())
    {
        ConversionError::doThrow("float", "Pt::String");
    }
    */
}


void convert(double& n, const String& str)
{
    const Pt::Char* end = parseFloat( str.c_str(), n );

    if(0 == end || *end != '\0')
        ConversionError::doThrow("double", "Pt::String");
}

//
// Conversions to std::string
//

void convert(std::string& str, int value)
{
    if( false == convertSigned(str, value) )
        ConversionError::doThrow("int", "std::string");
}


void convert(std::string& str, unsigned int value)
{
    if( false == convertUnsigned(str, value) )
        ConversionError::doThrow("unsigned int", "std::string");
}


void convert(std::string& str, float value)
{
    const size_t bufsize = 64;
    char buf[bufsize];
    const char* num = formatFloat(buf, bufsize, value);
    if( 0 == num  )
        ConversionError::doThrow("float", "std::string");

    str.assign(num);
}


void convert(std::string& str, double value)
{
    const size_t bufsize = 64;
    char buf[bufsize];
    const char* num = formatFloat(buf, bufsize, value);
    if( 0 == num  )
        ConversionError::doThrow("double", "std::string");

    str.assign(num);
}

//
// Conversions from std::string
//

void convert(bool& n, const std::string& str)
{
    if (str == "true" || str == "1")
        n = true;
    else if (str == "false" || str == "0")
        n = false;
    else
        ConversionError::doThrow("bool", "std::string");
}


void convert(float& n, const std::string& str)
{
    const char* end = parseFloat( str.c_str(), n );

    if(0 == end || *end != '\0')
        ConversionError::doThrow("float", "std::string");
}


void convert(double& n, const std::string& str)
{
    const char* end = parseFloat( str.c_str(), n );

    if(0 == end || *end != '\0')
        ConversionError::doThrow("float", "std::string");
}

}
