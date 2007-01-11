/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestMain.h"

#include <string>
#include <iostream>
#include <limits>

#include "ValueImpl.h"
#include "Pt/Variant.h"
#include "Pt/Date.h"
#include "Pt/Time.h"
#include "Pt/DateTime.h"

using namespace std;
using namespace Pt;
using namespace Pt::Db;


class ValueImplTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( ValueImplTest );
	
	CPPUNIT_TEST( testIsNull );
	CPPUNIT_TEST( testBool );
	CPPUNIT_TEST( testInt );
	CPPUNIT_TEST( testUnsigned );
	CPPUNIT_TEST( testFloat );
	CPPUNIT_TEST( testDouble );
	CPPUNIT_TEST( testChar );
	CPPUNIT_TEST( testString );
	CPPUNIT_TEST( testDate );
	CPPUNIT_TEST( testTime );
	CPPUNIT_TEST( testDateTime );
				
	CPPUNIT_TEST_SUITE_END();

protected:
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


CPPUNIT_TEST_SUITE_REGISTRATION( ValueImplTest );


void ValueImplTest::testIsNull()
{
	ValueImpl valImp;
	CPPUNIT_ASSERT( valImp.isNull() );
}

void ValueImplTest::testBool()
{
	ValueImpl valImp("true");
	CPPUNIT_ASSERT( valImp.getBool() == true );
	
	ValueImpl valImp2("T");
	CPPUNIT_ASSERT( valImp.getBool() == true );
	
	ValueImpl valImp3("1");
	CPPUNIT_ASSERT( valImp.getBool() == true );
	
	ValueImpl valImp4("Yes");
	CPPUNIT_ASSERT( valImp.getBool() == true );
	
	ValueImpl valImp5("y");
	CPPUNIT_ASSERT( valImp.getBool() == true );
}

void ValueImplTest::testInt()
{
	ValueImpl valImp(22);
	CPPUNIT_ASSERT( valImp.getInt() == 22 );
	
	ValueImpl valImp2(-42);
	CPPUNIT_ASSERT( valImp2.getInt() == -42 );
}

void ValueImplTest::testUnsigned()
{
	ValueImpl valImp(478);
	CPPUNIT_ASSERT( valImp.getInt() == 478 );
}

void ValueImplTest::testFloat()
{
	ValueImpl valImp(321.123f);
	CPPUNIT_ASSERT( valImp.getFloat() == 321.123f );
}

void ValueImplTest::testDouble()
{
	ValueImpl valImp(321.123456789);
	CPPUNIT_ASSERT( valImp.getDouble() == 321.123456789 );
}

void ValueImplTest::testChar()
{
	ValueImpl valImp('c');
	CPPUNIT_ASSERT( valImp.getChar() == 'c' );
}

void ValueImplTest::testString()
{
	ValueImpl valImp("Frag den Chemiker");
    std::string str;
    valImp.getString(str); 
	CPPUNIT_ASSERT( str == "Frag den Chemiker" );
}

void ValueImplTest::testDate()
{
	Date date(2001, 11, 30);
	ValueImpl valImp(date.toIsoString());
	CPPUNIT_ASSERT( valImp.getDate() == date );
}

void ValueImplTest::testTime()
{
	Time time(11, 55, 59, 999);
	ValueImpl valImp(time.toIsoString());
	CPPUNIT_ASSERT( valImp.getTime() == time );
}

void ValueImplTest::testDateTime()
{
	Date date(2001, 11, 30);
	Time time(11, 55, 59, 999);
	DateTime dateTime(date, time);
	ValueImpl valImp(dateTime.toIsoString());
	CPPUNIT_ASSERT( valImp.getDateTime() == dateTime );
}
