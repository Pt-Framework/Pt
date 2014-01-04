/*
 * Copyright (C) 2005-2006 by Marc Boris Duerner
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

#include "Pt/TypeTraits.h"

#include <string>
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"


class TypeTraitsTest : public Pt::Unit::TestSuite
{
    public:
        TypeTraitsTest()
        : Pt::Unit::TestSuite("TypeTraitsTest")
        {
            this->registerMethod( "ConstTest", *this, &TypeTraitsTest::ConstTest );
            this->registerMethod( "BoolTest", *this, &TypeTraitsTest::BoolTest );
            this->registerMethod( "CharTest", *this, &TypeTraitsTest::CharTest );
            this->registerMethod( "IntTest", *this, &TypeTraitsTest::IntTest );
            this->registerMethod( "FloatTest", *this, &TypeTraitsTest::FloatTest );
            this->registerMethod( "DoubleTest", *this, &TypeTraitsTest::DoubleTest );
            this->registerMethod( "StdStringTest", *this, &TypeTraitsTest::StdStringTest );
        }

    protected:
        void ConstTest()
        {
            PT_UNIT_ASSERT( Pt::TypeTraits<const int>::isPointer == false );
            PT_UNIT_ASSERT( Pt::TypeTraits<const int>::isConst == true );
            PT_UNIT_ASSERT( Pt::TypeTraits<const int>::isReference == false );
        }

        void BoolTest()
        {
            //PT_UNIT_ASSERT( Pt::TypeTraits<bool>::typeName() == std::string("bool") );
        }

        void CharTest()
        {
            //PT_UNIT_ASSERT( Pt::TypeTraits<char>::typeName() == std::string("char") );
        }

        void IntTest()
        {
            //PT_UNIT_ASSERT( Pt::TypeTraits<int>::typeName() == std::string("int") );
        }

        void FloatTest()
        {
            //PT_UNIT_ASSERT( Pt::TypeTraits<float>::typeName() == std::string("float") );
        }

        void DoubleTest()
        {
            //PT_UNIT_ASSERT( Pt::TypeTraits<double>::typeName() == std::string("double") );
        }

        void StdStringTest()
        {
            //PT_UNIT_ASSERT( Pt::TypeTraits<std::string>::typeName() == std::string("std::string") );
        }
};

Pt::Unit::RegisterTest<TypeTraitsTest> register_TypeTraitsTest;
