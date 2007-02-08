#include <fstream>
#include <cassert>
using namespace std;

#include "Pt/Pool.h"
using namespace Pt;

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestMain.h"


class PoolTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( PoolTest );
	CPPUNIT_TEST( test );
	CPPUNIT_TEST_SUITE_END();

protected:
	void test();
};


CPPUNIT_TEST_SUITE_REGISTRATION( PoolTest );


void PoolTest::test()
{
    Pool<std::string> pool(2);

    std::string* str1 = pool.alloc();
    std::string* str2 = pool.alloc();

    CPPUNIT_ASSERT ( str1 );

    *str1 = "test";

    pool.release(str1);
    str1 = pool.alloc();

    pool.release(str2);
    pool.release(str1);

    str1 = pool.alloc();
    str2 = pool.alloc();
    std::string* str3 = pool.alloc();

    *str3 == "test";

    pool.release( str1 );
    pool.release( str2 );
    pool.release( str3 );

	  CPPUNIT_ASSERT( pool.size() == 3 );
}

