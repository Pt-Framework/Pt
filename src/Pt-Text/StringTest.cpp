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

#include "Pt/Text/String.h"
#include "Pt/Exception.h"

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestMain.h"

using namespace std;
using namespace Pt::Text;

class StringTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( StringTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testCompare );
	CPPUNIT_TEST( testAssign );
	CPPUNIT_TEST( testAppend );
	CPPUNIT_TEST( testInsert );
	CPPUNIT_TEST( testClear );
	CPPUNIT_TEST( testErase );
	CPPUNIT_TEST( testReplace );
	CPPUNIT_TEST( testFind );
	CPPUNIT_TEST( testRFind );
	CPPUNIT_TEST( testFindFirstOf );
	CPPUNIT_TEST( testFindLastOf );
	CPPUNIT_TEST( testFindFirstNotOf );
	CPPUNIT_TEST( testFindLastNotOf );
	CPPUNIT_TEST( testCStr );
	CPPUNIT_TEST( testSubstr );
	CPPUNIT_TEST( testSwap );
	CPPUNIT_TEST( testIndexOperator );
	CPPUNIT_TEST( testAt );
	CPPUNIT_TEST( testPushBack );
	CPPUNIT_TEST( testCopy );
	CPPUNIT_TEST( testReserve );
	CPPUNIT_TEST( testLengthAndSize );

	CPPUNIT_TEST_SUITE_END();

public:


protected:
	void testConstructor();
	void testCompare();
	void testAssign();
	void testAppend();
	void testInsert();
	void testClear();
	void testErase();
	void testReplace();
	void testFind();
	void testRFind();
	void testFindFirstOf();
	void testFindLastOf();
	void testFindFirstNotOf();
	void testFindLastNotOf();
	void testCStr();
	void testSubstr();
	void testSwap();
	void testIndexOperator();
	void testAt();
	void testPushBack();
	void testCopy();
  void testReserve();
	void testLengthAndSize();
};

CPPUNIT_TEST_SUITE_REGISTRATION( StringTest );


void StringTest::testConstructor()
{
	String s1;
	CPPUNIT_ASSERT(s1 == String(L""));

	String s2(L"abcde");
	CPPUNIT_ASSERT(s2 == L"abcde");

	String s3(L"abcde", 3);
	CPPUNIT_ASSERT(s3 == L"abc");

	String s4(3, 'x');
	CPPUNIT_ASSERT(s4 == L"xxx");

	String s5(s2);
	CPPUNIT_ASSERT(s5 == L"abcde");

	String s6(s2, 1);
	CPPUNIT_ASSERT(s6 == L"bcde");

	String s7(s2, 1, 3);
	CPPUNIT_ASSERT(s7 == L"bcd");


	String s10;
	CPPUNIT_ASSERT(s10 == String(L""));

	const Char c11[] = { 'a', 'b', 'c', 'd', 'e', '\0' };
	String s11(c11);
	CPPUNIT_ASSERT(s11 == c11);

	const Char c12[] = { 'a', 'b', 'c', '\0' };
	String s12(L"abcde", 3);
	CPPUNIT_ASSERT(s12 == c12);

	const Char c13[] = { 'x', 'x', 'x', '\0' };
	String s13(3, 'x');
	CPPUNIT_ASSERT(s13 == c13);

	const Char c14[] = { 'a', 'b', 'c', 'd', 'e', '\0' };
	String s14(s11);
	CPPUNIT_ASSERT(s14 == c14);

	const Char c15[] = { 'b', 'c', 'd', 'e', '\0' };
	String s15(s11, 1);
	CPPUNIT_ASSERT(s15 == c15);

	const Char c16[] = { 'b', 'c', 'd', '\0' };
	String s16(s11, 1, 3);
	CPPUNIT_ASSERT(s16 == c16);


//  TODO API not implemented yet.
//	String s20(s2.begin(), s2.end());
//	CPPUNIT_ASSERT(s20 == L"abcde");
}

