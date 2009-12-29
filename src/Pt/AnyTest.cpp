/*
 * Copyright (C) 2005-2007 by Marc Boris Duerner
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

#include "Pt/Any.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <sstream>


class AnyTest : public Pt::Unit::TestSuite
{
    public:
        AnyTest()
        : Pt::Unit::TestSuite("AnyTest")
        {
            this->registerMethod( "Empty", *this, &AnyTest::Empty );
            this->registerMethod( "BoolTest", *this, &AnyTest::BoolTest );
            this->registerMethod( "CharTest", *this, &AnyTest::CharTest );
            this->registerMethod( "IntTest", *this, &AnyTest::IntTest );
            this->registerMethod( "FloatTest", *this, &AnyTest::FloatTest );
            this->registerMethod( "DoubleTest", *this, &AnyTest::DoubleTest );
            this->registerMethod( "StdStringTest", *this, &AnyTest::StdStringTest );
            this->registerMethod( "Reference", *this, &AnyTest::Reference );
            this->registerMethod( "ConstReference", *this, &AnyTest::ConstReference );
            this->registerMethod( "Pointer", *this, &AnyTest::Pointer );
            this->registerMethod( "ConstPointer", *this, &AnyTest::Pointer );
            //this->registerMethod( "Equals", *this, &AnyTest::Equals );
        }

    protected:
        void Empty()
        {
            Pt::Any a;
            PT_UNIT_ASSERT( a.empty() );

            Pt::Any b(a);
            PT_UNIT_ASSERT( a.get() == 0 );
            PT_UNIT_ASSERT( b.get() == 0 );
        }

        void Reference()
        {
            int n = 5;
            Pt::Any a(&n);

            int& i = Pt::any_cast<int&>(a);
            PT_UNIT_ASSERT( &i == &n );
        }

        void ConstReference()
        {
            int n = 5;
            const int& ref = n;
            Pt::Any a(&ref);

            const int& i = Pt::any_cast<const int&>(a);
            PT_UNIT_ASSERT( &i == &n );
        }

        void Pointer()
        {
            int n = 5;
            Pt::Any a(&n);

            int* p = Pt::any_cast<int*>(a);
            PT_UNIT_ASSERT( p == &n );
        }

        void ConstPointer()
        {
            int n = 5;
            const int* cp = &n;
            Pt::Any a(cp);

            const int* p = Pt::any_cast<const int*>(a);
            PT_UNIT_ASSERT( p == cp );
        }

        // void Equals()
        // {
        //     int n = 5;
        //     Pt::Any a = Pt::Any::makeRef(n);

        //     Pt::Any b(5);

        //     PT_UNIT_ASSERT( b == a );
        //     PT_UNIT_ASSERT( a == b );
        //     PT_UNIT_ASSERT( a == a );
        //     PT_UNIT_ASSERT( b == b );
        // }

        void BoolTest()
        {
            Pt::Any a = true;
            bool b = Pt::any_cast<bool>(a);
            PT_UNIT_ASSERT( b == true );
        }

        void CharTest()
        {
            Pt::Any a = 'c';
            char c = Pt::any_cast<char>(a);
            PT_UNIT_ASSERT( c == 'c' );
        }

        void IntTest()
        {
            Pt::Any a = 5;
            int i = Pt::any_cast<int>(a);
            PT_UNIT_ASSERT( i == 5 );
        }

        void FloatTest()
        {
            Pt::Any a = 1.5f;
            float f = Pt::any_cast<float>(a);
            PT_UNIT_ASSERT( f == 1.5 );
        }

        void DoubleTest()
        {
            Pt::Any a = 1.5;
            double d = Pt::any_cast<double>(a);
            PT_UNIT_ASSERT( d == 1.5 );
        }

        void StdStringTest()
        {
            Pt::Any a = std::string("hello");
            const std::string& s = Pt::any_cast<const std::string&>(a);
            PT_UNIT_ASSERT( s == "hello" );
        }
};

static Pt::Unit::RegisterTest<AnyTest> registerAnyTest;
