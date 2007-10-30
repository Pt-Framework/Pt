/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Dürner                          *
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

#include <Pt/Unit/TestContext.h>


namespace Pt {

namespace Unit {

TestContext::TestContext(TestFixture& fixture, Test& test, const SerializationInfo* args, size_t argCount )
: _fixture(fixture)
, _test(test)
, _args(args)
, _argCount(argCount)
, _setUp(false)
{ }


TestContext::~TestContext()
{
    try
    {
        if( _setUp )
            _fixture.tearDown();
    }
    catch(...)
    {}

    _test.finished.send(*this);
}


std::string TestContext::testName() const
{
    return _test.name();
}


void TestContext::run()
{
    try
    {
        _test.started.send(*this);
        _fixture.setUp();
        _setUp = true;
        _test.run(_args, _argCount);
        _test.success.send(*this);
    }
    catch(const Assertion& assertion)
    {
        _test.assertion.send(*this, assertion);
    }
    catch(const std::range_error& ex)
    {
        _test.exception.send(*this, ex);
    }
    catch(const std::runtime_error& ex)
    {
        _test.exception.send(*this, ex);
    }
    catch(const std::logic_error& ex)
    {
        _test.exception.send(*this, ex);
    }
    catch(const std::exception& ex)
    {
        _test.exception.send(*this, ex);
    }
    catch(...)
    {
        _test.error.send(*this);
    }
}

}

}
