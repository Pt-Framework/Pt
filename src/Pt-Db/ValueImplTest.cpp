/*
 * Copyright (C) 2006 PTV AG
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
#undef PT_DB_API_EXPORT

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>
#include <limits>

#include "ValueImpl.h"
//#include "Pt/Variant.h"
#include "Pt/Date.h"
#include "Pt/Time.h"
#include "Pt/DateTime.h"

using namespace std;
using namespace Pt;
using namespace Pt::Db;


class ValueImplTest : public Pt::Unit::TestSuite
{
public:
    ValueImplTest()
    : TestSuite("ValueImplTest")
    {
        Pt::Unit::TestSuite::registerMethod("Blob", *this, &ValueImplTest::Blob);
        Pt::Unit::TestSuite::registerMethod("testIsNull", *this, &ValueImplTest::testIsNull);
        Pt::Unit::TestSuite::registerMethod("testBool", *this, &ValueImplTest::testBool);
        Pt::Unit::TestSuite::registerMethod("testInt", *this, &ValueImplTest::testInt);
        Pt::Unit::TestSuite::registerMethod("testUnsigned", *this, &ValueImplTest::testUnsigned);
        Pt::Unit::TestSuite::registerMethod("testFloat", *this, &ValueImplTest::testFloat);
        Pt::Unit::TestSuite::registerMethod("testDouble", *this, &ValueImplTest::testDouble);
        Pt::Unit::TestSuite::registerMethod("testChar", *this, &ValueImplTest::testChar);
        Pt::Unit::TestSuite::registerMethod("testString", *this, &ValueImplTest::testString);
        Pt::Unit::TestSuite::registerMethod("testDate", *this, &ValueImplTest::testDate);
        Pt::Unit::TestSuite::registerMethod("testTime", *this, &ValueImplTest::testTime);
        Pt::Unit::TestSuite::registerMethod("testDateTime", *this, &ValueImplTest::testDateTime);
    }

protected:
    void Blob();
    void testIsNull();
    void testBool();
    void testInt();
    void testUnsigned();
    void testFloat();
    void testDouble();
    void testChar();
    void testString();
    void testDate();
    void testTime();
    void testDateTime();

};


Pt::Unit::RegisterTest<ValueImplTest> register_ValueImplTest;

void ValueImplTest::Blob()
{
    Pt::Db::Blob blob("Hello", 5);
    PT_UNIT_ASSERT(blob.size() == 5);
    PT_UNIT_ASSERT( std::strncmp(blob.data(), "Hello", 5) == 0);

    Pt::Db::Blob blob2;
    PT_UNIT_ASSERT(blob2.size() == 0);
    PT_UNIT_ASSERT(blob2.data() == 0);	

	blob2 = blob;
    PT_UNIT_ASSERT(blob2.size() == 5);
    PT_UNIT_ASSERT( std::strncmp(blob2.data(), "Hello", 5) == 0);

	blob.assign("World!", 6);
    PT_UNIT_ASSERT(blob2.size() == 5);
    PT_UNIT_ASSERT( std::strncmp(blob2.data(), "Hello", 5) == 0);
    PT_UNIT_ASSERT(blob.size() == 6);
    PT_UNIT_ASSERT( std::strncmp(blob.data(), "World!", 6) == 0);

    ValueImpl impl("Hello world!", 12);
    impl.getBlob(blob);
    PT_UNIT_ASSERT(blob.size() == 12);
    PT_UNIT_ASSERT( std::strncmp(blob.data(), "Hello world!", 12) == 0);
}

void ValueImplTest::testIsNull()
{
    ValueImpl valImp;
    PT_UNIT_ASSERT( valImp.isNull() );
}

void ValueImplTest::testBool()
{
    ValueImpl valImp("true");
    PT_UNIT_ASSERT( valImp.getBool() == true );

    ValueImpl valImp2("T");
    PT_UNIT_ASSERT( valImp.getBool() == true );

    ValueImpl valImp3("1");
    PT_UNIT_ASSERT( valImp.getBool() == true );

    ValueImpl valImp4("Yes");
    PT_UNIT_ASSERT( valImp.getBool() == true );

    ValueImpl valImp5("y");
    PT_UNIT_ASSERT( valImp.getBool() == true );
}

void ValueImplTest::testInt()
{
    ValueImpl valImp("22");
    PT_UNIT_ASSERT( valImp.getInt() == 22 );

    ValueImpl valImp2("-42");
    PT_UNIT_ASSERT( valImp2.getInt() == -42 );
}

void ValueImplTest::testUnsigned()
{
    ValueImpl valImp("478");
    PT_UNIT_ASSERT( valImp.getInt() == 478 );
}

void ValueImplTest::testFloat()
{
    ValueImpl valImp("321.125");
    PT_UNIT_ASSERT( valImp.getFloat() == 321.125f );
}

void ValueImplTest::testDouble()
{
    ValueImpl valImp("321.580078125");
    PT_UNIT_ASSERT( valImp.getDouble() == 321.580078125 );
}

void ValueImplTest::testChar()
{
    ValueImpl valImp("c");
    PT_UNIT_ASSERT( valImp.getChar() == 'c' );
}

void ValueImplTest::testString()
{
    ValueImpl valImp("Frag den Chemiker");
    std::string str;
    valImp.getString(str);
    PT_UNIT_ASSERT( str == "Frag den Chemiker" );
}

void ValueImplTest::testDate()
{
    Date date(2001, 11, 30);
    ValueImpl valImp(date.toIsoString());
    PT_UNIT_ASSERT( valImp.getDate() == date );
}

void ValueImplTest::testTime()
{
    Time time(11, 55, 59, 999);
    ValueImpl valImp(time.toIsoString());
    PT_UNIT_ASSERT( valImp.getTime() == time );
}

void ValueImplTest::testDateTime()
{
    DateTime dateTime(2001, 11, 30, 11, 55, 59, 999);
    ValueImpl valImp(dateTime.toIsoString());
    PT_UNIT_ASSERT( valImp.getDateTime() == dateTime );
}
