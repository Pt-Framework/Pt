/***************************************************************************
 *   Copyright (C) 2006 by Tobias Mller                                   *
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
 **************************************************************************/

#include "Pt/Text/Char.h"
#include "Pt/Exception.h"
#include "Pt/Main.h"

#include <cassert>

#include "Pt/Types.h"

using namespace std;
using namespace Pt::Text;

class CharTest
{
public:

	void testConstructor();
	void testAssign();
	void testCategory();
	void testCompare();
	void testOperators();
};

int main(int argc, char* argv[])
{
	CharTest ct;
	ct.testConstructor();
	ct.testAssign();
	ct.testCategory();
	ct.testCompare();
	ct.testOperators();
}

void CharTest::testConstructor()
{
	Char p1;
	assert(p1.value() == 0);

	Char p2((char)'a');
	assert(p2.value() == 97);

	Char p3((wchar_t)'a');
	assert(p3.value() == 97);

	Char p4((unsigned char)'a');
	assert(p4.value() == 97);

	Char p5((Pt::int16_t)'a');
	assert(p5.value() == 97);

	Char p6((Pt::uint16_t)'a');
	assert(p6.value() == 97);

	Char p7((Pt::int32_t)'a');
	assert(p7.value() == 97);

	Char p8((Pt::uint32_t)'a');
	assert(p8.value() == 97);

	Char p9((long)'a');
	assert(p9.value() == 97);

	Char p10((unsigned long)'a');
	assert(p10.value() == 97);
}

void CharTest::testAssign()
{
	Char p;
	assert(p.value() == 0);

	p = (char)'a';
	assert(p.value() == 97);

	p = (wchar_t)('b');
	assert(p.value() == 98);

	p = (unsigned char)'c';
	assert(p.value() == 99);

	p = (Pt::int16_t)'d';
	assert(p.value() == 100);

	p = (Pt::uint16_t)'e';
	assert(p.value() == 101);

	p = (Pt::int32_t)'f';
	assert(p.value() == 102);

	p = (Pt::uint32_t)'g';
	assert(p.value() == 103);

	p = (long)'h';
	assert(p.value() == 104);

	p = (unsigned long)'i';
	assert(p.value() == 105);
}

void CharTest::testCategory()
{
	Char p;

	p = 'a';
	assert( p.isAlpha());
	assert( p.isAlnum());
	assert(!p.isDigit());
	assert( p.isLower());
	assert(!p.isUpper());
	assert(!p.isControl());
	assert(!p.isGraph());
	assert( p.isPrint());
	assert(!p.isPunctuation());
	assert(!p.isSpace());

	p = p.toUpper();  // A
	assert( p.isAlpha());
	assert( p.isAlnum());
	assert(!p.isDigit());
	assert(!p.isLower());
	assert( p.isUpper());
	assert(!p.isControl());
	assert(!p.isGraph());
	assert( p.isPrint());
	assert(!p.isPunctuation());
	assert(!p.isSpace());

	p = 'l';
	assert( p.isAlpha());
	assert( p.isAlnum());
	assert(!p.isDigit());
	assert( p.isLower());
	assert(!p.isUpper());
	assert(!p.isControl());
	assert(!p.isGraph());
	assert( p.isPrint());
	assert(!p.isPunctuation());
	assert(!p.isSpace());


	p = p.toUpper();  // ?
	assert( p.isAlpha());
	assert( p.isAlnum());
	assert(!p.isDigit());
	assert(!p.isLower());
	assert( p.isUpper());
	assert(!p.isControl());
	assert(!p.isGraph());
	assert( p.isPrint());
	assert(!p.isPunctuation());
	assert(!p.isSpace());

	p = '1';
	assert(!p.isAlpha());
	assert( p.isAlnum());
	assert( p.isDigit());
	assert(!p.isLower());
	assert(!p.isUpper());
	assert(!p.isControl());
	assert(!p.isGraph());
	assert( p.isPrint());
	assert(!p.isPunctuation());
	assert(!p.isSpace());

	assert(p.toUpper() == p);

	p = '\t';
	assert(!p.isAlpha());
	assert(!p.isAlnum());
	assert(!p.isDigit());
	assert(!p.isLower());
	assert(!p.isUpper());
	assert( p.isControl());
	assert(!p.isGraph());
	assert(!p.isPrint());
	assert(!p.isPunctuation());
	assert(!p.isSpace());

	assert(p.toUpper() == p);

	p = ' ';
	assert(!p.isAlpha());
	assert(!p.isAlnum());
	assert(!p.isDigit());
	assert(!p.isLower());
	assert(!p.isUpper());
	assert(!p.isControl());
	assert(!p.isGraph());
	assert( p.isPrint());
	assert(!p.isPunctuation());
	assert( p.isSpace());

	assert(p.toUpper() == p);

	p = ':';
	assert(!p.isAlpha());
	assert(!p.isAlnum());
	assert(!p.isDigit());
	assert(!p.isLower());
	assert(!p.isUpper());
	assert(!p.isControl());
	assert(!p.isGraph());
	assert( p.isPrint());
	assert( p.isPunctuation());
	assert(!p.isSpace());

	assert(p.toUpper() == p);

	p = '+';
	assert(!p.isAlpha());
	assert(!p.isAlnum());
	assert(!p.isDigit());
	assert(!p.isLower());
	assert(!p.isUpper());
	assert(!p.isControl());
	assert( p.isGraph());
	assert( p.isPrint());
	assert(!p.isPunctuation());
	assert(!p.isSpace());

	assert(p.toUpper() == p);
}

void CharTest::testCompare()
{
	Char a1('a');
	Char a2('a');
	Char b('b');

	assert(a1 == a2);
	assert(a1 == 'a');
	assert(a1 == 97);

	assert(b != a1);
	assert(a1 < b);
	assert(b > a1);
}


void CharTest::testOperators()
{
	Char a('a'); // 97
	Char z('z'); // 122
	Char r;

	r = z - a;
	assert(r == 25);

	r = z - 'a';
	assert(r == 25);

	r = z + a;
	assert(r == 219);

	r = z + 'a';
	assert(r == 219);

	r = z;
	r -= a;
	assert(r == 25);

	r = a;
	r += z;
	assert(r == 219);


	Char v1(0x1E0); // 111100000
	Char v2(0x10F); // 100001111

	r = v1 | v2;
	assert(r == 0x1EF); // 111101111


	Char v3(0x3C); // 000111100
	Char v4(0x0F); // 000001111

	r = v3 & v4;
	assert(r == 0x0C); // 000001100
}
