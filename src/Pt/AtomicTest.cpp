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

            Pt::Unit::TestSuite::registerMethod( "new_GetSet", *this, &AtomicTestSuite::new_GetSet );
            Pt::Unit::TestSuite::registerMethod( "new_Integer", *this, &AtomicTestSuite::new_Integer );
            Pt::Unit::TestSuite::registerMethod( "new_Pointer", *this, &AtomicTestSuite::new_Pointer );
        }

    protected:
        ////////////////////////////////////////// START OF TEMPORARY TESTING ///////////////////////////////////////////
        void new_GetSet()
        {
            std::cout << "\n####################################################################\n";

            volatile Pt::new_atomic_t my_value(0);

            Pt::new_atomicSet(my_value, 3);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == 3);

            Pt::new_atomicSet(my_value, 0);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == 0);
        }

        void new_Integer()
        {
            std::cout << "\n####################################################################\n";

            PT_UNIT_ASSERT( sizeof(Pt::new_atomic_t) == sizeof(void*) );

            volatile Pt::new_atomic_t my_value(0);

            PT_UNIT_ASSERT(Pt::new_atomicIncrement(my_value) == 1);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == 1);

            PT_UNIT_ASSERT(Pt::new_atomicIncrement(my_value) == 2);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == 2);

            PT_UNIT_ASSERT(Pt::new_atomicDecrement(my_value) == 1);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == 1);

            PT_UNIT_ASSERT(Pt::new_atomicDecrement(my_value) == 0);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == 0);

            PT_UNIT_ASSERT(Pt::new_atomicDecrement(my_value) == -1);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == -1);

            PT_UNIT_ASSERT(Pt::new_atomicDecrement(my_value) == -2);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == -2);

            PT_UNIT_ASSERT(Pt::new_atomicExchange(my_value, -5) == -2);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == -5);

            PT_UNIT_ASSERT(Pt::new_atomicExchange(my_value, 1) == -5);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == 1);

            PT_UNIT_ASSERT(Pt::new_atomicExchange(my_value, 0) == 1);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == 0);

            PT_UNIT_ASSERT(Pt::new_atomicExchangeAdd(my_value, 3) == 0);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == 3);

            PT_UNIT_ASSERT(Pt::new_atomicExchangeAdd(my_value, -5) == 3);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == -2);

            PT_UNIT_ASSERT(Pt::new_atomicExchangeAdd(my_value, 6) == -2);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == 4);

            PT_UNIT_ASSERT(Pt::new_atomicCompareExchange(my_value, 5, 4) == 4);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == 5);

            PT_UNIT_ASSERT(Pt::new_atomicCompareExchange(my_value, 9, 7) == 5);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == 5);

            PT_UNIT_ASSERT(Pt::new_atomicCompareExchange(my_value, -20, 5) == 5);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == -20);

            PT_UNIT_ASSERT(Pt::new_atomicCompareExchange(my_value, -200, -20) == -20);
            PT_UNIT_ASSERT(Pt::new_atomicGet(my_value) == -200);
        }

        void new_Pointer()
        {
            std::cout << "\n####################################################################\n";

            int            a = 1;
            int            b = 2;
            void* volatile p = 0;

            PT_UNIT_ASSERT(Pt::new_atomicExchange(p, (void*)&a) == 0);
            PT_UNIT_ASSERT(p == (void*)&a);

            PT_UNIT_ASSERT(Pt::new_atomicCompareExchange(p, (void*)&b, (void*)&a) == (void*)&a);
            PT_UNIT_ASSERT(p == (void*)&b);

            PT_UNIT_ASSERT(Pt::new_atomicCompareExchange(p, (void*)&a, (void*)&a) == (void*)&b);
            PT_UNIT_ASSERT(p == (void*)&b);
        }

        ////////////////////////////////////////// END OF TEMPORARY TESTING ///////////////////////////////////////////

        void GetSet()
        {
			volatile Pt::atomic_t x = 1;
			Pt::atomicSet(x, 3);

			volatile Pt::atomic_t y = Pt::atomicGet(x);
			PT_UNIT_ASSERT(y == 3);
		}

        void Integer()
        {
            PT_UNIT_ASSERT( sizeof(Pt::atomic_t) == sizeof(void*) );

            volatile Pt::atomic_t v = 0;

            Pt::atomicIncrement(v);
            PT_UNIT_ASSERT(v == 1);

            Pt::atomicIncrement(v);
            PT_UNIT_ASSERT(v == 2);

            Pt::atomicDecrement(v);
            PT_UNIT_ASSERT(v == 1);

            Pt::atomicDecrement(v);
            PT_UNIT_ASSERT(v == 0);

            Pt::atomicDecrement(v);
            PT_UNIT_ASSERT(v == -1);

            Pt::atomicDecrement(v);
            PT_UNIT_ASSERT(v == -2);

            Pt::atomicExchange(v, -5);
            PT_UNIT_ASSERT(v == -5);

            Pt::atomicExchange(v, 1);
            PT_UNIT_ASSERT(v == 1);

            Pt::atomicExchange(v, 0);
            PT_UNIT_ASSERT(v == 0);

            Pt::atomicExchangeAdd(v, 3);
            PT_UNIT_ASSERT(v == 3);

            Pt::atomicExchangeAdd(v, -5);
            PT_UNIT_ASSERT(v == -2);

            Pt::atomicExchangeAdd(v, 6);
            PT_UNIT_ASSERT(v == 4);

            Pt::atomicCompareExchange(v, 5, 4);
            PT_UNIT_ASSERT(v == 5);

            Pt::atomicCompareExchange(v, 9, 7);
            PT_UNIT_ASSERT(v == 5);

            Pt::atomicCompareExchange(v, -20, 5);
            PT_UNIT_ASSERT(v == -20);

            Pt::atomicCompareExchange(v, -200, -20);
            PT_UNIT_ASSERT(v == -200);
        }

        void Pointer()
        {
            int a = 0, b = 1;
            void* volatile p = 0;

            Pt::atomicExchange( p, (void*)&a );
            PT_UNIT_ASSERT(p == (void*)&a);

            Pt::atomicCompareExchange( p, (void*)(&b), (void*)&a );
            PT_UNIT_ASSERT(p == (void*)&b);

            Pt::atomicCompareExchange( p, (void*)(&a), (void*)&a );
            PT_UNIT_ASSERT(p == (void*)&b);
        }
};

Pt::Unit::RegisterTest<AtomicTestSuite> register_AtomicTestSuite;
