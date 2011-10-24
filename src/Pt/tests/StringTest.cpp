/*
 * Copyright (C) 2004 Marc Boris Duerner
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
 */
#undef PT_API_EXPORT

//#include <vld.h>
//#include <vldapi.h>

#include "Pt/Api.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/String.h"
#include <string>
#include <sstream>


class StringTest : public Pt::Unit::TestSuite
{
    public:
        StringTest()
        : Pt::Unit::TestSuite("StringTest")
        {
            Pt::Unit::TestSuite::registerMethod( "testConstructor", *this, &StringTest::testConstructor );
            Pt::Unit::TestSuite::registerMethod( "testCompare", *this, &StringTest::testCompare );
            Pt::Unit::TestSuite::registerMethod( "testAssign", *this, &StringTest::testAssign );
            Pt::Unit::TestSuite::registerMethod( "testAppend", *this, &StringTest::testAppend );
            Pt::Unit::TestSuite::registerMethod( "testInsert", *this, &StringTest::testInsert );
            Pt::Unit::TestSuite::registerMethod( "testClear", *this, &StringTest::testClear );
            Pt::Unit::TestSuite::registerMethod( "testErase", *this, &StringTest::testErase );
            Pt::Unit::TestSuite::registerMethod( "testReplace", *this, &StringTest::testReplace );
            Pt::Unit::TestSuite::registerMethod( "testFind", *this, &StringTest::testFind );
            Pt::Unit::TestSuite::registerMethod( "testRFind", *this, &StringTest::testRFind );
            Pt::Unit::TestSuite::registerMethod( "testFindFirstOf", *this, &StringTest::testFindFirstOf );
            Pt::Unit::TestSuite::registerMethod( "testFindLastOf", *this, &StringTest::testFindLastOf );
            Pt::Unit::TestSuite::registerMethod( "testFindFirstNotOf", *this, &StringTest::testFindFirstNotOf );
            Pt::Unit::TestSuite::registerMethod( "testFindLastNotOf", *this, &StringTest::testFindLastNotOf );
            Pt::Unit::TestSuite::registerMethod( "testCStr", *this, &StringTest::testCStr );
            Pt::Unit::TestSuite::registerMethod( "testSubstr", *this, &StringTest::testSubstr );
            Pt::Unit::TestSuite::registerMethod( "testSwap", *this, &StringTest::testSwap );
            Pt::Unit::TestSuite::registerMethod( "testAt", *this, &StringTest::testAt );
            Pt::Unit::TestSuite::registerMethod( "testPushBack", *this, &StringTest::testPushBack );
            Pt::Unit::TestSuite::registerMethod( "testCopy", *this, &StringTest::testCopy );
            Pt::Unit::TestSuite::registerMethod( "testReserve", *this, &StringTest::testReserve );
            Pt::Unit::TestSuite::registerMethod( "testReserveEmpty", *this, &StringTest::testReserveEmpty );
            Pt::Unit::TestSuite::registerMethod( "testLengthAndSize", *this, &StringTest::testLengthAndSize );
        }

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
        void testReserveEmpty();
        void testLengthAndSize();
};

Pt::Unit::RegisterTest<StringTest> _registerStringTest;



