/***************************************************************************
 *   Copyright (C) 2005 Aloysius Indrayanto                                *
 *   Copyright (C) 2004 Marc Boris Duerner                                 *
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
#if 1

#include <iostream>
using namespace std;

#include "Pt/Byteorder.h"
using namespace Pt;

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestCase.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"


class ByteorderTest : public Pt::Unit::TestCase
{
	public:
	ByteorderTest()
	: TestCase("ByteorderTest")
		{ }

		virtual void test()
		{
			const Pt::uint8_t  val8u  = 0x10;
			const Pt::uint16_t val16u = 0x1020;
			const Pt::uint32_t val32u = 0x10203040;
#ifdef PT_64BIT
			const Pt::uint64_t val64u = 0x1020304050607080ULL;
#endif
			const float        valf   = -754.0f;
			const double       vald   = -754.0;

			cerr << "Before swap()s:" << endl;
			cerr << hex         << (void*) val8u  << endl;
			cerr << hex << "0x" <<         val16u << endl;
			cerr << hex << "0x" <<         val32u << endl;
#ifdef PT_64BIT
			cerr << hex << "0x" <<         val64u << endl;
#endif
			cerr << valf << hex << " (0x" << *reinterpret_cast<const Pt::uint32_t*>(&valf) << ")" << endl;
			cerr << vald << hex << " (0x" << *reinterpret_cast<const Pt::uint64_t*>(&vald) << ")" << endl;

			cerr << endl;
			Pt::uint8_t  sval8u  = swap(val8u);
			Pt::uint16_t sval16u = swap(val16u);
			Pt::uint32_t sval32u = swap(val32u);
#ifdef PT_64BIT
			Pt::uint64_t sval64u = swap(val64u);
#endif
			float        svalf   = swap(valf);
			double       svald   = swap(vald);

			cerr << "After swap()s:" << endl;
			cerr << hex         << (void*) sval8u  << endl;
			cerr << hex << "0x" <<         sval16u << endl;
			cerr << hex << "0x" <<         sval32u << endl;
#ifdef PT_64BIT
			cerr << hex << "0x" <<         sval64u << endl;
#endif
			cerr << svalf << hex << " (0x" << *reinterpret_cast<const Pt::uint32_t*>(&svalf) << ")" << endl;
			cerr << svald << hex << " (0x" << *reinterpret_cast<const Pt::uint64_t*>(&svald) << ")" << endl;

			cerr << endl;
			swabUnaligned(reinterpret_cast<Pt::uint8_t*>(&sval16u), sizeof(sval16u));
			swabUnaligned(reinterpret_cast<Pt::uint8_t*>(&sval32u), sizeof(sval32u));
#ifdef PT_64BIT
			swabUnaligned(reinterpret_cast<Pt::uint8_t*>(&sval64u), sizeof(sval64u));
#endif
			swabUnaligned(reinterpret_cast<Pt::uint8_t*>(&svalf),   sizeof(svalf));
			swabUnaligned(reinterpret_cast<Pt::uint8_t*>(&svald),   sizeof(svald));

			cerr << "After swabUnaligned()s:" << endl;
			cerr << hex << "0x" << sval16u << endl;
			cerr << hex << "0x" << sval32u << endl;
#ifdef PT_64BIT
			cerr << hex << "0x" << sval64u << endl;
#endif
			cerr << svalf << hex << " (0x" << *reinterpret_cast<const Pt::uint32_t*>(&svalf) << ")" << endl;
			cerr << svald << hex << " (0x" << *reinterpret_cast<const Pt::uint64_t*>(&svald) << ")" << endl;

			PT_UNIT_ASSERT( val8u  == sval8u  );
			PT_UNIT_ASSERT( val16u == sval16u );
			PT_UNIT_ASSERT( val32u == sval32u );
#ifdef PT_64BIT
			PT_UNIT_ASSERT( val64u == sval64u );
#endif
			PT_UNIT_ASSERT( valf   == svalf   );
			PT_UNIT_ASSERT( vald   == svald   );
		}
	};

Pt::Unit::RegisterTest<ByteorderTest> register_ByteorderTest;

#else

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern uint16_t swap16(uint16_t value)
{
#if 0
	value = ( (value & 0x00FF) << 8 ) |
				  ( (value & 0xFF00) >> 8 );
  return(value);
	/*
	movzwl	8(%ebp), %edx
	movl	%edx, %eax
	sall	$8, %eax
	shrl	$8, %edx
	orl	%edx, %eax
	movzwl	%ax, %eax
	leave
	ret
	*/
#else
	union {
		uint16_t v;
		uint8_t  b[2];
	} u;

	u.v = value;

	const uint8_t b0 = u.b[0];
	const uint8_t b1 = u.b[1];
	u.b[0] = b1;
	u.b[1] = b0;

	return(u.v);
	/*
	movl	8(%ebp), %edx
	movl	%edx, %eax
	movzbl	%dh, %ecx
	movb	%cl, %al
	movb	%dl, %ah
	movzwl	%ax, %eax
	leave
	ret
	*/
#endif
}

