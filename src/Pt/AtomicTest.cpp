/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Dürner                          *
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

#include "Pt/AtomicInt.h"

#include <string>
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/TestSchedule.h"
#include "Pt/Unit/RegisterTest.h"


class AtomicTestSuite : public Pt::Unit::TestSuite
{
    public:
        class Protocol : public Pt::Unit::TestSchedule
        {
            private:
                Pt::Args _args;

            public:
                Protocol()
                {
                    _args.push_back(300);

                    this->includeTest( "ConstructorTest" );
                    this->includeTest( "AdditionTest" );
                    this->includeTest( "AssignmentTest", _args );
                    this->includeTest( "SubstractionTest" );
                    this->includeTest( "CompareExchange" );
                }
        } _protocol;

    public:
        AtomicTestSuite()
        : Pt::Unit::TestSuite("AtomicIntTest", _protocol)
        {
            Pt::Unit::TestSuite::registerMethod( "ConstructorTest", *this, &AtomicTestSuite::ConstructorTest );
            Pt::Unit::TestSuite::registerMethod( "AssignmentTest", *this, &AtomicTestSuite::AssignmentTest );
            Pt::Unit::TestSuite::registerMethod( "SubstractionTest", *this, &AtomicTestSuite::SubstractionTest );
            Pt::Unit::TestSuite::registerMethod( "AdditionTest", *this, &AtomicTestSuite::AdditionTest );
            Pt::Unit::TestSuite::registerMethod( "CompareExchange", *this, &AtomicTestSuite::CompareExchange );
        }

        virtual void setUp()
        {
            _value = 5;
        }

    protected:
        void ConstructorTest()
        {
            Pt::AtomicInt a(5);
            PT_UNIT_ASSERT( a.value() == 5 );
        }

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
            Pt::AtomicInt value(5);
            value += 3;
            PT_UNIT_ASSERT( value.value() == 8 );
        }

        void CompareExchange()
        {
            Pt::AtomicInt value(8);
            PT_UNIT_ASSERT( value.value() == 8 );

            bool res = value.compareExchange(8, 10);
            PT_UNIT_ASSERT( value.value() == 10 );
            PT_UNIT_ASSERT( res );

            res = value.compareExchange(10, 10);
            PT_UNIT_ASSERT( res );

            res = value.compareExchange(8, 10);
            PT_UNIT_ASSERT( res == false );
        }

    private:
        Pt::AtomicInt _value;
};

Pt::Unit::RegisterTest<AtomicTestSuite> register_AtomicTestSuite;

