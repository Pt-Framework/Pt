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
#include "Pt/Any.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <sstream>


class AnyTest : public Pt::Unit::TestSuite
{
    public:
        AnyTest()
        : Pt::Unit::TestSuite("AnyTest")
        {
            this->registerMethod( "InitTest", *this, &AnyTest::InitTest );
            this->registerMethod( "BoolTest", *this, &AnyTest::BoolTest );
            this->registerMethod( "CharTest", *this, &AnyTest::CharTest );
            this->registerMethod( "IntTest", *this, &AnyTest::IntTest );
            this->registerMethod( "FloatTest", *this, &AnyTest::FloatTest );
            this->registerMethod( "DoubleTest", *this, &AnyTest::DoubleTest );
            this->registerMethod( "StdStringTest", *this, &AnyTest::StdStringTest );
        }

    protected:
        void InitTest()
        {
            Pt::Any a;
            PT_UNIT_ASSERT( a.empty() );

            a.init("int");
            PT_UNIT_ASSERT( a.typeName() == std::string("int") );

            a.init<float>();
            PT_UNIT_ASSERT( a.typeName() == std::string("float") );

            Pt::Any b(a);
            PT_UNIT_ASSERT( a == b );
        }

        void BoolTest()
        {
            Pt::Any a = true;
            bool b = Pt::any_cast<bool>(a);
            PT_UNIT_ASSERT( b == true );

            std::istringstream ss("1");
            ss >> a;
            PT_UNIT_ASSERT( a == true );
        }

        void CharTest()
        {
            Pt::Any a = 'c';
            char c = Pt::any_cast<char>(a);
            PT_UNIT_ASSERT( c == 'c' );

            std::istringstream ss("c");
            ss >> a;
            PT_UNIT_ASSERT( a == 'c' );
        }

        void IntTest()
        {
            Pt::Any a = 5;
            int i = Pt::any_cast<int>(a);
            PT_UNIT_ASSERT( i == 5 );

            std::istringstream ss("7");
            ss >> a;
            PT_UNIT_ASSERT( a == 7 );
        }

        void FloatTest()
        {
            Pt::Any a = 1.5f;
            float f = Pt::any_cast<float>(a);
            PT_UNIT_ASSERT( f == 1.5 );

            std::istringstream ss("1.5");
            ss >> a;
            PT_UNIT_ASSERT( a == 1.5f );
        }

        void DoubleTest()
        {
            Pt::Any a = 1.5;
            double d = Pt::any_cast<double>(a);
            PT_UNIT_ASSERT( d == 1.5 );

            std::istringstream ss("1.5");
            ss >> a;
            PT_UNIT_ASSERT( a == 1.5 );
        }

        void StdStringTest()
        {
            Pt::Any a = std::string("hello");
            const std::string& s = Pt::any_cast<const std::string&>(a);
            PT_UNIT_ASSERT( s == "hello" );

            std::istringstream ss("bye");
            ss >> a;
            PT_UNIT_ASSERT( a == std::string("bye") );
        }
};

static Pt::Unit::RegisterTest<AnyTest> registerAnyTest;
