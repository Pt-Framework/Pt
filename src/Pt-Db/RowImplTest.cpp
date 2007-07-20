/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/
#undef PT_DB_API_EXPORT

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>
#include <sstream>
#include <limits>

#include "RowImpl.h"
#include "ValueImpl.h"

#include "Pt/Db/Value.h"

using namespace std;
using namespace Pt;
using namespace Pt::Db;


class RowImplTest : public Pt::Unit::TestSuite
{
public:
    RowImplTest()
    : TestSuite("RowImplTest")
    {
        Pt::Unit::TestSuite::registerMethod("testSize", *this, &RowImplTest::testSize);
        Pt::Unit::TestSuite::registerMethod("testGetValue", *this, &RowImplTest::testGetValue);
    }

protected:
    void testSize();
    void testGetValue();

};


Pt::Unit::RegisterTest<RowImplTest> register_RowImplTest;


void RowImplTest::testSize()
{
    RowImpl rowImpl;

    for(unsigned int i=0; i<25; i++)
    {
        std::stringstream ss;
        ss << i;

        ValueImpl* impl = new ValueImpl( ss.str() );
        Value v(impl);
        rowImpl.add( v );
    }

    PT_UNIT_ASSERT( rowImpl.size() == 25 );
}


void RowImplTest::testGetValue()
{
    RowImpl rowImpl;

    for(unsigned int i=0; i<25; i++)
    {
        std::stringstream ss;
        ss << i;

        ValueImpl* impl = new ValueImpl( ss.str() );
        Value v(impl);
        rowImpl.add( v );
    }

    for(unsigned int i=24; i<=0; i--)
    {
        PT_UNIT_ASSERT( rowImpl.getValue(i) );
    }
}
