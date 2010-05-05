
#include <Pt/Alloc/Chunk.h>
#include <Pt/Alloc/PoolAllocator.h>
#include <Pt/Types.h>
#include <Pt/DateTime.h>
#include <Pt/System/Clock.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/RegisterTest.h>

#include "ChunkTest.h"

void ChunkTest::initTest()
{
    Pt::Alloc::Chunk chunk;
    chunk.init(4, 10);
    PT_UNIT_ASSERT(chunk._pData > 0 );
    PT_UNIT_ASSERT(chunk._blocksAvailable == 10);
    PT_UNIT_ASSERT(chunk._firstAvailableBlock == 0);
}

void ChunkTest::trimTest()
{
	Pt::Alloc::PoolAllocator pool(4096, 256, 4);

	unsigned* tmp;

	for( int i=0; i<2000; i++)
	{
		tmp = (unsigned*)pool.allocate(sizeof(unsigned));

		pool.deallocate(tmp, sizeof(unsigned) );

		tmp++;
	}
}

void ChunkTest::complexAllocateDeallocateTest()
{
    Pt::Alloc::Chunk chunkArray;
    std::vector<Pt::Alloc::Chunk> test;
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
                   Pt::Alloc::Chunk chunkArray1;
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
    //std::cout << "Elapsed time: " << (int)timeSpan.msecs() << " ms\n";
}

void ChunkTest::allocateDeallocate()
{
    Pt::uint32_t* array[100];
    Pt::Alloc::Chunk chunk;
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

Pt::Unit::RegisterTest<ChunkTest> register_ChunkTest;


