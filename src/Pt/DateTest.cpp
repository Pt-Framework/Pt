/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/
#include "Pt/Date.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>


class DateTest : public Pt::Unit::TestSuite
{
    public:
        DateTest()
        : Pt::Unit::TestSuite("DateTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Null", *this, &DateTest::Null );
            Pt::Unit::TestSuite::registerMethod( "Assign", *this, &DateTest::Assign );
            Pt::Unit::TestSuite::registerMethod( "IsoConvert", *this, &DateTest::IsoConvert );
        }

    protected:
        void Null();
        void Assign();
        void IsoConvert();
};

Pt::Unit::RegisterTest<DateTest> register_DateTest;


void DateTest::Null()
{
	Pt::Date date;
	PT_UNIT_ASSERT( date.isNull() );
}

void DateTest::Assign()
{
	Pt::Date date(2001, 11, 15);
	PT_UNIT_ASSERT( !date.isNull() );
	PT_UNIT_ASSERT( date.years() == 2001 );
	PT_UNIT_ASSERT( date.months() == 11 );
	PT_UNIT_ASSERT( date.days() == 15 );

	date.set(1789, 5, 12);
	PT_UNIT_ASSERT( !date.isNull() );
	PT_UNIT_ASSERT( date.years() == 1789 );
	PT_UNIT_ASSERT( date.months() == 5 );
	PT_UNIT_ASSERT( date.days() == 12 );
}


void DateTest::IsoConvert()
{
	Pt::Date date(2001, 11, 15);
	std::string isoString = date.toIsoString();
	PT_UNIT_ASSERT( isoString == "2001-11-15" );

	date = Pt::Date::fromIsoString("1789-05-12");
	PT_UNIT_ASSERT( !date.isNull() );
	PT_UNIT_ASSERT( date.years() == 1789 );
	PT_UNIT_ASSERT( date.months() == 5 );
	PT_UNIT_ASSERT( date.days() == 12 );
}

