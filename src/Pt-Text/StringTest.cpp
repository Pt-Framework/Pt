/***************************************************************************
 *   Copyright (C) 2006 by Tobias Müller                                   *
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

#include "Pt/Text/String.h"
#include "Pt/Exception.h"
#include "Pt/Main.h"
#include <cassert>
#include <vector>

using namespace std;
using namespace Pt::Text;

class StringTest
{

public:
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
	void testLengthAndSize();
};

int main(int argc, char* argv[])
{
	StringTest st;
	st.testConstructor();
	st.testCompare();
	st.testAssign();
	st.testAppend();
	st.testInsert();
	st.testClear();
	st.testErase();
	st.testReplace();
	st.testFind();
	st.testRFind();
	st.testFindFirstOf();
	st.testFindLastOf();
	st.testFindFirstNotOf();
	st.testFindLastNotOf();
	st.testCStr();
	st.testSubstr();
	st.testSwap();
	st.testIndexOperator();
	st.testAt();
	st.testPushBack();
	st.testCopy();
	st.testLengthAndSize();
}


void StringTest::testConstructor()
{
	Pt::String s1;
	assert(s1 == Pt::String(L""));

	Pt::String s2(L"abcde");
	assert(s2 == L"abcde");

	Pt::String s3(L"abcde", 3);
	assert(s3 == L"abc");

	Pt::String s4(3, 'x');
	assert(s4 == L"xxx");

	Pt::String s5(s2);
	assert(s5 == L"abcde");

	Pt::String s6(s2, 1);
	assert(s6 == L"bcde");

	Pt::String s7(s2, 1, 3);
	assert(s7 == L"bcd");


	Pt::String s10;
	assert(s10 == Pt::String(L""));

	const Char c11[] = { 'a', 'b', 'c', 'd', 'e', '\0' };
	Pt::String s11(c11);
	assert(s11 == c11);

	const Char c12[] = { 'a', 'b', 'c', '\0' };
	Pt::String s12(L"abcde", 3);
	assert(s12 == c12);

	const Char c13[] = { 'x', 'x', 'x', '\0' };
	Pt::String s13(3, 'x');
	assert(s13 == c13);

	const Char c14[] = { 'a', 'b', 'c', 'd', 'e', '\0' };
	Pt::String s14(s11);
	assert(s14 == c14);

	const Char c15[] = { 'b', 'c', 'd', 'e', '\0' };
	Pt::String s15(s11, 1);
	assert(s15 == c15);

	const Char c16[] = { 'b', 'c', 'd', '\0' };
	Pt::String s16(s11, 1, 3);
	assert(s16 == c16);


//  TODO API not implemented yet.
//	Pt::String s20(s2.begin(), s2.end());
//	assert(s20 == L"abcde");
}

void StringTest::testCompare()
{
	const Char abc[] = { 'a', 'b', 'c', '\0' };

	const wchar_t* z = L"abcxyz";
	Pt::String s(L"abcd");
	Pt::String t(abc);

	assert(s.compare(s)                 == 0);
	assert(s.compare(t)                 == 1);
	assert(s.compare(z)                 == -1);
	assert(s.compare(1, 3, t)           == 1);
	assert(s.compare(1, 3, t, 1, 2)     == 1);
	assert(s.compare(1, 3, z)           == 1);
	assert(s.compare(1, 2, z + 1, 0, 2) == 0);

	Pt::String x1(L"abc");
	Pt::String x2(abc);
	assert(x1 == x2);
	assert(x1 == abc);
	assert(x2 == abc);

	const Char empty[] = { '\0' };
	Pt::String y1(L"");
	Pt::String y2(empty);
	assert(y1 == y2);
	assert(y1 == empty);
	assert(y2 == empty);
}

void StringTest::testAssign()
{
	const wchar_t* z = L"abcde";
	vector<wchar_t> v(z, z + 5);
	Pt::String s;
	Pt::String t(z);

	s.assign(z);
	assert(s == L"abcde");

	s.assign(z + 1, 0, 3);
	assert(s == L"bcd");

	s.assign(3, 'x');
	assert(s == L"xxx");

	s.assign(t);
	assert(s == L"abcde");

	s.assign(t, 1, 3);
	assert(s == L"bcd");

/*  TODO API not implemented yet.
	s.assign(v.begin(), v.end());
	assert(s == L"abcde");
*/
}

