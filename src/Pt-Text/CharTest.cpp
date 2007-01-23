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
#undef PT_TEXT_API_EXPORT

#include "Pt/Api.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/TestSchedule.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Text/Char.h"

#include <string>
#include <sstream>


class CharTest : public Pt::Unit::TestSuite
{
	public:
        CharTest()
        : Pt::Unit::TestSuite("CharTest")
        {
            Pt::Unit::TestSuite::registerMethod( "testConstructor", *this, &CharTest::testConstructor  );
            Pt::Unit::TestSuite::registerMethod( "testAssign", *this, &CharTest::testAssign  );
            Pt::Unit::TestSuite::registerMethod( "testCategory", *this, &CharTest::testCategory  );
            Pt::Unit::TestSuite::registerMethod( "testCompare", *this, &CharTest::testCompare  );
            Pt::Unit::TestSuite::registerMethod( "testOperators", *this, &CharTest::testOperators  );
        }

    protected:
        void testConstructor();
        void testAssign();
        void testCategory();
        void testCompare();
        void testOperators();
};

Pt::Unit::RegisterTest<CharTest> _registerCharTest;


void CharTest::testConstructor()
{
	Pt::Text::Char p1;
	PT_UNIT_ASSERT(p1.value() == 0);

	Pt::Text::Char p2((char)'a');
	PT_UNIT_ASSERT(p2.value() == 97);

	Pt::Text::Char p3((wchar_t)'a');
	PT_UNIT_ASSERT(p3.value() == 97);

	Pt::Text::Char p4((unsigned char)'a');
	PT_UNIT_ASSERT(p4.value() == 97);

	Pt::Text::Char p5((Pt::int16_t)'a');
	PT_UNIT_ASSERT(p5.value() == 97);

	Pt::Text::Char p6((Pt::uint16_t)'a');
	PT_UNIT_ASSERT(p6.value() == 97);

	Pt::Text::Char p7((Pt::int32_t)'a');
	PT_UNIT_ASSERT(p7.value() == 97);

	Pt::Text::Char p8((Pt::uint32_t)'a');
	PT_UNIT_ASSERT(p8.value() == 97);

	Pt::Text::Char p9((long)'a');
	PT_UNIT_ASSERT(p9.value() == 97);

	Pt::Text::Char p10((unsigned long)'a');
	PT_UNIT_ASSERT(p10.value() == 97);
}

void CharTest::testAssign()
{
	Pt::Text::Char p;
	PT_UNIT_ASSERT(p.value() == 0);

	p = (char)'a';
	PT_UNIT_ASSERT(p.value() == 97);

	p = (wchar_t)('b');
	PT_UNIT_ASSERT(p.value() == 98);

	p = (unsigned char)'c';
	PT_UNIT_ASSERT(p.value() == 99);

	p = (Pt::int16_t)'d';
	PT_UNIT_ASSERT(p.value() == 100);

	p = (Pt::uint16_t)'e';
	PT_UNIT_ASSERT(p.value() == 101);

	p = (Pt::int32_t)'f';
	PT_UNIT_ASSERT(p.value() == 102);

	p = (Pt::uint32_t)'g';
	PT_UNIT_ASSERT(p.value() == 103);

	p = (long)'h';
	PT_UNIT_ASSERT(p.value() == 104);

	p = (unsigned long)'i';
	PT_UNIT_ASSERT(p.value() == 105);
}

