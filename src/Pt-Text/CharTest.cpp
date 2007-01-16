/***************************************************************************
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

#include "Pt/Text/Char.h"
#include "Pt/Exception.h"

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestMain.h"

#include "Pt/Types.h"

using namespace std;
using namespace Pt::Text;

class CharTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( CharTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testAssign );
	CPPUNIT_TEST( testCategory );
	CPPUNIT_TEST( testCompare );
	CPPUNIT_TEST( testOperators );

	CPPUNIT_TEST_SUITE_END();

public:


protected:
	void testConstructor();
	void testAssign();
	void testCategory();
	void testCompare();
	void testOperators();
};

CPPUNIT_TEST_SUITE_REGISTRATION( CharTest );


void CharTest::testConstructor()
{
	Char p1;
	CPPUNIT_ASSERT(p1.value() == 0);

	Char p2((char)'a');
	CPPUNIT_ASSERT(p2.value() == 97);

	Char p3((wchar_t)'a');
	CPPUNIT_ASSERT(p3.value() == 97);

	Char p4((unsigned char)'a');
	CPPUNIT_ASSERT(p4.value() == 97);

	Char p5((Pt::int16_t)'a');
	CPPUNIT_ASSERT(p5.value() == 97);

	Char p6((Pt::uint16_t)'a');
	CPPUNIT_ASSERT(p6.value() == 97);

	Char p7((Pt::int32_t)'a');
	CPPUNIT_ASSERT(p7.value() == 97);

	Char p8((Pt::uint32_t)'a');
	CPPUNIT_ASSERT(p8.value() == 97);

	Char p9((long)'a');
	CPPUNIT_ASSERT(p9.value() == 97);

	Char p10((unsigned long)'a');
	CPPUNIT_ASSERT(p10.value() == 97);
}

void CharTest::testAssign()
{
	Char p;
	CPPUNIT_ASSERT(p.value() == 0);

	p = (char)'a';
	CPPUNIT_ASSERT(p.value() == 97);

	p = (wchar_t)('b');
	CPPUNIT_ASSERT(p.value() == 98);

	p = (unsigned char)'c';
	CPPUNIT_ASSERT(p.value() == 99);

	p = (Pt::int16_t)'d';
	CPPUNIT_ASSERT(p.value() == 100);

	p = (Pt::uint16_t)'e';
	CPPUNIT_ASSERT(p.value() == 101);

	p = (Pt::int32_t)'f';
	CPPUNIT_ASSERT(p.value() == 102);

	p = (Pt::uint32_t)'g';
	CPPUNIT_ASSERT(p.value() == 103);

	p = (long)'h';
	CPPUNIT_ASSERT(p.value() == 104);

	p = (unsigned long)'i';
	CPPUNIT_ASSERT(p.value() == 105);
}

void CharTest::testCategory()
{
	Char p;

	p = 'a';
	CPPUNIT_ASSERT( p.isAlpha());
	CPPUNIT_ASSERT( p.isAlnum());
	CPPUNIT_ASSERT(!p.isDigit());
	CPPUNIT_ASSERT( p.isLower());
	CPPUNIT_ASSERT(!p.isUpper());
	CPPUNIT_ASSERT(!p.isControl());
	CPPUNIT_ASSERT(!p.isGraph());
	CPPUNIT_ASSERT( p.isPrint());
	CPPUNIT_ASSERT(!p.isPunctuation());
	CPPUNIT_ASSERT(!p.isSpace());

	p = p.toUpper();  // A
	CPPUNIT_ASSERT( p.isAlpha());
	CPPUNIT_ASSERT( p.isAlnum());
	CPPUNIT_ASSERT(!p.isDigit());
	CPPUNIT_ASSERT(!p.isLower());
	CPPUNIT_ASSERT( p.isUpper());
	CPPUNIT_ASSERT(!p.isControl());
	CPPUNIT_ASSERT(!p.isGraph());
	CPPUNIT_ASSERT( p.isPrint());
	CPPUNIT_ASSERT(!p.isPunctuation());
	CPPUNIT_ASSERT(!p.isSpace());

	p = wchar_t(248);
	CPPUNIT_ASSERT( p.isAlpha());
	CPPUNIT_ASSERT( p.isAlnum());
	CPPUNIT_ASSERT(!p.isDigit());
	CPPUNIT_ASSERT( p.isLower());
	CPPUNIT_ASSERT(!p.isUpper());
	CPPUNIT_ASSERT(!p.isControl());
	CPPUNIT_ASSERT(!p.isGraph());
	CPPUNIT_ASSERT( p.isPrint());
	CPPUNIT_ASSERT(!p.isPunctuation());
	CPPUNIT_ASSERT(!p.isSpace());


	p = p.toUpper();  // Ö
	CPPUNIT_ASSERT( p.isAlpha());
	CPPUNIT_ASSERT( p.isAlnum());
	CPPUNIT_ASSERT(!p.isDigit());
	CPPUNIT_ASSERT(!p.isLower());
	CPPUNIT_ASSERT( p.isUpper());
	CPPUNIT_ASSERT(!p.isControl());
	CPPUNIT_ASSERT(!p.isGraph());
	CPPUNIT_ASSERT( p.isPrint());
	CPPUNIT_ASSERT(!p.isPunctuation());
	CPPUNIT_ASSERT(!p.isSpace());

	p = '1';
	CPPUNIT_ASSERT(!p.isAlpha());
	CPPUNIT_ASSERT( p.isAlnum());
	CPPUNIT_ASSERT( p.isDigit());
	CPPUNIT_ASSERT(!p.isLower());
	CPPUNIT_ASSERT(!p.isUpper());
	CPPUNIT_ASSERT(!p.isControl());
	CPPUNIT_ASSERT(!p.isGraph());
	CPPUNIT_ASSERT( p.isPrint());
	CPPUNIT_ASSERT(!p.isPunctuation());
	CPPUNIT_ASSERT(!p.isSpace());

	CPPUNIT_ASSERT(p.toUpper() == p);

	p = '\t';
	CPPUNIT_ASSERT(!p.isAlpha());
	CPPUNIT_ASSERT(!p.isAlnum());
	CPPUNIT_ASSERT(!p.isDigit());
	CPPUNIT_ASSERT(!p.isLower());
	CPPUNIT_ASSERT(!p.isUpper());
	CPPUNIT_ASSERT( p.isControl());
	CPPUNIT_ASSERT(!p.isGraph());
	CPPUNIT_ASSERT(!p.isPrint());
	CPPUNIT_ASSERT(!p.isPunctuation());
	CPPUNIT_ASSERT(!p.isSpace());

	CPPUNIT_ASSERT(p.toUpper() == p);

	p = ' ';
	CPPUNIT_ASSERT(!p.isAlpha());
	CPPUNIT_ASSERT(!p.isAlnum());
	CPPUNIT_ASSERT(!p.isDigit());
	CPPUNIT_ASSERT(!p.isLower());
	CPPUNIT_ASSERT(!p.isUpper());
	CPPUNIT_ASSERT(!p.isControl());
	CPPUNIT_ASSERT(!p.isGraph());
	CPPUNIT_ASSERT( p.isPrint());
	CPPUNIT_ASSERT(!p.isPunctuation());
	CPPUNIT_ASSERT( p.isSpace());

	CPPUNIT_ASSERT(p.toUpper() == p);

	p = ':';
	CPPUNIT_ASSERT(!p.isAlpha());
	CPPUNIT_ASSERT(!p.isAlnum());
	CPPUNIT_ASSERT(!p.isDigit());
	CPPUNIT_ASSERT(!p.isLower());
	CPPUNIT_ASSERT(!p.isUpper());
	CPPUNIT_ASSERT(!p.isControl());
	CPPUNIT_ASSERT(!p.isGraph());
	CPPUNIT_ASSERT( p.isPrint());
	CPPUNIT_ASSERT( p.isPunctuation());
	CPPUNIT_ASSERT(!p.isSpace());

	CPPUNIT_ASSERT(p.toUpper() == p);

	p = '+';
	CPPUNIT_ASSERT(!p.isAlpha());
	CPPUNIT_ASSERT(!p.isAlnum());
	CPPUNIT_ASSERT(!p.isDigit());
	CPPUNIT_ASSERT(!p.isLower());
	CPPUNIT_ASSERT(!p.isUpper());
	CPPUNIT_ASSERT(!p.isControl());
	CPPUNIT_ASSERT( p.isGraph());
	CPPUNIT_ASSERT( p.isPrint());
	CPPUNIT_ASSERT(!p.isPunctuation());
	CPPUNIT_ASSERT(!p.isSpace());

	CPPUNIT_ASSERT(p.toUpper() == p);
}

void CharTest::testCompare()
{
	Char a1('a');
	Char a2('a');
	Char b('b');

	CPPUNIT_ASSERT(a1 == a2);
	CPPUNIT_ASSERT(a1 == 'a');
	CPPUNIT_ASSERT(a1 == 97);

	CPPUNIT_ASSERT(b != a1);
	CPPUNIT_ASSERT(a1 < b);
	CPPUNIT_ASSERT(b > a1);
}


void CharTest::testOperators()
{
	Char a('a'); // 97
	Char z('z'); // 122
	Char r;

	r = z - a;
	CPPUNIT_ASSERT(r == 25);

	r = z - 'a';
	CPPUNIT_ASSERT(r == 25);

	r = z + a;
	CPPUNIT_ASSERT(r == 219);

	r = z + 'a';
	CPPUNIT_ASSERT(r == 219);

	r = z;
	r -= a;
	CPPUNIT_ASSERT(r == 25);

	r = a;
	r += z;
	CPPUNIT_ASSERT(r == 219);


	Char v1(0x1E0); // 111100000
	Char v2(0x10F); // 100001111

	r = v1 | v2;
	CPPUNIT_ASSERT(r == 0x1EF); // 111101111


	Char v3(0x3C); // 000111100
	Char v4(0x0F); // 000001111

	r = v3 & v4;
	CPPUNIT_ASSERT(r == 0x0C); // 000001100
}
