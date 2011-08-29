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
#include <iostream>
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

template <typename T>
inline bool formatNegate(T& i)
{
    bool isNeg = i < 0;
    if(isNeg)
        i = -i;

    return isNeg;
}

inline bool formatNegate(unsigned char&)
{
    return false;
}

inline bool formatNegate(unsigned short&)
{
    return false;
}

inline bool formatNegate(unsigned int&)
{
    return false;
}

inline bool formatNegate(unsigned long&)
{
    return false;
}

inline bool formatNegate(unsigned long long&)
{
    return false;
}


template <typename CharT, typename T>
inline CharT* formatInt(CharT* buf, std::streamsize buflen, T i, 
                        const CharT* basetab, std::size_t base, CharT neg)
{
    CharT* end = buf + buflen;
    CharT* cur = end;

    bool isNeg = formatNegate(i); 

    do
    {
        T lsd = i % base;
        i /= base;
        --cur;
        const CharT* ch = basetab + int(lsd);
        *cur = *ch;
    } 
    while(i != 0 && cur != buf);
    
    if(cur == buf)
        return buf;
    
    if(isNeg)
    {
        --cur;
        *cur = neg;
    }

    return cur;
}

/*
template <typename T>
inline char* formatInt(char* buf, std::streamsize buflen, T i)
{
    static const char basetab[] = "0123456789";
    return formatInt(buf, buflen, i, basetab, sizeof(basetab)/sizeof(char), '-');
}

template <typename T>
inline Pt::Char* formatInt(Pt::Char* buf, std::streamsize buflen, T i)
{
    static const Pt::Char basetab[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    return formatInt(buf, buflen, i, basetab, sizeof(basetab)/sizeof(Pt::Char), Pt::Char('-'));
}

template <typename CharT, typename OutIterT, typename T>
inline OutIterT putInt(OutIterT it, T i)
{
    return it;
}
*/

template <typename CharT, typename T>
inline std::streamsize formatFloat(CharT* fraction, std::streamsize precision, int& intpart, int& exp, T n,
                                   const CharT* basetab, std::size_t base)
{
    intpart = 0;
    exp = 0;
    
    if(n == T(0.0) || n != n)
        return 0;

    const bool neg = n < 0;
    if(n < 0)
        n = -n;
    
    if( n == std::numeric_limits<T>::infinity() )
        return 0;
    
    exp = static_cast<int>( std::log10(n) );
    
    if(exp != 0)
        n /= std::pow(T(10.0), exp);

    intpart = static_cast<int>( std::floor(n) );
    n -= intpart;
    if(neg)
        intpart = -intpart;
    
    int digit = 0;
    T eps = std::numeric_limits<T>::epsilon();
    std::streamsize places = 0;

    while(n > eps && places <= precision)
    {
        eps *= 10.0;
        n *= 10.0;
        digit = static_cast<int>( std::floor(n) );
        n -= digit;
        const CharT* c = basetab + digit;

        *fraction++ = *c;
        ++places;
    }

    return places;
}

template <typename T>
inline std::streamsize formatFloat(char* fraction, std::streamsize precision, int& intpart, int& exp, T n)
{
    static const char basetab[] = "0123456789";
    return formatFloat(fraction, precision, intpart, exp, n, basetab, sizeof(basetab)/sizeof(char));
}

template <typename T>
inline std::streamsize formatFloat(Pt::Char* fraction, std::streamsize precision, int& intpart, int& exp, T n)
{
    static const Pt::Char basetab[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    return formatFloat(fraction, precision, intpart, exp, n, basetab, sizeof(basetab)/sizeof(Pt::Char));
}
/*
template <typename CharT, typename OutIterT, typename T>
inline OutIterT putFloat(OutIterT it, T n)
{
    return it;
}*/


struct FloatFormat
{
    typedef char CharT;

    static CharT neg()
    { return '-'; }
};


template <typename OutIterT, typename T, typename FormatT>
inline OutIterT putFloat(OutIterT it, T d, FormatT& fmt)
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
        *it = fmt.neg();
        ++it;
    }

    T num = std::fabs(d);

    // 3. Test for infinity
    if( num == std::numeric_limits<T>::infinity() ) 
    {
        *it = 'i'; ++it;
        *it = 'n'; ++it;
        *it = 'f'; ++it;
        return it;
    }
    
    const std::streamsize bufsize = std::numeric_limits<T>::digits10;
    typename FormatT::CharT fract[bufsize];
    int i = 0;
    int e = 0;
    std::streamsize fractSize = Pt::formatFloat(fract, bufsize, i, e, num);

    // show only significant digits for default format
    std::streamsize precision = 1;
    if(e < fractSize)
        precision = fractSize - e;

    std::streamsize n = 0;
    if(e >= 0)
    {
        *it++ = '0' + i;
        for(; n < e; ++n)
            *it++ = (n < fractSize) ? fract[n] : '0';

        *it++ = '.';
    }
    else
    {
        *it++ = '0';
        *it++ = '.';

        for( ;n > ++e && precision > 0; --precision)
            *it++ = '0';

        if(precision-- > 0)
            *it++ = '0' + i;
    }

    for(; precision > 0; ++n, --precision)
        *it++ = (n < fractSize) ?  fract[n] : '0'; 

    return it;
}


