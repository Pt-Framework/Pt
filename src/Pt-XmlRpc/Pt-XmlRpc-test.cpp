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

#include "Pt/System/Clock.h"

struct Color
{
    int red;
    int green;
    int blue;
};


void operator >>=(const Pt::SerializationInfo& si, Color& color)
{
    color.red = si.getValue<int>("red");
    color.green = si.getValue<int>("green");
    color.blue = si.getValue<int>("blue");
}


void operator <<=(Pt::SerializationInfo& si, const Color& color)
{
    si.addMember("red") <<= color.red;
    si.addMember("green") <<= color.green;
    si.addMember("blue") <<= color.blue;
}


class PtXmlRpcTest : public Pt::Unit::TestSuite
{
    public:
        PtXmlRpcTest()
        : Pt::Unit::TestSuite("Pt-XmlRpc-Test")
        {
            this->registerMethod("Integer", *this, &PtXmlRpcTest::Integer);
            this->registerMethod("VectorOfInt", *this, &PtXmlRpcTest::VectorOfInt);
            this->registerMethod("ReturnStruct", *this, &PtXmlRpcTest::ReturnStruct);
            this->registerMethod("ReturnArray", *this, &PtXmlRpcTest::ReturnArray);
        }

        void Integer()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyInt);

            std::stringstream in;
            Pt::XmlRpc::RemoteMethod<int, int, int> multiply(in, "multiply");
            multiply.begin(2, 3);

            Pt::XmlRpc::RequestHandler req(service);

            std::size_t n = 0;
            std::size_t contentLength = in.str().length();
            while(n < contentLength)
            {
                n += req.advance(in);
            }

            std::stringstream out;
            req.finish(out);

            Pt::XmlRpc::ResponseHandler<int> resp(out);

            n = 0;
            contentLength = out.str().length();
            while(n < contentLength)
            {
                n += resp.advance();
            }

            std::cerr << "Result: " << resp.result() << std::endl;
            PT_UNIT_ASSERT(resp.result() == 6);
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
            in << "                         <value><int>3</int></value>";
            in << "                         <value><int>4</int></value>";
            in << "                         <value><int>3</int></value>";
            in << "                         <value><int>4</int></value>";
            in << "                         <value><int>3</int></value>";
            in << "                         <value><int>4</int></value>";
            in << "                         <value><int>3</int></value>";
            in << "                         <value><int>4</int></value>";
            in << "                         <value><int>3</int></value>";
            in << "                         <value><int>4</int></value>";
            in << "                    </data>";
            in << "                </array>";
            in << "            </value>";
            in << "        </param>";
            in << "    </params>";
            in << "</methodCall>";


            Pt::System::Clock clock;
            clock.start();
            for(int x = 0; x < 5000;++x)
            {
                in.clear();
                in.seekg(std::ios::beg);
                Pt::XmlRpc::RequestHandler req(service);

                std::size_t contentLength = in.str().length();
                //std::cerr << "Request Size: " <<  contentLength << std::endl;
                std::size_t n = 0;
                while(n < contentLength)
                {
                    n += req.advance(in);
                }

                std::stringstream out;
                req.finish(out);

                Pt::XmlRpc::ResponseHandler<int> resp(out);

                contentLength = out.str().length();
                n = 0;
                while(n < contentLength)
                {
                    n += resp.advance();
                }

                //std::cerr << "Result: " << resp.result() << std::endl;
            }
            std::cerr << "TIME: " << clock.stop().totalMSecs() << std::endl;
            std::exit(1);
        }

        void ReturnStruct()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("getColor", *this, &PtXmlRpcTest::getColor);

            std::stringstream in;
            in << "<?xml version=\"1.0\"?>";
            in << "<methodCall>";
            in << "   <methodName>getColor</methodName>";
            in << "   <params>";
            in << "     <param>";
            in << "         <value><i4>10</i4></value>";
            in << "         </param>";
            in << "     <param>";
            in << "         <value><i4>20</i4></value>";
            in << "         </param>";
            in << "      </params>";
            in << "   </methodCall>";
 
            Pt::XmlRpc::RequestHandler req(service);

            std::size_t contentLength = in.str().length();
            std::cerr << "Request Size: " <<  contentLength << std::endl;
            std::size_t n = 0;
            while(n < contentLength)
            {
                n += req.advance(in);
            }

            std::stringstream out;
            req.finish(out);

            Pt::XmlRpc::ResponseHandler<Color> resp(out);

            contentLength = out.str().length();
            //std::cerr << "Response: " << out.str() << std::endl;
            n = 0;
            while(n < contentLength)
            {
                n += resp.advance();
            }

            Color color = resp.result();
            std::cerr << "Result: " << color.red << ":" << color.green << ":" << color.blue << std::endl;
        }

        void ReturnArray()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("getVector", *this, &PtXmlRpcTest::getVector);

            std::stringstream in;
            in << "<?xml version=\"1.0\"?>";
            in << "<methodCall>";
            in << "   <methodName>getVector</methodName>";
            in << "   <params>";
            in << "     <param>";
            in << "         <value><i4>1</i4></value>";
            in << "         </param>";
            in << "     <param>";
            in << "         <value><i4>2</i4></value>";
            in << "         </param>";
            in << "      </params>";
            in << "   </methodCall>";
 
            Pt::XmlRpc::RequestHandler req(service);

            std::size_t contentLength = in.str().length();
            std::cerr << "Request Size: " <<  contentLength << std::endl;
            std::size_t n = 0;
            while(n < contentLength)
            {
                n += req.advance(in);
            }

            std::stringstream out;
            req.finish(out);

            Pt::XmlRpc::ResponseHandler< std::vector<int> > resp(out);

            contentLength = out.str().length();
            //std::cerr << "Response: " << out.str() << std::endl;
            n = 0;
            while(n < contentLength)
            {
                n += resp.advance();
            }

            std::vector<int> vec = resp.result();
            std::cerr << "Result: " << vec.front() << " - " << vec.back() << std::endl;
        }

        int multiplyVector(int a, const std::vector<int>& v)
        {
            //std::cerr << "multiplyVector(" << a << ", " << v.front() << ", " << v.back() << ")" << std::endl;
            return a * v.front() * v.back();
        }

        int multiplyInt(int a, int b)
        {
            return a*b;
        }

        Color getColor(int a, int b)
        {
            Color color;
            color.red = a;
            color.green = b;
            color.blue = a + b;
            return color;
        }

        std::vector<int> getVector(int a, int b)
        {
            std::vector<int> v;
            v.push_back(a);
            v.push_back(b);
            v.push_back(a+b);
            return v;
        }
};

Pt::Unit::RegisterTest<PtXmlRpcTest> register_PtXmlRpcTest;
