/*
 * Copyright (C) 2004-2013 by Marc Boris Duerner
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
#include <Pt/ConversionError.h>
#include <Pt/TypeTraits.h>
#include <string>
#include <limits>
#include <iterator>
#include <cctype>
#include <cmath>
#include <cassert>

namespace Pt {

template <bool signedX, bool signedY>
struct LessThanMin
{
    template <class X, class Y>
    static bool check(X x, Y y_min)
    { return x < y_min; }
};


template <>
struct LessThanMin<false, true>
{
    template <class X, class Y>
    static bool check(X, Y)
    { return false; }
};


template <>
struct LessThanMin<true, false>
{
    template <class X, class Y>
    static bool check(X x, Y)
    { return x < 0; }
};


template <bool signedX, bool signedY>
struct GreaterThanMax
{
    // both signed or both unsigned
    template <class X, class Y>
    static bool check(X x, Y ymax)
    { return x > ymax; }
};


template <>
struct GreaterThanMax<false, true>
{
    // x is unsigned, y is signed
    template <class X, class Y>
    static bool check(X x, Y ymax)
    { 
        return x > static_cast<X>(ymax);
    }
};


template <>
struct GreaterThanMax<true, false>
{
    // x is signed, y is unsigned
    template <class X, class Y>
    static bool check(X x, Y ymax)
    { 
        return x > 0 && static_cast<Y>(x) > ymax;
    }
};


/** @brief Checked numeric conversion.

    @ingroup CoreTypes
*/
template<typename R, typename T>
inline R narrow(T from)
{
    typedef std::numeric_limits<T> SourceTraits;
    typedef std::numeric_limits<R> ResultTraits;

    const bool fromSigned = SourceTraits::is_signed;
    const bool toSigned = ResultTraits::is_signed;

    if( LessThanMin<fromSigned, toSigned>::check( from, ResultTraits::min() )
     || GreaterThanMax<fromSigned, toSigned>::check( from, ResultTraits::max() ) )
    {
        throw ConversionError("numeric conversion failed");
    }
    
    return static_cast<R>(from);
}


/** @brief Formats an integer in a given format.

    @ingroup CoreTypes
*/
template <typename OutIterT, typename T, typename FormatT>
inline OutIterT formatInt(OutIterT it, T i, const FormatT& fmt);

/** @brief Formats an integer in a decimal format.

    @ingroup CoreTypes
*/
template <typename OutIterT, typename T>
inline OutIterT formatInt(OutIterT it, T i);


/** @brief Formats a floating point value in a given format.

    @ingroup CoreTypes
*/
template <typename OutIterT, typename T, typename FormatT>
OutIterT formatFloat(OutIterT it, T d, const FormatT& fmt, int precision);

/** @brief Formats a floating point value in default format.

    @ingroup CoreTypes
*/
template <typename OutIterT, typename T>
OutIterT formatFloat(OutIterT it, T d);


/** @brief Parses an integer value in a given format.

    @ingroup CoreTypes
*/
template <typename InIterT, typename T, typename FormatT>
InIterT parseInt(InIterT it, InIterT end, T& n, const FormatT& fmt, bool& ok);

/** @brief Parses an integer value in a given format.

    @ingroup CoreTypes
*/
template <typename InIterT, typename T, typename FormatT>
InIterT parseInt(InIterT it, InIterT end, T& n, const FormatT& fmt)
{
    bool ok = false;

    InIterT r = parseInt(it, end, n, fmt, ok);
    if( ! ok )
        throw ConversionError("conversion failed");
    
    return r;
}

/** @brief Parses an integer value in decimal format.

    @ingroup CoreTypes
*/
template <typename InIterT, typename T>
InIterT parseInt(InIterT it, InIterT end, T& n, bool& ok);

/** @brief Parses an integer value in decimal format.

    @ingroup CoreTypes
*/
template <typename InIterT, typename T>
InIterT parseInt(InIterT it, InIterT end, T& n)
{
    bool ok = false;

    InIterT r = parseInt(it, end, n, ok);
    if( ! ok )
        throw ConversionError("conversion failed");
    
    return r;
}


