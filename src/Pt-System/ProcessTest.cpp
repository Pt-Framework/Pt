/***************************************************************************
 *   Copyright (C) 2008 by Adrian Ghinet                                   *
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
 
#include "Pt/System/Process.h"
#include "Pt/System/Pipe.h"
 
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"
  
class ProcessTest : public Pt::Unit::TestSuite
{
	public:
		ProcessTest()
        : Pt::Unit::TestSuite("ProcessTest")
        {
		Pt::Unit::TestSuite::registerMethod( "redirectOutputStream", *this, &ProcessTest::redirectOutputStream );
        }

    protected:
        void redirectOutputStream();
};

void ProcessTest::redirectOutputStream()
{
#ifdef NDEBUG
    Pt::System::ProcessInfo procInfo( "ProcessTestChild");
#else
    Pt::System::ProcessInfo procInfo( "ProcessTestChildd");
#endif
    procInfo.addArgument( "testString");

    Pt::System::Pipe pipe;
    procInfo.setStdOutput( &pipe.output());

    Pt::System::Process p(procInfo);

    p.start();
    p.wait();

    char buffer[1024];
    int n = pipe.input().read( buffer, 1024);
    buffer[n] = '\0';

    reportMessage( std::string("child output: ") + buffer);

    PT_UNIT_ASSERT( n > 0);
	PT_UNIT_ASSERT( std::string( buffer) == "testString");
}

Pt::Unit::RegisterTest<ProcessTest> register_ProcessTest;
