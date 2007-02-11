/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestMain.h"

#include <string>
#include <iostream>
#include <limits>

#include "RowImpl.h"
#include "ValueImpl.h"

#include "Pt/Db/Value.h"

using namespace std;
using namespace Pt;
using namespace Pt::Db;


class RowImplTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( RowImplTest );
    
    CPPUNIT_TEST( testSize );
    CPPUNIT_TEST( testGetValue );
                
    CPPUNIT_TEST_SUITE_END();

protected:
    void testSize();
    void testGetValue();

};


CPPUNIT_TEST_SUITE_REGISTRATION( RowImplTest );


void RowImplTest::testSize()
{
    RowImpl rowImpl;
    
    for(unsigned int i=0; i<25; i++)
    {
        ValueImpl* impl = new ValueImpl(i);
        Value v(impl);
        rowImpl.add( v );
    }
    
    CPPUNIT_ASSERT( rowImpl.size() == 25 );
}


void RowImplTest::testGetValue()
{
    RowImpl rowImpl;
    
    for(unsigned int i=0; i<25; i++)
    {
        ValueImpl* impl = new ValueImpl(i);
        Value v(impl);
        rowImpl.add( v );
    }
    
    for(unsigned int i=24; i<=0; i--)
    {
        CPPUNIT_ASSERT( rowImpl.getValue(i) );
    }
}
