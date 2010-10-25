/*
 * Copyright (C) 2006-2010 by Marc Boris Duerner
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
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

#include "Pt/Atomicity.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

class AtomicTestSuite : public Pt::Unit::TestSuite
{
    public:
        AtomicTestSuite()
        : Pt::Unit::TestSuite("AtomicityTest")
        {
            Pt::Unit::TestSuite::registerMethod( "GetSet", *this, &AtomicTestSuite::GetSet );
            Pt::Unit::TestSuite::registerMethod( "Integer", *this, &AtomicTestSuite::Integer );
            Pt::Unit::TestSuite::registerMethod( "Pointer", *this, &AtomicTestSuite::Pointer );
        }

    protected:
        void GetSet()
        {
            volatile Pt::atomic_t my_value(0);

            Pt::atomicSet(my_value, 3);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == 3);

            Pt::atomicSet(my_value, 0);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == 0);
        }

        void Integer()
        {
            volatile Pt::atomic_t my_value(0);

            PT_UNIT_ASSERT(Pt::atomicIncrement(my_value) == 1);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == 1);

            PT_UNIT_ASSERT(Pt::atomicIncrement(my_value) == 2);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == 2);

            PT_UNIT_ASSERT(Pt::atomicDecrement(my_value) == 1);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == 1);

            PT_UNIT_ASSERT(Pt::atomicDecrement(my_value) == 0);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == 0);

            PT_UNIT_ASSERT(Pt::atomicDecrement(my_value) == -1);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == -1);

            PT_UNIT_ASSERT(Pt::atomicDecrement(my_value) == -2);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == -2);

            PT_UNIT_ASSERT(Pt::atomicExchange(my_value, -5) == -2);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == -5);

            PT_UNIT_ASSERT(Pt::atomicExchange(my_value, 1) == -5);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == 1);

            PT_UNIT_ASSERT(Pt::atomicExchange(my_value, 0) == 1);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == 0);

            PT_UNIT_ASSERT(Pt::atomicExchangeAdd(my_value, 3) == 0);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == 3);

            PT_UNIT_ASSERT(Pt::atomicExchangeAdd(my_value, -5) == 3);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == -2);

            PT_UNIT_ASSERT(Pt::atomicExchangeAdd(my_value, 6) == -2);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == 4);

            PT_UNIT_ASSERT(Pt::atomicCompareExchange(my_value, 5, 4) == 4);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == 5);

            PT_UNIT_ASSERT(Pt::atomicCompareExchange(my_value, 9, 7) == 5);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == 5);

            PT_UNIT_ASSERT(Pt::atomicCompareExchange(my_value, -20, 5) == 5);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == -20);

            PT_UNIT_ASSERT(Pt::atomicCompareExchange(my_value, -200, -20) == -20);
            PT_UNIT_ASSERT(Pt::atomicGet(my_value) == -200);
        }

        void Pointer()
        {
            int            a = 1;
            int            b = 2;
            void* volatile p = 0;

            PT_UNIT_ASSERT(Pt::atomicExchange(p, (void*)&a) == 0);
            PT_UNIT_ASSERT(p == (void*)&a);

            PT_UNIT_ASSERT(Pt::atomicCompareExchange(p, (void*)&b, (void*)&a) == (void*)&a);
            PT_UNIT_ASSERT(p == (void*)&b);

            PT_UNIT_ASSERT(Pt::atomicCompareExchange(p, (void*)&a, (void*)&a) == (void*)&b);
            PT_UNIT_ASSERT(p == (void*)&b);
        }
};

Pt::Unit::RegisterTest<AtomicTestSuite> register_AtomicTestSuite;
