/***************************************************************************
 *   Copyright (C) 2005 Aloysius Indrayanto                                *
 *   Copyright (C) 2004 Marc Boris Duerner                                 *
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
#include <sstream>
using namespace std;

#include "Pt/Types.h"
using namespace Pt;

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestFixture.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"


class TypesTest : public Pt::Unit::TestSuite
{
	public:
		TypesTest()
		: TestSuite("TypesTest")
		{
			this->registerMethod("test", *this, &TypesTest::test);;
		}

		void test1(int a)
		{
			std::cerr << "\ncalled: " << a << "\n";
		}

		void test()
		{
			stringstream ss;

			PT_UNIT_ASSERT( sizeof(Pt::int8_t) == 1 );
			PT_UNIT_ASSERT( sizeof(Pt::uint8_t) == 1 );
			PT_UNIT_ASSERT( sizeof(Pt::int16_t) == 2 );
			PT_UNIT_ASSERT( sizeof(Pt::uint16_t) == 2 );
			PT_UNIT_ASSERT( sizeof(Pt::int32_t) == 4 );
			PT_UNIT_ASSERT( sizeof(Pt::uint32_t) == 4 );

			PT_UNIT_ASSERT( sizeof(float) == 4 );
			PT_UNIT_ASSERT( sizeof(double) == 8 );

			ss << endl;
			ss << "sizeof(Pt::uchar   ) = " << sizeof(Pt::uchar   ) << endl;
			ss << "sizeof(Pt::ushort  ) = " << sizeof(Pt::ushort  ) << endl;
			ss << "sizeof(Pt::uint    ) = " << sizeof(Pt::uint    ) << endl;
			ss << "sizeof(Pt::ulong   ) = " << sizeof(Pt::ulong   ) << endl;
			ss << endl;
			ss << "sizeof(Pt::size_t  ) = " << sizeof(Pt::size_t  ) << endl;
			ss << "sizeof(Pt::ssize_t ) = " << sizeof(Pt::ssize_t ) << endl;
			ss << endl;
			ss << "sizeof(Pt::int8_t  ) = " << sizeof(Pt::int8_t  ) << endl;
			ss << "sizeof(Pt::uint8_t ) = " << sizeof(Pt::uint8_t ) << endl;
			ss << endl;
			ss << "sizeof(Pt::int16_t ) = " << sizeof(Pt::int16_t ) << endl;
			ss << "sizeof(Pt::uint16_t) = " << sizeof(Pt::uint16_t) << endl;
			ss << endl;
			ss << "sizeof(Pt::int32_t ) = " << sizeof(Pt::int32_t ) << endl;
			ss << "sizeof(Pt::uint32_t) = " << sizeof(Pt::uint32_t) << endl;
			ss << endl;
#ifdef PT_64BIT
			PT_UNIT_ASSERT( sizeof(Pt::int64_t) == 8 );
			PT_UNIT_ASSERT( sizeof(Pt::uint64_t) == 8 );
			ss << "sizeof(Pt::int64_t ) = " << sizeof(Pt::int64_t ) << endl;
			ss << "sizeof(Pt::uint64_t) = " << sizeof(Pt::uint64_t) << endl;
			ss << endl;
#endif
			ss << "sizeof(float       ) = " << sizeof(float       ) << endl;
			ss << "sizeof(double      ) = " << sizeof(double      ) << endl;
			ss << endl;

			//Unit::Application::message(ss.str());
			cerr << ss.str();
		}
};

Pt::Unit::RegisterTest<TypesTest> register_TypesTest;

