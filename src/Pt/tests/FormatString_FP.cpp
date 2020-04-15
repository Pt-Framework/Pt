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
 *     http://git.musl-libc.org/cgit/musl/plain/src/stdio/vfprintf.c
 *
 *     ----------------------------------------------------------------------
 *     musl as a whole is licensed under the following standard MIT license:
 *     ----------------------------------------------------------------------
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
 * ----------------------------------------------------------------------
 *
 */

#include <Pt/String.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <errno.h>


namespace Pt {


// Check if the compiler defines the size of "long double" correctly
#if LDBL_MANT_DIG == 53
typedef char check_if_the_compiler_defines_long_double_incorrectly[ 9 - sizeof(long double) ];
#endif

// Utility macros
#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)<(b) ? (a) : (b))



/* Convenient bit representation for modifier flags, which all fall
 * within 31 codepoints of the space character. */

#define ALT_FORM   (1U<<('#'-' '))
#define ZERO_PAD   (1U<<('0'-' '))
#define LEFT_ADJ   (1U<<('-'-' '))
#define PAD_POS    (1U<<(' '-' '))
#define MARK_POS   (1U<<('+'-' '))
#define GROUPED    (1U<<('\''-' '))




static void out(FILE *f, const char *s, size_t l)
{
    //if (!(f->flags & F_ERR)) __fwritex((void *)s, l, f);
}

static void pad(FILE *f, char c, int w, int l, int fl)
{
    /*
    char pad[256];
    if (fl & (LEFT_ADJ | ZERO_PAD) || l >= w) return;
    l = w - l;
    memset(pad, c, l>sizeof pad ? sizeof pad : l);
    for (; l >= sizeof pad; l -= sizeof pad)
        out(f, pad, sizeof pad);
    out(f, pad, l);
    */
}

static char *fmt_u(uintmax_t x, char *s)
{
    unsigned long y;
    for (   ; x>ULONG_MAX; x/=10) *--s = '0' + x%10;
    for (y=x;           y; y/=10) *--s = '0' + y%10;
    return s;
}

/* Do not override this check. The floating point printing code below
 * depends on the float.h constants being right. If they are wrong, it
 * may overflow the stack. */

/*
 * w = string width, p = precision, fl = flags, t = type
 *
 * default formatting : w =  0, p = -1
 * "%20.5g            : w = 20, p =  5, fl = 0, t = 'g'
 */
static int fmt_fp(FILE *f, long double y, int w, int p, int fl, int t)
{
    // Check for uppercase mode
    bool uppercase = false;
         if(t == 'A') { t = 'a'; uppercase = true; }
    else if(t == 'E') { t = 'e'; uppercase = true; }
    else if(t == 'F') { t = 'f'; uppercase = true; }
    else if(t == 'G') { t = 'g'; uppercase = true; }

    // Select the digits
    static const char* L_DIGITS = "0123456789abcdef";
    static const char* U_DIGITS = "0123456789ABCDEF";
           const char* X_DIGITS = uppercase ? U_DIGITS : L_DIGITS;



    uint32_t big[(LDBL_MANT_DIG+28)/29 + 1          // mantissa expansion
        + (LDBL_MAX_EXP+LDBL_MANT_DIG+28+8)/9]; // exponent expansion
    uint32_t *a, *d, *r, *z;
    int e2=0, e, i, j, l;
    char buf[9+LDBL_MANT_DIG/4], *s;
    const char *prefix="-0X+0X 0X-0x+0x 0x";
    int pl;
    char ebuf0[3*sizeof(int)], *ebuf=&ebuf0[3*sizeof(int)], *estr;

    pl=1;
    if (signbit(y)) {
        y=-y;
    } else if (fl & MARK_POS) {
        prefix+=3;
    } else if (fl & PAD_POS) {
        prefix+=6;
    } else prefix++, pl=0;

    /*
    if (!isfinite(y)) {
        char *s = (t&32)?"inf":"INF";
        if (y!=y) s=(t&32)?"nan":"NAN";
        pad(f, ' ', w, 3+pl, fl&~ZERO_PAD);
        out(f, prefix, pl);
        out(f, s, 3);
        pad(f, ' ', w, 3+pl, fl^LEFT_ADJ);
        return MAX(w, 3+pl);
    }
    */

    y = frexpl(y, &e2) * 2;
    if (y) e2--;

    if ((t|32)=='a') {
        long double round = 8.0;
        int re;

        if (t&32) prefix += 9;
        pl += 2;

        if (p<0 || p>=LDBL_MANT_DIG/4-1) re=0;
        else re=LDBL_MANT_DIG/4-1-p;

        if (re) {
            round *= 1<<(LDBL_MANT_DIG%4);
            while (re--) round*=16;
            if (*prefix=='-') {
                y=-y;
                y-=round;
                y+=round;
                y=-y;
            } else {
                y+=round;
                y-=round;
            }
        }

        estr=fmt_u(e2<0 ? -e2 : e2, ebuf);
        if (estr==ebuf) *--estr='0';
        *--estr = (e2<0 ? '-' : '+');
        *--estr = t+('p'-'a');

        s=buf;
        do {
            int x=y;
            *s++=X_DIGITS[x]|(t&32);
            y=16*(y-x);
            if (s-buf==1 && (y||p>0||(fl&ALT_FORM))) *s++='.';
        } while (y);

        if (p > INT_MAX-2-(ebuf-estr)-pl)
            return -1;
        if (p && s-buf-2 < p)
            l = (p+2) + (ebuf-estr);
        else
            l = (s-buf) + (ebuf-estr);

        pad(f, ' ', w, pl+l, fl);
        out(f, prefix, pl);
        pad(f, '0', w, pl+l, fl^ZERO_PAD);
        out(f, buf, s-buf);
        pad(f, '0', l-(ebuf-estr)-(s-buf), 0, 0);
        out(f, estr, ebuf-estr);
        pad(f, ' ', w, pl+l, fl^LEFT_ADJ);
        return MAX(w, pl+l);
    }
    if (p<0) p=6;

    if (y) y *= 0x1p28, e2-=28;

    if (e2<0) a=r=z=big;
    else a=r=z=big+sizeof(big)/sizeof(*big) - LDBL_MANT_DIG - 1;

    do {
        *z = y;
        y = 1000000000*(y-*z++);
    } while (y);

    while (e2>0) {
        uint32_t carry=0;
        int sh=MIN(29,e2);
        for (d=z-1; d>=a; d--) {
            uint64_t x = ((uint64_t)*d<<sh)+carry;
            *d = x % 1000000000;
            carry = x / 1000000000;
        }
        if (carry) *--a = carry;
        while (z>a && !z[-1]) z--;
        e2-=sh;
    }
    while (e2<0) {
        uint32_t carry=0, *b;
        int sh=MIN(9,-e2), need=1+(p+LDBL_MANT_DIG/3U+8)/9;
        for (d=a; d<z; d++) {
            uint32_t rm = *d & (1<<sh)-1;
            *d = (*d>>sh) + carry;
            carry = (1000000000>>sh) * rm;
        }
        if (!*a) a++;
        if (carry) *z++ = carry;
        /* Avoid (slow!) computation past requested precision */
        b = (t|32)=='f' ? r : a;
        if (z-b > need) z = b+need;
        e2+=sh;
    }

    if (a<z) for (i=10, e=9*(r-a); *a>=i; i*=10, e++);
    else e=0;

    /* Perform rounding: j is precision after the radix (possibly neg) */
    j = p - ((t|32)!='f')*e - ((t|32)=='g' && p);
    if (j < 9*(z-r-1)) {
        uint32_t x;
        /* We avoid C's broken division of negative numbers */
        d = r + 1 + ((j+9*LDBL_MAX_EXP)/9 - LDBL_MAX_EXP);
        j += 9*LDBL_MAX_EXP;
        j %= 9;
        for (i=10, j++; j<9; i*=10, j++);
        x = *d % i;
        /* Are there any significant digits past j? */
        if (x || d+1!=z) {
            long double round = 2/LDBL_EPSILON;
            long double small;
            if ((*d/i & 1) || (i==1000000000 && d>a && (d[-1]&1)))
                round += 2;
            if (x<i/2) small=0x0.8p0;
            else if (x==i/2 && d+1==z) small=0x1.0p0;
            else small=0x1.8p0;
            if (pl && *prefix=='-') round*=-1, small*=-1;
            *d -= x;
            /* Decide whether to round by probing round+small */
            if (round+small != round) {
                *d = *d + i;
                while (*d > 999999999) {
                    *d--=0;
                    if (d<a) *--a=0;
                    (*d)++;
                }
                for (i=10, e=9*(r-a); *a>=i; i*=10, e++);
            }
        }
        if (z>d+1) z=d+1;
    }
    for (; z>a && !z[-1]; z--);

    if ((t|32)=='g') {
        if (!p) p++;
        if (p>e && e>=-4) {
            t--;
            p-=e+1;
        } else {
            t-=2;
            p--;
        }
        if (!(fl&ALT_FORM)) {
            /* Count trailing zeros in last place */
            if (z>a && z[-1]) for (i=10, j=0; z[-1]%i==0; i*=10, j++);
            else j=9;
            if ((t|32)=='f')
                p = MIN(p,MAX(0,9*(z-r-1)-j));
            else
                p = MIN(p,MAX(0,9*(z-r-1)+e-j));
        }
    }
    if (p > INT_MAX-1-(p || (fl&ALT_FORM)))
        return -1;
    l = 1 + p + (p || (fl&ALT_FORM));
    if ((t|32)=='f') {
        if (e > INT_MAX-l) return -1;
        if (e>0) l+=e;
    } else {
        estr=fmt_u(e<0 ? -e : e, ebuf);
        while(ebuf-estr<2) *--estr='0';
        *--estr = (e<0 ? '-' : '+');
        *--estr = t;
        if (ebuf-estr > INT_MAX-l) return -1;
        l += ebuf-estr;
    }

    if (l > INT_MAX-pl) return -1;
    pad(f, ' ', w, pl+l, fl);
    out(f, prefix, pl);
    pad(f, '0', w, pl+l, fl^ZERO_PAD);

    if ((t|32)=='f') {
        if (a>r) a=r;
        for (d=a; d<=r; d++) {
            char *s = fmt_u(*d, buf+9);
            if (d!=a) while (s>buf) *--s='0';
            else if (s==buf+9) *--s='0';
            out(f, s, buf+9-s);
        }
        if (p || (fl&ALT_FORM)) out(f, ".", 1);
        for (; d<z && p>0; d++, p-=9) {
            char *s = fmt_u(*d, buf+9);
            while (s>buf) *--s='0';
            out(f, s, MIN(9,p));
        }
        pad(f, '0', p+9, 9, 0);
    } else {
        if (z<=a) z=a+1;
        for (d=a; d<z && p>=0; d++) {
            char *s = fmt_u(*d, buf+9);
            if (s==buf+9) *--s='0';
            if (d!=a) while (s>buf) *--s='0';
            else {
                out(f, s++, 1);
                if (p>0||(fl&ALT_FORM)) out(f, ".", 1);
            }
            out(f, s, MIN(buf+9-s, p));
            p -= buf+9-s;
        }
        pad(f, '0', p+18, 18, 0);
        out(f, estr, ebuf-estr);
    }

    pad(f, ' ', w, pl+l, fl^LEFT_ADJ);

    return MAX(w, pl+l);
}


} // namespace