void StringTest::testConstructor()
{
    Pt::String s1;
    PT_UNIT_ASSERT(s1 == Pt::String(L""));

    Pt::String s2(L"abcde");
    PT_UNIT_ASSERT(s2 == L"abcde");

    Pt::String s3(L"abcde", 3);
    PT_UNIT_ASSERT(s3 == L"abc");

    Pt::String s4(3, 'x');
    PT_UNIT_ASSERT(s4 == L"xxx");

    Pt::String s5(s2);
    PT_UNIT_ASSERT(s5 == L"abcde");

    Pt::String s6(s2, 1);
    PT_UNIT_ASSERT(s6 == L"bcde");

    Pt::String s7(s2, 1, 3);
    PT_UNIT_ASSERT(s7 == L"bcd");

    Pt::String s10;
    PT_UNIT_ASSERT(s10 == Pt::String(L""));

    const Pt::Char c11[] = { 'a', 'b', 'c', 'd', 'e', '\0' };
    Pt::String s11(c11);
    PT_UNIT_ASSERT(s11 == c11);

    const Pt::Char c12[] = { 'a', 'b', 'c', '\0' };
    Pt::String s12(L"abcde", 3);
    PT_UNIT_ASSERT(s12 == c12);

    const Pt::Char c13[] = { 'x', 'x', 'x', '\0' };
    Pt::String s13(3, 'x');
    PT_UNIT_ASSERT(s13 == c13);

    const Pt::Char c14[] = { 'a', 'b', 'c', 'd', 'e', '\0' };
    Pt::String s14(s11);
    PT_UNIT_ASSERT(s14 == c14);

    const Pt::Char c15[] = { 'b', 'c', 'd', 'e', '\0' };
    Pt::String s15(s11, 1);
    PT_UNIT_ASSERT(s15 == c15);

    const Pt::Char c16[] = { 'b', 'c', 'd', '\0' };
    Pt::String s16(s11, 1, 3);
    PT_UNIT_ASSERT(s16 == c16);

//  TODO API not implemented yet.
//    Pt::String s20(s2.begin(), s2.end());
//    PT_UNIT_ASSERT(s20 == L"abcde");
}

void StringTest::testCompare()
{
    const Pt::Char abc[] = { 'a', 'b', 'c', '\0' };

    const wchar_t* z = L"abcxyz";
    Pt::String s(L"abcd");
    Pt::String t(abc);

    PT_UNIT_ASSERT_EQUALS(s.compare(s)                 , 0);
    PT_UNIT_ASSERT_EQUALS(s.compare(t)                 , 1);
    PT_UNIT_ASSERT_EQUALS(s.compare(z)                 , -1);
    PT_UNIT_ASSERT_EQUALS(s.compare(1, 3, t)           , 1);
    PT_UNIT_ASSERT_EQUALS(s.compare(1, 3, t, 1, 2)     , 1);
    PT_UNIT_ASSERT_EQUALS(s.compare(1, 3, z)           , 1);
    PT_UNIT_ASSERT_EQUALS(s.compare(1, 2, z + 1, 0, 2) , 0);

    Pt::String x1(L"abc");
    Pt::String x2(abc);
    PT_UNIT_ASSERT(x1 == x2);
    PT_UNIT_ASSERT(x1 == abc);
    PT_UNIT_ASSERT(x2 == abc);

    const Pt::Char empty[] = { '\0' };
    Pt::String y1(L"");
    Pt::String y2(empty);
    PT_UNIT_ASSERT(y1 == y2);
    PT_UNIT_ASSERT(y1 == empty);
    PT_UNIT_ASSERT(y2 == empty);
}

void StringTest::testAssign()
{

{
    Pt::String s1;
    Pt::String s2(L"abc");
    Pt::String s3 = s1;

    Pt::Char& ref = s2[0]; // make it unsharable
    ref = L'x';
    s1 = s2;
}

    const wchar_t* z = L"abcde";
    std::vector<wchar_t> v(z, z + 5);
    Pt::String s;
    Pt::String t(z);

    s.assign(z);
    PT_UNIT_ASSERT(s == L"abcde");

    s.assign(z + 1, 0, 3);
    PT_UNIT_ASSERT(s == L"bcd");

    s.assign(3, 'x');
    PT_UNIT_ASSERT(s == L"xxx");

    s.assign(t);
    PT_UNIT_ASSERT(s == L"abcde");

    s.assign(t, 1, 3);
    PT_UNIT_ASSERT(s == L"bcd");

/*  TODO API not implemented yet.
    s.assign(v.begin(), v.end());
    PT_UNIT_ASSERT(s == L"abcde");
*/
    s = s;
    PT_UNIT_ASSERT(s == L"bcd");
    s.assign(t);
    s = s.c_str();
    PT_UNIT_ASSERT(s == L"abcde");
}

