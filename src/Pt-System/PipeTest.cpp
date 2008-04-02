/***************************************************************************
 *   Copyright (C) 2007 by Laurentiu-Gheorghe Crisan                       *
 *   Copyright (C) 2007 by Marc Boris D�rner                               *
 *   Copyright (C) 2007 by Bjoern Oliver Streule                           *
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
#include "Pt/System/Pipe.h"
#include "Pt/System/Selector.h"
#include "Pt/System/Process.h"

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>


class PipeTest : public Pt::Unit::TestSuite
{
    public:
        PipeTest()
        : Pt::Unit::TestSuite("PipeTest")
        {
            Pt::Unit::TestSuite::registerMethod( "testAsyncWriteRead", *this, &PipeTest::testAsyncWriteRead );
            Pt::Unit::TestSuite::registerMethod( "testSyncWriteRead", *this, &PipeTest::testSyncWriteRead );
	    Pt::Unit::TestSuite::registerMethod( "redirectStreams", *this, &PipeTest::redirectStreams );
        }

    protected:
        void testAsyncWriteRead();
        void testSyncWriteRead();
        void redirectStreams();
};

void PipeTest::redirectStreams()
{
    Pt::System::Process p("dir");
    Pt::System::Pipe pi();


    p.setOutput( pi.output());

    p.start();
    p.wait();

    char buffer[1024];
    int n = pi.input().read( buffer, 1024);

    PT_UNIT_ASSERT( n > 0);
}


void PipeTest::testAsyncWriteRead()
{
    std::string out("Hello World, where do you want to GOTO day!");    
    std::string in;
    const int size = 8;
    char buffer[size];

    Pt::System::Pipe pipe( Pt::System::IODevice::Async );
    Pt::System::Selector selector;
    
    std::size_t writtenBytes = 0;
    while( writtenBytes < out.size() )
    {
        Pt::System::IOResult& writeRes = pipe.output().beginWrite( out.c_str() + writtenBytes, 
                                                                   out.size() - writtenBytes );
        selector.add(writeRes);
        bool activity = selector.wait(500);
        PT_UNIT_ASSERT_MSG(activity, "Wait output failed");

        writtenBytes += pipe.output().endWrite(writeRes);
    }

    std::size_t readBytes = 0;
    while( readBytes < out.size() )
    {       
        Pt::System::IOResult& res = pipe.input().beginRead(buffer, size);
        selector.add(res);
        bool activity = selector.wait(500);
        PT_UNIT_ASSERT_MSG(activity, "Wait input failed");

        const std::size_t n = pipe.input().endRead(res);
        readBytes +=  n;
        in.append(buffer, n);
    }

    PT_UNIT_ASSERT_MSG( in == out, "Input does not match output");    
}

void PipeTest::testSyncWriteRead()
{
    std::string out("Hello World, where do you want to GOTO day!");    
    std::string in;
    const int size = 8;
    char buffer[size];    

    Pt::System::Pipe pipe(Pt::System::IODevice::Sync);

    std::size_t writtenBytes = 0;
    while(writtenBytes < out.size())
    {
        writtenBytes += pipe.output().write( out.c_str(), out.size() );
    }

    std::size_t totalReadBytes = 0;
    while(totalReadBytes < out.size())
    {
        const std::size_t readBytes = pipe.input().read(buffer, size);
        totalReadBytes +=  readBytes;
        in.append(buffer, readBytes);
    }

    PT_UNIT_ASSERT_MSG( in == out, "Input does not match output");
}

Pt::Unit::RegisterTest<PipeTest> register_PipeTest;
