/*
 * Copyright (C) 2005 Aloysius Indrayanto
 * Copyright (C) 2004 Marc Boris Duerner
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

#include "Pt/Singleton.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include <iostream>

class TestSingleton : public Pt::Singleton<TestSingleton> 
{
    friend class Pt::Singleton<TestSingleton>;

    public:
        TestSingleton()
        {}

        ~TestSingleton()
        {}
};


class SingletonTest : public Pt::Unit::TestSuite
{
    public:
        SingletonTest()
        : TestSuite("SingletonTest")
        {
            this->registerMethod("testEqualInstance", *this, &SingletonTest::testEqualInstance);
        }

        virtual void testEqualInstance()
        {
            TestSingleton* _inst1 = &( TestSingleton::instance() );
            TestSingleton* _inst2 = &( TestSingleton::instance() );
            PT_UNIT_ASSERT( _inst1 == _inst2 );
        }

};

Pt::Unit::RegisterTest<SingletonTest> register_SingletonTest;

