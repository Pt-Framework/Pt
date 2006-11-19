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
#include "Pt/Unit/TestFixture.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"


class ByteorderTest : public Pt::Unit::TestSuite
{
	public:
		ByteorderTest()
	: TestSuite("ByteorderTest")
		{
			this->registerMethod("test", *this, &ByteorderTest::test);
		}

		virtual void test()
		{
			Pt::uint8_t  val8u  = 0x10;
			Pt::uint16_t val16u = 0x1020;
			Pt::uint32_t val32u = 0x10203040;
#ifdef PT_64BIT
			Pt::uint64_t val64u = 0x1020304050607080ULL;
#endif

			cerr << "Before swap()s:" << endl;
			cerr << hex << "0x" << (void*) val8u  << endl;
			cerr << hex << "0x" <<         val16u << endl;
			cerr << hex << "0x" <<         val32u << endl;
#ifdef PT_64BIT
			cerr << hex << "0x" <<         val64u << endl;
#endif

			cerr << endl;
			val8u  = swap(val8u);
			val16u = swap(val16u);
			val32u = swap(val32u);
#ifdef PT_64BIT
			val64u = swap(val64u);
#endif

			cerr << "After swap()s:" << endl;
			cerr << hex << "0x" << (void*) val8u  << endl;
			cerr << hex << "0x" <<         val16u << endl;
			cerr << hex << "0x" <<         val32u << endl;
#ifdef PT_64BIT
			cerr << hex << "0x" <<         val64u << endl;
#endif

			cerr << endl;
			swabUnaligned(reinterpret_cast<Pt::uint8_t*>(&val16u), sizeof(val16u));
			swabUnaligned(reinterpret_cast<Pt::uint8_t*>(&val32u), sizeof(val32u));
#ifdef PT_64BIT
			swabUnaligned(reinterpret_cast<Pt::uint8_t*>(&val64u), sizeof(val64u));
#endif

			cerr << "After swabUnaligned()s:" << endl;
			cerr << hex << "0x" << val16u << endl;
			cerr << hex << "0x" << val32u << endl;
#ifdef PT_64BIT
			cerr << hex << "0x" << val64u << endl;
#endif
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

int main(int argc, void *argv[])
{
	uint16_t X = 0x2010;
	uint32_t Y = 0x40302010;

	printf("%04X\n", X);
	X = swap16(X);
	printf("%04X\n", X);

	printf("\n");

	printf("%08X\n", Y);
	Y = swap32(Y);
	printf("%08X\n", Y);

	return(X);
}

#endif