void StringTest::testCompare()
{
	const Char abc[] = { 'a', 'b', 'c', '\0' };

	const wchar_t* z = L"abcxyz";
	String s(L"abcd");
	String t(abc);

	CPPUNIT_ASSERT(s.compare(s)                 == 0);
	CPPUNIT_ASSERT(s.compare(t)                 == 1);
	CPPUNIT_ASSERT(s.compare(z)                 == -1);
	CPPUNIT_ASSERT(s.compare(1, 3, t)           == 1);
	CPPUNIT_ASSERT(s.compare(1, 3, t, 1, 2)     == 1);
	CPPUNIT_ASSERT(s.compare(1, 3, z)           == 1);
	CPPUNIT_ASSERT(s.compare(1, 2, z + 1, 0, 2) == 0);

	String x1(L"abc");
	String x2(abc);
	CPPUNIT_ASSERT(x1 == x2);
	CPPUNIT_ASSERT(x1 == abc);
	CPPUNIT_ASSERT(x2 == abc);

	const Char empty[] = { '\0' };
	String y1(L"");
	String y2(empty);
	CPPUNIT_ASSERT(y1 == y2);
	CPPUNIT_ASSERT(y1 == empty);
	CPPUNIT_ASSERT(y2 == empty);
}

void StringTest::testAssign()
{
	const wchar_t* z = L"abcde";
	vector<wchar_t> v(z, z + 5);
	String s;
	String t(z);

	s.assign(z);
	CPPUNIT_ASSERT(s == L"abcde");

	s.assign(z + 1, 0, 3);
	CPPUNIT_ASSERT(s == L"bcd");

	s.assign(3, 'x');
	CPPUNIT_ASSERT(s == L"xxx");

	s.assign(t);
	CPPUNIT_ASSERT(s == L"abcde");

	s.assign(t, 1, 3);
	CPPUNIT_ASSERT(s == L"bcd");

/*  TODO API not implemented yet.
	s.assign(v.begin(), v.end());
	CPPUNIT_ASSERT(s == L"abcde");
*/
}

void StringTest::testAppend()
{
	const wchar_t* z = L"abcde";
	vector<wchar_t> v(z, z + 5);
	String s(L"ABC");
	String t(z);

	s.append(z);
	CPPUNIT_ASSERT(s == L"ABCabcde");

	s = L"ABC";
	s.append(z + 1, 0, 3);
	CPPUNIT_ASSERT(s == L"ABCbcd");

	s = L"ABC";
	s.append(3, 'x');
	CPPUNIT_ASSERT(s == L"ABCxxx");

	s = L"ABC";
	s.append(t);
	CPPUNIT_ASSERT(s == L"ABCabcde");

	s = L"ABC";
	s.append(t, 1, 3);
	CPPUNIT_ASSERT(s == L"ABCbcd");

/*  TODO API not implemented yet.
	s = L"ABC";
	s.append(v.begin(), v.end());
	CPPUNIT_ASSERT(s == L"ABCabcde");
*/
	// operator +=
	s = L"ABC";
	s += z;
	CPPUNIT_ASSERT(s == L"ABCabcde");

	s = L"ABC";
	s += 'x';
	CPPUNIT_ASSERT(s == L"ABCx");

	s = L"ABC";
	s += t;
	CPPUNIT_ASSERT(s == L"ABCabcde");

	// operator +
/*	s = L"ABC";
	String u = s + t;
	CPPUNIT_ASSERT(u == L"ABCabcde");*/
}

void StringTest::testInsert()
{
	String::iterator i;
	const wchar_t* z = L"abcde";
	vector<wchar_t> v(z, z + 5);
	String s(L"ABC");
	String t(z);

	s.insert(2, z);
	CPPUNIT_ASSERT(s == L"ABabcdeC");

	s = L"ABC";
	s.insert(2, z + 1, 0, 3);
	CPPUNIT_ASSERT(s == L"ABbcdC");

	s = L"ABC";
	s.insert(2, 3, 'x');
	CPPUNIT_ASSERT(s == L"ABxxxC");

	s = L"ABC";
	s.insert(2, t);
	CPPUNIT_ASSERT(s == L"ABabcdeC");

	s = L"ABC";
	s.insert(2, t, 1, 3);
	CPPUNIT_ASSERT(s == L"ABbcdC");

	s = L"ABC";
	i = s.begin() + 2;
	s.insert(i, 'x');
	CPPUNIT_ASSERT(s == L"ABxC");

	s = L"ABC";
	i = s.begin() + 2;
	s.insert(i, 3, 'x');
	CPPUNIT_ASSERT(s == L"ABxxxC");

/*  TODO API not implemented yet.
	s = L"ABC";
	s.insert(i, v.begin(), v.end());
	i = s.begin() + 2;
	CPPUNIT_ASSERT(s == L"ABabcdeC");*/
}

