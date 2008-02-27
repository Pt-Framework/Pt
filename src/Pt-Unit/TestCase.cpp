/***************************************************************************
 *   Copyright (C) 2005-2008 by Dr. Marc Boris Duerner                     *
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
#include "Pt/Unit/TestCase.h"
#include "Pt/Unit/TestContext.h"

namespace Pt {

namespace Unit {

TestCase::TestCase(const std::string& name)
: Test(name)
, _testMethod(name, *this, &TestCase::test)
{
    connect(_testMethod.started,   this->started);
    connect(_testMethod.finished,  this->finished);
    connect(_testMethod.success,   this->success);
    connect(_testMethod.assertion, this->assertion);
    connect(_testMethod.exception, this->exception);
    connect(_testMethod.error,     this->error);
    connect(_testMethod.message,   this->message);
}


void TestCase::run(const SerializationInfo* si, std::size_t argCount)
{
    TestContext ctx(*this, _testMethod, si, argCount);
    ctx.run();
}


void TestCase::setUp()
{
}


void TestCase::tearDown()
{
}

}

}