extern uint32_t swap32(uint32_t value)
{
#if 0
	value = ( (value & 0x000000FF) << 24 ) |
				  ( (value & 0x0000FF00) <<  8 ) |
				  ( (value & 0x00FF0000) >>  8 ) |
				  ( (value & 0xFF000000) >> 24 );
	return(value);
	/*
	movl	8(%ebp), %ecx
	movl	%ecx, %eax
	sall	$24, %eax
	movl	%ecx, %edx
	andl	$65280, %edx
	sall	$8, %edx
	orl	%edx, %eax
	movl	%ecx, %edx
	andl	$16711680, %edx
	shrl	$8, %edx
	shrl	$24, %ecx
	orl	%ecx, %edx
	orl	%edx, %eax
	leave
	ret
	*/
#else
	uint8_t *w = reinterpret_cast<uint8_t*>(&value);

	const uint8_t w0 = w[0];
	const uint8_t w1 = w[1];
	const uint8_t w2 = w[2];
	const uint8_t w3 = w[3];
	w[0] = w3;
	w[1] = w2;
	w[2] = w1;
	w[3] = w0;
	return(value);
	/*
	movb	8(%ebp), %cl
	movb	9(%ebp), %dl
	movb	11(%ebp), %al
	movb	%al, 8(%ebp)
	movb	10(%ebp), %al
	movb	%al, 9(%ebp)
	movb	%dl, 10(%ebp)
	movb	%cl, 11(%ebp)
	movl	8(%ebp), %eax
	leave
	ret
	*/
#endif
}

extern uint64_t swap64(uint64_t value)
{
#if 0
	value = ( (value & 0x00000000000000FFULL) << 56 ) |
					( (value & 0x000000000000FF00ULL) << 40 ) |
					( (value & 0x0000000000FF0000ULL) << 24 ) |
					( (value & 0x00000000FF000000ULL) <<  8 ) |
					( (value & 0x000000FF00000000ULL) >>  8 ) |
					( (value & 0x0000FF0000000000ULL) >> 24 ) |
					( (value & 0x00FF000000000000ULL) >> 40 ) |
					( (value & 0xFF00000000000000ULL) >> 56 );
	return(value);
	/*
	movzbl	8(%ebp),%eax
	xorl	%edx, %edx
	movl	%eax, %edx
	movl	$0, %eax
	sall	$24, %edx
	movl	8(%ebp), %ecx
	andl	$65280, %ecx
	xorl	%ebx, %ebx
	movl	%ecx, %ebx
	movl	$0, %ecx
	sall	$8, %ebx
	orl	%ecx, %eax
	orl	%ebx, %edx
	xorl	%ecx, %ecx
	movzbl	12(%ebp),%ebx
	shrdl	$8, %ebx, %ecx
	shrl	$8, %ebx
	orl	%ecx, %eax
	orl	%ebx, %edx
	xorl	%ecx, %ecx
	movl	12(%ebp), %ebx
	andl	$16711680, %ebx
	movl	%ebx, %ecx
	xorl	%ebx, %ebx
	shrl	$8, %ecx
	orl	%ecx, %eax
	orl	%ebx, %edx
	movl	8(%ebp), %ecx
	andl	$16711680, %ecx
	xorl	%ebx, %ebx
	shldl	$24, %ecx, %ebx
	sall	$24, %ecx
	movl	8(%ebp), %esi
	andl	$-16777216, %esi
	xorl	%edi, %edi
	shldl	$8, %esi, %edi
	sall	$8, %esi
	orl	%esi, %ecx
	orl	%edi, %ebx
	xorl	%esi, %esi
	movl	12(%ebp), %edi
	andl	$65280, %edi
	shrdl	$24, %edi, %esi
	shrl	$24, %edi
	orl	%esi, %ecx
	orl	%edi, %ebx
	xorl	%esi, %esi
	movl	12(%ebp), %edi
	andl	$-16777216, %edi
	movl	%edi, %esi
	xorl	%edi, %edi
	shrl	$24, %esi
	orl	%esi, %ecx
	orl	%edi, %ebx
	orl	%ecx, %eax
	orl	%ebx, %edx
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret
	*/
#else
	uint8_t *w = reinterpret_cast<uint8_t*>(&value);

	const uint8_t w0 = w[0];
	const uint8_t w1 = w[1];
	const uint8_t w2 = w[2];
	const uint8_t w3 = w[3];
	const uint8_t w4 = w[4];
	const uint8_t w5 = w[5];
	const uint8_t w6 = w[6];
	const uint8_t w7 = w[7];
	w[0] = w7;
	w[1] = w6;
	w[2] = w5;
	w[3] = w4;
	w[4] = w3;
	w[5] = w2;
	w[6] = w1;
	w[7] = w0;
	return(value);
	/*
	movl	8(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	12(%ebp), %eax
	movl	%eax, -12(%ebp)
	movb	-16(%ebp), %al
	movb	%al, -17(%ebp)
	movb	-15(%ebp), %bl
	movb	-14(%ebp), %cl
	movb	-13(%ebp), %dl
	movb	-9(%ebp), %al
	movb	%al, -16(%ebp)
	movb	-10(%ebp), %al
	movb	%al, -15(%ebp)
	movb	-11(%ebp), %al
	movb	%al, -14(%ebp)
	movb	-12(%ebp), %al
	movb	%al, -13(%ebp)
	movb	%dl, -12(%ebp)
	movb	%cl, -11(%ebp)
	movb	%bl, -10(%ebp)
	movb	-17(%ebp), %al
	movb	%al, -9(%ebp)
	movl	-16(%ebp), %eax
	movl	-12(%ebp), %edx
	addl	$20, %esp
	popl	%ebx
	leave
	ret
	*/
#endif
}

int main(int argc, void *argv[])
{
	uint16_t X = 0x2010;
	uint32_t Y = 0x40302010;
	uint64_t Z = 0x8070605040302010ULL;

	printf("%04X\n", X);
	X = swap16(X);
	printf("%04X\n", X);

	printf("\n");

	printf("%08X\n", Y);
	Y = swap32(Y);
	printf("%08X\n", Y);

	printf("\n");

	printf("%08LX\n", Z);
	Z = swap64(Z);
	printf("%08LX\n", Z);

	return(0);
}

#endif