void StringTest::testAppend()
{
    const wchar_t* z = L"abcde";
    std::vector<wchar_t> v(z, z + 5);
    Pt::String s(L"ABC");
    Pt::String t(z);

    s.append(z);
    PT_UNIT_ASSERT(s == L"ABCabcde");

    s = L"ABC";
    s.append(z + 1, 0, 3);
    PT_UNIT_ASSERT(s == L"ABCbcd");

    s = L"ABC";
    s.append(3, 'x');
    PT_UNIT_ASSERT(s == L"ABCxxx");

    s = L"ABC";
    s.append(t);
    PT_UNIT_ASSERT(s == L"ABCabcde");

    s = L"ABC";
    s.append(t, 1, 3);
    PT_UNIT_ASSERT(s == L"ABCbcd");

/*  TODO API not implemented yet.
    s = L"ABC";
    s.append(v.begin(), v.end());
    PT_UNIT_ASSERT(s == L"ABCabcde");
*/
    // operator +=
    s = L"ABC";
    s += z;
    PT_UNIT_ASSERT(s == L"ABCabcde");

    s = L"ABC";
    s += 'x';
    PT_UNIT_ASSERT(s == L"ABCx");

    s = L"ABC";
    s += t;
    PT_UNIT_ASSERT(s == L"ABCabcde");

    s = L"ABC";
    Pt::String u = s + t;
    PT_UNIT_ASSERT(u == L"ABCabcde");
}

void StringTest::testInsert()
{
    Pt::String::iterator i;
    const wchar_t* z = L"abcde";
    std::vector<wchar_t> v(z, z + 5);
    Pt::String s(L"ABC");
    Pt::String t(z);

    s.insert(2, z);
    PT_UNIT_ASSERT(s == L"ABabcdeC");

    s = L"ABC";
    s.insert(2, z + 1, 0, 3);
    PT_UNIT_ASSERT(s == L"ABbcdC");

    s = L"ABC";
    s.insert(2, 3, 'x');
    PT_UNIT_ASSERT(s == L"ABxxxC");

    s = L"ABC";
    s.insert(2, t);
    PT_UNIT_ASSERT(s == L"ABabcdeC");

    s = L"ABC";
    s.insert(2, t, 1, 3);
    PT_UNIT_ASSERT(s == L"ABbcdC");

    s = L"ABC";
    i = s.begin() + 2;
    s.insert(i, 'x');
    PT_UNIT_ASSERT(s == L"ABxC");

    s = L"ABC";
    i = s.begin() + 2;
    s.insert(i, 3, 'x');
    PT_UNIT_ASSERT(s == L"ABxxxC");

/*  TODO API not implemented yet.
    s = L"ABC";
    s.insert(i, v.begin(), v.end());
    i = s.begin() + 2;
    PT_UNIT_ASSERT(s == L"ABabcdeC");*/
}

void StringTest::testClear()
{
    Pt::String s(L"abcdefg");

    s.clear();
    PT_UNIT_ASSERT(s == L"");
}

void StringTest::testErase()
{
    Pt::String s(L"abcdefg");
    Pt::String::iterator p = s.begin() + 2;
    Pt::String::iterator q = s.end() - 2;

    s.erase();
    PT_UNIT_ASSERT(s == L"");

    s = L"abcdefg";
    s.erase(2);
    PT_UNIT_ASSERT(s == L"ab");

    s = L"abcdefg";
    s.erase(2, 3);
    PT_UNIT_ASSERT(s == L"abfg");

    s = L"abcdefg";
    p = s.begin() + 2;
    s.erase(p);
    PT_UNIT_ASSERT(s == L"abdefg");

    s = L"abcdefg";
    p = s.begin() + 2;
    q = s.end()   - 2;
    s.erase(p, q);
    PT_UNIT_ASSERT(s == L"abfg");
}