void StringTest::testAppend()
{
	const wchar_t* z = L"abcde";
	vector<wchar_t> v(z, z + 5);
	Pt::String s(L"ABC");
	Pt::String t(z);

	s.append(z);
	assert(s == L"ABCabcde");

	s = L"ABC";
	s.append(z + 1, 0, 3);
	assert(s == L"ABCbcd");

	s = L"ABC";
	s.append(3, 'x');
	assert(s == L"ABCxxx");

	s = L"ABC";
	s.append(t);
	assert(s == L"ABCabcde");

	s = L"ABC";
	s.append(t, 1, 3);
	assert(s == L"ABCbcd");

/*  TODO API not implemented yet.
	s = L"ABC";
	s.append(v.begin(), v.end());
	assert(s == L"ABCabcde");
*/
	// operator +=
	s = L"ABC";
	s += z;
	assert(s == L"ABCabcde");

	s = L"ABC";
	s += 'x';
	assert(s == L"ABCx");

	s = L"ABC";
	s += t;
	assert(s == L"ABCabcde");

	// operator +
/*	s = L"ABC";
	Pt::String u = s + t;
	assert(u == L"ABCabcde");*/
}

void StringTest::testInsert()
{
	Pt::String::iterator i;
	const wchar_t* z = L"abcde";
	vector<wchar_t> v(z, z + 5);
	Pt::String s(L"ABC");
	Pt::String t(z);

	s.insert(2, z);
	assert(s == L"ABabcdeC");

	s = L"ABC";
	s.insert(2, z + 1, 0, 3);
	assert(s == L"ABbcdC");

	s = L"ABC";
	s.insert(2, 3, 'x');
	assert(s == L"ABxxxC");

	s = L"ABC";
	s.insert(2, t);
	assert(s == L"ABabcdeC");

	s = L"ABC";
	s.insert(2, t, 1, 3);
	assert(s == L"ABbcdC");

	s = L"ABC";
	i = s.begin() + 2;
	s.insert(i, 'x');
	assert(s == L"ABxC");

	s = L"ABC";
	i = s.begin() + 2;
	s.insert(i, 3, 'x');
	assert(s == L"ABxxxC");

/*  TODO API not implemented yet.
	s = L"ABC";
	s.insert(i, v.begin(), v.end());
	i = s.begin() + 2;
	assert(s == L"ABabcdeC");*/
}

void StringTest::testClear()
{
	Pt::String s(L"abcdefg");

	s.clear();
	assert(s == L"");
}

void StringTest::testErase()
{
	Pt::String s(L"abcdefg");
	Pt::String::iterator p = s.begin() + 2;
	Pt::String::iterator q = s.end() - 2;

	s.erase();
	assert(s == L"");

	s = L"abcdefg";
	s.erase(2);
	assert(s == L"ab");

	s = L"abcdefg";
	s.erase(2, 3);
	assert(s == L"abfg");

	s = L"abcdefg";
	p = s.begin() + 2;
	s.erase(p);
	assert(s == L"abdefg");

	s = L"abcdefg";
	p = s.begin() + 2;
	q = s.end()   - 2;
	s.erase(p, q);
	assert(s == L"abfg");
}