/** @brief Parses a floating point value in a given format.

    @ingroup CoreTypes
*/
template <typename InIterT, typename T, typename FormatT>
InIterT parseFloat(InIterT it, InIterT end, T& n, const FormatT& fmt, bool& ok);

/** @brief Parses a floating point value in a given format.

    @ingroup CoreTypes
*/
template <typename InIterT, typename T, typename FormatT>
InIterT parseFloat(InIterT it, InIterT end, T& n, const FormatT& fmt)
{
    bool ok = false;

    InIterT r = parseFloat(it, end, n, fmt, ok);
    if( ! ok )
        throw ConversionError("conversion failed");
    
    return r;
}

/** @brief Parses a floating point value.

    @ingroup CoreTypes
*/
template <typename InIterT, typename T>
InIterT parseFloat(InIterT it, InIterT end, T& n, bool& ok);

/** @brief Parses a floating point value.

    @ingroup CoreTypes
*/
template <typename InIterT, typename T>
InIterT parseFloat(InIterT it, InIterT end, T& n)
{
    bool ok = false;

    InIterT r = parseFloat(it, end, n, ok);
    if( ! ok )
        throw ConversionError("conversion failed");
    
    return r;
}


template <typename CharType>
struct NumberFormat
{
    typedef CharType CharT;

    static bool isSpace(CharT ch)
    { return (ch == 0x20) || (ch > 0x08 && ch < 0x0e); }

    static CharT plus()
    { return '+'; }

    static CharT minus()
    { return '-'; }
};


template <typename CharType>
struct DecimalFormat : public NumberFormat<CharType>
{
    typedef CharType CharT;

    static const unsigned base = 10;
    
    /** @brief Converts a digit to a character.
    */
    static CharT toChar(unsigned char n)
    {
        return '0' + n; 
    }

    /** @brief Returns true if character is a digit.        
    */
    static bool isDigit(CharT ch)
    {
        typename std::char_traits<CharT>::int_type cc = std::char_traits<CharT>::to_int_type(ch);
        return cc > 47 && cc < 58;
    }
    
    /** @brief Converts a character to a digit.
    */
    static unsigned toDigit(CharT ch)
    {
        typename std::char_traits<CharT>::int_type cc = std::char_traits<CharT>::to_int_type(ch);
        return static_cast<unsigned>(cc - 48);
    }
};


template <typename CharType>
struct OctalFormat : public NumberFormat<CharType>
{
    typedef CharType CharT;

    static const unsigned base = 8;

    /** @brief Converts a digit to a character.
    */
    static CharT toChar(unsigned char n)
    {
        return '0' + n; 
    }

    /** @brief Returns true if character is a digit.        
    */
    static bool isDigit(CharT ch)
    {
        typename std::char_traits<CharT>::int_type cc = std::char_traits<CharT>::to_int_type(ch);
        return cc > 47 && cc < 56;
    }

    /** @brief Converts a character to a digit.        
    */
    static unsigned toDigit(CharT ch)
    {
        typename std::char_traits<CharT>::int_type cc = std::char_traits<CharT>::to_int_type(ch);
        
        // let negatives overrun
        return static_cast<unsigned>(cc - 48);
    }
};


template <typename CharType>
struct HexFormat : public NumberFormat<CharType>
{
    typedef CharType CharT;

    static const unsigned base = 16;

    /** @brief Converts a digit to a character.
    */
    static CharT toChar(unsigned char n)
    {
        n &= 0x1F; // prevent overrun
        static const char* digtab = "0123456789abcdef";
        return digtab[n]; 
    }

    /** @brief Returns true if character is a digit.        
    */
    static bool isDigit(CharT ch)
    {
        typename std::char_traits<CharT>::int_type cc = std::char_traits<CharT>::to_int_type(ch);
        return (cc > 47 && cc < 59) || (cc > 64 && cc < 71) || (cc > 96 && cc < 103);
    }

