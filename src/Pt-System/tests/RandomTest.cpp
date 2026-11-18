/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <Pt/System/Random.h>
#include <cstring>
#include <stdexcept>

class RandomTest : public Pt::Unit::TestSuite
{
    public:
        RandomTest()
        : Pt::Unit::TestSuite("Pt::System::RandomTest")
        {
            this->registerMethod("Get", *this, &RandomTest::Get);
            this->registerMethod("GetZero", *this, &RandomTest::GetZero);
            this->registerMethod("GetNullBuffer", *this, &RandomTest::GetNullBuffer);
            this->registerMethod("GetUInt32Range", *this, &RandomTest::GetUInt32Range);
            this->registerMethod("GetUInt64Range", *this, &RandomTest::GetUInt64Range);
            this->registerMethod("TwoInstances", *this, &RandomTest::TwoInstances);
        }

        void Get()
        {
            Pt::System::Random rnd;
            Pt::uint8_t a[16] = {};
            Pt::uint8_t b[16] = {};

            rnd.get(a, sizeof(a));
            rnd.get(b, sizeof(b));

            bool aZero = true;
            for(std::size_t i = 0; i < sizeof(a); ++i)
            {
                if(a[i] != 0)
                    aZero = false;
            }

            PT_UNIT_ASSERT( ! aZero );
            PT_UNIT_ASSERT(std::memcmp(a, b, sizeof(a)) != 0);
        }

        void GetZero()
        {
            Pt::System::Random rnd;
            Pt::uint8_t marker = 0xA5;
            rnd.get(&marker, 0);
            PT_UNIT_ASSERT_EQUALS(marker, 0xA5);

            rnd.get(0, 0);
        }

        void GetNullBuffer()
        {
            Pt::System::Random rnd;
            PT_UNIT_ASSERT_THROW(rnd.get(0, 1), std::invalid_argument);
        }

        void GetUInt32Range()
        {
            Pt::System::Random rnd;
            PT_UNIT_ASSERT_EQUALS(rnd.getUInt32(1), 0);
            PT_UNIT_ASSERT_THROW(rnd.getUInt32(0), std::invalid_argument);

            for(unsigned i = 0; i < 1000; ++i)
            {
                Pt::uint32_t x = rnd.getUInt32(6);
                PT_UNIT_ASSERT(x < 6);
            }
        }

        void GetUInt64Range()
        {
            Pt::System::Random rnd;
            PT_UNIT_ASSERT_EQUALS(rnd.getUInt64(1), 0);
            PT_UNIT_ASSERT_THROW(rnd.getUInt64(0), std::invalid_argument);

            for(unsigned i = 0; i < 1000; ++i)
            {
                Pt::uint64_t x = rnd.getUInt64(6);
                PT_UNIT_ASSERT(x < 6);
            }
        }

        void TwoInstances()
        {
            Pt::System::Random a;
            Pt::System::Random b;
            Pt::uint8_t bufA[16] = {};
            Pt::uint8_t bufB[16] = {};

            a.get(bufA, sizeof(bufA));
            b.get(bufB, sizeof(bufB));

            PT_UNIT_ASSERT(std::memcmp(bufA, bufB, sizeof(bufA)) != 0);
        }
};

Pt::Unit::RegisterTest<RandomTest> register_RandomTest;