void StringTest::testReplace()
{
	const wchar_t* z = L"vwxyz";
	vector<wchar_t> v(z, z + 5);
	Pt::String s(L"ABCDEF");
	Pt::String t(z);
	Pt::String::iterator i1;
	Pt::String::iterator i2;

	s.replace(1, 4, z);
	assert(s == L"AvwxyzF");

	s = L"ABCDEF";
	s.replace(1, 4, z + 1, 0, 3);
	assert(s == L"AwxyF");

	s = L"ABCDEF";
	s.replace(1, 4, 3, 'x');
	assert(s == L"AxxxF");

	s = L"ABCDEF";
	s.replace(1, 4, t);
	assert(s == L"AvwxyzF");

	s = L"ABCDEF";
	s.replace(1, 4, t, 1, 3);
	assert(s == L"AwxyF");

	s = L"ABCDEF";
	i1 = s.begin() + 1;
	i2 = s.end() - 1;
	s.replace(i1, i2, z);
	assert(s == L"AvwxyzF");

	Char z2[] = { 'v', 'w', 'x', 'y'  };
	i1 = s.begin() + 1;
	i2 = s.end() - 1;
	s.replace(i1, i2, z2 + 1, 3);
	assert(s == L"AwxyF");

	s = L"ABCDEF";
	i1 = s.begin() + 1;
	i2 = s.end() - 1;
	s.replace(i1, i2, 3, 'x');
	assert(s == L"AxxxF");

	s = L"ABCDEF";
	i1 = s.begin() + 1;
	i2 = s.end() - 1;
	s.replace(i1, i2, t);
	assert(s == L"AvwxyzF");

/*  TODO API not implemented yet.
	s = L"ABCDEF";
	i1 = s.begin() + 1;
	i2 = s.end() - 1;
	s.replace(i1, i2, v.begin(), v.end());
	assert(s == L"AvwxyzF");
*/
}

void StringTest::testFind()
{
	Pt::String s(L"abc-abc");
	Pt::String t(L"bc");
	Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

	assert(s.find(t)          == 1);
	assert(s.find(t, 2)       == 5);
	assert(s.find(L"bc")      == 1);
	assert(s.find(L"bc", 2)   == 5);
	assert(s.find(abcd, 2, 3) == 4);
	assert(s.find('b')        == 1);
	assert(s.find('b', 2)     == 5);
}

void StringTest::testRFind()
{
	Pt::String s(L"abc-abc");
	Pt::String t(L"bc");
	Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

	assert(s.rfind(t)          == 5);
	assert(s.rfind(t, 2)       == 1);
	assert(s.rfind(L"bc")      == 5);
	assert(s.rfind(L"bc", 2)   == 1);
	assert(s.rfind(abcd, 2, 3) == 0);
	assert(s.rfind('b')        == 5);
	assert(s.rfind('b', 2)     == 1);
}

void StringTest::testFindFirstOf()
{
	Pt::String s(L"abc-abc");
	Pt::String t(L"a-x");
	Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

	assert(s.find_first_of(t)          == 0);
	assert(s.find_first_of(t, 2)       == 3);
	assert(s.find_first_of(L"bc")      == 1);
	assert(s.find_first_of(L"bc", 2)   == 2);
	assert(s.find_first_of(abcd, 2, 3) == 2);
	assert(s.find_first_of('b')        == 1);
	assert(s.find_first_of('b', 2)     == 5);
}

void StringTest::testFindLastOf()
{
	Pt::String s(L"abc-abc");
	Pt::String t(L"a-x");
	Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

	assert(s.find_last_of(t)          == 4);
	assert(s.find_last_of(t, 2)       == 0);
	assert(s.find_last_of(L"bc")      == 6);
	assert(s.find_last_of(L"bc", 2)   == 2);
	assert(s.find_last_of(abcd, 2, 3) == 2);
	assert(s.find_last_of('b')        == 5);
	assert(s.find_last_of('b', 2)     == 1);
}

void StringTest::testFindFirstNotOf()
{
	Pt::String s(L"abc-abc");
	Pt::String t(L"a-x");
	Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

	assert(s.find_first_not_of(t)          == 1);
	assert(s.find_first_not_of(t, 2)       == 2);
	assert(s.find_first_not_of(L"bc")      == 0);
	assert(s.find_first_not_of(L"bc", 2)   == 3);
	assert(s.find_first_not_of(abcd, 2, 3) == 3);
	assert(s.find_first_not_of('b')        == 0);
	assert(s.find_first_not_of('b', 2)     == 2);
}