    /** @brief Converts a character to a digit.
    */
    static unsigned toDigit(CharT ch)
    {
        static const unsigned char chartab[64] = 
        {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,10,11,12,13,14,15,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,10,11,12,13,14,15,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        };

        typename std::char_traits<CharT>::int_type cc = std::char_traits<CharT>::to_int_type(ch);
        
        unsigned idx = static_cast<unsigned>(cc - 48);
        return chartab[ idx ];
    }
};


template <typename CharType>
struct BinaryFormat : public NumberFormat<CharType>
{
    typedef CharType CharT;

    static const unsigned base = 2;

    /** @brief Converts a digit to a character.
    */
    static CharT toChar(unsigned char n)
    {
        return '0' + n; 
    }
    
    /** @brief Returns true if character is a digit.        
    */
    static bool isDigit(CharT ch)
    {
        typename std::char_traits<CharT>::int_type cc = std::char_traits<CharT>::to_int_type(ch);
        return cc > 47 && cc < 50;
    }

    /** @brief Converts a character to a digit.        
    */
    static unsigned toDigit(CharT ch)
    {
        typename std::char_traits<CharT>::int_type cc = std::char_traits<CharT>::to_int_type(ch);
        return static_cast<unsigned>(cc - 48);
    }
};


template <typename CharType>
struct FloatFormat : public DecimalFormat<CharType>
{
    typedef CharType CharT;

    static CharT point()
    { return '.'; }

    static CharT e()
    { return 'e'; }

    static bool isE(CharT ch)
    { return ch == 'E' || ch == 'e'; }

    static bool isNan(CharT ch, unsigned pos, bool& isNan)
    {
        switch(pos)
        {
            case 0: return ch == 'n' || ch == 'N';
            case 1: return ch == 'a' || ch == 'A';
            case 2: isNan = ch == 'n' || ch == 'N'; return isNan;
        }

        return (ch > 46 && ch < 58) || (ch > 64 && ch < 91) || (ch > 96 && ch < 123);
    }

    static bool isInfinity(CharT ch, unsigned pos, bool& isInf)
    {
        switch(pos)
        {
            case 0: return ch == 'i' || ch == 'I';
            case 1: return ch == 'n' || ch == 'N';
            case 2: isInf = ch == 'f' || ch == 'F'; return isInf;
            case 3: return ch == 'i' || ch == 'I';
            case 4: return ch == 'n' || ch == 'N';
            case 5: return ch == 'i' || ch == 'I';
            case 6: return ch == 't' || ch == 'T';
            case 7: isInf = ch == 'y' || ch == 'Y'; return isInf;
        }

        return false;
    }

    static const CharT* nan()
    { 
        static const CharT nanstr[] = { 'n', 'a', 'n', 0 };
        return nanstr; 
    }

    static const CharT* infinity()
    { 
        static const CharT nanstr[] = { 'i', 'n', 'f', 0 };
        return nanstr; 
    }
};


inline unsigned char getAbs(char i, bool& isNeg)
{
    isNeg = i < 0;
    unsigned char u = isNeg ? -i : static_cast<unsigned char>(i);
    return u;
}


inline unsigned char getAbs(unsigned char i, bool& isNeg)
{
    isNeg = false;
    return i;
}


inline unsigned short getAbs(short i, bool& isNeg)
{
    isNeg = i < 0;
    unsigned short u = isNeg ? -i : static_cast<unsigned short>(i);
    return u;
}


inline unsigned short getAbs(unsigned short i, bool& isNeg)
{
    isNeg = false;
    return i;
}


inline unsigned int getAbs(int i, bool& isNeg)
{
    isNeg = i < 0;
    unsigned int u = isNeg ? -i : static_cast<unsigned int>(i);
    return u;
}


inline unsigned int getAbs(unsigned int i, bool& isNeg)
{
    isNeg = false;
    return i;
}