void StringTest::testClear()
{
	String s(L"abcdefg");

	s.clear();
	CPPUNIT_ASSERT(s == L"");
}

void StringTest::testErase()
{
	String s(L"abcdefg");
	String::iterator p = s.begin() + 2;
	String::iterator q = s.end() - 2;

	s.erase();
	CPPUNIT_ASSERT(s == L"");

	s = L"abcdefg";
	s.erase(2);
	CPPUNIT_ASSERT(s == L"ab");

	s = L"abcdefg";
	s.erase(2, 3);
	CPPUNIT_ASSERT(s == L"abfg");

	s = L"abcdefg";
	p = s.begin() + 2;
	s.erase(p);
	CPPUNIT_ASSERT(s == L"abdefg");

	s = L"abcdefg";
	p = s.begin() + 2;
	q = s.end()   - 2;
	s.erase(p, q);
	CPPUNIT_ASSERT(s == L"abfg");
}

void StringTest::testReplace()
{
	const wchar_t* z = L"vwxyz";
	vector<wchar_t> v(z, z + 5);
	String s(L"ABCDEF");
	String t(z);
	String::iterator i1;
	String::iterator i2;

	s.replace(1, 4, z);
	CPPUNIT_ASSERT(s == L"AvwxyzF");

	s = L"ABCDEF";
	s.replace(1, 4, z + 1, 0, 3);
	CPPUNIT_ASSERT(s == L"AwxyF");

	s = L"ABCDEF";
	s.replace(1, 4, 3, 'x');
	CPPUNIT_ASSERT(s == L"AxxxF");

	s = L"ABCDEF";
	s.replace(1, 4, t);
	CPPUNIT_ASSERT(s == L"AvwxyzF");

	s = L"ABCDEF";
	s.replace(1, 4, t, 1, 3);
	CPPUNIT_ASSERT(s == L"AwxyF");

	s = L"ABCDEF";
	i1 = s.begin() + 1;
	i2 = s.end() - 1;
	s.replace(i1, i2, z);
	CPPUNIT_ASSERT(s == L"AvwxyzF");

	Char z2[] = { 'v', 'w', 'x', 'y'  };
	i1 = s.begin() + 1;
	i2 = s.end() - 1;
	s.replace(i1, i2, z2 + 1, 3);
	CPPUNIT_ASSERT(s == L"AwxyF");

	s = L"ABCDEF";
	i1 = s.begin() + 1;
	i2 = s.end() - 1;
	s.replace(i1, i2, 3, 'x');
	CPPUNIT_ASSERT(s == L"AxxxF");

	s = L"ABCDEF";
	i1 = s.begin() + 1;
	i2 = s.end() - 1;
	s.replace(i1, i2, t);
	CPPUNIT_ASSERT(s == L"AvwxyzF");

/*  TODO API not implemented yet.
	s = L"ABCDEF";
	i1 = s.begin() + 1;
	i2 = s.end() - 1;
	s.replace(i1, i2, v.begin(), v.end());
	CPPUNIT_ASSERT(s == L"AvwxyzF");
*/
}

void StringTest::testFind()
{
	String s(L"abc-abc");
	String t(L"bc");
	Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

	CPPUNIT_ASSERT(s.find(t)          == 1);
	CPPUNIT_ASSERT(s.find(t, 2)       == 5);
	CPPUNIT_ASSERT(s.find(L"bc")      == 1);
	CPPUNIT_ASSERT(s.find(L"bc", 2)   == 5);
	CPPUNIT_ASSERT(s.find(abcd, 2, 3) == 4);
	CPPUNIT_ASSERT(s.find('b')        == 1);
	CPPUNIT_ASSERT(s.find('b', 2)     == 5);
}

