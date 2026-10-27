/* Copyright (C) 2016 Marc Boris Duerner

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   As a special exception, you may use this file as part of a free
   software library without restriction. Specifically, if other files
   instantiate templates or use macros or inline functions from this
   file, or you compile this file and link it with other files to
   produce an executable, this file does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License. This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU Library
   General Public License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA  02110-1301  USA
*/

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <Pt/String.h>
#include <Pt/Utf8.h>
#include <algorithm>
#include <iterator>
#include <string>

class Utf8Test : public Pt::Unit::TestSuite
{
    public:
        Utf8Test()
        : Pt::Unit::TestSuite("Utf8Test")
        , _oe(0366)
        , _utf8("Hell\303\266 W\303\266rld!")
        {
            Pt::Unit::TestSuite::registerMethod( "Utf8Iterator",
                                                 *this, &Utf8Test::Utf8Iterator);

            Pt::Unit::TestSuite::registerMethod( "Utf8Appender",
                                                 *this, &Utf8Test::Utf8Appender);

            Pt::Unit::TestSuite::registerMethod( "Utf8ConvertFromBytes",
                                                 *this, &Utf8Test::Utf8ConvertFromBytes);

            Pt::Unit::TestSuite::registerMethod( "Utf8ConvertToBytes",
                                                 *this, &Utf8Test::Utf8ConvertToBytes);

            Pt::Unit::TestSuite::registerMethod( "IsValidUtf8",
                                                 *this, &Utf8Test::IsValidUtf8);

            Pt::Unit::TestSuite::registerMethod( "Utf8Validator",
                                                 *this, &Utf8Test::Utf8Validator);
        }

    protected:
        void Utf8Iterator();
        void Utf8Appender();
        void Utf8ConvertToBytes();
        void Utf8ConvertFromBytes();
        void IsValidUtf8();
        void Utf8Validator();

    private:
        const Pt::Char    _oe;
        const std::string _utf8;
};

Pt::Unit::RegisterTest<Utf8Test> _registerUtf8Test;


void Utf8Test::Utf8Iterator()
{
    Pt::Utf8Iterator it(_utf8);
    Pt::Utf8Iterator end;

    int n = std::count(it, end, _oe);
    PT_UNIT_ASSERT_EQUAL(n, 2);
}


void Utf8Test::Utf8Appender()
{
    std::string s;
    Pt::Utf8Appender a(s);

    std::fill_n(a, 3, _oe);
    PT_UNIT_ASSERT_EQUAL(s, "\303\266\303\266\303\266");
}


void Utf8Test::Utf8ConvertToBytes()
{
    Pt::Utf8Codec codec(1);
    Pt::Utf8Convert conv(&codec);

    std::string bytes = conv.toBytes(_utf8);

    PT_UNIT_ASSERT_EQUAL(bytes, _utf8);
}


void Utf8Test::Utf8ConvertFromBytes()
{
    Pt::Utf8Convert conv(new Pt::Utf8Codec);

    std::string s = conv.fromBytes(_utf8);

    PT_UNIT_ASSERT_EQUAL(s, _utf8);
}


void Utf8Test::IsValidUtf8()
{
    PT_UNIT_ASSERT( Pt::isValidUtf8("", 0) );
    PT_UNIT_ASSERT( Pt::isValidUtf8("abc", 3) );
    PT_UNIT_ASSERT( Pt::isValidUtf8("\xC3\xA9", 2) );
    PT_UNIT_ASSERT( Pt::isValidUtf8("\xE2\x82\xAC", 3) );
    PT_UNIT_ASSERT( Pt::isValidUtf8("\xF0\x9F\x98\x80", 4) );

    PT_UNIT_ASSERT( ! Pt::isValidUtf8("\x80", 1) );
    PT_UNIT_ASSERT( ! Pt::isValidUtf8("\xC3", 1) );
    PT_UNIT_ASSERT( ! Pt::isValidUtf8("\xC0\x80", 2) );
    PT_UNIT_ASSERT( ! Pt::isValidUtf8("\xE0\x9F\x80", 3) );
    PT_UNIT_ASSERT( ! Pt::isValidUtf8("\xED\xA0\x80", 3) );
    PT_UNIT_ASSERT( ! Pt::isValidUtf8("\xF0\x80\x80\x80", 4) );
    PT_UNIT_ASSERT( ! Pt::isValidUtf8("\xF4\x90\x80\x80", 4) );
    PT_UNIT_ASSERT( ! Pt::isValidUtf8("\xF5\x80\x80\x80", 4) );
}


void Utf8Test::Utf8Validator()
{
    Pt::Utf8Validator v;
    PT_UNIT_ASSERT( v.advance("\xC3", 1) );
    PT_UNIT_ASSERT( ! v.finish() );
    PT_UNIT_ASSERT( v.advance("\xA9", 1) );
    PT_UNIT_ASSERT( v.finish() );

    v.reset();
    PT_UNIT_ASSERT( ! v.advance("\x80", 1) );
    PT_UNIT_ASSERT( ! v.advance("a", 1) );
    PT_UNIT_ASSERT( ! v.finish() );

    v.reset();
    PT_UNIT_ASSERT( v.finish() );
    PT_UNIT_ASSERT( v.advance("ok", 2) );
    PT_UNIT_ASSERT( v.finish() );
}
