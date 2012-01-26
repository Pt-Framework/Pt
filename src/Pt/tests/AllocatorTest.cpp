/*
 * Copyright (C) 2010 by Bendri Batti
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

#include <Pt/PoolAllocator.h>
#include <Pt/PageAllocator.h>
#include <Pt/Types.h>
#include <Pt/DateTime.h>
#include <Pt/System/Clock.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/RegisterTest.h>
#include <Pt/Unit/TestSuite.h>

class AllocatorTest : public Pt::Unit::TestSuite
{
    public:
        AllocatorTest() : Pt::Unit::TestSuite("AllocatorTest")
        {  
            Pt::Unit::TestSuite::registerMethod( "PageAllocatorBenchmark", *this, &AllocatorTest::PageAllocatorBenchmark );
            Pt::Unit::TestSuite::registerMethod( "PoolAllocatorBenchmark", *this, &AllocatorTest::PoolAllocatorBenchmark );
            Pt::Unit::TestSuite::registerMethod( "MemoryPool", *this, &AllocatorTest::MemoryPool );
            Pt::Unit::TestSuite::registerMethod( "PoolAllocator", *this, &AllocatorTest::PoolAllocator );
            Pt::Unit::TestSuite::registerMethod( "PageAllocator", *this, &AllocatorTest::PageAllocator );
        }

    protected:
        void PageAllocatorBenchmark();
        void PoolAllocatorBenchmark();
        void MemoryPool();
        void PoolAllocator();
        void PageAllocator();
};


void AllocatorTest::PageAllocatorBenchmark()
{
    double* allocated[1000];

    //
    // the page allocator
    //
    Pt::PageAllocator allocator;
    Pt::DateTime timeStart = Pt::System::Clock::getLocalTime();

    for(unsigned j = 0; j < 2000; ++j)
    {
        for(unsigned i = 0; i < 1000; ++i)
            allocated[i] = (double*) allocator.allocate( sizeof(double) );

        for(unsigned i = 0; i < 1000; ++i)
            allocator.deallocate( (void*)allocated[i], sizeof(double) );
    }

    Pt::DateTime timeEnd  = Pt::System::Clock::getLocalTime();
    Pt::Timespan timeSpan = timeEnd - timeStart;
    std::cout << "\npage allocator: " << (int)timeSpan.msecs() << " ms\n";

    //
    // the system default allocator
    //
    timeStart = Pt::System::Clock::getLocalTime();

    for(unsigned j = 0; j < 2000; ++j)
    {
        for(unsigned i = 0; i < 1000; ++i)
            allocated[i] = new double;

        for(unsigned i = 0; i < 1000; ++i)
            delete allocated[i];
    }

    timeEnd  = Pt::System::Clock::getLocalTime();
    timeSpan = timeEnd - timeStart;
    std::cout << "default allocator: " << (int)timeSpan.msecs() << " ms\n";
}


void AllocatorTest::PoolAllocatorBenchmark()
{
    double* allocated[1000];

    //
    // the pool allocator
    //
    Pt::PoolAllocator allocator(64, 16, 4096);
    Pt::DateTime timeStart = Pt::System::Clock::getLocalTime();

    for(unsigned j = 0; j < 2000; ++j)
    {
        for(unsigned i = 0; i < 1000; ++i)
            allocated[i] = (double*) allocator.allocate( sizeof(double) );

        for(unsigned i = 0; i < 1000; ++i)
            allocator.deallocate( (void*)allocated[i], sizeof(double) );
    }

    Pt::DateTime timeEnd  = Pt::System::Clock::getLocalTime();
    Pt::Timespan timeSpan = timeEnd - timeStart;
    std::cout << "\npool allocator: " << (int)timeSpan.msecs() << " ms\n";

    //
    // the system default allocator
    //
    timeStart = Pt::System::Clock::getLocalTime();

    for(unsigned j = 0; j < 2000; ++j)
    {
        for(unsigned i = 0; i < 1000; ++i)
            allocated[i] = new double;

        for(unsigned i = 0; i < 1000; ++i)
            delete allocated[i];
    }

    timeEnd  = Pt::System::Clock::getLocalTime();
    timeSpan = timeEnd - timeStart;
    std::cout << "default allocator: " << (int)timeSpan.msecs() << " ms\n";
}


void AllocatorTest::MemoryPool()
{
    Pt::MemoryPool pool( sizeof(Pt::uint32_t), 4096 );

    Pt::uint32_t* array[2000];

    for (Pt::uint32_t i = 0; i < 2000; i++)
    {
        array[i] = (Pt::uint32_t*) pool.allocate();
    }

    for (Pt::uint32_t i = 0; i < 1000; i++)
    {
        pool.deallocate( array[i] );
    }

    for (Pt::uint32_t i = 0; i < 1000; i++)
    {
        array[i] = (Pt::uint32_t*) pool.allocate();
    }

    for (Pt::uint32_t i = 0; i < 2000; i++)
    {
        pool.deallocate( array[i] );
    }
}


void AllocatorTest::PoolAllocator()
{
    Pt::PoolAllocator alloc( 256, 8, 4096 );

    Pt::uint32_t* array[2000];

    for (Pt::uint32_t i = 0; i < 2000; i++)
    {
        array[i] = (Pt::uint32_t*) alloc.allocate( sizeof(Pt::uint32_t) );
    }

    for (Pt::uint32_t i = 0; i < 1000; i++)
    {
        alloc.deallocate( array[i], sizeof(Pt::uint32_t) );
    }

    for (Pt::uint32_t i = 0; i < 1000; i++)
    {
        array[i] = (Pt::uint32_t*) alloc.allocate( sizeof(Pt::uint32_t) );
    }

    for (Pt::uint32_t i = 0; i < 2000; i++)
    {
        alloc.deallocate( array[i], sizeof(Pt::uint32_t) );
    }
}


void AllocatorTest::PageAllocator()
{
    Pt::PageAllocator alloc;

    Pt::uint32_t* array[1000];

    for (Pt::uint32_t i = 0; i < 1000; i++)
    {
        array[i] = (Pt::uint32_t*) alloc.allocate( sizeof(Pt::uint32_t) );
    }

    for (Pt::uint32_t i = 0; i < 500; i++)
    {
        alloc.deallocate( array[i], sizeof(Pt::uint32_t) );
    }

    for (Pt::uint32_t i = 0; i < 500; i++)
    {
        array[i] = (Pt::uint32_t*) alloc.allocate( sizeof(Pt::uint32_t) );
    }

    for (Pt::uint32_t i = 0; i < 1000; i++)
    {
        alloc.deallocate( array[i], sizeof(Pt::uint32_t) );
    }
}

Pt::Unit::RegisterTest<AllocatorTest> register_AllocatorTest;

