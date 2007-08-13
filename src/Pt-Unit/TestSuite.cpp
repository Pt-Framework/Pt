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
#include <Pt/Reflex/Reflectable.h>


using namespace Pt;
using namespace Unit;


TestProtocol TestSuite::defaultProtocol;


inline void TestProtocol::run(TestSuite& suite)
{
    Reflex::Reflectable::MethodIterator it;
    for(it = suite.reflectable()->methodsBegin(); it != suite.reflectable()->methodsEnd(); ++it)
    {
        const Any* a = 0;
        suite.runTest( it->name(), a, 0 );
    }
}


TestSuite::TestSuite(const std::string& name, TestProtocol& protocol)
: Test(name)
, _protocol(&protocol)
{
    m_reflectable = new Pt::Reflex::Reflectable("Pt.Unit.TestSuite");
}

TestSuite::~TestSuite()
{
    delete m_reflectable;
}


void TestSuite::setProperty(const std::string& name, const Pt::SerializationInfo& si)
{
    Pt::Reflex::PropertyInfo& pi = m_reflectable->propertyInfo(name);
    si >>= pi;
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

void TestSuite::run()
{
    _protocol->run(*this);
}

void TestSuite::runTest(const std::string& name, const Any* args, size_t argCount )
{
    Context ctx(*this, name, args, argCount);
    ctx.run();
}


void TestSuite::runTest( const std::string& name, const SerializationInfo* si, size_t argCount )
{
    Pt::Reflex::CallableInfo& cb = m_reflectable->methodInfo( name );

    if(argCount == 0)
    {
        this->runTest(name);
    }

    std::vector<Pt::Any> args(argCount);
    for(size_t n = 0; n < argCount; ++n)
    {
        const char* argName = cb.argName(n);

        std::map<std::string, Deserialize>::const_iterator it = _deserializers.find(argName);
        if( it == _deserializers.end() )
            throw SerializationError("Could not deserialize type " + std::string(argName) , PT_SOURCEINFO);

        Any a;
        it->second(si[n], args[n]);
        args.push_back(a);
    }

    this->runTest(name, &args[0], argCount);
}

void TestSuite::call( const std::string& name, const Any* args, size_t argCount )
{
    m_reflectable->call(name, args, argCount);
}

void TestSuite::registerCallable(Pt::Reflex::CallableInfo* ci)
{
    m_reflectable->registerCallableInfo(ci);
}

