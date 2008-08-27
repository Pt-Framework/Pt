/***************************************************************************
 *   Copyright (C) 2005-2007 by Marc Boris Duerner                         *
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

#include "Pt/Float.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <sstream>


class FloatTest : public Pt::Unit::TestSuite
{
    public:
        FloatTest()
        : Pt::Unit::TestSuite("FloatTest")
        {
            this->registerMethod( "Test", *this, &FloatTest::Test );
        }

    protected:
        void Test()
        {
            Pt::uint32_t bf = Pt::Float::toIEEE754Single(0.085f);
            float f = Pt::Float::fromIEEE754Single(bf);
            float diff = f - 0.085f;
            PT_UNIT_ASSERT(diff < 0.0001f);

            bf = Pt::Float::toIEEE754Single(18.4f);
            f = Pt::Float::fromIEEE754Single(bf);
            diff = f - 18.4f;
            PT_UNIT_ASSERT(diff < 0.0001f);
            
            bf = Pt::Float::toIEEE754Single(-5.0f);
            f = Pt::Float::fromIEEE754Single(bf);
            diff = f - (-5.0f);
            PT_UNIT_ASSERT(diff < 0.0001f);

            bf = Pt::Float::toIEEE754Single(1.0f);
            f = Pt::Float::fromIEEE754Single(bf);
            diff = f - 1.0f;
            PT_UNIT_ASSERT(diff < 0.0001f);

            bf = Pt::Float::toIEEE754Single(-1.0f);
            f = Pt::Float::fromIEEE754Single(bf);
            diff = f - (-1.0f);
            PT_UNIT_ASSERT(diff < 0.0001f);

            bf = Pt::Float::toIEEE754Single(0.0f);
            f = Pt::Float::fromIEEE754Single(bf);
            diff = f - 0.0f;
            PT_UNIT_ASSERT(diff < 0.0001f);

            bf = Pt::Float::toIEEE754Single(-0.0f);
            f = Pt::Float::fromIEEE754Single(bf);
            diff = f - (-0.0f);
            PT_UNIT_ASSERT(diff < 0.0001f);
            
            bf = Pt::Float::toIEEE754Single(1123.23f);
            f = Pt::Float::fromIEEE754Single(bf);
            diff = f - 1123.23f;
            PT_UNIT_ASSERT(diff < 0.0001f);
            
            bf = 0xc0d9999a;
            f = Pt::Float::fromIEEE754Single(bf);
            diff = f - (-6.8f);
            PT_UNIT_ASSERT(diff < 0.0001f);

            float maxIEEE754Single = (1.0f - pow(2.0f, -24)) * pow(2.0f, 128);   
            bf = Pt::Float::toIEEE754Single(maxIEEE754Single);
            f = Pt::Float::fromIEEE754Single(bf);
            diff = f - maxIEEE754Single; 
            PT_UNIT_ASSERT(diff < 0.0001f);

            float minIEEE754Single = pow(2.0f, -126);
            bf = Pt::Float::toIEEE754Single(minIEEE754Single);
            f = Pt::Float::fromIEEE754Single(bf);
            diff = f - minIEEE754Single;
            PT_UNIT_ASSERT(diff < 0.0001f);

            float minFloat = std::numeric_limits<float>::min();
            bf = Pt::Float::toIEEE754Single(minFloat);
            f = Pt::Float::fromIEEE754Single(bf);
            diff = f - minFloat;
            PT_UNIT_ASSERT(diff < 0.0001f);

            float maxFloat = std::numeric_limits<float>::max();
            bf = Pt::Float::toIEEE754Single(maxFloat);
            f = Pt::Float::fromIEEE754Single(bf);
            diff = f - maxFloat;
            PT_UNIT_ASSERT(diff < 0.0001f);
            
        }
};

static Pt::Unit::RegisterTest<FloatTest> registerFloatTest;
