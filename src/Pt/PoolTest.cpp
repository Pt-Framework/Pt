/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
 *   Copyright (C) 2006 by Laurentiu Crisan                                *
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
#include "Pt/Pool.h"

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include <fstream>
#include <cassert>


class PoolTest : public Pt::Unit::TestSuite
{
    public:
        PoolTest()
        : Pt::Unit::TestSuite("TimeTest")
        {
            Pt::Unit::TestSuite::registerMethod( "AllocRelease", *this, &PoolTest::AllocRelease );
        }

    protected:
        void AllocRelease();
};

Pt::Unit::RegisterTest<PoolTest> register_PoolTest;


void PoolTest::AllocRelease()
{
    Pt::Pool<std::string> pool(2);

    std::string* str1 = pool.alloc();
    std::string* str2 = pool.alloc();

    PT_UNIT_ASSERT ( str1 );

    *str1 = "test";

    pool.release(str1);
    str1 = pool.alloc();

    pool.release(str2);
    pool.release(str1);

    str1 = pool.alloc();
    str2 = pool.alloc();
    std::string* str3 = pool.alloc();

    *str3 == "test";

    pool.release( str1 );
    pool.release( str2 );
    pool.release( str3 );
    PT_UNIT_ASSERT( pool.size() == 3 );
}

