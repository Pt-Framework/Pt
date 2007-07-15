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


namespace Pt {

namespace Unit {


TestProtocol TestSuite::defaultProtocol;


inline void TestProtocol::run(TestSuite& suite)
{
    Reflectable::MethodIterator it;
    for(it = suite.methodsBegin(); it != suite.methodsEnd(); ++it)
    {
        suite.runTest( it->name(), Args() );
    }
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

void TestSuite::runTest(const std::string& name, const Args& args)
{
    Context ctx(*this, name, args);
    ctx.run();
}

}

}
