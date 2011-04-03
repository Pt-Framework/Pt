/*
 * Copyright (C) 2006 by Tommi Maekitalo
 * Copyright (C) 2006 by Marc Boris Duerner
 * Copyright (C) 2006 by Stefan Bueder
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

#include "Pt/Variant.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <iostream>
#include <limits>


class VariantTest : public Pt::Unit::TestSuite
{
    public:
        VariantTest()
        : Pt::Unit::TestSuite("VariantTest")
        {
            Pt::Unit::TestSuite::registerMethod( "AssignInt", *this, &VariantTest::AssignInt );
            Pt::Unit::TestSuite::registerMethod( "GetInt", *this, &VariantTest::GetInt );
            Pt::Unit::TestSuite::registerMethod( "Float", *this, &VariantTest::Float );
            Pt::Unit::TestSuite::registerMethod( "CompareInt", *this, &VariantTest::CompareInt );
            Pt::Unit::TestSuite::registerMethod( "AssignVariant", *this, &VariantTest::AssignVariant );
            Pt::Unit::TestSuite::registerMethod( "CompareVariant", *this, &VariantTest::CompareVariant );
        }

    protected:
        void AssignInt();
        void Float();
        void GetInt();
        void CompareInt();
        void AssignVariant();
        void CompareVariant();
};

Pt::Unit::RegisterTest<VariantTest> register_VariantTest;


void VariantTest::AssignInt()
{
    Pt::Variant v(5);
    PT_UNIT_ASSERT( v.str() == Pt::String(L"5") );

    v = 10;
    PT_UNIT_ASSERT( v.str() == Pt::String(L"10") );
}


void VariantTest::Float()
{
    Pt::Variant v( std::numeric_limits<float>::quiet_NaN() );
    PT_UNIT_ASSERT_EQUALS( v.str(), L"nan" );
}


void VariantTest::GetInt()
{
    Pt::Variant v(5);
    int n = 0;
    v.get(n);

    PT_UNIT_ASSERT( n == 5 );
}


void VariantTest::CompareInt()
{
    Pt::Variant v(5);
    PT_UNIT_ASSERT( v == 5 );
    PT_UNIT_ASSERT( v < 6 );
    PT_UNIT_ASSERT( v > 4 );
}


void VariantTest::AssignVariant()
{
    Pt::Variant v(5);
    Pt::Variant v2( v );
    PT_UNIT_ASSERT( v2.str() == Pt::String(L"5") );

    Pt::Variant v3;
    v3 = v;
    PT_UNIT_ASSERT( v.str() == Pt::String(L"5") );
}


void VariantTest::CompareVariant()
{
    Pt::Variant v(5);
    Pt::Variant v2(5);
    Pt::Variant v3(6);

    PT_UNIT_ASSERT( v == v2 );
    PT_UNIT_ASSERT( v2 < v3 );
    PT_UNIT_ASSERT( v3 > v );
}
