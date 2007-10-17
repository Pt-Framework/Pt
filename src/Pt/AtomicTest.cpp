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

#include "Pt/Atomicity.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <cctype>
//#include <locale>


class AtomicTestSuite : public Pt::Unit::TestSuite
{
    public:
        AtomicTestSuite()
        : Pt::Unit::TestSuite("AtomicityTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Integer", *this, &AtomicTestSuite::Integer );
            Pt::Unit::TestSuite::registerMethod( "Pointer", *this, &AtomicTestSuite::Pointer );
        }

    protected:
        void Integer()
        {
            volatile Pt::atomic_t v = 0;

            Pt::atomicIncrement(v);
            PT_UNIT_ASSERT(v == 1);

            Pt::atomicIncrement(v);
            PT_UNIT_ASSERT(v == 2);

            Pt::atomicIncrement(v);
            PT_UNIT_ASSERT(v == 3);

            Pt::atomicDecrement(v);
            PT_UNIT_ASSERT(v == 2);

            Pt::atomicDecrement(v);
            PT_UNIT_ASSERT(v == 1);

            Pt::atomicDecrement(v);
            PT_UNIT_ASSERT(v == 0);

            Pt::atomicExchange(v, 1);
            PT_UNIT_ASSERT(v == 1);

            Pt::atomicExchange(v, 0);
            PT_UNIT_ASSERT(v == 0);

            Pt::atomicExchangeAdd(v, 3);
            PT_UNIT_ASSERT(v == 3);

            Pt::atomicExchangeAdd(v, 1);
            PT_UNIT_ASSERT(v == 4);

            Pt::atomicCompareExchange(v, 5, 4);
            PT_UNIT_ASSERT(v == 5);

            Pt::atomicCompareExchange(v, 9, 7);
            PT_UNIT_ASSERT(v == 5);
        }

        void Pointer()
        {
            int a = 0, b = 1;
            volatile void* p = 0;

            Pt::atomicExchange( p, (void*)&a );
            PT_UNIT_ASSERT(p == (void*)&a);

            Pt::atomicCompareExchange( p, (void*)(&b), (void*)&a );
            PT_UNIT_ASSERT(p == (void*)&b);

            Pt::atomicCompareExchange( p, (void*)(&a), (void*)&a );
            PT_UNIT_ASSERT(p == (void*)&b);
        }
};

Pt::Unit::RegisterTest<AtomicTestSuite> register_AtomicTestSuite;

