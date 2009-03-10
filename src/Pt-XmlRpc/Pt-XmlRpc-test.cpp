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
#include "Pt/XmlRpc/Client.h"
#include "Pt/XmlRpc/RequestHandler.h"
#include <sstream>

class PtXmlRpcTest : public Pt::Unit::TestSuite
{
    public:
        PtXmlRpcTest()
        : Pt::Unit::TestSuite("Pt-XmlRpc-Test")
        {
            this->registerMethod("Integer", *this, &PtXmlRpcTest::Integer);
            this->registerMethod("VectorOfInt", *this, &PtXmlRpcTest::VectorOfInt);
        }

        void Integer()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyInt);

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
 
            Pt::XmlRpc::RequestReader req(service, in);

            std::size_t contentLength = in.str().length();
            std::cerr << "Request Size: " <<  contentLength << std::endl;
            std::size_t n = 0;
            while(n < contentLength)
            {
                n += req.advance();
            }

            std::stringstream out;
            req.finish(out);

            Pt::XmlRpc::ResponseReader<int> resp(out);

            contentLength = out.str().length();
            n = 0;
            while(n < contentLength)
            {
                n += resp.advance();
            }

            std::cerr << "Result: " << resp.result() << std::endl;
        }

        void VectorOfInt()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyVector);

            std::stringstream in;
            in << "<?xml version=\"1.0\"?>";
            in << "<methodCall>";
            in << "    <methodName>multiply</methodName>";
            in << "    <params>";
            in << "         <param>";
            in << "             <value><i4>2</i4></value>";
            in << "         </param>";
            in << "         <param>";
            in << "             <value>";
            in << "                 <array>";
            in << "                     <data>";
            in << "                         <value><int>4</int></value>";
            in << "                    </data>";
            in << "                </array>";
            in << "            </value>";
            in << "        </param>";
            in << "    </params>";
            in << "</methodCall>";
 
            Pt::XmlRpc::RequestReader req(service, in);

            std::size_t contentLength = in.str().length();
            std::cerr << "Request Size: " <<  contentLength << std::endl;
            std::size_t n = 0;
            while(n < contentLength)
            {
                n += req.advance();
            }

            std::stringstream out;
            req.finish(out);

            Pt::XmlRpc::ResponseReader<int> resp(out);

            contentLength = out.str().length();
            n = 0;
            while(n < contentLength)
            {
                n += resp.advance();
            }

            std::cerr << "Result: " << resp.result() << std::endl;
        }

        int multiplyVector(int a, const std::vector<int>& v)
        {
            return a * v.back();
        }

        int multiplyInt(int a, int b)
        {
            return a*b;
        }
};

Pt::Unit::RegisterTest<PtXmlRpcTest> register_PtXmlRpcTest;
