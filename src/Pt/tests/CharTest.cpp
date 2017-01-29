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

#include <Pt/Api.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <Pt/String.h>

class CharTest : public Pt::Unit::TestSuite
{
    public:
        CharTest()
        : Pt::Unit::TestSuite("CharTest")
        {
            Pt::Unit::TestSuite::registerMethod("Null", *this, &CharTest::Null);
            Pt::Unit::TestSuite::registerMethod("ToLower", *this, &CharTest::ToLower);
            Pt::Unit::TestSuite::registerMethod("ToUpper", *this, &CharTest::ToUpper);
            Pt::Unit::TestSuite::registerMethod("CType", *this, &CharTest::CType);
        }

    protected:
        void Null();
        void ToLower();
        void ToUpper();
        void CType();
};

Pt::Unit::RegisterTest<CharTest> _registerCharTest;


void CharTest::Null()
{
    PT_UNIT_ASSERT(Pt::Char() == 0);
}


void CharTest::ToLower()
{
    Pt::Char ch('A');
    Pt::Char l = Pt::tolower(ch);
    PT_UNIT_ASSERT(l == 97);

}

void CharTest::ToUpper()
{
    Pt::Char ch(0xDF);
    Pt::Char u = Pt::toupper(ch);
    PT_UNIT_ASSERT(u == 65);
}

void CharTest::CType()
{
    Pt::Char ch;

    ch = 'a';
    PT_UNIT_ASSERT( isalpha(ch));
    PT_UNIT_ASSERT( isalnum(ch));
    PT_UNIT_ASSERT(!isdigit(ch));
    PT_UNIT_ASSERT( islower(ch));
    PT_UNIT_ASSERT(!isupper(ch));
    PT_UNIT_ASSERT(!iscntrl(ch));
    PT_UNIT_ASSERT( isgraph(ch));
    PT_UNIT_ASSERT( isprint(ch));
    PT_UNIT_ASSERT(!ispunct(ch));
    PT_UNIT_ASSERT(!isspace(ch));

    ch = 'A';
    PT_UNIT_ASSERT( isalpha(ch));
    PT_UNIT_ASSERT( isalnum(ch));
    PT_UNIT_ASSERT(!isdigit(ch));
    PT_UNIT_ASSERT(!islower(ch));
    PT_UNIT_ASSERT( isupper(ch));
    PT_UNIT_ASSERT(!iscntrl(ch));
    PT_UNIT_ASSERT( isgraph(ch));
    PT_UNIT_ASSERT( isprint(ch));
    PT_UNIT_ASSERT(!ispunct(ch));
    PT_UNIT_ASSERT(!isspace(ch));

    ch = wchar_t(248);
    PT_UNIT_ASSERT( isalpha(ch));
    PT_UNIT_ASSERT( isalnum(ch));
    PT_UNIT_ASSERT(!isdigit(ch));
    PT_UNIT_ASSERT( islower(ch));
    PT_UNIT_ASSERT(!isupper(ch));
    PT_UNIT_ASSERT(!iscntrl(ch));
    PT_UNIT_ASSERT( isgraph(ch));
    PT_UNIT_ASSERT( isprint(ch));
    PT_UNIT_ASSERT(!ispunct(ch));
    PT_UNIT_ASSERT(!isspace(ch));

    ch = toupper(ch);
    PT_UNIT_ASSERT( isalpha(ch));
    PT_UNIT_ASSERT( isalnum(ch));
    PT_UNIT_ASSERT(!isdigit(ch));
    PT_UNIT_ASSERT(!islower(ch));
    PT_UNIT_ASSERT( isupper(ch));
    PT_UNIT_ASSERT(!iscntrl(ch));
    PT_UNIT_ASSERT( isgraph(ch));
    PT_UNIT_ASSERT( isprint(ch));
    PT_UNIT_ASSERT(!ispunct(ch));
    PT_UNIT_ASSERT(!isspace(ch));

    ch = '1';
    PT_UNIT_ASSERT(!isalpha(ch));
    PT_UNIT_ASSERT( isalnum(ch));
    PT_UNIT_ASSERT( isdigit(ch));
    PT_UNIT_ASSERT(!islower(ch));
    PT_UNIT_ASSERT(!isupper(ch));
    PT_UNIT_ASSERT(!iscntrl(ch));
    PT_UNIT_ASSERT( isgraph(ch));
    PT_UNIT_ASSERT( isprint(ch));
    PT_UNIT_ASSERT(!ispunct(ch));
    PT_UNIT_ASSERT(!isspace(ch));

    ch = '\t';
    PT_UNIT_ASSERT(!isalpha(ch));
    PT_UNIT_ASSERT(!isalnum(ch));
    PT_UNIT_ASSERT(!isdigit(ch));
    PT_UNIT_ASSERT(!islower(ch));
    PT_UNIT_ASSERT(!isupper(ch));
    PT_UNIT_ASSERT( iscntrl(ch));
    PT_UNIT_ASSERT(!isgraph(ch));
    PT_UNIT_ASSERT(!isprint(ch));
    PT_UNIT_ASSERT(!ispunct(ch));
    PT_UNIT_ASSERT( isspace(ch));

    ch = ' ';
    PT_UNIT_ASSERT(!isalpha(ch));
    PT_UNIT_ASSERT(!isalnum(ch));
    PT_UNIT_ASSERT(!isdigit(ch));
    PT_UNIT_ASSERT(!islower(ch));
    PT_UNIT_ASSERT(!isupper(ch));
    PT_UNIT_ASSERT(!iscntrl(ch));
    PT_UNIT_ASSERT(!isgraph(ch));
    PT_UNIT_ASSERT( isprint(ch));
    PT_UNIT_ASSERT(!ispunct(ch));
    PT_UNIT_ASSERT( isspace(ch));

    ch = ':';
    PT_UNIT_ASSERT(!isalpha(ch));
    PT_UNIT_ASSERT(!isalnum(ch));
    PT_UNIT_ASSERT(!isdigit(ch));
    PT_UNIT_ASSERT(!islower(ch));
    PT_UNIT_ASSERT(!isupper(ch));
    PT_UNIT_ASSERT(!iscntrl(ch));
    PT_UNIT_ASSERT( isgraph(ch));
    PT_UNIT_ASSERT( isprint(ch));
    PT_UNIT_ASSERT( ispunct(ch));
    PT_UNIT_ASSERT(!isspace(ch));

    ch = '+';
    PT_UNIT_ASSERT(!isalpha(ch));
    PT_UNIT_ASSERT(!isalnum(ch));
    PT_UNIT_ASSERT(!isdigit(ch));
    PT_UNIT_ASSERT(!islower(ch));
    PT_UNIT_ASSERT(!isupper(ch));
    PT_UNIT_ASSERT(!iscntrl(ch));
    PT_UNIT_ASSERT( isgraph(ch));
    PT_UNIT_ASSERT( isprint(ch));
    PT_UNIT_ASSERT( ispunct(ch));
    PT_UNIT_ASSERT(!isspace(ch));
}
