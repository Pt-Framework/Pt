/***************************************************************************
 *   Copyright (C) 2006 by Tommi Maekitalo                                  *
 *   Copyright (C) 2006 by Marc Boris Duerner                               *
 *   Copyright (C) 2006 by Stefan Bueder                                    *
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
#undef PT_API_EXPORT

#include "Pt/SerializationData.h"
#include "Pt/Date.h"
#include "Pt/Time.h"
#include "Pt/DateTime.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>


class SerializationTest : public Pt::Unit::TestSuite
{
    public:
        SerializationTest()
        : Pt::Unit::TestSuite("SerializationTest")
        {
            Pt::Unit::TestSuite::registerMethod( "ObjectDataTest", *this, &SerializationTest::ObjectDataTest );
            Pt::Unit::TestSuite::registerMethod( "ConstObjectIterator", *this, &SerializationTest::ConstObjectIterator );
            Pt::Unit::TestSuite::registerMethod( "Date", *this, &SerializationTest::Date );
            Pt::Unit::TestSuite::registerMethod( "Time", *this, &SerializationTest::Time );
            Pt::Unit::TestSuite::registerMethod( "DateTime", *this, &SerializationTest::DateTime );
        }

    protected:
        void ObjectDataTest()
        {
            Pt::SerializationData data;
            data.addEntry(L"testEntry", Pt::Variant(1) );
            const Pt::Variant* entry = data.getEntry(L"testEntry");
            PT_UNIT_ASSERT( entry );
            PT_UNIT_ASSERT( *entry == L"1" );
        }

        void ConstObjectIterator()
        {
            Pt::SerializationData data;
            //data.addEntry(L"testEntry1", Pt::Variant(1) );
            data.addData(L"testData1" );
            //data.addEntry(L"testEntry2", Pt::Variant(1) );
            data.addData(L"testData2" );
            data.addData(L"testData3" );

            Pt::SerializationData::ConstObjectIterator it;

            it = data.objectsBegin();
            PT_UNIT_ASSERT( it->name() == Pt::String(L"testData1") );

            ++it;
            PT_UNIT_ASSERT( it->name() == Pt::String(L"testData2") );

            ++it;
            PT_UNIT_ASSERT( it->name() == Pt::String(L"testData3") );

            ++it;
            PT_UNIT_ASSERT( it == data.objectsEnd() )
        }

        void Date()
        {
            Pt::Date date(2000, 10, 20);
            Pt::SerializationData data;
            data << date;

            Pt::Date date2(1,1,1);
            data >> date2;

            PT_UNIT_ASSERT(date == date2);
        }

        void Time()
        {
            Pt::Time time(18, 40, 5, 1);
            Pt::SerializationData data;
            data << time;

            Pt::Time time2;
            data >> time2;

            PT_UNIT_ASSERT(time == time2);
        }

        void DateTime()
        {
            Pt::DateTime datetime(2000, 10, 20, 18, 40, 5, 1);
            Pt::SerializationData data;
            data << datetime;

            Pt::DateTime datetime2;
            data >> datetime2;


            const Pt::Date date = datetime.date();
            const Pt::Date date2 = datetime2.date();
            PT_UNIT_ASSERT(date == date2);

            const Pt::Time time = datetime.time();
            const Pt::Time time2 = datetime2.time();

            PT_UNIT_ASSERT(time == time2);
        }
};

Pt::Unit::RegisterTest<SerializationTest> register_SerializationTest;
