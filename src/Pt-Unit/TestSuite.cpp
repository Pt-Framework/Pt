/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Duerner                     *
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

#include <Pt/Unit/TestSuite.h>
#include <Pt/SerializationInfo.h>


namespace Pt {

namespace Unit {


TestProtocol TestSuite::defaultProtocol;


TestSuite::TestSuite(const std::string& name, TestProtocol& protocol)
: Test(name)
, _protocol(&protocol)
{
}


TestSuite::~TestSuite()
{
    std::multimap<std::string, Test*>::iterator it;
    for(it = _tests.begin(); it != _tests.end(); ++it)
    {
        delete it->second;
    }
}


void TestSuite::setProperty(const std::string& name, const Pt::SerializationInfo& si)
{
    _properties[name] = si;
}


const Pt::SerializationInfo& TestSuite::property(const std::string& name) const
{
    std::map<std::string, Pt::SerializationInfo>::const_iterator it;
    it = _properties.find(name);

    if( it == _properties.end() )
        throw std::invalid_argument("No such property");

    return it->second;
}


void TestSuite::setProtocol(TestProtocol* protocol)
{
    _protocol = protocol;
}


void TestSuite::setUp()
{
}


void TestSuite::tearDown()
{
}


void TestSuite::run(const SerializationInfo* si, size_t argCount)
{
    _protocol->run(*this);
}


void TestSuite::runTest( const std::string& name, const SerializationInfo* si, size_t argCount )
{
    Test* test = this->findTest(name);
    if(!test)
        throw std::runtime_error("No such test");

    TestContext ctx(*this, *test, si, argCount);
    ctx.run();
}


void TestSuite::runAll()
{
    std::map<std::string, Test*>::iterator it;
    for(it = _tests.begin(); it != _tests.end(); ++it)
    {
        Test* test = it->second;
        TestContext ctx(*this, *test);
        ctx.run();
    }
}


Test* TestSuite::findTest(const std::string& name)
{
    std::map<std::string, Test*>::iterator it = _tests.find(name);
    if( it== _tests.end() )
        return 0;

    return it->second;
}


void TestSuite::registerTest(Test* test)
{
    connect(test->started,   this->started);
    connect(test->finished,  this->finished);
    connect(test->success,   this->success);
    connect(test->assertion, this->assertion);
    connect(test->exception, this->exception);
    connect(test->error,     this->error);
    connect(test->message,   this->message);

    _tests.insert( std::make_pair(test->name(), test) );
}


} // namespace Unit

} // namespace Pt