void StringTest::testReplace()
{
    const wchar_t* z = L"vwxyz";
    std::vector<wchar_t> v(z, z + 5);
    Pt::String s(L"ABCDEF");
    Pt::String t(z);
    Pt::String::iterator i1;
    Pt::String::iterator i2;

    s.replace(1, 4, z);
    PT_UNIT_ASSERT(s == L"AvwxyzF");

    s = L"ABCDEF";
    s.replace(1, 4, z + 1, 0, 3);
    PT_UNIT_ASSERT(s == L"AwxyF");

    s = L"ABCDEF";
    s.replace(1, 4, 3, 'x');
    PT_UNIT_ASSERT(s == L"AxxxF");

    s = L"ABCDEF";
    s.replace(1, 4, t);
    PT_UNIT_ASSERT(s == L"AvwxyzF");

    s = L"ABCDEF";
    s.replace(1, 4, t, 1, 3);
    PT_UNIT_ASSERT(s == L"AwxyF");

    s = L"ABCDEF";
    i1 = s.begin() + 1;
    i2 = s.end() - 1;
    s.replace(i1, i2, z);
    PT_UNIT_ASSERT(s == L"AvwxyzF");

    Pt::Char z2[] = { 'v', 'w', 'x', 'y'  };
    i1 = s.begin() + 1;
    i2 = s.end() - 1;
    s.replace(i1, i2, z2 + 1, 3);
    PT_UNIT_ASSERT(s == L"AwxyF");

    s = L"ABCDEF";
    i1 = s.begin() + 1;
    i2 = s.end() - 1;
    s.replace(i1, i2, 3, 'x');
    PT_UNIT_ASSERT(s == L"AxxxF");

    s = L"ABCDEF";
    i1 = s.begin() + 1;
    i2 = s.end() - 1;
    s.replace(i1, i2, t);
    PT_UNIT_ASSERT(s == L"AvwxyzF");

/*  TODO API not implemented yet.
    s = L"ABCDEF";
    i1 = s.begin() + 1;
    i2 = s.end() - 1;
    s.replace(i1, i2, v.begin(), v.end());
    PT_UNIT_ASSERT(s == L"AvwxyzF");
*/
}

void StringTest::testFind()
{
    Pt::String s(L"abc-abc");
    Pt::String t(L"bc");
    Pt::Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

    PT_UNIT_ASSERT_EQUALS(s.find(t)          , 1);
    PT_UNIT_ASSERT_EQUALS(s.find(t, 2)       , 5);
    PT_UNIT_ASSERT_EQUALS(s.find(L"bc")      , 1);
    PT_UNIT_ASSERT_EQUALS(s.find(L"bc", 2)   , 5);
    PT_UNIT_ASSERT_EQUALS(s.find(abcd, 2, 3) , 4);
    PT_UNIT_ASSERT_EQUALS(s.find('b')        , 1);
    PT_UNIT_ASSERT_EQUALS(s.find('b', 2)     , 5);
}

void StringTest::testRFind()
{
    Pt::String s(L"abc-abc");
    Pt::String t(L"bc");
    Pt::Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

    PT_UNIT_ASSERT_EQUALS(s.rfind(t)          , 5);
    PT_UNIT_ASSERT_EQUALS(s.rfind(t, 2)       , 1);
    PT_UNIT_ASSERT_EQUALS(s.rfind(L"bc")      , 5);
    PT_UNIT_ASSERT_EQUALS(s.rfind(L"bc", 2)   , 1);
    PT_UNIT_ASSERT_EQUALS(s.rfind(abcd, 2, 3) , 0);
    PT_UNIT_ASSERT_EQUALS(s.rfind('b')        , 5);
    PT_UNIT_ASSERT_EQUALS(s.rfind('b', 2)     , 1);
}

void StringTest::testFindFirstOf()
{
    Pt::String s(L"abc-abc");
    Pt::String t(L"a-x");
    Pt::Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

    PT_UNIT_ASSERT_EQUALS(s.find_first_of(t)          , 0);
    PT_UNIT_ASSERT_EQUALS(s.find_first_of(t, 2)       , 3);
    PT_UNIT_ASSERT_EQUALS(s.find_first_of(L"bc")      , 1);
    PT_UNIT_ASSERT_EQUALS(s.find_first_of(L"bc", 2)   , 2);
    PT_UNIT_ASSERT_EQUALS(s.find_first_of(abcd, 2, 3) , 2);
    PT_UNIT_ASSERT_EQUALS(s.find_first_of('b')        , 1);
    PT_UNIT_ASSERT_EQUALS(s.find_first_of('b', 2)     , 5);
}