void StringTest::testFindLastNotOf()
{
	Pt::String s(L"abc-abc");
	Pt::String t(L"a-x");
	Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

	assert(s.find_last_not_of(t)          == 6);
	assert(s.find_last_not_of(t, 2)       == 2);
	assert(s.find_last_not_of(L"bc")      == 4);
	assert(s.find_last_not_of(L"bc", 2)   == 0);
	assert(s.find_last_not_of(abcd, 2, 3) == Pt::String::npos);
	assert(s.find_last_not_of('b')        == 6);
	assert(s.find_last_not_of('b', 2)     == 2);
}

void StringTest::testCStr()
{
	Pt::String s1(L"abc");
	assert(s1.c_str()[0] == 'a' && s1.c_str()[1] == 'b' && s1.c_str()[2] == 'c' && s1.c_str()[3] == '\0');

	Pt::String s2;
	assert(s2.c_str()[0] == '\0');

	Char abc[] = { 'a', 'b', 'c', '\0' };
	Pt::String s3(abc);
	assert(s3.c_str()[0] == 'a' && s3.c_str()[1] == 'b' && s3.c_str()[2] == 'c' && s3.c_str()[3] == '\0');

	Char zero[] = { '\0' };
	Pt::String s4(zero);
	assert(s4.c_str()[0] == '\0');
}

void StringTest::testSubstr()
{
	Pt::String s(L"abcdefg");

	assert(s.substr()     == L"abcdefg");
	assert(s.substr(2)    == L"cdefg");
	assert(s.substr(2, 3) == L"cde");
}

void StringTest::testSwap()
{
	Pt::String s1(L"abc");
	Pt::String s2(L"xyz");

	assert(s1 == L"abc");
	assert(s2 == L"xyz");

	s1.swap(s2);

	assert(s1 == L"xyz");
	assert(s2 == L"abc");

	s2.swap(s1);

	assert(s1 == L"abc");
	assert(s2 == L"xyz");
}

void StringTest::testIndexOperator()
{
	Pt::String s(L"abcdef");

	assert(s[0] == 'a');
	assert(s[5] == 'f');
	assert(s[6] == '\0');

/*	bool exceptionOccured = false;
	try {
		s[10];
	} catch (const ptv::Exception& e) {
		exceptionOccured = true;
	}*/
}

void StringTest::testAt()
{
	Pt::String s(L"abcdef");

	assert(s.at(0) == 'a');
	assert(s.at(5) == 'f');
}

void StringTest::testPushBack()
{
	Pt::String s(L"abc");

	s.push_back('d');
	assert(s == L"abcd");
}

void StringTest::testCopy()
{
	Char t1[3];
	Pt::String s(L"abcd");

	s.copy(t1, 2);
	t1[2] = '\0';

	const Char c1[] = { 'a', 'b', '\0' };
	assert(char_traits<Char>::compare(t1, c1, 3) == 0);


	Char t2[5];
	s.copy(t2, 4);
	t2[4] = '\0';

	const Char c2[] = { 'a', 'b', 'c', 'd', '\0' };
	assert(char_traits<Char>::compare(t2, c2, 5) == 0);


	Char t3[3];
	s.copy(t3, 2, 2);
	t3[2] = '\0';

	const Char c3[] = { 'c', 'd', '\0' };
	assert(char_traits<Char>::compare(t3, c3, 3) == 0);
}

void StringTest::testLengthAndSize()
{
	Pt::String s1;
	assert(s1.length() == 0);
	assert(s1.size()   == 0);

	Pt::String s2(L"ab");
	assert(s2.length() == 2);
	assert(s2.size()   == 2);

	s2 += L"cd";
	assert(s2.length() == 4);
	assert(s2.size()   == 4);


	Char ab[] = { 'a', 'b', '\0' };
	Pt::String s3(ab);
	assert(s3.length() == 2);
	assert(s3.size()   == 2);

	Char cd[] = { 'c', 'd', '\0' };
	s3 += cd;
	assert(s3.length() == 4);
	assert(s3.size()   == 4);
}
