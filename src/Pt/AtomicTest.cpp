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




class AtomicTestSuite : public Pt::Unit::TestSuite
{
	public:
		class Protocol : public Pt::Unit::TestSuiteProtocol
		{
			public:
				Protocol()
				{
					this->includeTest( "AdditionTest" );
					this->includeTest( "AssignmentTest", 42 );
					this->includeTest( "AssignmentTest", 56 );
					this->includeTest( "AssignmentTest", 3445 );
					this->includeTest( "AssignmentTest", 777 );
					this->includeTest( "SubstractionTest" );
				}
		} _protocol;

	public:
		AtomicTestSuite()
		: Pt::Unit::TestSuite("AtomicIntTest", _protocol)
		{
			Pt::Unit::TestSuite::registerMethod( *this, &AtomicTestSuite::AssignmentTest, "AssignmentTest" );
			Pt::Unit::TestSuite::registerMethod( *this, &AtomicTestSuite::SubstractionTest, "SubstractionTest" );
			Pt::Unit::TestSuite::registerMethod( *this, &AtomicTestSuite::AdditionTest, "AdditionTest" );
		}

		virtual void setUp()
		{
			_value = 5;
		}

	protected:
		void AssignmentTest(int value)
		{
			Pt::AtomicInt a;
			a = value;
			PT_UNIT_ASSERT( a.value() == value );
		}

		void SubstractionTest()
		{
			_value -= 3;
			PT_UNIT_ASSERT( _value.value() == 2 );
		}

		void AdditionTest()
		{
			_value += 3;
			PT_UNIT_ASSERT( _value.value() == 8 );
		}

	private:
		Pt::AtomicInt _value;
};

Pt::Unit::RegisterTest<AtomicTestSuite> register_AtomicTestSuite;
