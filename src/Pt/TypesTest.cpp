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

#include "Pt/Types.h"
using namespace Pt;


int main()
{
	cerr << "----- TypesTest -----" << endl;

	try
	{
		cerr << endl;
		cerr << "sizeof(Pt::uchar   ) = " << sizeof(Pt::uchar   ) << endl;
		cerr << "sizeof(Pt::ushort  ) = " << sizeof(Pt::ushort  ) << endl;
		cerr << "sizeof(Pt::uint    ) = " << sizeof(Pt::uint    ) << endl;
		cerr << "sizeof(Pt::ulong   ) = " << sizeof(Pt::ulong   ) << endl;
		cerr << endl;
		cerr << "sizeof(Pt::size_t  ) = " << sizeof(Pt::size_t  ) << endl;
		cerr << "sizeof(Pt::ssize_t ) = " << sizeof(Pt::ssize_t ) << endl;
		cerr << endl;
		cerr << "sizeof(Pt::int8_t  ) = " << sizeof(Pt::int8_t  ) << endl;
		cerr << "sizeof(Pt::uint8_t ) = " << sizeof(Pt::uint8_t ) << endl;
		cerr << endl;
		cerr << "sizeof(Pt::int16_t ) = " << sizeof(Pt::int16_t ) << endl;
		cerr << "sizeof(Pt::uint16_t) = " << sizeof(Pt::uint16_t) << endl;
		cerr << endl;
		cerr << "sizeof(Pt::int32_t ) = " << sizeof(Pt::int32_t ) << endl;
		cerr << "sizeof(Pt::uint32_t) = " << sizeof(Pt::uint32_t) << endl;
		cerr << endl;
#ifdef PT_64BIT
		cerr << "sizeof(Pt::int64_t ) = " << sizeof(Pt::int64_t ) << endl;
		cerr << "sizeof(Pt::uint64_t) = " << sizeof(Pt::uint64_t) << endl;
		cerr << endl;
#endif
	}
	catch(const std::exception& e) {
		cerr << "failed. " << e.what() << endl;
		return 1;
	}

	return 0;
}