void StringTest::testRFind()
{
	String s(L"abc-abc");
	String t(L"bc");
	Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

	CPPUNIT_ASSERT(s.rfind(t)          == 5);
	CPPUNIT_ASSERT(s.rfind(t, 2)       == 1);
	CPPUNIT_ASSERT(s.rfind(L"bc")      == 5);
	CPPUNIT_ASSERT(s.rfind(L"bc", 2)   == 1);
	CPPUNIT_ASSERT(s.rfind(abcd, 2, 3) == 0);
	CPPUNIT_ASSERT(s.rfind('b')        == 5);
	CPPUNIT_ASSERT(s.rfind('b', 2)     == 1);
}

void StringTest::testFindFirstOf()
{
	String s(L"abc-abc");
	String t(L"a-x");
	Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

	CPPUNIT_ASSERT(s.find_first_of(t)          == 0);
	CPPUNIT_ASSERT(s.find_first_of(t, 2)       == 3);
	CPPUNIT_ASSERT(s.find_first_of(L"bc")      == 1);
	CPPUNIT_ASSERT(s.find_first_of(L"bc", 2)   == 2);
	CPPUNIT_ASSERT(s.find_first_of(abcd, 2, 3) == 2);
	CPPUNIT_ASSERT(s.find_first_of('b')        == 1);
	CPPUNIT_ASSERT(s.find_first_of('b', 2)     == 5);
}

void StringTest::testFindLastOf()
{
	String s(L"abc-abc");
	String t(L"a-x");
	Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

	CPPUNIT_ASSERT(s.find_last_of(t)          == 4);
	CPPUNIT_ASSERT(s.find_last_of(t, 2)       == 0);
	CPPUNIT_ASSERT(s.find_last_of(L"bc")      == 6);
	CPPUNIT_ASSERT(s.find_last_of(L"bc", 2)   == 2);
	CPPUNIT_ASSERT(s.find_last_of(abcd, 2, 3) == 2);
	CPPUNIT_ASSERT(s.find_last_of('b')        == 5);
	CPPUNIT_ASSERT(s.find_last_of('b', 2)     == 1);
}

void StringTest::testFindFirstNotOf()
{
	String s(L"abc-abc");
	String t(L"a-x");
	Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

	CPPUNIT_ASSERT(s.find_first_not_of(t)          == 1);
	CPPUNIT_ASSERT(s.find_first_not_of(t, 2)       == 2);
	CPPUNIT_ASSERT(s.find_first_not_of(L"bc")      == 0);
	CPPUNIT_ASSERT(s.find_first_not_of(L"bc", 2)   == 3);
	CPPUNIT_ASSERT(s.find_first_not_of(abcd, 2, 3) == 3);
	CPPUNIT_ASSERT(s.find_first_not_of('b')        == 0);
	CPPUNIT_ASSERT(s.find_first_not_of('b', 2)     == 2);
}

void StringTest::testFindLastNotOf()
{
	String s(L"abc-abc");
	String t(L"a-x");
	Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

	CPPUNIT_ASSERT(s.find_last_not_of(t)          == 6);
	CPPUNIT_ASSERT(s.find_last_not_of(t, 2)       == 2);
	CPPUNIT_ASSERT(s.find_last_not_of(L"bc")      == 4);
	CPPUNIT_ASSERT(s.find_last_not_of(L"bc", 2)   == 0);
	CPPUNIT_ASSERT(s.find_last_not_of(abcd, 2, 3) == String::npos);
	CPPUNIT_ASSERT(s.find_last_not_of('b')        == 6);
	CPPUNIT_ASSERT(s.find_last_not_of('b', 2)     == 2);
}

void StringTest::testCStr()
{
	String s1(L"abc");
	CPPUNIT_ASSERT(s1.c_str()[0] == 'a' && s1.c_str()[1] == 'b' && s1.c_str()[2] == 'c' && s1.c_str()[3] == '\0');

	String s2;
	CPPUNIT_ASSERT(s2.c_str()[0] == '\0');

	Char abc[] = { 'a', 'b', 'c', '\0' };
	String s3(abc);
	CPPUNIT_ASSERT(s3.c_str()[0] == 'a' && s3.c_str()[1] == 'b' && s3.c_str()[2] == 'c' && s3.c_str()[3] == '\0');

	Char zero[] = { '\0' };
	String s4(zero);
	CPPUNIT_ASSERT(s4.c_str()[0] == '\0');
}

