/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestMain.h"
#include "Pt/Time.h"
#include <string>
#include <iostream>

using namespace Pt;
using namespace std;


class TimeTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TimeTest );
	CPPUNIT_TEST( testNull );
	CPPUNIT_TEST( testAssign );
	CPPUNIT_TEST( testIsoConvert );

	CPPUNIT_TEST_SUITE_END();

protected:
	void testNull();
	void testAssign();
	void testIsoConvert();
};


CPPUNIT_TEST_SUITE_REGISTRATION( TimeTest );


void TimeTest::testNull()
{
	Time time;
	CPPUNIT_ASSERT( time.isNull() );
}

void TimeTest::testAssign()
{
	Time time(12, 45, 23, 956);
	CPPUNIT_ASSERT( !time.isNull() );
	CPPUNIT_ASSERT( time.hours() == 12 );
	CPPUNIT_ASSERT( time.minutes() == 45 );
	CPPUNIT_ASSERT( time.seconds() == 23 );
	CPPUNIT_ASSERT( time.msecs() == 956 );

	time.set(23, 59, 59, 999);
	CPPUNIT_ASSERT( !time.isNull() );
	CPPUNIT_ASSERT( time.hours() == 23 );
	CPPUNIT_ASSERT( time.minutes() == 59 );
	CPPUNIT_ASSERT( time.seconds() == 59 );
	CPPUNIT_ASSERT( time.msecs() == 999 );
}


void TimeTest::testIsoConvert()
{
	Time time(12, 45, 23, 956);
	std::string isoString = time.toIsoString();
	CPPUNIT_ASSERT( isoString == "12:45:23.956" );

	time = Time::fromIsoString("23:59:59.999");
	CPPUNIT_ASSERT( !time.isNull() );
	CPPUNIT_ASSERT( time.hours() == 23 );
	CPPUNIT_ASSERT( time.minutes() == 59 );
	CPPUNIT_ASSERT( time.seconds() == 59 );
	CPPUNIT_ASSERT( time.msecs() == 999 );
}