inline unsigned long getAbs(long i, bool& isNeg)
{
    isNeg = i < 0;
    unsigned long u = isNeg ? -i : static_cast<unsigned long>(i);
    return u;
}


inline unsigned long getAbs(unsigned long i, bool& isNeg)
{
    isNeg = false;
    return i;
}


inline unsigned long long getAbs(long long i, bool& isNeg)
{
    isNeg = i < 0;
    unsigned long long u = isNeg ? -i : static_cast<unsigned long long>(i);
    return u;
}


inline unsigned long long getAbs(unsigned long long i, bool& isNeg)
{
    isNeg = false;
    return i;
}

/** @brief Formats an integer in a given format.

    @ingroup CoreTypes
 */
template <typename CharT, typename T, typename FormatT>
inline CharT* formatInt(CharT* buf, std::size_t buflen, T si, const FormatT& fmt)
{
    typedef typename IntTraits<T>::Unsigned UnsignedInt;

    CharT* end = buf + buflen;
    CharT* cur = end;

    const unsigned base = fmt.base;

    bool isNeg = false;
    UnsignedInt u = getAbs(si, isNeg);

    do
    {
        T lsd = u % base;
        u /= base;
        --cur;
        *cur = fmt.toChar( unsigned(lsd) );
    } 
    while(u != 0 && cur != buf);
    
    if(cur == buf)
        return buf;
    
    if(isNeg)
    {
        --cur;
        *cur = fmt.minus();
    }

    return cur;
}


template <typename CharT, typename T>
inline CharT* formatInt(CharT* buf, std::size_t buflen, T i, const BinaryFormat<CharT>& fmt)
{
    CharT* end = buf + buflen;
    CharT* cur = end;
    T mask = 1;
    
    do
    {
        --cur;
        *cur = fmt.toChar( unsigned(i & mask));
        i = i >> 1;
    } 
    while(i != 0 && cur != buf);
    
    if(cur == buf)
        return buf;
    
    return cur;
}
   

template <typename OutIterT, typename T, typename FormatT>
inline OutIterT formatInt(OutIterT it, T i, const FormatT& fmt)
{
    // large enough even for binary and a sign
    const std::size_t buflen = (sizeof(T) * 8) + 1;
    typename FormatT::CharT buf[buflen];
    typename FormatT::CharT* p = Pt::formatInt(buf, buflen, i, fmt);

    typename FormatT::CharT* end = buf + buflen;
    for(; p != end; ++p)
        *it++ = *p;

    return it;
}


template <typename OutIterT, typename T>
inline OutIterT formatInt(OutIterT it, T i)
{
    DecimalFormat<char> fmt;
    return formatInt(it, i, fmt);
}


template <typename CharT, typename T, typename FormatT>
inline int formatFloat(CharT* fraction, int fractSize, int& intpart, int& exp, T n,
                       const FormatT& fmt, int precision, bool fixed)
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
    
    exp = static_cast<int>( std::floor( std::log10(n) ) );
    
    // precision for fixed notation is the maximum number of digits after the
    // decimal point, otherwise the maximum number of significant digits
    if(fixed)
        precision += exp + 1;

    if(precision > fractSize)
        precision = fractSize;

    // intpart is first significant digit
    --precision;

    // move remaining significant digits before dot and 
    // add 0.5 to round last signicifant digit
    n *= std::pow(T(10.0), precision - exp);
    n += 0.5;

    int places = 0;
    bool trailZero = true;
    for(int d = precision - 1; d >= 0; --d)
    {
        T p = n / T(10.0);
        T q = std::floor(p) * T(10.0);
        int digit = static_cast<int>( std::floor(n - q) );
        n = p;

        // ignore trailing zeros
        trailZero = trailZero && (digit == 0);

        if( trailZero )
            continue;

        CharT c = fmt.toChar(digit);
        assert(d < fractSize);
        fraction[d] = c;

        ++places;
    }

    intpart = static_cast<int>( std::floor(n) );
    if(neg)
        intpart = -intpart;

    return places;
}


