#include "Pt/Pool.h"

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include <fstream>
#include <cassert>


class PoolTest : public Pt::Unit::TestSuite
{
    public:
        PoolTest()
        : Pt::Unit::TestSuite("TimeTest")
        {
            Pt::Unit::TestSuite::registerMethod( "AllocRelease", *this, &PoolTest::AllocRelease );
        }

    protected:
        void AllocRelease();
};

Pt::Unit::RegisterTest<PoolTest> register_PoolTest;


void PoolTest::AllocRelease()
{
    Pt::Pool<std::string> pool(2);

    std::string* str1 = pool.alloc();
    std::string* str2 = pool.alloc();

    PT_UNIT_ASSERT ( str1 );

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
    PT_UNIT_ASSERT( pool.size() == 3 );
}

