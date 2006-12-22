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
#include <iostream>
using namespace std;

#include "Pt/Singleton.h"
using namespace Pt;

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"


class TestSingleton : public Singleton<TestSingleton> 
{
	friend class Singleton<TestSingleton>;

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

Pt::Unit::RegisterTest<SingletonTest> register_TypesTest;

