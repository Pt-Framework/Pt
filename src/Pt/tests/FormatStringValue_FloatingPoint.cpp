/* Copyright (C) 2020-2020 Aloysius Indrayanto
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
 *
 * This code is based on the fmt_fp() function from musl libc:
 *
 *     ----------------------------------------------------------------------
 *     http://git.musl-libc.org/cgit/musl/plain/src/stdio/vfprintf.c
 *     ----------------------------------------------------------------------
 *     musl as a whole is licensed under the following standard MIT license:
 *
 *     Copyright © 2005-2020 Rich Felker, et al.
 *
 *     Permission is hereby granted, free of charge, to any person obtaining
 *     a copy of this software and associated documentation files (the
 *     "Software"), to deal in the Software without restriction, including
 *     without limitation the rights to use, copy, modify, merge, publish,
 *     distribute, sublicense, and/or sell copies of the Software, and to
 *     permit persons to whom the Software is furnished to do so, subject to
 *     the following conditions:
 *
 *     The above copyright notice and this permission notice shall be
 *     included in all copies or substantial portions of the Software.
 *
 *     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *     EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *     IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *     CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *     TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *     SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *     ----------------------------------------------------------------------
 */

#include <cfloat>
#include <cmath>

#include "FormatStringValue.h"


namespace Pt {


//
// Check if the compiler defines the size of "long double" correctly
//
#if LDBL_MANT_DIG == 53
typedef char check_if_the_compiler_defines_long_double_incorrectly[ 9 - sizeof(long double) ];
#endif


//
// Utilities
//
#define MAX(A, B) ( (A) > (B) ? (A) : (B) )
#define MIN(A, B) ( (A) < (B) ? (A) : (B) )


static inline void puts(Pt::String& dst, const char* src, size_t len)
{
    const size_t olen = dst.length();

    dst.resize(olen + len);

          Pt::Char* it    = &dst[olen];
    const Pt::Char* itEnd = it + len;

    while(it != itEnd) *it++ = *src++;
}


static inline void put0(Pt::String& dst, size_t len)
{
    static const char zeroes[] = { '0', '0', '0', '0', '0', '0', '0', '0',
                                   '0', '0', '0', '0', '0', '0', '0', '0'
                                 } ;

    for(; len >= sizeof(zeroes); len -= sizeof(zeroes))
        puts(dst, zeroes, sizeof(zeroes));

    puts(dst, zeroes, len);
}


static inline char* formatUnsigned(char* dst, Pt::uint32_t val)
{
    for(; val > ULLONG_MAX; val /= 10)
        *--dst = '0' + (val % 10);

    for(Pt::uint32_t rem = val; rem; rem /= 10)
        *--dst = '0' + (rem % 10);

    return dst;
}


//
// Print positive floating-point number into string
//
bool FormatStringValue::printPositiveFloatingPoint(Pt::String& dst, long double val, size_t precision, bool altForm, char type)
{
    // TODO: Optimize!

    // Clear the destination buffer
    dst.clear();

    // Check for uppercase mode
    bool uppercase = false;
         if(type == 'A') { type = 'a'; uppercase = true; }
    else if(type == 'E') { type = 'e'; uppercase = true; }
    else if(type == 'F') { type = 'f'; uppercase = true; }
    else if(type == 'G') { type = 'g'; uppercase = true; }

    // Select the digits
    const char* XDIGITS = selectXDigits(uppercase);

    // Preparation
    Pt::uint32_t  big[   ( ( LDBL_MANT_DIG + 28                    ) / 29 + 1 ) // Mantissa expansion
                       + ( ( LDBL_MAX_EXP + LDBL_MANT_DIG + 28 + 8 ) / 9      ) // Exponent expansion
                     ];
    Pt::uint32_t* a;
    Pt::uint32_t* d;
    Pt::uint32_t* r;
    Pt::uint32_t* z;

    char        buf[9 + LDBL_MANT_DIG / 4];
    char*       s;

    char        ebuf0[ 3 * sizeof(Pt::int32_t) ];
    char*       ebuf = &ebuf0[ 3 * sizeof(Pt::int32_t) ];
    char*       estr;
    Pt::int32_t e2 = 0;
    Pt::int32_t e;

    Pt::int32_t l;

    Pt::int32_t pl = 0;

    // Decompose into a normalized fraction and an integral power of two
    val = frexpl(val, &e2) * 2;
    if(val) e2--;

    // Hexadecimal floating-point
    if(type == 'a') {
        // Perform rounding as needed
        long double round = 8.0;
        int         re;
        if(precision < 0 || precision >= LDBL_MANT_DIG / 4 - 1) re = 0;
        else                                                    re = LDBL_MANT_DIG / 4 - 1 -precision;
        if(re) {
            round *= ( 1 << (LDBL_MANT_DIG % 4) );
            while(re--) round *= 16;
            val += round;
            val -= round;
        }
        // Process the exponent
        estr = formatUnsigned( ebuf, (e2 < 0) ? -e2 : e2 );
        if(estr == ebuf) *--estr = '0';
        *--estr = (e2<0 ? '-' : '+');
        *--estr = uppercase ? 'P' : 'p';
        // Process the mantissa
        /* ### NOTE ###
         *
         * for number 1.18973149535723176502E+4932
         *
         *     sprintf() produces F.FFFFFFFFFFFFFFF00000P+16380L
         *     this code produces 1.FFFFFFFFFFFFFFFE0000P+16383L
         *
         * Both are acctually correct and really represent the same number.
         */
        s = buf;
        do {
            const Pt::int32_t x = val;
            *s++ = XDIGITS[x];
            val = 16 * (val - x);
            if( s - buf == 1 && ( val || precision > 0 || altForm ) ) *s++ = '.';
        } while(val);
        *s = 0;
        //printf("### %s\n", buf);
        // Store the result
        pl += 2;
        if( (ssize_t) precision > (INT_MAX - 2 - (ebuf - estr) - pl) ) return false;
        if( precision && (s - buf - 2 < (ssize_t) precision) ) l = (precision + 2) + (ebuf - estr);
        else                                                   l = (s - buf) + (ebuf - estr);
        puts(dst, buf, s - buf);
        put0(dst, l - (ebuf - estr) - (s - buf));
        puts(dst, estr, ebuf - estr);
        // Done
        return true;
    }

    // Use the default precision as needed
    if(precision < 0) precision = DEFAULT_PRECISION;

    // Perform some adjustment
    if(val) {
        val *= 0x1p28;
        e2  -= 28;
    }

    if(e2 < 0)  a = r = z = big;
    else        a = r = z = big + sizeof(big) / sizeof(*big) - LDBL_MANT_DIG - 1;

    do {
        *z  = val;
        val = 1000000000 * (val - *z++);
    } while(val);

    while(e2 > 0) {
              Pt::uint32_t carry = 0;
        const Pt::int32_t  sh    = MIN(29, e2);
        for(d = z - 1; d >= a; --d) {
            const Pt::uint64_t x = ( (Pt::uint64_t) *d << sh ) + carry;
            *d    = x % 1000000000;
            carry = x / 1000000000;
        }
        if(carry) *--a = carry;
        while(z > a && !z[-1]) --z;
        e2 -= sh;
    }

    while(e2 < 0) {
              Pt::uint32_t* b;
              Pt::uint32_t  carry = 0;
        const Pt::int32_t   sh    = MIN(9, -e2);
        const Pt::int32_t   need  = 1 + ( precision + LDBL_MANT_DIG / 3U + 8 ) / 9;
        for(d = a; d < z; ++d) {
            const Pt::uint32_t rm = *d & ( (1 << sh) - 1 );
            *d    = (*d >> sh) + carry;
            carry = (1000000000 >> sh) * rm;
        }
        if(!*a) ++a;
        if(carry) *z++ = carry;
        // Avoid slow computation past requested precision
        b = (type == 'f') ? r : a;
        if(z - b > need) z = b + need;
        e2 += sh;
    }

    if(a < z) {
        e = 9 * (r - a);
        for(Pt::uint32_t i = 10; *a >= i; i *= 10) ++e;
    }
    else {
        e = 0;
    }

    // Perform rounding: j is precision after the radix (possibly negative)
    Pt::int32_t j = precision - (type != 'f') * e - (type == 'g' && precision);

    if( j < 9 * (z - r - 1) ) {
        // Avoid C's broken division of negative numbers
        d = r + 1 + ( ( j + 9 * LDBL_MAX_EXP ) / 9 - LDBL_MAX_EXP );
        j += (9 * LDBL_MAX_EXP);
        j %= 9;
        Pt::uint32_t mf = 10;
        ++j;
        for(; j < 9; ++j) mf *=10;
        Pt::uint32_t x = *d % mf;
        // Are there any significant digits past j?
        if( x || d + 1 != z ) {
            long double round = 2 / LDBL_EPSILON;
            if( ( *d / mf & 1 ) || ( mf == 1000000000 && d > a && ( d[-1] & 1 ) ) ) round += 2;
            long double small;
                 if( x < mf / 2               ) small = 0x0.8p0;
            else if( x == mf / 2 && d + 1 == z) small = 0x1.0p0;
            else                                small = 0x1.8p0;
            *d -= x;
            // Decide whether to round by probing (round + small)
            if( round + small != round ) {
                *d = *d + mf;
                while(*d > 999999999) {
                    *d-- = 0;
                    if(d < a) *--a = 0;
                    ++(*d);
                }
                e = 9 * (r - a);
                for(Pt::uint32_t i = 10; *a >= i; i *= 10) ++e;
            }
        }
        if(z > d + 1) z = d + 1;
    }
    for(; z > a && !z[-1]; --z);

    // Choose the shortest one between 'f' and 'e'
    if(type == 'g') {
        if(!precision) ++precision;
        if((ssize_t) precision > e && e >= -4) {
            // Select 'f'
            type--;
            precision -= (e + 1);
        }
        else {
            // Select 'e'
            type -= 2;
            precision--;
        }
        // Handle non alternate form
        if( !altForm ) {
            // Count trailing zeros in last place
            if(z > a && z[-1]) {
                j = 0;
                for(Pt::uint32_t i = 10; z[-1] %i == 0; i *= 10) ++j;
            }
            else {
                j = 9;
            }
            if(type == 'f') precision = MIN( precision, MAX( 0, 9 * (z - r - 1)     - j ) );
            else            precision = MIN( precision, MAX( 0, 9 * (z - r - 1) + e - j ) );
        }
    }

    if( (ssize_t) precision > INT_MAX - 1 - (precision || altForm ) ) return false;

    l = 1 + precision + (precision || altForm );

    if(type == 'f') {
        if(e > INT_MAX - l) return false;
        if(e > 0) l += e;
    }
    else {
        estr = formatUnsigned( ebuf, (e < 0) ? -e : e );
        while(ebuf - estr < 2) *--estr = '0';
        *--estr = ( (e < 0) ? '-' : '+' );
        *--estr = uppercase ? 'E' : 'e';
        if(ebuf - estr > INT_MAX - l) return false;
        l += (ebuf - estr);
    }

    if(l > INT_MAX - pl) return false;

    if(type == 'f') {
        if(a > r) a = r;
        for(d = a; d <= r; ++d) {
            char* s = formatUnsigned(buf + 9, *d);
            if(d != a) {
                while(s > buf) *--s = '0';
            }
            else if(s == buf + 9) {
                *--s = '0';
            }
            puts(dst, s, buf + 9 - s);
        }
        if(precision || altForm ) puts(dst, ".", 1);
        for(; d < z && precision > 0; ++d, precision -= 9) {
            char* s = formatUnsigned(buf + 9, *d);
            while(s > buf) *--s = '0';
            puts(dst, s, MIN(9, precision));
        }
    }
    else {
        if(z <= a) z = a + 1;
        for(d = a; d < z && precision >= 0; ++d) {
            char* s = formatUnsigned(buf + 9, *d);
            if(s == buf+9) *--s = '0';
            if(d != a) {
                while(s > buf) *--s = '0';
            }
            else {
                puts(dst, s++, 1);
                if(precision > 0 || altForm) puts(dst, ".", 1);
            }
            puts(dst, s, MIN(buf + 9 - s, (ssize_t) precision));
            precision -= (buf + 9 - s);
        }
        puts(dst, estr, ebuf-estr);
    }

    // Done
    return true;
}


} // namespace
