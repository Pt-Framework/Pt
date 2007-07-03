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
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>


class SerializationTest : public Pt::Unit::TestSuite
{
    public:
        SerializationTest()
        : Pt::Unit::TestSuite("SerializationTest")
        {
            Pt::Unit::TestSuite::registerMethod( "ObjectData", *this, &SerializationTest::ObjectData );
            Pt::Unit::TestSuite::registerMethod( "Date", *this, &SerializationTest::Date );
        }

    protected:
        void ObjectData()
        {
            Pt::ObjectData data;
            data.addEntry(L"testEntry", Pt::Variant(1) );
            const Pt::Variant* entry = data.getEntry(L"testEntry");

            PT_UNIT_ASSERT( *entry == "1" );
        }

        void Date()
        {
            Pt::Date date(2000, 10, 20);
            Pt::ObjectData data;
            data << date;

            Pt::Date date2(1,1,1);
            data >> date2;

            //PT_UNIT_ASSERT(date == date2);

        }
};

Pt::Unit::RegisterTest<SerializationTest> register_SerializationTest;
