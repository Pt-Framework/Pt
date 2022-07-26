/*
 * Copyright (C) 2022 Marc Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA 02110-1301 USA
 */

#include <Pt/System/UriQuery.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

class UriQueryTest : public Pt::Unit::TestSuite
{
    public:
        UriQueryTest()
        : Pt::Unit::TestSuite("UriQueryTest")
        {
            registerMethod("ParseUnreserved", *this, &UriQueryTest::ParseUnreserved);
            registerMethod("ParseEmpty", *this, &UriQueryTest::ParseEmpty);
            registerMethod("ParseSpace", *this, &UriQueryTest::ParseSpace);
            registerMethod("ParseMultiQuery", *this, &UriQueryTest::ParseMultiQuery);
            registerMethod("ParsePercentEncoded", *this, &UriQueryTest::ParsePercentEncoded);
            registerMethod("FormatUnreserved", *this, &UriQueryTest::FormatUnreserved);
            registerMethod("FormatPercentEncoded", *this, &UriQueryTest::FormatPercentEncoded);
            registerMethod("CompareQuery", *this, &UriQueryTest::CompareQuery);
        }

        void ParseUnreserved()
        {
            Pt::System::UriQuery query("a.b-c=_1~");
            PT_UNIT_ASSERT( query.hasField("a.b-c") );
            PT_UNIT_ASSERT_EQUALS(query.field("a.b-c"), "_1~");
        }
        
        void ParseEmpty()
        {
            Pt::System::UriQuery query("abc&klm=1&xyz");
            PT_UNIT_ASSERT( query.hasField("abc") );
            PT_UNIT_ASSERT( query.hasField("klm") );
            PT_UNIT_ASSERT( query.hasField("xyz") );
        }

        void ParseSpace()
        {
            Pt::System::UriQuery query("ab+c=1+");
            PT_UNIT_ASSERT( query.hasField("ab c") );
            PT_UNIT_ASSERT_EQUALS(query.field("ab c"), "1 ");
        }

        void ParseMultiQuery()
        {
            Pt::System::UriQuery query("abc=1&xyz=42");
            PT_UNIT_ASSERT( query.hasField("abc") );
            PT_UNIT_ASSERT( query.hasField("xyz") );

            PT_UNIT_ASSERT_EQUALS(query.field("abc"), "1");
            PT_UNIT_ASSERT_EQUALS(query.field("xyz"), "42");
        }

        void ParsePercentEncoded()
        {
            Pt::System::UriQuery query("abc=%2f%2A");
            PT_UNIT_ASSERT( query.hasField("abc") );

            PT_UNIT_ASSERT_EQUALS(query.field("abc"), "/*");
        }

        void FormatUnreserved()
        {
            Pt::System::UriQuery query;
            query.setField("a.b-c", "_1~");
            std::string s = query.toEncoded();
            PT_UNIT_ASSERT_EQUALS(s, "a.b-c=_1~");
        }

        void FormatPercentEncoded()
        {
            Pt::System::UriQuery query;
            query.setField("abc", "/*");
            std::string s = query.toEncoded();
            PT_UNIT_ASSERT_EQUALS(s, "abc=%2f%2a");
        }

        void CompareQuery()
        {
            Pt::System::UriQuery query1;
            Pt::System::UriQuery query2;
            PT_UNIT_ASSERT(query1 == query2);

            query2.setField("abc", "123");
            PT_UNIT_ASSERT(query1 != query2);
        }
};

Pt::Unit::RegisterTest<UriQueryTest> register_UriQueryTest;