template <typename OutIterT, typename T, typename FormatT>
inline OutIterT formatFloat(OutIterT it, T d, const FormatT& fmt, int precision)
{
    typedef typename FormatT::CharT CharT;
    CharT zero = fmt.toChar(0);

    // 1. Test for not-a-number with d != d
    if( d != d ) 
    {
        for(const CharT* nanstr = fmt.nan(); *nanstr != 0; ++nanstr)
        {
            *it = *nanstr;
            ++it;
        }

        return it;
    }

    // 2. check sign
    if(d < 0.0)
    {
        *it = fmt.minus();
        ++it;
    }

    T num = std::fabs(d);

    // 3. Test for infinity
    if( num == std::numeric_limits<T>::infinity() ) 
    {
        for(const CharT* infstr = fmt.infinity(); *infstr != 0; ++infstr)
        {
            *it = *infstr;
            ++it;
        }

        return it;
    }
    
    const int bufsize = std::numeric_limits<T>::digits10;
    CharT fract[bufsize];
    int i = 0;
    int e = 0;
    int fractSize = Pt::formatFloat(fract, bufsize, i, e, num, fmt, precision, false);

    int n = 0;
    if(e >= 0)
    {
        *it++ = fmt.toChar(i);

        for(; n < e; ++n)
            *it++ = (n < fractSize) ? fract[n] : zero;

        *it++ = fmt.point();
        
        // show at least one digit after decimal point
        *it++ = (n < fractSize) ? fract[n] : zero;
        ++n;
    }
    else // e < 0
    {
        *it++ = zero;
        *it++ = fmt.point();

        while(++e < 0)
            *it++ = zero;

        *it++ = fmt.toChar(i);
    }

    for(; n < fractSize; ++n)
        *it++ = (n < fractSize) ?  fract[n] : zero;

    return it;
}


template <typename OutIterT, typename T>
inline OutIterT formatFloat(OutIterT it, T d)
{
    const int precision = std::numeric_limits<T>::digits10;
    FloatFormat<char> fmt;
    return formatFloat(it, d, fmt, precision);
}


template <typename InIterT, typename FormatT>
InIterT getWhitespace(InIterT it, InIterT end, const FormatT& fmt)
{   
    while( it != end && fmt.isSpace(*it) )
        ++it;

    return it;
}


template <typename InIterT, typename FormatT>
InIterT getSign(InIterT it, InIterT end, bool& pos, const FormatT& fmt)
{
    pos = true;

    if( *it == fmt.minus() )
    {
        pos = false;
        ++it;
    }
    else if( *it == fmt.plus() )
    {
        ++it;
    }

    return it;
}


template <typename InIterT, typename T, typename FormatT>
InIterT parseInt(InIterT it, InIterT end, T& n, const FormatT& fmt, bool& ok)
{
    typedef typename IntTraits<T>::Unsigned UnsignedInt;
    typedef typename IntTraits<T>::Signed SignedInt;

    n = 0;
    ok = false;
    UnsignedInt max = static_cast<UnsignedInt>( std::numeric_limits<T>::max() );

    it = getWhitespace(it, end, fmt);

    bool pos = false;
    it = getSign(it, end, pos, fmt);

    if (it == end)
        return it;

    bool isNeg = ! pos;
    if( isNeg )
    {
        // return if minus sign was parsed for unsigned type
        if( isNeg != std::numeric_limits<T>::is_signed)
            return it;

        // abs(min) is max for negative signed types
        SignedInt smin = std::numeric_limits<T>::min();
        max = static_cast<UnsignedInt>(-smin);
    }

    // parse number
    UnsignedInt u = 0;
    const UnsignedInt base = fmt.base;
    unsigned d = 0;
    while(it != end)
    {    
        if( ! fmt.isDigit(*it) )
            break;

        d = fmt.toDigit(*it);
        
        if ( u != 0u && base > (max/u) )
          return it;

        u *= base;

        if(static_cast<unsigned char>(d) > max - u)
            return it;

        u += d;
        ++it;
    }

    if( isNeg ) 
        n = static_cast<T>(u * -1);
    else
        n = static_cast<T>(u);

    ok = true;
    return it;
}


