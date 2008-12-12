/*
 * Copyright (C) 2006 PTV AG
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
#undef PT_DB_API_EXPORT

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>
#include <sstream>
#include <limits>

#include "ResultImpl.h"
#include "ValueImpl.h"
#include "RowImpl.h"

#include "Pt/Db/Row.h"

using namespace std;
using namespace Pt;
using namespace Pt::Db;


class ResultImplTest : public Pt::Unit::TestSuite
{
public:
    ResultImplTest()
    : TestSuite("ResultImplTest")
    {
        Pt::Unit::TestSuite::registerMethod("testSize", *this, &ResultImplTest::testSize);
        Pt::Unit::TestSuite::registerMethod("testGetRow", *this, &ResultImplTest::testGetRow);
    }

protected:
    void testSize();
    void testGetRow();

};


Pt::Unit::RegisterTest<ResultImplTest> register_ResultImplTest;


void ResultImplTest::testSize()
{
    ResultImpl resImpl;

    for(unsigned int j=0; j<10; j++){
        RowImpl* rowImpl = new RowImpl();
        for(unsigned int i=0; i<25; i++)
        {
            std::stringstream ss;
            ss << j*i;

            ValueImpl* impl = new ValueImpl( ss.str() );
            Value v(impl);
            rowImpl->add( v );
        }
        resImpl.add(rowImpl);
    }

    PT_UNIT_ASSERT( resImpl.size() == 10 );
    PT_UNIT_ASSERT( resImpl.getFieldCount() == 25 );
}


void ResultImplTest::testGetRow()
{
    ResultImpl resImpl;

    for(unsigned int j=0; j<10; j++){
        RowImpl* rowImpl = new RowImpl();
        for(unsigned int i=0; i<25; i++)
        {
            std::stringstream ss;
            ss << j*i;

            ValueImpl* impl = new ValueImpl( ss.str() );
            Value v(impl);
            rowImpl->add( v );
        }
        resImpl.add(rowImpl);
    }

    for(int i=0; i<10; i++)
    {
        Row row = resImpl.getRow(i);
        PT_UNIT_ASSERT( row.getInt(24) == i * 24 );
    }
}
