/*
Quick attempt at extraction of fmt_fp for http://stackoverflow.com/questions/7228438/convert-double-float-to-string/7231988

Based on http://git.etalabs.net/cgi-bin/gitweb.cgi?p=musl;a=blob;f=src/stdio/vfprintf.c;h=4c990e5e54fa730c755206a9c5a6b4c597e8b2a3;hb=HEAD

Contents of http://git.etalabs.net/cgi-bin/gitweb.cgi?p=musl;a=blob_plain;f=COPYRIGHT;hb=HEAD follows:

musl as a whole is licensed under the GNU LGPL version 2.1 or later.
See the file COPYING for the text of this license.

See below for the copyright status on all code included in musl:

The TRE regular expression implementation (src/regex/reg* and
src/regex/tre*) is Copyright Â© 2001-2006 Ville Laurikari and licensed
under the terms of the GNU LGPL version 2.1 or later. The included
version was heavily modified in Spring 2006 by Rich Felker in the
interests of size, simplicity, and namespace cleanliness.

Most of the math library code (src/math/*) is Copyright Â© 1993 Sun
Microsystems, Inc. Some files are Copyright Â© 2003 Steven G. Kargl and
labelled as such. All have been licensed under extremely permissive
terms which are compatible with the GNU LGPL. See the comments in the
individual files for details.

The implementation of DES for crypt (src/misc/crypt.c) is Copyright Â©
1994 David Burren. It is licensed under a BSD license compatible with
the GNU LGPL.

The smoothsort implementation (src/stdlib/qsort.c) is Copyright Â© 2011
Valentin Ochs and is licensed under an MIT-style license compatible
with the GNU LGPL.

The BSD PRNG implementation (src/prng/random.c) and XSI search API
(src/search/*.c) functions are Copyright Â© 2011 Szabolcs Nagy and
licensed under following terms: "Permission to use, copy, modify,
and/or distribute this code for any purpose with or without fee is
hereby granted. There is no warranty."

The x86_64 port was written by Nicholas J. Kain. See individual files
for their copyright status.

All files which have no copyright comments are original works
Copyright Â© 2005-2011 Rich Felker, the main author of this library.
The decision to exclude such comments is intentional, as it should be
possible to carry around the complete source code on tiny storage
media. All public header files (include/*) should be treated as Public
Domain as they intentionally contain no content which can be covered
by copyright. Some source modules may fall in this category as well.
If you believe that a file is so trivial that it should be in the
Public Domain, please contact me and, if I agree, I will explicitly
release it from copyright.

The following files are trivial, in my opinion not copyrightable in
the first place, and hereby explicitly released to the Public Domain:

All public headers: include/*
Startup files: crt/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#define __USE_POSIX
#define __USE_XOPEN
#include <limits.h>
#include <float.h>
#include <math.h>
#include <errno.h>

/* Some useful macros */

#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define CONCAT2(x,y) x ## y
#define CONCAT(x,y) CONCAT2(x,y)

/* Convenient bit representation for modifier flags, which all fall
 * within 31 codepoints of the space character. */

#define ALT_FORM   (1U<<('#'-' '))
#define ZERO_PAD   (1U<<('0'-' '))
#define LEFT_ADJ   (1U<<('-'-' '))
#define PAD_POS    (1U<<(' '-' '))
#define MARK_POS   (1U<<('+'-' '))
#define GROUPED    (1U<<('\''-' '))

#define FLAGMASK (ALT_FORM|ZERO_PAD|LEFT_ADJ|PAD_POS|MARK_POS|GROUPED)

#if UINT_MAX == ULONG_MAX
#define LONG_IS_INT
#endif

#if SIZE_MAX != ULONG_MAX || UINTMAX_MAX != ULLONG_MAX
#define ODD_TYPES
#endif

static void out(char **sp, const char *s, size_t l)
{
	//__fwritex((void *)s, l, f);
	//fwrite(s, l, 1, f);
	while (l--) {
		**sp = *s;
		(*sp)++;
		s++;
	}
}

static void pad(char **sp, char c, int w, int l, int fl)
{
	char pad[256];
	if (fl & (LEFT_ADJ | ZERO_PAD) || l >= w) return;
	l = w - l;
	memset(pad, c, l>sizeof pad ? sizeof pad : l);
	for (; l >= sizeof pad; l -= sizeof pad)
		out(sp, pad, sizeof pad);
	out(sp, pad, l);
}

static char *fmt_u(uintmax_t x, char *s)
{
	unsigned long y;
	for (   ; x>ULONG_MAX; x/=10) *--s = '0' + x%10;
	for (y=x;           y; y/=10) *--s = '0' + y%10;
	return s;
}

