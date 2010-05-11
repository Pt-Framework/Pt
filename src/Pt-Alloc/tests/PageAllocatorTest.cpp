/***************************************************************************
 *   Copyright (C) 2009-2010 by Bendri Batti                               *
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

#ifndef PAGE_ALLOCATOR_TEST_H
#define PAGE_ALLOCATOR_TEST_H

#undef PT_API_EXPORT

#include <Pt/Unit/TestSuite.h>
#include <Pt/Alloc/PageAllocator.h>
#include <Pt/Types.h>
#include <Pt/DateTime.h>
#include <Pt/System/Clock.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/RegisterTest.h>

class PageAllocatorTest : public Pt::Unit::TestSuite
{
public:
    PageAllocatorTest() : Pt::Unit::TestSuite("PageAllocatorTest")
    {
        Pt::Unit::TestSuite::registerMethod( "benchMarkPrimitiveTypes", *this, &PageAllocatorTest::benchMarkPrimitiveTypes );
        //Pt::Unit::TestSuite::registerMethod( "Test chunk deallocate method", *this, &ChunkTest::deallocate );
    }

protected:
	void benchMarkPrimitiveTypes()
	{
		Pt::uint16_t* arrayOfUnsigned[1000];

		Pt::DateTime timeStart = Pt::System::Clock::getLocalTime();
		Pt::Alloc::PageAllocator pageAllocator;
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
};

Pt::Unit::RegisterTest<PageAllocatorTest> register_PageAllocatorTest;

#endif