template <typename OutIterT, typename T>
inline OutIterT putFloat(OutIterT it, T d)
{
    FloatFormat fmt;
    return putFloat(it, d, fmt);
}


// TODO: move to num_put facet
template <typename OutIterT, typename CharT>
inline OutIterT putNumber(OutIterT it, const CharT* beg, const CharT* end,
                          std::ios_base::fmtflags flags, 
                          std::streamsize width, CharT fill) 
{
    bool hasSign = *beg == '+' || *beg == '-';

    std::streamsize len = end - beg;
    if (len >= width)
    {
        return std::copy(beg, end, it);
    }

    std::streamsize pad =  width - len;
    std::ios_base::fmtflags dir = flags & std::ios_base::adjustfield;

    if (dir == std::ios_base::left) 
    {
        it = std::copy(beg, end, it);
        for ( ; pad > 0; --pad)  *it++ = fill;
        return it;
    }
    
    if( dir == std::ios_base::internal && hasSign) 
    {
        *it++ = *beg;
        for ( ; pad > 0; --pad)  *it++ = fill;
        return std::copy(beg + 1, end, it);
    }

    // right adjustment
    for (; pad > 0; --pad)  *it++ = fill;
    return std::copy(beg, end, it);
}

// TODO: move to num_put facet
template <typename OutIterT, typename T, typename CharT>
inline OutIterT putDecimal(OutIterT it, T i, 
                           std::ios_base::fmtflags flags, 
                           std::streamsize width, CharT fill)
{
    bool showPos = (flags & std::ios_base::showpos) == std::ios_base::showpos;

    static const CharT basetab[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

    // large enough for decimal with a sign
    const std::size_t buflen = (sizeof(T) * 4) + 1;
    CharT buf[buflen];
    CharT* number = Pt::formatInt(buf, buflen, i, basetab, sizeof(basetab)/sizeof(CharT), CharT('-'));

    CharT first = *number;
    if(showPos && first != '-' && number != buf)
        *(--number) = '+';

    return putNumber(it, number, buf+buflen, flags, width, fill);
}

// TODO: move to num_put facet
template <typename OutIterT, typename T, typename CharT>
inline OutIterT putHex(OutIterT it, T i, 
                       std::ios_base::fmtflags flags, 
                       std::streamsize width, CharT fill)
{
    bool showPos = (flags & std::ios_base::showpos) == std::ios_base::showpos;
    bool showBase = (flags & std::ios_base::showbase) == std::ios_base::showbase;
    bool upperCase = (flags & std::ios_base::uppercase) == std::ios_base::uppercase;
    
    static const CharT basetabL[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                      'a', 'b', 'c', 'd', 'e', 'f'};

    static const CharT basetabU[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                      'A', 'B', 'C', 'D', 'E', 'F'};

    const CharT* basetab = basetabL;
    if(upperCase)
        basetab = basetabU;

    // large enough for hex number, sign and base
    const std::size_t buflen = (sizeof(T) * 4) + 3;
    CharT buf[buflen];

    CharT* number = Pt::formatInt(buf, buflen, i, basetab, sizeof(basetabL)/sizeof(CharT), CharT('-'));

    CharT first = *number;
    if(showBase && (number - buf >= 2))
    {
        if(first != '-') 
            --number;

        *number-- = 'x';
        *number = '0';

        if(first == '-') 
            *(--number) = first;
    }

    if(showPos && first != '-' && number != buf)
        *(--number) = '+';

    return putNumber(it, number, buf+buflen, flags, width, fill);
}

// TODO: move to num_put facet
template <typename OutIterT, typename T, typename CharT>
inline OutIterT putOctal(OutIterT it, T i, 
                         std::ios_base::fmtflags flags, 
                         std::streamsize width, CharT fill)
{
    bool showPos = (flags & std::ios_base::showpos) == std::ios_base::showpos;
    bool showBase = (flags & std::ios_base::showbase) == std::ios_base::showbase;
    
    static const CharT basetab[] = { '0', '1', '2', '3', '4', '5', '6', '7' };

    // large enough for octal with a sign and base
    const std::size_t buflen = (sizeof(T) * 4) + 2;
    CharT buf[buflen];

    CharT* number = Pt::formatInt(buf, buflen, i, basetab, sizeof(basetab)/sizeof(CharT), CharT('-'));

    CharT first = *number;
    if(showBase && (number != buf))
    {
        if(first != '-') 
            --number;

        *number = '0';

        if(first == '-') 
            *(--number) = first;
    }

    if(showPos && first != '-' && number != buf)
        *(--number) = '+';

    return putNumber(it, number, buf+buflen, flags, width, fill);
}

// TODO: move to num_put facet
template <typename IterT, typename T, typename CharT>
inline IterT putFloat(IterT it, T d, 
                      std::ios_base::fmtflags flags, 
                      std::streamsize width, CharT fill,
                      std::streamsize precision = 6)
{
    bool scientific = (flags & std::ios_base::scientific) == std::ios_base::scientific;
    //bool fixed = (flags & std::ios_base::fixed) == std::ios_base::fixed;
    bool leftAdjust = (flags & std::ios_base::left) == std::ios_base::left;
    bool internalAdjust = (flags & std::ios_base::internal) == std::ios_base::internal;
    bool rightAdjust = ! (leftAdjust || internalAdjust);

    const std::streamsize bufsize = std::numeric_limits<T>::digits10;
    CharT fract[bufsize];
    int i = 0;
    int e = 0;
    std::streamsize fractSize = Pt::formatFloat(fract, bufsize, i, e, d);

    std::streamsize len = 0;
    if( 0 == (flags & std::ios_base::floatfield) )
    {
        // show only significant digits for default format
        precision = 1;
        if(e < fractSize)
            precision = fractSize - e;
    }

    if(scientific)
    {
        len += precision + 6; // fraction digits, intpart, 3 exp digits, signed e/E
    }
    else // fixed and default
    {
        len += precision + 1;
    
        if(e > 0)
            len += e;
    }

    bool hasSign = (i < 0) || (flags & std::ios_base::showpos);
    if(hasSign)
        ++len;

    bool hasPoint = (precision > 0) || (flags & std::ios_base::showpoint);
    if(hasPoint)
        len++;
    
    if(rightAdjust) 
        while(len++ < width)
            *it++ = fill;

    if(hasSign)
        *it++ = (i < 0) ? '-' : '+';

    if (internalAdjust) 
        while(len++ < width)
            *it++ = fill;

    i = (i < 0) ? -i : i;   
    std::streamsize n = 0;

    if(scientific) 
    {
        *it++ = '0' + i;

        if(hasPoint)
            *it++ = '.'; 
    }
    else if(e >= 0) // fixed and default
    {
        *it++ = '0' + i;
        for(; n < e; ++n)
            *it++ = (n < fractSize) ? fract[n] : CharT('0');

        if(hasPoint)
            *it++ = '.';
    }
    else
    {
        *it++ = '0';
        
        if(hasPoint)
            *it++ = '.';

        for( ;n > ++e && precision > 0; --precision)
            *it++ = '0';

        if(precision-- > 0)
            *it++ = '0' + i;
    }

    for(; precision > 0; ++n, --precision)
        *it++ = (n < fractSize) ?  fract[n] : CharT('0');   

    if(scientific) 
    {
        *it++ = (flags & std::ios_base::uppercase) ? 'E' : 'e';

        CharT sign = '+';
        if(e < 0)
        {
            e = -e;
            sign = '-';
        }
    
        *it++ = sign;

        if(e < 100)
            *it++ = '0';
        if(e < 10)
            *it++ = '0';

        it = putDecimal(it, e, std::ios_base::dec, 0, ' ');
    }

    if (leftAdjust) 
        while ( len++ < width)
            *it++ = fill;

    return it;
}


/*template <typename IterT, typename T, typename CharT>
inline IterT putFixed(IterT it, T d, 
                      std::ios_base::fmtflags flags, 
                      std::streamsize width, CharT fill,
                      std::streamsize precision = 6)
{
    bool leftAdjust = (flags & std::ios_base::left) == std::ios_base::left;
    bool internalAdjust = (flags & std::ios_base::internal) == std::ios_base::internal;
    bool rightAdjust = ! (leftAdjust || internalAdjust);

    const std::streamsize bufsize = std::numeric_limits<T>::digits10;
    CharT buf[bufsize];
    int i = 0;
    int e = 0;
    std::streamsize fractSize = Pt::formatFloat(buf, bufsize, i, e, d);

    // show only significant digits
    //precision = 1;
    //if(e < fractSize)
    //    precision = fractSize - e;
        
    std::streamsize len = 0;
    bool hasSign = (i < 0) || flags & std::ios_base::showpos;
    if(hasSign)
        ++len;

    len += precision + 1;
	
	if(e > 0)
	    len += e;

    bool hasPoint = (precision > 0) || (flags & std::ios_base::showpoint);
	if(hasPoint)
	    len++;

    if(rightAdjust) 
        while(len++ < width)
            *it++ = fill;

    if(hasSign)
        *it++ = (i < 0) ? '-' : '+';

    if (internalAdjust) 
        while(len++ < width)
            *it++ = fill;

    i = (i < 0) ? -i : i;   
    std::streamsize n = 0;

    if(e >= 0)
    {
        *it++ = '0' + i;
        for(; n < e; ++n)
            *it++ = (n < fractSize) ? buf[n] : '0';

        if(hasPoint)
            *it++ = '.';
    }
    else
    {
        *it++ = '0';
        
        if(hasPoint)
            *it++ = '.';

        for( ;n > ++e && precision > 0; --precision)
            *it++ = '0';

        if(precision-- > 0)
            *it++ = '0' + i;
    }

    for(; precision > 0; ++n, --precision)
        *it++ = (n < fractSize) ?  buf[n] : '0';

    if (leftAdjust) 
        while ( len++ < width)
            *it++ = fill;

    return it;
}*/

/*template <typename IterT, typename T, typename CharT>
inline IterT putScientific(IterT it, T d, 
                           std::ios_base::fmtflags flags, 
                           std::streamsize width, CharT fill,
                           std::streamsize precision = 6)
{
    bool leftAdjust = (flags & std::ios_base::left) == std::ios_base::left;
    bool internalAdjust = (flags & std::ios_base::internal) == std::ios_base::internal;
    bool rightAdjust = ! (leftAdjust || internalAdjust);

    const std::streamsize bufsize = std::numeric_limits<T>::digits10;
    CharT fract[bufsize];
    int i = 0;
    int e = 0;
    std::streamsize fractSize = Pt::formatFloat(fract, bufsize, i, e, d);

    std::streamsize len = precision + 6; // fraction digits, intpart, 3 exp digits, signed e/E
    
    bool hasSign = (i < 0) || (flags & std::ios_base::showpos);
    if(hasSign)
        ++len;

    bool hasPoint = (precision > 0) || (flags & std::ios_base::showpoint);
	if(hasPoint)
	    len++;

    if(rightAdjust) 
        while(len++ < width)
            *it++ = fill;

    if(hasSign)
        *it++ = (i < 0) ? '-' : '+';

    if (internalAdjust) 
        while(len++ < width)
            *it++ = fill;

    i = (i < 0) ? -i : i;   
    *it++ = '0' + i;

    if(hasPoint)
        *it++ = '.'; 
        
    for( std::streamsize n = 0; n < precision; ++n)
        *it++ = (n < fractSize) ? fract[n] : '0';

    *it++ = (flags & std::ios_base::uppercase) ? 'E' : 'e';

    bool negExp = e < 0;
    if(negExp)
        e = -e;

    *it++ = (negExp) ? '-' : '+';

    if(e < 100)
        *it++ = '0';
    if(e < 10)
        *it++ = '0';

    it = putDecimal(it, e, std::ios_base::dec, 0, ' ');

    if(leftAdjust) 
        while(len++ < width)
            *it++ = fill;

    return it;
}*/


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

} // namespace Pt

#endif
