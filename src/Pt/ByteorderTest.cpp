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
using namespace std;

#define PT_LE

#include "Pt/Byteorder.h"
using namespace Pt;

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestFixture.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"


class ByteorderTest : public Pt::Unit::TestSuite
{
	public:
		ByteorderTest()
	: TestSuite("ByteorderTest")
		{
			this->registerMethod("test", *this, &ByteorderTest::test);
		}

		virtual void test()
		{
			Pt::uint8_t  val8u  = 0x10;
			Pt::uint16_t val16u = 0x1020;
			Pt::uint32_t val32u = 0x10203040;
#ifdef PT_64BIT
			Pt::uint64_t val64u = 0x1020304050607080ULL;
#endif

			cerr << "Before swap()s:" << endl;
			cerr << hex << "0x" << (void*) val8u  << endl;
			cerr << hex << "0x" <<         val16u << endl;
			cerr << hex << "0x" <<         val32u << endl;
#ifdef PT_64BIT
			cerr << hex << "0x" <<         val64u << endl;
#endif

			cerr << endl;
			val8u  = swap(val8u);
			val16u = swap(val16u);
			val32u = swap(val32u);
#ifdef PT_64BIT
			val64u = swap(val64u);
#endif

			cerr << "After swap()s:" << endl;
			cerr << hex << "0x" << (void*) val8u  << endl;
			cerr << hex << "0x" <<         val16u << endl;
			cerr << hex << "0x" <<         val32u << endl;
#ifdef PT_64BIT
			cerr << hex << "0x" <<         val64u << endl;
#endif

			cerr << endl;
			swabUnaligned(reinterpret_cast<Pt::uint8_t*>(&val16u), sizeof(val16u));
			swabUnaligned(reinterpret_cast<Pt::uint8_t*>(&val32u), sizeof(val32u));
#ifdef PT_64BIT
			swabUnaligned(reinterpret_cast<Pt::uint8_t*>(&val64u), sizeof(val64u));
#endif

			cerr << "After swabUnaligned()s:" << endl;
			cerr << hex << "0x" << val16u << endl;
			cerr << hex << "0x" << val32u << endl;
#ifdef PT_64BIT
			cerr << hex << "0x" << val64u << endl;
#endif
		}

	};

Pt::Unit::RegisterTest<ByteorderTest> register_ByteorderTest;
