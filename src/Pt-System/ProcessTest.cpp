/*
 * Copyright (C) 2008 by Adrian Ghinet
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
#include "Pt/System/Process.h"
#include "Pt/System/Pipe.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

class ProcessTest : public Pt::Unit::TestSuite
{
    public:
        ProcessTest()
        : Pt::Unit::TestSuite("ProcessTest")
        {
            Pt::Unit::TestSuite::registerMethod( "RedirectIO", *this, &ProcessTest::RedirectIO );
            Pt::Unit::TestSuite::registerMethod( "EnvVar", *this, &ProcessTest::EnvVar );
        }

    protected:
        void RedirectIO();
        void EnvVar();
};


void ProcessTest::RedirectIO()
{
#ifdef _WIN32_WCE
    return;
#endif

#ifdef NDEBUG
    Pt::System::ProcessInfo procInfo( "ProcessTestChild");
#else
    Pt::System::ProcessInfo procInfo( "ProcessTestChildd");
#endif
    procInfo.addArg( "testString");

    Pt::System::Pipe pipe;
    procInfo.setStdOutput(&pipe.in());
    procInfo.setStdError(0);

    Pt::System::Process p(procInfo);

    p.setEnvVar("PATH", ".");
    p.start();
    p.wait();

    char buffer[1024];
    int n = pipe.out().read( buffer, 1024);
    buffer[n] = '\0';

    reportMessage( std::string("child output: ") + buffer);

    PT_UNIT_ASSERT( n > 0);
    PT_UNIT_ASSERT( std::string( buffer) == "testString");
}


void ProcessTest::EnvVar()
{
    Pt::System::Process::setEnvVar("PT_PROCESS_TEST", "true");
    std::string value = Pt::System::Process::getEnvVar("PT_PROCESS_TEST");
	reportMessage( "value: " + value);
    PT_UNIT_ASSERT( value == "true");
}


Pt::Unit::RegisterTest<ProcessTest> register_ProcessTest;
