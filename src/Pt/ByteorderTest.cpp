/***************************************************************************
 *   Copyright (C) 2005 Aloysius Indrayanto                                *
 *   Copyright (C) 2004 Marc Boris Dürner                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#undef PT_API_EXPORT

#include "Pt/Byteorder.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/System/Clock.h"
#include <limits>

class ByteorderTest : public Pt::Unit::TestSuite
{
    public:
    ByteorderTest()
    : TestSuite("ByteorderTest")
    {
        this->registerMethod("Swab", *this, &ByteorderTest::test );
        //this->registerMethod("Performance", *this, &ByteorderTest::perf );
    }

    void test()
    {
        #if defined(PT_LE)
            Pt::uint16_t a = 0xaabb;
            PT_UNIT_ASSERT( Pt::uint16_t(0xbbaa) == Pt::hostToBe(a) );

            Pt::int16_t b = (Pt::int16_t)(0xaabb);
            PT_UNIT_ASSERT( Pt::int16_t(0xbbaa) == Pt::hostToBe(b) );

            Pt::uint32_t c = 0xaabbccdd;
            PT_UNIT_ASSERT( Pt::uint32_t(0xddccbbaa) == Pt::hostToBe(c) );

            Pt::int32_t d = 0xaabbccdd;
            PT_UNIT_ASSERT( Pt::int32_t(0xddccbbaa) == Pt::hostToBe(d) );

            #if defined(PT_WITH_INT64)
            Pt::uint64_t e = 0x1122334455667788ULL;
            PT_UNIT_ASSERT( Pt::uint64_t(0x8877665544332211ULL) == Pt::hostToBe(e) );

            Pt::int64_t f = 0x1122334455667788LL;
            PT_UNIT_ASSERT( Pt::int64_t(0x8877665544332211LL) == Pt::hostToBe(f) );
            #endif
        #elif defined(PT_BE)
            Pt::uint16_t a = 0xbbaa;
            PT_UNIT_ASSERT( Pt::uint16_t(0xaabb) == Pt::hostToLe(a) );

            Pt::int16_t b = (Pt::int16_t)(0xbbaa);
            PT_UNIT_ASSERT( Pt::int16_t(0xaabb) == Pt::hostToLe(b) );

            Pt::uint32_t c = 0xddccbbaa;
            PT_UNIT_ASSERT( Pt::uint32_t(0xaabbccdd) == Pt::hostToLe(c) );

            Pt::int32_t d = 0xddccbbaa;
            PT_UNIT_ASSERT( Pt::int32_t(0xaabbccdd) == Pt::hostToLe(d) );

            #if defined(PT_WITH_INT64)
            Pt::uint64_t e = 0x8877665544332211ULL;
            PT_UNIT_ASSERT( Pt::uint64_t(0x1122334455667788ULL) == Pt::hostToLe(e) );

            Pt::int64_t f = 0x8877665544332211LL;
            PT_UNIT_ASSERT( Pt::int64_t(0x1122334455667788LL) == Pt::hostToLe(f) );
            #endif
        #else
            #error "Neither PT_BE not PT_LE is defined"
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
};

Pt::Unit::RegisterTest<ByteorderTest> register_ByteorderTest;
