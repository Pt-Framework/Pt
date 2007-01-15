/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Duerner                          *
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
#include "Pt/TypeTraits.h"

#include <string>
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"


class TypeTraitsTest : public Pt::Unit::TestSuite
{
    public:
        TypeTraitsTest()
        : Pt::Unit::TestSuite("TypeTraitsTest")
        {
            this->registerMethod( "VoidTest", *this, &TypeTraitsTest::VoidTest );
            this->registerMethod( "BoolTest", *this, &TypeTraitsTest::BoolTest );
            this->registerMethod( "CharTest", *this, &TypeTraitsTest::CharTest );
            this->registerMethod( "IntTest", *this, &TypeTraitsTest::IntTest );
            this->registerMethod( "FloatTest", *this, &TypeTraitsTest::FloatTest );
            this->registerMethod( "DoubleTest", *this, &TypeTraitsTest::DoubleTest );
            this->registerMethod( "StdStringTest", *this, &TypeTraitsTest::StdStringTest );
        }

    protected:
        void VoidTest()
        {
			PT_UNIT_ASSERT( Pt::TypeTraits<void>::typeName() == "void" );
        }

        void BoolTest()
        {
			PT_UNIT_ASSERT( Pt::TypeTraits<bool>::typeName() == "bool" );
        }

        void CharTest()
        {
			PT_UNIT_ASSERT( Pt::TypeTraits<char>::typeName() == "char" );
        }

        void IntTest()
        {
			PT_UNIT_ASSERT( Pt::TypeTraits<int>::typeName() == "int" );
        }

        void FloatTest()
        {
			PT_UNIT_ASSERT( Pt::TypeTraits<float>::typeName() == "float" );
        }

        void DoubleTest()
        {
			PT_UNIT_ASSERT( Pt::TypeTraits<double>::typeName() == "double" );
        }

        void StdStringTest()
        {
			PT_UNIT_ASSERT( Pt::TypeTraits<std::string>::typeName() == "std::string" );
        }
};

Pt::Unit::RegisterTest<TypeTraitsTest> register_AtomicTestSuite;
