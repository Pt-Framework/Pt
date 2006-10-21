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

#include "Pt/SourceInfo.h"
using namespace Pt;


void TestFunc1(void)
{
	const Pt::SourceInfo& siTestFunc1 = PT_SOURCEINFO;
	cerr << "const Pt::SourceInfo& siTestFunc1 = PT_SOURCEINFO;" << endl;
	cerr << "\t\tsiTestFunc1.file() = " << siTestFunc1.file() << endl;
	cerr << "\t\tsiTestFunc1.line() = " << siTestFunc1.line() << endl;
	cerr << "\t\tsiTestFunc1.func() = " << siTestFunc1.func() << endl;
	cerr << endl;
}

bool TestFunc2(void)
{
	const Pt::SourceInfo& siTestFunc2 = PT_SOURCEINFO;
	cerr << "const Pt::SourceInfo& siTestFunc2 = PT_SOURCEINFO;" << endl;
	cerr << "\t\tsiTestFunc2.file() = " << siTestFunc2.file() << endl;
	cerr << "\t\tsiTestFunc2.line() = " << siTestFunc2.line() << endl;
	cerr << "\t\tsiTestFunc2.func() = " << siTestFunc2.func() << endl;
	cerr << endl;
	return(true);
}

bool TestFunc3(float)
{
	const Pt::SourceInfo& siTestFunc3 = PT_SOURCEINFO;
	cerr << "const Pt::SourceInfo& siTestFunc3 = PT_SOURCEINFO;" << endl;
	cerr << "\t\tsiTestFunc3.file() = " << siTestFunc3.file() << endl;
	cerr << "\t\tsiTestFunc3.line() = " << siTestFunc3.line() << endl;
	cerr << "\t\tsiTestFunc3.func() = " << siTestFunc3.func() << endl;
	cerr << endl;
	return(true);
}


class TestClass {
	public:
		TestClass()
		{
			const Pt::SourceInfo& siTestCtor = PT_SOURCEINFO;
			cerr << "const Pt::SourceInfo& siTestCtor = PT_SOURCEINFO;" << endl;
			cerr << "\t\tsiTestCtor.file() = " << siTestCtor.file() << endl;
			cerr << "\t\tsiTestCtor.line() = " << siTestCtor.line() << endl;
			cerr << "\t\tsiTestCtor.func() = " << siTestCtor.func() << endl;
			cerr << endl;
		}

		~TestClass()
		{
			const Pt::SourceInfo& siTestDtor = PT_SOURCEINFO;
			cerr << "const Pt::SourceInfo& siTestDtor = PT_SOURCEINFO;" << endl;
			cerr << "\t\tsiTestDtor.file() = " << siTestDtor.file() << endl;
			cerr << "\t\tsiTestDtor.line() = " << siTestDtor.line() << endl;
			cerr << "\t\tsiTestDtor.func() = " << siTestDtor.func() << endl;
			cerr << endl;
		}

		void dummy()
		{
			class TestSubClass {
				public:
					TestSubClass()
					{
						const Pt::SourceInfo& siTestSubCtor = PT_SOURCEINFO;
						cerr << "const Pt::SourceInfo& siTestSubCtor = PT_SOURCEINFO;" << endl;
						cerr << "\t\tsiTestSubCtor.file() = " << siTestSubCtor.file() << endl;
						cerr << "\t\tsiTestSubCtor.line() = " << siTestSubCtor.line() << endl;
						cerr << "\t\tsiTestSubCtor.func() = " << siTestSubCtor.func() << endl;
						cerr << endl;
					}

					~TestSubClass()
					{
						const Pt::SourceInfo& siTestSubDtor = PT_SOURCEINFO;
						cerr << "const Pt::SourceInfo& siTestSubDtor = PT_SOURCEINFO;" << endl;
						cerr << "\t\tsiTestSubDtor.file() = " << siTestSubDtor.file() << endl;
						cerr << "\t\tsiTestSubDtor.line() = " << siTestSubDtor.line() << endl;
						cerr << "\t\tsiTestSubDtor.func() = " << siTestSubDtor.func() << endl;
						cerr << endl;
					}
			};

			TestSubClass foo;

			const Pt::SourceInfo& siTestDummy = PT_SOURCEINFO;
			cerr << "const Pt::SourceInfo& siTestDummy = PT_SOURCEINFO;" << endl;
			cerr << "\t\tsiTestDummy.file() = " << siTestDummy.file() << endl;
			cerr << "\t\tsiTestDummy.line() = " << siTestDummy.line() << endl;
			cerr << "\t\tsiTestDummy.func() = " << siTestDummy.func() << endl;
			cerr << endl;
		}
};


int main()
{
	cerr << "----- SourceInfoTest -----" << endl;

	try
	{
		cerr << endl;

		const Pt::SourceInfo& siA = PT_SOURCEINFO;
		cerr << "const Pt::SourceInfo& siA = PT_SOURCEINFO;" << endl;
		cerr << "\t\tsiA.file() = " << siA.file() << endl;
		cerr << "\t\tsiA.line() = " << siA.line() << endl;
		cerr << "\t\tsiA.func() = " << siA.func() << endl;
		cerr << endl;

		const Pt::SourceInfo siB(siA);
		cerr << "const Pt::SourceInfo siB(siA);" << endl;
		cerr << "\t\tsiB.file() = " << siB.file() << endl;
		cerr << "\t\tsiB.line() = " << siB.line() << endl;
		cerr << "\t\tsiB.func() = " << siB.func() << endl;
		cerr << endl;

		const Pt::SourceInfo& siC = PT_SOURCEINFO;
		      Pt::SourceInfo  siD = PT_SOURCEINFO;
		cerr << "const Pt::SourceInfo& siC = PT_SOURCEINFO;" << endl;
		cerr << "      Pt::SourceInfo& siD = PT_SOURCEINFO;" << endl;
		cerr << "\t\tsiC.file() = " << siC.file() << endl;
		cerr << "\t\tsiC.line() = " << siC.line() << endl;
		cerr << "\t\tsiC.func() = " << siC.func() << endl;
		cerr << "\t\tsiD.file() = " << siD.file() << endl;
		cerr << "\t\tsiD.line() = " << siD.line() << endl;
		cerr << "\t\tsiD.func() = " << siD.func() << endl;
		siD = siC;
		cerr << "siD = siC;" << endl;
		cerr << "\t\tsiD.file() = " << siD.file() << endl;
		cerr << "\t\tsiD.line() = " << siD.line() << endl;
		cerr << "\t\tsiD.func() = " << siD.func() << endl;
		cerr << endl;

		TestFunc1();
		TestFunc2();
		TestFunc3(1.0f);

		{
			TestClass testInst;
			testInst.dummy();
		}
	}
	catch(const std::exception& e) {
		cerr << "failed. " << e.what() << endl;
		return 1;
	}

	return 0;
}
