/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestMain.h"

#include <string>
#include <iostream>
using namespace std;

#include "Pt/Variant.h"
using namespace Pt;


class VariantTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( VariantTest );
	CPPUNIT_TEST( testAssignInt );
	CPPUNIT_TEST( testGetInt );
	CPPUNIT_TEST( testCompareInt );

	CPPUNIT_TEST( testAssignVariant );
	CPPUNIT_TEST( testCompareVariant );

	CPPUNIT_TEST_SUITE_END();

protected:
	void testAssignInt();
	void testGetInt();
	void testCompareInt();

	void testAssignVariant();
	void testCompareVariant();
};


CPPUNIT_TEST_SUITE_REGISTRATION( VariantTest );


void VariantTest::testAssignInt()
{
	Variant v(5);
	CPPUNIT_ASSERT( v.str() == "5" );

	v = 10;
	CPPUNIT_ASSERT( v.str() == "10" );
}


void VariantTest::testGetInt()
{
	Variant v(5);
	int n = 0;
	v.get(n);

	CPPUNIT_ASSERT( n == 5 );
}


void VariantTest::testCompareInt()
{
	Variant v(5);
	CPPUNIT_ASSERT( v == 5 );
	CPPUNIT_ASSERT( v < 6 );
	CPPUNIT_ASSERT( v > 4 );
}


void VariantTest::testAssignVariant()
{
	Variant v(5);
	Variant v2( v );
	CPPUNIT_ASSERT( v2.str() == "5" );

	Variant v3;
	v3 = v;
	CPPUNIT_ASSERT( v.str() == "5" );
}


void VariantTest::testCompareVariant()
{
	Variant v(5);
	Variant v2(5);
	Variant v3(6);

	CPPUNIT_ASSERT( v == v2 );
	CPPUNIT_ASSERT( v2 < v3 );
	CPPUNIT_ASSERT( v3 > v );
}
