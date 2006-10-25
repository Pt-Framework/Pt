/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Dürner                      *
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
#include "Pt/AtomicInt.h"

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestFixture.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"

#include <string>


class AtomicIntConstructorTest : public Pt::Unit::TestCase
{
	public:
		AtomicIntConstructorTest()
		: TestCase("AtomicIntConstructorTest")
		{}

		virtual void test()
		{
			Pt::AtomicInt a(5);
			PT_UNIT_ASSERT( a.value() == 5 );
		}
};

Pt::Unit::RegisterTest<AtomicIntConstructorTest> register_AtomicIntConstructorTest;




class AtomicOperatorTest : public Pt::Unit::TestFixture
{
	public:
		virtual void setUp()
		{
			_value = 5;
		}

		virtual void tearDown()
		{}

		void testSubstract()
		{
			_value -= 3;
			PT_UNIT_ASSERT( _value.value() == 2 );
		}

		void testAdd()
		{
			_value += 3;
			PT_UNIT_ASSERT( _value.value() == 8 );
		}


	private:
		Pt::AtomicInt _value;
};


class AtomicTestSuite : public Pt::Unit::TestSuite
{
	public:
		AtomicTestSuite()
		: Pt::Unit::TestSuite("AtomicIntTest")
		{
			Pt::Unit::TestSuite::registerMethod( *this, &AtomicTestSuite::testAssign, "AssignmentTest" );
			Pt::Unit::TestSuite::registerMethod( _operatorTest, &AtomicOperatorTest::testSubstract, "SubstractionTest" );
			Pt::Unit::TestSuite::registerMethod( _operatorTest, &AtomicOperatorTest::testAdd, "AdditionTest" );
		}

	protected:
		void testAssign()
		{
			Pt::AtomicInt a;
			a = 10;
			PT_UNIT_ASSERT( a.value() == 10 );
		}

	private:
		AtomicOperatorTest _operatorTest;
};

Pt::Unit::RegisterTest<AtomicTestSuite> register_AtomicTestSuite;
