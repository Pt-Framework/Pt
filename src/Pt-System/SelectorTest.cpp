/***************************************************************************
 *   Copyright (C) 2007 by Laurentiu-Gheorghe Crisan                       *
 *   Copyright (C) 2007 by Marc Boris Dürner                               *
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
#include "Pt/System/Selector.h"

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>


class SelectorTest : public Pt::Unit::TestSuite
{
    public:
        SelectorTest()
        : Pt::Unit::TestSuite("SelectorTest")
        {
            Pt::Unit::TestSuite::registerMethod( "WaitTimer", *this, &SelectorTest::WaitTimer );
            Pt::Unit::TestSuite::registerMethod( "WaitIODevice", *this, &SelectorTest::WaitIODevice );
        }

    private:
        void WaitTimer()
        {

        }

        void WaitIODevice()
        {

        }
};

Pt::Unit::RegisterTest<SelectorTest> register_SelectorTest;
