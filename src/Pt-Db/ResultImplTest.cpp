/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestMain.h"

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


class ResultImplTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ResultImplTest );
    
    CPPUNIT_TEST( testSize );
    CPPUNIT_TEST( testGetRow );
                
    CPPUNIT_TEST_SUITE_END();

protected:
    void testSize();
    void testGetRow();

};


CPPUNIT_TEST_SUITE_REGISTRATION( ResultImplTest );


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
    
    CPPUNIT_ASSERT( resImpl.size() == 10 );
    CPPUNIT_ASSERT( resImpl.getFieldCount() == 25 );
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
        CPPUNIT_ASSERT( row.getInt(24) == i * 24 );
    }
}
