/*
 * Copyright (C) 2005-2006 by Marc Boris Duerner
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
#undef PT_XMLRPC_API_EXPORT

#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Unit/TestMain.h"

#include "Pt/XmlRpc/Service.h"
#include <sstream>


class PtXmlRpcTest : public Pt::Unit::TestSuite
{
    public:
        PtXmlRpcTest()
        : Pt::Unit::TestSuite("Pt-XmlRpc-Test")
        {
            std::cerr << "START" << std::endl;

            std::stringstream in;
            in << "<?xml version=\"1.0\"?>";
            in << "<methodCall>";
            in << "   <methodName>multiply</methodName>";
            in << "   <params>";
            in << "     <param>";
            in << "         <value><i4>2</i4></value>";
            in << "         </param>";
            in << "     <param>";
            in << "         <value><i4>3</i4></value>";
            in << "         </param>";
            in << "      </params>";
            in << "   </methodCall>";

            std::size_t contentLength = in.str().length();
            std::cerr << "BYTES TO READ: " <<  contentLength << std::endl;

            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiply);

            Pt::XmlRpc::MethodCaller caller( in, service );
            std::size_t n = 0;

            while(n < contentLength)
            {
                n += caller.exec();
            }

            std::stringstream out;
            caller.finish(out);

            std::cerr << "RESULT: " << out.str() << std::endl;
            std::exit(0);
        }

        int multiply(int a, int b)
        {
            return a*b;
        }
};

Pt::Unit::RegisterTest<PtXmlRpcTest> register_PtXmlRpcTest;