template <typename InIterT, typename T>
InIterT parseInt(InIterT it, InIterT end, T& n, bool& ok)
{
    typedef typename std::iterator_traits<InIterT>::value_type CharType;    
    return parseInt(it, end, n, DecimalFormat<CharType>(), ok );
}


template <typename InIterT, typename T, typename FormatT>
InIterT parseFloat(InIterT it, InIterT end, T& n, const FormatT& fmt, bool& ok)
{
    typedef typename FormatT::CharT CharT;
    CharT zero = fmt.toChar(0);
    n = 0.0;
    ok = false;

    it = getWhitespace(it, end, fmt);

    bool pos = false;
    it = getSign(it, end, pos, fmt);
    
    if(it == end)
        return it;

    unsigned strpos = 0;
    if( fmt.isNan(*it, strpos, ok) )
    {
        while( ++it != end && fmt.isNan(*it, ++strpos, ok) )
            ;

        if(ok)
            n = std::numeric_limits<T>::quiet_NaN();

        return it;
    }

    strpos = 0;
    if( fmt.isInfinity(*it, strpos, ok) )
    {
        while( ++it != end && fmt.isInfinity(*it, ++strpos, ok) )
            ;

        if(ok)
        {
            n = pos ? std::numeric_limits<T>::infinity() 
                    : std::numeric_limits<T>::infinity() * -1;
        }

        return it;
    }

    if(it == end)
        return it;

    // integral part
    bool withFractional = false;
    for( ; it != end; ++it)
    {
        if( *it == fmt.point() || fmt.isE(*it) )
        {
            if( *it == fmt.point() )
            {
                withFractional = true;
                ++it;
            }
            break;
        }
        
        if( ! fmt.isDigit(*it) )
            return it;

        unsigned digit = fmt.toDigit(*it); 
        
        n *= 10;
        n += digit; 
    }
    
    // it is ok, if fraction is missing
    if(it == end)
    {
        if( ! pos )
            n *= -1;

        ok = true;
        return it;
    }

    T base = 10.0;
    if( withFractional)
    {
        // fractional part, ignore 0 digits after dot
        unsigned short fractDigits = 0;
        std::size_t maxDigits = std::numeric_limits<unsigned short>::max() - std::numeric_limits<T>::digits10;
        while(it != end && *it == zero)
        {
            if( fractDigits > maxDigits )
                return it;

            ++fractDigits;
            ++it;
        }
 
        // fractional part, parse like integer, skip insignificant digits
        unsigned short significants = 0;
        T fraction = 0.0;
        for( ; it != end; ++it)
        {
            if( ! fmt.isDigit(*it) )
                break;

            unsigned digit = fmt.toDigit(*it); 

            if( significants <= std::numeric_limits<T>::digits10 )
            {
                fraction *= 10;
                fraction += digit;

                ++fractDigits;
                ++significants;
            }
        }

        // fractional part, scale down
        fraction /= std::pow(base, T(fractDigits));
        n += fraction;
    }
    
    // exponent [e|E][+|-][0-9]*
    if(it != end && fmt.isE(*it) )
    {
        if(++it == end)
            return it;

        long exp = 0;
        it = parseInt(it, end, exp, fmt, ok);
        if( ! ok )
            return it;
            
        n *= std::pow(base, T(exp));
    }

    if( ! pos )
        n *= -1;

    ok = true;
    return it;
}


template <typename InIterT, typename T>
InIterT parseFloat(InIterT it, InIterT end, T& n, bool& ok)
{
    typedef typename std::iterator_traits<InIterT>::value_type CharType;
    return parseFloat( it, end, n, FloatFormat<CharType>(), ok);
}

} // namespace Pt

#endif