static int fmt_fp(char *outstr, long double y, int w, int p, int fl, int t)
{
	char* sp = outstr;
	uint32_t big[(LDBL_MAX_EXP+LDBL_MANT_DIG)/9+1];
	uint32_t *a, *d, *r, *z;
	int e2=0, e, i, j, l;
	char buf[9+LDBL_MANT_DIG/4], *s;
	const char *prefix="-0X+0X 0X-0x+0x 0x";
	int pl;
	char ebuf0[3*sizeof(int)], *ebuf=&ebuf0[3*sizeof(int)], *estr;

	pl=1;
	if (y<0 || 1/y<0) {
		y=-y;
	} else if (fl & MARK_POS) {
		prefix+=3;
	} else if (fl & PAD_POS) {
		prefix+=6;
	} else prefix++, pl=0;

	if (!isfinite(y)) {
		char *s = (t&32)?"inf":"INF";
		if (y!=y) s=(t&32)?"nan":"NAN", pl=0;
		pad(&sp, ' ', w, 3+pl, fl&~ZERO_PAD);
		out(&sp, prefix, pl);
		out(&sp, s, 3);
		pad(&sp, ' ', w, 3+pl, fl^LEFT_ADJ);
		return MAX(w, 3+pl);
	}

	y = frexpl(y, &e2) * 2;
	if (y) e2--;

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
		if (!z[-1] && z>a) z--;
		if (carry) *--a = carry;
		e2-=sh;
	}
	while (e2<0) {
		uint32_t carry=0, *z2;
		int sh=MIN(9,-e2);
		for (d=a; d<z; d++) {
			uint32_t rm = *d & (1<<sh)-1;
			*d = (*d>>sh) + carry;
			carry = (1000000000>>sh) * rm;
		}
		if (!*a) a++;
		if (carry) *z++ = carry;
		/* Avoid (slow!) computation past requested precision */
		z2 = ((t|32)=='f' ? r : a) + 2 + p/9;
		z = MIN(z, z2);
		e2+=sh;
	}

	if (a<z) for (i=10, e=9*(r-a); *a>=i; i*=10, e++);
	else e=0;

	/* Perform rounding: j is precision after the radix (possibly neg) */
	j = p - ((t|32)!='f')*e - ((t|32)=='g' && p);
	if (j < 9*(z-r-1)) {
		uint32_t x;
		/* We avoid C's broken division of negative numbers */
		d = r + 1 + (j+9*LDBL_MAX_EXP)/9 - LDBL_MAX_EXP;
		j += 9*LDBL_MAX_EXP;
		j %= 9;
		for (i=10, j++; j<9; i*=10, j++);
		x = *d % i;
		/* Are there any significant digits past j? */
		if (x || d+1!=z) {
			long double round = CONCAT(0x1p,LDBL_MANT_DIG);
			long double small;
			if (*d/i & 1) round += 2;
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
					(*d)++;
				}
				if (d<a) a=d;
				for (i=10, e=9*(r-a); *a>=i; i*=10, e++);
			}
		}
		if (z>d+1) z=d+1;
		for (; !z[-1] && z>a; z--);
	}
	
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
	l = 1 + p + (p || (fl&ALT_FORM));
	if ((t|32)=='f') {
		if (e>0) l+=e;
	} else {
		estr=fmt_u(e<0 ? -e : e, ebuf);
		while(ebuf-estr<2) *--estr='0';
		*--estr = (e<0 ? '-' : '+');
		*--estr = t;
		l += ebuf-estr;
	}

	pad(&sp, ' ', w, pl+l, fl);
	out(&sp, prefix, pl);
	pad(&sp, '0', w, pl+l, fl^ZERO_PAD);

	if ((t|32)=='f') {
		if (a>r) a=r;
		for (d=a; d<=r; d++) {
			char *s = fmt_u(*d, buf+9);
			if (d!=a) while (s>buf) *--s='0';
			else if (s==buf+9) *--s='0';
			out(&sp, s, buf+9-s);
		}
		if (p || (fl&ALT_FORM)) out(&sp, ".", 1);
		for (; d<z && p>0; d++, p-=9) {
			char *s = fmt_u(*d, buf+9);
			while (s>buf) *--s='0';
			out(&sp, s, MIN(9,p));
		}
		pad(&sp, '0', p+9, 9, 0);
	} else {
		if (z<=a) z=a+1;
		for (d=a; d<z && p>=0; d++) {
			char *s = fmt_u(*d, buf+9);
			if (s==buf+9) *--s='0';
			if (d!=a) while (s>buf) *--s='0';
			else {
				out(&sp, s++, 1);
				if (p>0||(fl&ALT_FORM)) out(&sp, ".", 1);
			}
			out(&sp, s, MIN(buf+9-s, p));
			p -= buf+9-s;
		}
		pad(&sp, '0', p+18, 18, 0);
		out(&sp, estr, ebuf-estr);
	}

	pad(&sp, ' ', w, pl+l, fl^LEFT_ADJ);
	*sp = 0;

	return 1+MAX(w, pl+l);
}

int main() {
	double test[] = { 0x1.921f9f01b866ep+1, 0x1.9eb851eb851ecp-1, 0x1.0000000000000p+57 };
	size_t i;
	for (i = 0; i < sizeof(test)/sizeof(test[0]); i++) {
		char buffer[256];
		fmt_fp(buffer, test[i], 0, -1, 0, 'f');
		printf("%s\n",buffer);
	}
	return 0;
}