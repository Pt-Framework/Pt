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

#include "Chunk.h"
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
        Pt::Unit::TestSuite::registerMethod( "Test chunk complex test for allocate and deallocate method", *this, &AllocatorTest::complexAllocateDeallocateTest );
        Pt::Unit::TestSuite::registerMethod( "Test chunk allocate deallocate method", *this, &AllocatorTest::allocateDeallocate );
		Pt::Unit::TestSuite::registerMethod( "Test the trim method", *this, &AllocatorTest::trimTest );
		Pt::Unit::TestSuite::registerMethod( "benchMarkPrimitiveTypes", *this, &AllocatorTest::benchMarkPrimitiveTypes );
    }

protected:
    void initTest();
    void complexAllocateDeallocateTest();
    void allocateDeallocate();
	void trimTest();
	void benchMarkPrimitiveTypes();

};


void AllocatorTest::benchMarkPrimitiveTypes()
{
	Pt::uint16_t* arrayOfUnsigned[1000];

	Pt::DateTime timeStart = Pt::System::Clock::getLocalTime();
	Pt::PageAllocator pageAllocator;
	for(Pt::uint32_t j = 0; j < 500; ++j)
	{
		for(Pt::uint32_t i = 0; i < 1000; ++i)
		{
			arrayOfUnsigned[i] = (Pt::uint16_t*)pageAllocator.allocate(sizeof(Pt::uint16_t));
		}

		for(Pt::uint32_t i = 0; i < 1000; ++i)
		{
			pageAllocator.deallocate((void*)arrayOfUnsigned[i], sizeof(Pt::uint16_t));
		}
	}
	Pt::DateTime timeEnd  = Pt::System::Clock::getLocalTime();
	Pt::Timespan timeSpan = timeEnd - timeStart;
	std::cout << "Elapsed time with page allocator: " << (int)timeSpan.msecs() << " ms\n";

	Pt::DateTime timeStart1 = Pt::System::Clock::getLocalTime();
	for(Pt::uint32_t j = 0; j < 500; ++j)
	{
		for(Pt::uint32_t i = 0; i < 1000; ++i)
		{
			arrayOfUnsigned[i] = new Pt::uint16_t;
		}

		for(Pt::uint32_t i = 0; i < 1000; ++i)
		{
			delete arrayOfUnsigned[i];
		}
	}
	Pt::DateTime timeEnd1  = Pt::System::Clock::getLocalTime();
	Pt::Timespan timeSpan1 = timeEnd1 - timeStart1;
	std::cout << "Elapsed time without page allocator: " << (int)timeSpan1.msecs() << " ms\n";
}

void AllocatorTest::trimTest()
{
	Pt::PoolAllocator pool(4096, 256, 4);

	unsigned* tmp;

	for( int i=0; i<2000; i++)
	{
		tmp = (unsigned*)pool.allocate(sizeof(unsigned));

		pool.deallocate(tmp, sizeof(unsigned) );

		tmp++;
	}
}

void AllocatorTest::complexAllocateDeallocateTest()
{
    Pt::Chunk chunkArray;
    std::vector<Pt::Chunk> test;
    Pt::uint32_t* array[1024];
    test.push_back(chunkArray);
	test[0].init(sizeof(Pt::uint32_t), 255);

    Pt::DateTime timeStart = Pt::System::Clock::getLocalTime();
    Pt::uint32_t k = 0;
    for(Pt::uint32_t i=0; i < 500; i++)
    {
        for(Pt::uint32_t j=0; j < 1024; j++)
        {
            Pt::uint32_t* array1 = (Pt::uint32_t*)test[k].allocate(sizeof(Pt::uint32_t));
             if(array1 == NULL)
             {
                k++;
                if(k > test.size()-1)
                {
                    Pt::Chunk chunkArray1;
                    std::size_t size = test.size();
                    test.reserve(size * 2);
                    chunkArray1.init(sizeof(Pt::uint32_t), 255);
                    test.push_back(chunkArray1);
                }
                array1 = (Pt::uint32_t*)test[k].allocate(sizeof(Pt::uint32_t));
             }
             array[j] = array1;
        }
         k = 0;
         for(Pt::uint32_t j=0; j < 1024;)
         {
             Pt::uint32_t l = 0;
             while(j < 1024 && l < 255)
             {
                 test[k].deallocate(array[j], sizeof(Pt::uint32_t));
                 j++;
                 l++;
             }
             k++;
         }
         k = 0;
    }
     Pt::DateTime timeEnd  = Pt::System::Clock::getLocalTime();
     Pt::Timespan timeSpan = timeEnd - timeStart;
     Pt::uint32_t m = 0;
     while(m < test.size())
     {
         test[m].release();
         m++;
     }
     test.clear();
     std::cout << "Elapsed time: " << (int)timeSpan.msecs() << " ms\n";
}

void AllocatorTest::allocateDeallocate()
{
    Pt::uint32_t* array[100];
    Pt::Chunk chunk;
    chunk.init(sizeof(Pt::uint32_t), 255);
    PT_UNIT_ASSERT(chunk._pData > 0 );
    PT_UNIT_ASSERT(chunk._blocksAvailable == 255);
    PT_UNIT_ASSERT(chunk._firstAvailableBlock == 0);

    for (Pt::uint32_t i = 0; i < 100; i++)
    {
        array[i] = (Pt::uint32_t*)chunk.allocate(sizeof(Pt::uint32_t));
        PT_UNIT_ASSERT(array[i] > 0);
        PT_UNIT_ASSERT(chunk._blocksAvailable == 255 - (i+1));
        PT_UNIT_ASSERT(chunk._firstAvailableBlock == (i+1));
    }

    for (Pt::uint32_t i = 0; i < 100; i++)
    {
        chunk.deallocate(array[i], sizeof(Pt::uint32_t));
        PT_UNIT_ASSERT(chunk._firstAvailableBlock == i);
        PT_UNIT_ASSERT(chunk._blocksAvailable == 155 + (i+1));
    }
}

Pt::Unit::RegisterTest<AllocatorTest> register_AllocatorTest;
