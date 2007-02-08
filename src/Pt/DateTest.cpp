/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestMain.h"
#include "Pt/Date.h"
#include <string>
#include <iostream>

using namespace Pt;
using namespace std;


class DateTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( DateTest );
	CPPUNIT_TEST( testNull );
	CPPUNIT_TEST( testAssign );
	CPPUNIT_TEST( testIsoConvert );

	CPPUNIT_TEST_SUITE_END();

protected:
	void testNull();
	void testAssign();
	void testIsoConvert();
};


CPPUNIT_TEST_SUITE_REGISTRATION( DateTest );


void DateTest::testNull()
{
	Date date;
	CPPUNIT_ASSERT( date.isNull() );
}

void DateTest::testAssign()
{
	Date date(2001, 11, 15);
	CPPUNIT_ASSERT( !date.isNull() );
	CPPUNIT_ASSERT( date.years() == 2001 );
	CPPUNIT_ASSERT( date.months() == 11 );
	CPPUNIT_ASSERT( date.days() == 15 );

	date.set(1789, 5, 12);
	CPPUNIT_ASSERT( !date.isNull() );
	CPPUNIT_ASSERT( date.years() == 1789 );
	CPPUNIT_ASSERT( date.months() == 5 );
	CPPUNIT_ASSERT( date.days() == 12 );
}


void DateTest::testIsoConvert()
{
	Date date(2001, 11, 15);
	std::string isoString = date.toIsoString();
	CPPUNIT_ASSERT( isoString == "2001-11-15" );

	date = Date::fromIsoString("1789-05-12");
	CPPUNIT_ASSERT( !date.isNull() );
	CPPUNIT_ASSERT( date.years() == 1789 );
	CPPUNIT_ASSERT( date.months() == 5 );
	CPPUNIT_ASSERT( date.days() == 12 );
}

