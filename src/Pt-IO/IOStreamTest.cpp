/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestMain.h"

#include <string>
#include <iostream>
using namespace std;

class IOStreamTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( IOStreamTest );
    CPPUNIT_TEST( test );
    CPPUNIT_TEST_SUITE_END();

    protected:
        void test();
};


CPPUNIT_TEST_SUITE_REGISTRATION( IOStreamTest );

void IOStreamTest::test()
{

    //CPPUNIT_ASSERT();
}