void StringTest::testFindLastOf()
{
    Pt::String s(L"abc-abc");
    Pt::String t(L"a-x");
    Pt::Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

    PT_UNIT_ASSERT_EQUALS(s.find_last_of(t)          , 4);
    PT_UNIT_ASSERT_EQUALS(s.find_last_of(t, 2)       , 0);
    PT_UNIT_ASSERT_EQUALS(s.find_last_of(L"bc")      , 6);
    PT_UNIT_ASSERT_EQUALS(s.find_last_of(L"bc", 2)   , 2);
    PT_UNIT_ASSERT_EQUALS(s.find_last_of(abcd, 2, 3) , 2);
    PT_UNIT_ASSERT_EQUALS(s.find_last_of('b')        , 5);
    PT_UNIT_ASSERT_EQUALS(s.find_last_of('b', 2)     , 1);
}

void StringTest::testFindFirstNotOf()
{
    Pt::String s(L"abc-abc");
    Pt::String t(L"a-x");
    Pt::Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

    PT_UNIT_ASSERT_EQUALS(s.find_first_not_of(t)          , 1);
    PT_UNIT_ASSERT_EQUALS(s.find_first_not_of(t, 2)       , 2);
    PT_UNIT_ASSERT_EQUALS(s.find_first_not_of(L"bc")      , 0);
    PT_UNIT_ASSERT_EQUALS(s.find_first_not_of(L"bc", 2)   , 3);
    PT_UNIT_ASSERT_EQUALS(s.find_first_not_of(abcd, 2, 3) , 3);
    PT_UNIT_ASSERT_EQUALS(s.find_first_not_of('b')        , 0);
    PT_UNIT_ASSERT_EQUALS(s.find_first_not_of('b', 2)     , 2);
}

void StringTest::testFindLastNotOf()
{
    Pt::String s(L"abc-abc");
    Pt::String t(L"a-x");
    Pt::Char   abcd[] = { 'a', 'b', 'c', 'd', '\0' };

    PT_UNIT_ASSERT_EQUALS(s.find_last_not_of(t)          , 6);
    PT_UNIT_ASSERT_EQUALS(s.find_last_not_of(t, 2)       , 2);
    PT_UNIT_ASSERT_EQUALS(s.find_last_not_of(L"bc")      , 4);
    PT_UNIT_ASSERT_EQUALS(s.find_last_not_of(L"bc", 2)   , 0);
    PT_UNIT_ASSERT_EQUALS(s.find_last_not_of(abcd, 2, 3) , Pt::String::npos);
    PT_UNIT_ASSERT_EQUALS(s.find_last_not_of('b')        , 6);
    PT_UNIT_ASSERT_EQUALS(s.find_last_not_of('b', 2)     , 2);
}

void StringTest::testCStr()
{
    Pt::String s1(L"abc");
    PT_UNIT_ASSERT(s1.c_str()[0] == 'a' && s1.c_str()[1] == 'b' && s1.c_str()[2] == 'c' && s1.c_str()[3] == '\0');

    Pt::String s2;
    PT_UNIT_ASSERT(s2.c_str()[0] == '\0');

    Pt::Char abc[] = { 'a', 'b', 'c', '\0' };
    Pt::String s3(abc);
    PT_UNIT_ASSERT(s3.c_str()[0] == 'a' && s3.c_str()[1] == 'b' && s3.c_str()[2] == 'c' && s3.c_str()[3] == '\0');

    Pt::Char zero[] = { '\0' };
    Pt::String s4(zero);
    PT_UNIT_ASSERT(s4.c_str()[0] == '\0');
}

void StringTest::testSubstr()
{
    Pt::String s(L"abcdefg");

    PT_UNIT_ASSERT(s.substr()     == L"abcdefg");
    PT_UNIT_ASSERT(s.substr(2)    == L"cdefg");
    PT_UNIT_ASSERT(s.substr(2, 3) == L"cde");
}

void StringTest::testSwap()
{
    Pt::String s1(L"abc");
    Pt::String s2(L"xyz");

    PT_UNIT_ASSERT(s1 == L"abc");
    PT_UNIT_ASSERT(s2 == L"xyz");

    s1.swap(s2);

    PT_UNIT_ASSERT(s1 == L"xyz");
    PT_UNIT_ASSERT(s2 == L"abc");

    s2.swap(s1);

    PT_UNIT_ASSERT(s1 == L"abc");
    PT_UNIT_ASSERT(s2 == L"xyz");
}

