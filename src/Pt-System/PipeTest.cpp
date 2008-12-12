/*
 * Copyright (C) 2007 by Laurentiu-Gheorghe Crisan
 * Copyright (C) 2007 by Marc Boris D�rner
 * Copyright (C) 2007 by Bjoern Oliver Streule
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
#include "Pt/System/Pipe.h"
#include "Pt/System/Selector.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
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
        }

    protected:
        void testAsyncWriteRead();
        void testSyncWriteRead();
};

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
