/*
 * Copyright (C) 2005 by Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#undef PT_API_EXPORT

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include "Pt/Types.h"
#include "Pt/Byteorder.h"


class ByteorderTest : public Pt::Unit::TestSuite
{
public:
    ByteorderTest()
    : TestSuite("ByteorderTest")
    {
        Pt::Unit::TestSuite::registerMethod("testBeToLe", *this, &ByteorderTest::testBeToLe);
        Pt::Unit::TestSuite::registerMethod("testLeToBe", *this, &ByteorderTest::testLeToBe);
    }

protected:
    void testBeToLe();
    void testLeToBe();
};


Pt::Unit::RegisterTest<ByteorderTest> register_ByteorderTest;


void ByteorderTest::testBeToLe()
{

#ifdef PTV_LE
    Pt::uint16_t a = 0xaabb;
    PT_UNIT_ASSERT( Pt::uint16_t(0xbbaa) == Pt::hostToBe(a) );

    Pt::int16_t b = (Pt::int16_t)(0xaabb);
    PT_UNIT_ASSERT( Pt::int16_t(0xbbaa) == Pt::hostToBe(b) );

    Pt::uint32_t c = 0xaabbccdd;
    PT_UNIT_ASSERT( Pt::uint32_t(0xddccbbaa) == Pt::hostToBe(c) );

    Pt::int32_t d = 0xaabbccdd;
    PT_UNIT_ASSERT( Pt::int32_t(0xddccbbaa) == Pt::hostToBe(d) );

   #ifdef PTV_64BIT
    Pt::uint64_t e = 0x1122334455667788ULL;
    PT_UNIT_ASSERT( Pt::uint64_t(0x8877665544332211ULL) == Pt::hostToBe(e) );

    Pt::int64_t f = 0x1122334455667788LL;
    PT_UNIT_ASSERT( Pt::int64_t(0x8877665544332211LL) == Pt::hostToBe(f) );
   #endif
#elif PTV_BE
    Pt::uint16_t a = 0xaabb;
    PT_UNIT_ASSERT( Pt::uint16_t(0xaabb) == Pt::hostToBe(a) );

    Pt::int16_t b = (Pt::int16_t)(0xaabb);
    PT_UNIT_ASSERT( Pt::int16_t(0xaabb) == Pt::hostToBe(b) );

    Pt::uint32_t c = 0xaabbccdd;
    PT_UNIT_ASSERT( Pt::uint32_t(0xaabbccdd) == Pt::hostToBe(c) );

    Pt::int32_t d = 0xaabbccdd;
    PT_UNIT_ASSERT( Pt::int32_t(0xaabbccdd) == Pt::hostToBe(d) );

   #ifdef PTV_64BIT
    Pt::uint64_t e = 0x1122334455667788ULL;
    PT_UNIT_ASSERT( Pt::uint64_t(0x1122334455667788ULL) == Pt::hostToBe(e) );

    Pt::int64_t f = 0x1122334455667788LL;
    PT_UNIT_ASSERT( Pt::int64_t(0x1122334455667788LL) == Pt::hostToBe(f) );
   #endif
#endif

}

void ByteorderTest::testLeToBe()
{
#ifdef PTV_LE
    Pt::uint16_t a = 0xaabb;
    PT_UNIT_ASSERT( Pt::uint16_t(0xaabb) == Pt::hostToLe(a) );

    Pt::int16_t b = (Pt::int16_t) 0xaabb;
    PT_UNIT_ASSERT( Pt::int16_t(0xaabb) == Pt::hostToLe(b) );

    Pt::uint32_t c = 0xaabbccdd;
    PT_UNIT_ASSERT( Pt::uint32_t(0xaabbccdd) == Pt::hostToLe(c) );

    Pt::int32_t d = 0xaabbccdd;
    PT_UNIT_ASSERT( Pt::int32_t(0xaabbccdd) == Pt::hostToLe(d) );

   #ifdef PTV_64BIT
    Pt::uint64_t e = 0x1122334455667788ULL;
    PT_UNIT_ASSERT( Pt::uint64_t(0x1122334455667788ULL) == Pt::hostToLe(e) );

    Pt::int64_t f = 0x1122334455667788LL;
    PT_UNIT_ASSERT( Pt::int64_t(0x1122334455667788LL) == Pt::hostToLe(f) );
   #endif
#elif PTV_BE
    Pt::uint16_t a = 0xaabb;
    PT_UNIT_ASSERT( Pt::uint16_t(0xbbaa) == Pt::hostToLe(a) );

    Pt::int16_t b = (Pt::int16_t) 0xaabb;
    PT_UNIT_ASSERT( Pt::int16_t(0xbbaa) == Pt::hostToLe(b) );

    Pt::uint32_t c = 0xaabbccdd;
    PT_UNIT_ASSERT( Pt::uint32_t(0xddccbbaa) == Pt::hostToLe(c) );

    Pt::int32_t d = 0xaabbccdd;
    PT_UNIT_ASSERT( Pt::int32_t(0xddccbbaa) == Pt::hostToLe(d) );

   #ifdef PTV_64BIT
    Pt::uint64_t e = 0x1122334455667788ULL;
    PT_UNIT_ASSERT( Pt::uint64_t(0x8877665544332211ULL) == Pt::hostToLe(e) );

    Pt::int64_t f = 0x1122334455667788LL;
    PT_UNIT_ASSERT( Pt::int64_t(0x8877665544332211ULL) == Pt::hostToLe(f) );
   #endif
#endif
}

/*
    void perf()
    {
        volatile unsigned  r = 0;

        Pt::System::Clock clock;

        clock.start();
        for(unsigned i = 0; i < 5000; ++i)
        {
            for(unsigned x = 0; x < 60000; ++x)
            {
                r = Pt::swab(x);
            }
        }
        Pt::System::TimeValue tv = clock.stop();
        std::cerr << "\n Sec:" << tv.seconds() << " Micro-Secs:" << tv.microSeconds() << std::endl;
    }
*/