void StringTest::testIndexOperator()
{
    Pt::String s(L"abcdef");

    PT_UNIT_ASSERT(s[0] == 'a');
    PT_UNIT_ASSERT(s[5] == 'f');
    PT_UNIT_ASSERT(s[6] == '\0');

/*    bool exceptionOccured = false;
    try {
        s[10];
    } catch (const Pt::Exception& e) {
        exceptionOccured = true;
    }*/
}

void StringTest::testAt()
{
    Pt::String s(L"abcdef");

    PT_UNIT_ASSERT(s.at(0) == 'a');
    PT_UNIT_ASSERT(s.at(5) == 'f');
}

void StringTest::testPushBack()
{
    Pt::String s(L"abc");

    s.push_back('d');
    PT_UNIT_ASSERT(s == L"abcd");
}

void StringTest::testCopy()
{
    Pt::Char t1[3];
    Pt::String s(L"abcd");

    s.copy(t1, 2);
    t1[2] = '\0';

    const Pt::Char c1[] = { 'a', 'b', '\0' };
    PT_UNIT_ASSERT(std::char_traits<Pt::Char>::compare(t1, c1, 3) == 0);


    Pt::Char t2[5];
    s.copy(t2, 4);
    t2[4] = '\0';

    const Pt::Char c2[] = { 'a', 'b', 'c', 'd', '\0' };
    PT_UNIT_ASSERT_EQUALS(std::char_traits<Pt::Char>::compare(t2, c2, 5) , 0);


    Pt::Char t3[3];
    s.copy(t3, 2, 2);
    t3[2] = '\0';

    const Pt::Char c3[] = { 'c', 'd', '\0' };
    PT_UNIT_ASSERT_EQUALS(std::char_traits<Pt::Char>::compare(t3, c3, 3) , 0);
}


void StringTest::testReserve()
{
    const Pt::Char c1[] = { 'a', 'b', 'c', 'd', '\0' };
    Pt::String s(L"abcd");
    Pt::String s2 = s;
    s2.reserve(10);

    PT_UNIT_ASSERT( s2.capacity() >= 10 );
    PT_UNIT_ASSERT( s2.size() == 4 );
    PT_UNIT_ASSERT_EQUALS( std::char_traits<Pt::Char>::compare(s2.c_str(), c1, 4) , 0 );

    PT_UNIT_ASSERT( s.capacity() >= 4 );
    PT_UNIT_ASSERT_EQUALS( s.size(), 4 );
    PT_UNIT_ASSERT_EQUALS( std::char_traits<Pt::Char>::compare(s.c_str(), c1, 4) , 0 );
}


void StringTest::testReserveEmpty()
{
    Pt::String s;
    s.reserve(0);
    PT_UNIT_ASSERT( s.capacity() >= 0 );
    PT_UNIT_ASSERT_EQUALS( s.size() , 0 );
}


void StringTest::testLengthAndSize()
{
    Pt::String s1;
    PT_UNIT_ASSERT_EQUALS(s1.length() , 0);
    PT_UNIT_ASSERT_EQUALS(s1.size()   , 0);

    Pt::String s2(L"ab");
    PT_UNIT_ASSERT_EQUALS(s2.length() , 2);
    PT_UNIT_ASSERT_EQUALS(s2.size()   , 2);

    s2 += L"cd";
    PT_UNIT_ASSERT_EQUALS(s2.length() , 4);
    PT_UNIT_ASSERT_EQUALS(s2.size()   , 4);


    Pt::Char ab[] = { 'a', 'b', '\0' };
    Pt::String s3(ab);
    PT_UNIT_ASSERT_EQUALS(s3.length() , 2);
    PT_UNIT_ASSERT_EQUALS(s3.size()   , 2);

    Pt::Char cd[] = { 'c', 'd', '\0' };
    s3 += cd;
    PT_UNIT_ASSERT_EQUALS(s3.length() , 4);
    PT_UNIT_ASSERT_EQUALS(s3.size()   , 4);
}
