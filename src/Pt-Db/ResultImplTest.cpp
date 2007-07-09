/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/
#undef PT_DB_API_EXPORT

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>
#include <limits>

#include "ResultImpl.h"
#include "ValueImpl.h"
#include "RowImpl.h"

#include "Pt/Db/Row.h"

using namespace std;
using namespace Pt;
using namespace Pt::Db;


class ResultImplTest : public Pt::Unit::TestSuite
{
public:
    ResultImplTest()
    : TestSuite("ResultImplTest")
    {
        Pt::Unit::TestSuite::registerMethod("testSize", *this, &ResultImplTest::testSize);
        Pt::Unit::TestSuite::registerMethod("testGetRow", *this, &ResultImplTest::testGetRow);
    }

protected:
    void testSize();
    void testGetRow();

};


Pt::Unit::RegisterTest<ResultImplTest> register_ResultImplTest;


void ResultImplTest::testSize()
{
    ResultImpl resImpl;

    for(unsigned int j=0; j<10; j++){
        RowImpl* rowImpl = new RowImpl();
        for(unsigned int i=0; i<25; i++)
        {
            ValueImpl* impl = new ValueImpl(j*i);
            Value v(impl);
            rowImpl->add( v );
        }
        resImpl.add(rowImpl);
    }

    PT_UNIT_ASSERT( resImpl.size() == 10 );
    PT_UNIT_ASSERT( resImpl.getFieldCount() == 25 );
}


void ResultImplTest::testGetRow()
{
    ResultImpl resImpl;

    for(unsigned int j=0; j<10; j++){
        RowImpl* rowImpl = new RowImpl();
        for(unsigned int i=0; i<25; i++)
        {
            ValueImpl* impl = new ValueImpl(j*i);
            Value v(impl);
            rowImpl->add( v );
        }
        resImpl.add(rowImpl);
    }

    for(int i=0; i<10; i++)
    {
        Row row = resImpl.getRow(i);
        PT_UNIT_ASSERT( row.getInt(24) == i * 24 );
    }
}
