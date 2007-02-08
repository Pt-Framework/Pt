/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestMain.h"
#include "Pt/DateTime.h"
#include <string>
#include <iostream>

using namespace Pt;
using namespace std;


class DateTimeTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( DateTimeTest );
	CPPUNIT_TEST( testNull );
	CPPUNIT_TEST( testAssign );
	CPPUNIT_TEST( testIsoConvert );

	CPPUNIT_TEST_SUITE_END();

protected:
	void testNull();
	void testAssign();
	void testIsoConvert();
};


CPPUNIT_TEST_SUITE_REGISTRATION( DateTimeTest );


void DateTimeTest::testNull()
{
	DateTime dt;
	CPPUNIT_ASSERT( dt.isNull() );

	Date date;
	Time time;
	DateTime dt2(date, time);
	CPPUNIT_ASSERT( dt.isNull() );
}

void DateTimeTest::testAssign()
{
	Date date(2001, 11, 15);
	Time time(12, 45, 23, 956);
	DateTime dt(date, time);
	CPPUNIT_ASSERT( !dt.isNull() );
	CPPUNIT_ASSERT( dt.years() == 2001 );
	CPPUNIT_ASSERT( dt.months() == 11 );
	CPPUNIT_ASSERT( dt.days() == 15 );
	CPPUNIT_ASSERT( dt.hours() == 12 );
	CPPUNIT_ASSERT( dt.minutes() == 45 );
	CPPUNIT_ASSERT( dt.seconds() == 23 );
	CPPUNIT_ASSERT( dt.msecs() == 956 );

	dt.set(1789, 5, 12, 23, 59, 59, 999);
	CPPUNIT_ASSERT( !dt.isNull() );
	CPPUNIT_ASSERT( dt.years() == 1789 );
	CPPUNIT_ASSERT( dt.months() == 5 );
	CPPUNIT_ASSERT( dt.days() == 12 );
	CPPUNIT_ASSERT( dt.hours() == 23 );
	CPPUNIT_ASSERT( dt.minutes() == 59 );
	CPPUNIT_ASSERT( dt.seconds() == 59 );
	CPPUNIT_ASSERT( dt.msecs() == 999 );
}


void DateTimeTest::testIsoConvert()
{
	Date date(2001, 11, 15);
	Time time(12, 45, 23, 956);
	DateTime dt(date, time);
	std::string isoString = dt.toIsoString();
	CPPUNIT_ASSERT( isoString == "2001-11-15 12:45:23.956" );

	dt = DateTime::fromIsoString("1789-05-12 23:59:59.999");
	CPPUNIT_ASSERT( !dt.isNull() );
	CPPUNIT_ASSERT( dt.years() == 1789 );
	CPPUNIT_ASSERT( dt.months() == 5 );
	CPPUNIT_ASSERT( dt.days() == 12 );
	CPPUNIT_ASSERT( dt.hours() == 23 );
	CPPUNIT_ASSERT( dt.minutes() == 59 );
	CPPUNIT_ASSERT( dt.seconds() == 59 );
	CPPUNIT_ASSERT( dt.msecs() == 999 );
}