void CharTest::testCategory()
{
	Pt::Text::Char p;

	p = 'a';
	PT_UNIT_ASSERT( p.isAlpha());
	PT_UNIT_ASSERT( p.isAlnum());
	PT_UNIT_ASSERT(!p.isDigit());
	PT_UNIT_ASSERT( p.isLower());
	PT_UNIT_ASSERT(!p.isUpper());
	PT_UNIT_ASSERT(!p.isControl());
	PT_UNIT_ASSERT(!p.isGraph());
	PT_UNIT_ASSERT( p.isPrint());
	PT_UNIT_ASSERT(!p.isPunctuation());
	PT_UNIT_ASSERT(!p.isSpace());

	p = p.toUpper();  // A
	PT_UNIT_ASSERT( p.isAlpha());
	PT_UNIT_ASSERT( p.isAlnum());
	PT_UNIT_ASSERT(!p.isDigit());
	PT_UNIT_ASSERT(!p.isLower());
	PT_UNIT_ASSERT( p.isUpper());
	PT_UNIT_ASSERT(!p.isControl());
	PT_UNIT_ASSERT(!p.isGraph());
	PT_UNIT_ASSERT( p.isPrint());
	PT_UNIT_ASSERT(!p.isPunctuation());
	PT_UNIT_ASSERT(!p.isSpace());

	p = wchar_t(248);
	PT_UNIT_ASSERT( p.isAlpha());
	PT_UNIT_ASSERT( p.isAlnum());
	PT_UNIT_ASSERT(!p.isDigit());
	PT_UNIT_ASSERT( p.isLower());
	PT_UNIT_ASSERT(!p.isUpper());
	PT_UNIT_ASSERT(!p.isControl());
	PT_UNIT_ASSERT(!p.isGraph());
	PT_UNIT_ASSERT( p.isPrint());
	PT_UNIT_ASSERT(!p.isPunctuation());
	PT_UNIT_ASSERT(!p.isSpace());


	p = p.toUpper();  // �
	PT_UNIT_ASSERT( p.isAlpha());
	PT_UNIT_ASSERT( p.isAlnum());
	PT_UNIT_ASSERT(!p.isDigit());
	PT_UNIT_ASSERT(!p.isLower());
	PT_UNIT_ASSERT( p.isUpper());
	PT_UNIT_ASSERT(!p.isControl());
	PT_UNIT_ASSERT(!p.isGraph());
	PT_UNIT_ASSERT( p.isPrint());
	PT_UNIT_ASSERT(!p.isPunctuation());
	PT_UNIT_ASSERT(!p.isSpace());

	p = '1';
	PT_UNIT_ASSERT(!p.isAlpha());
	PT_UNIT_ASSERT( p.isAlnum());
	PT_UNIT_ASSERT( p.isDigit());
	PT_UNIT_ASSERT(!p.isLower());
	PT_UNIT_ASSERT(!p.isUpper());
	PT_UNIT_ASSERT(!p.isControl());
	PT_UNIT_ASSERT(!p.isGraph());
	PT_UNIT_ASSERT( p.isPrint());
	PT_UNIT_ASSERT(!p.isPunctuation());
	PT_UNIT_ASSERT(!p.isSpace());

	PT_UNIT_ASSERT(p.toUpper() == p);

	p = '\t';
	PT_UNIT_ASSERT(!p.isAlpha());
	PT_UNIT_ASSERT(!p.isAlnum());
	PT_UNIT_ASSERT(!p.isDigit());
	PT_UNIT_ASSERT(!p.isLower());
	PT_UNIT_ASSERT(!p.isUpper());
	PT_UNIT_ASSERT( p.isControl());
	PT_UNIT_ASSERT(!p.isGraph());
	PT_UNIT_ASSERT(!p.isPrint());
	PT_UNIT_ASSERT(!p.isPunctuation());
	PT_UNIT_ASSERT(!p.isSpace());

	PT_UNIT_ASSERT(p.toUpper() == p);

	p = ' ';
	PT_UNIT_ASSERT(!p.isAlpha());
	PT_UNIT_ASSERT(!p.isAlnum());
	PT_UNIT_ASSERT(!p.isDigit());
	PT_UNIT_ASSERT(!p.isLower());
	PT_UNIT_ASSERT(!p.isUpper());
	PT_UNIT_ASSERT(!p.isControl());
	PT_UNIT_ASSERT(!p.isGraph());
	PT_UNIT_ASSERT( p.isPrint());
	PT_UNIT_ASSERT(!p.isPunctuation());
	PT_UNIT_ASSERT( p.isSpace());

	PT_UNIT_ASSERT(p.toUpper() == p);

	p = ':';
	PT_UNIT_ASSERT(!p.isAlpha());
	PT_UNIT_ASSERT(!p.isAlnum());
	PT_UNIT_ASSERT(!p.isDigit());
	PT_UNIT_ASSERT(!p.isLower());
	PT_UNIT_ASSERT(!p.isUpper());
	PT_UNIT_ASSERT(!p.isControl());
	PT_UNIT_ASSERT(!p.isGraph());
	PT_UNIT_ASSERT( p.isPrint());
	PT_UNIT_ASSERT( p.isPunctuation());
	PT_UNIT_ASSERT(!p.isSpace());

	PT_UNIT_ASSERT(p.toUpper() == p);

	p = '+';
	PT_UNIT_ASSERT(!p.isAlpha());
	PT_UNIT_ASSERT(!p.isAlnum());
	PT_UNIT_ASSERT(!p.isDigit());
	PT_UNIT_ASSERT(!p.isLower());
	PT_UNIT_ASSERT(!p.isUpper());
	PT_UNIT_ASSERT(!p.isControl());
	PT_UNIT_ASSERT( p.isGraph());
	PT_UNIT_ASSERT( p.isPrint());
	PT_UNIT_ASSERT(!p.isPunctuation());
	PT_UNIT_ASSERT(!p.isSpace());

	PT_UNIT_ASSERT(p.toUpper() == p);
}

void CharTest::testCompare()
{
	Pt::Text::Char a1('a');
	Pt::Text::Char a2('a');
	Pt::Text::Char b('b');

	PT_UNIT_ASSERT(a1 == a2);
	PT_UNIT_ASSERT(a1 == 'a');
	PT_UNIT_ASSERT(a1 == 97);

	PT_UNIT_ASSERT(b != a1);
	PT_UNIT_ASSERT(a1 < b);
	PT_UNIT_ASSERT(b > a1);
}


void CharTest::testOperators()
{
	Pt::Text::Char a('a'); // 97
	Pt::Text::Char z('z'); // 122
	Pt::Text::Char r;

	r = z - a;
	PT_UNIT_ASSERT(r == 25);

	r = z - 'a';
	PT_UNIT_ASSERT(r == 25);

	r = z + a;
	PT_UNIT_ASSERT(r == 219);

	r = z + 'a';
	PT_UNIT_ASSERT(r == 219);

	r = z;
	r -= a;
	PT_UNIT_ASSERT(r == 25);

	r = a;
	r += z;
	PT_UNIT_ASSERT(r == 219);


	Pt::Text::Char v1(0x1E0); // 111100000
	Pt::Text::Char v2(0x10F); // 100001111

	r = v1 | v2;
	PT_UNIT_ASSERT(r == 0x1EF); // 111101111


	Pt::Text::Char v3(0x3C); // 000111100
	Pt::Text::Char v4(0x0F); // 000001111

	r = v3 & v4;
	PT_UNIT_ASSERT(r == 0x0C); // 000001100
}
