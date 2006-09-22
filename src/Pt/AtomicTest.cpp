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
#include "Pt/Method.h"
#include "Pt/Unit/TestMain.h"

#include <string>

using namespace std;
using namespace Pt;


class AtomicTest : public Pt::Unit::TestCase
{
	public:
		AtomicTest()
		: Pt::Unit::TestCase("AtomicIntTest")
		{
			Unit::TestCase::registerTest( callable(this, &AtomicTest::test), "test" );
		}

	protected:
		void test()
		{
			AtomicInt a(5);
			PT_UNIT_ASSERT( a.value() == 5 );

			int value = a.value();
			PT_UNIT_ASSERT( value == 5 );

			a = 10;
			PT_UNIT_ASSERT( a.value() == 10 );

			a -= 3;
			PT_UNIT_ASSERT( a.value() == 7 );

			a += 2;
			PT_UNIT_ASSERT( a.value() == 9 );
		}
};


static AtomicTest atomicTest;



/*
int main(int agrv, char* argv[])
{
			const size_t len = strlen( argv[0] );
			char buffer[len + 1];
			memcpy(buffer, argv[0], len);
			buffer[len+1] = '\0';
			cerr << buffer << endl;
			
	cerr << "\n----- AtomicTest -----" << endl;

	cerr << "AtomicInt::AtomicInt(atomic_t): ";
	AtomicInt a(5);
	assert(a.value() == 5);
	cerr << "ok\n";

	cerr << "AtomicInt::value(): ";
	int value = a.value();
	assert(value == 5);
	cerr << "ok\n";

	cerr << "AtomicInt::operator=: ";
	a = 10;
	assert(a.value() == 10);
	cerr << "ok\n";

	cerr << "AtomicInt::operator-=: ";
	a -= 3;
	assert(a.value() == 7);
	cerr << "ok\n";

	cerr << "AtomicInt::operator+=: ";
	a += 2;
	assert(a.value() == 9);
	cerr << "ok\n";

	cerr << "=> success.\n";
	return 0;
}
*/