void StringTest::testSubstr()
{
	String s(L"abcdefg");

	CPPUNIT_ASSERT(s.substr()     == L"abcdefg");
	CPPUNIT_ASSERT(s.substr(2)    == L"cdefg");
	CPPUNIT_ASSERT(s.substr(2, 3) == L"cde");
}

void StringTest::testSwap()
{
	String s1(L"abc");
	String s2(L"xyz");

	CPPUNIT_ASSERT(s1 == L"abc");
	CPPUNIT_ASSERT(s2 == L"xyz");

	s1.swap(s2);

	CPPUNIT_ASSERT(s1 == L"xyz");
	CPPUNIT_ASSERT(s2 == L"abc");

	s2.swap(s1);

	CPPUNIT_ASSERT(s1 == L"abc");
	CPPUNIT_ASSERT(s2 == L"xyz");
}

void StringTest::testIndexOperator()
{
	String s(L"abcdef");

	CPPUNIT_ASSERT(s[0] == 'a');
	CPPUNIT_ASSERT(s[5] == 'f');
	CPPUNIT_ASSERT(s[6] == '\0');

/*	bool exceptionOccured = false;
	try {
		s[10];
	} catch (const Pt::Exception& e) {
		exceptionOccured = true;
	}*/
}

void StringTest::testAt()
{
	String s(L"abcdef");

	CPPUNIT_ASSERT(s.at(0) == 'a');
	CPPUNIT_ASSERT(s.at(5) == 'f');
}

void StringTest::testPushBack()
{
	String s(L"abc");

	s.push_back('d');
	CPPUNIT_ASSERT(s == L"abcd");
}

void StringTest::testCopy()
{
	Char t1[3];
	String s(L"abcd");

	s.copy(t1, 2);
	t1[2] = '\0';

	const Char c1[] = { 'a', 'b', '\0' };
	CPPUNIT_ASSERT(char_traits<Char>::compare(t1, c1, 3) == 0);


	Char t2[5];
	s.copy(t2, 4);
	t2[4] = '\0';

	const Char c2[] = { 'a', 'b', 'c', 'd', '\0' };
	CPPUNIT_ASSERT(char_traits<Char>::compare(t2, c2, 5) == 0);


	Char t3[3];
	s.copy(t3, 2, 2);
	t3[2] = '\0';

	const Char c3[] = { 'c', 'd', '\0' };
	CPPUNIT_ASSERT(char_traits<Char>::compare(t3, c3, 3) == 0);
}


void StringTest::testReserve()
{
    const Char c1[] = { 'a', 'b', 'c', 'd', '\0' };
    String s(L"abcd");
    String s2 = s;
    s2.reserve(10);

    CPPUNIT_ASSERT( s2.capacity() == 10 );
    CPPUNIT_ASSERT( s2.size() == 4 );
    CPPUNIT_ASSERT( char_traits<Char>::compare(s2.c_str(), c1, 4) == 0 );

    CPPUNIT_ASSERT( s.capacity() == 4 );
    CPPUNIT_ASSERT( s.size() == 4 );
    CPPUNIT_ASSERT( char_traits<Char>::compare(s.c_str(), c1, 4) == 0 );
}


void StringTest::testLengthAndSize()
{
	String s1;
	CPPUNIT_ASSERT(s1.length() == 0);
	CPPUNIT_ASSERT(s1.size()   == 0);

	String s2(L"ab");
	CPPUNIT_ASSERT(s2.length() == 2);
	CPPUNIT_ASSERT(s2.size()   == 2);

	s2 += L"cd";
	CPPUNIT_ASSERT(s2.length() == 4);
	CPPUNIT_ASSERT(s2.size()   == 4);


	Char ab[] = { 'a', 'b', '\0' };
	String s3(ab);
	CPPUNIT_ASSERT(s3.length() == 2);
	CPPUNIT_ASSERT(s3.size()   == 2);

	Char cd[] = { 'c', 'd', '\0' };
	s3 += cd;
	CPPUNIT_ASSERT(s3.length() == 4);
	CPPUNIT_ASSERT(s3.size()   == 4);
}
