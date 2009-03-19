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
#include "Pt/XmlRpc/RemoteProcedure.h"
#include "Pt/Net/HttpServer.h"
#include "Pt/System/EventLoop.h"


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
    private:
        Pt::System::EventLoop* _loop;
        Pt::Net::HttpServer* _server;

    public:
        PtXmlRpcTest()
        : Pt::Unit::TestSuite("Pt-XmlRpc-Test")
        {
            this->registerMethod("Nothing", *this, &PtXmlRpcTest::Nothing);
            this->registerMethod("Boolean", *this, &PtXmlRpcTest::Boolean);
            this->registerMethod("Integer", *this, &PtXmlRpcTest::Integer);
            this->registerMethod("Double", *this, &PtXmlRpcTest::Double);
            this->registerMethod("String", *this, &PtXmlRpcTest::String);
            this->registerMethod("Array", *this, &PtXmlRpcTest::Array);
            this->registerMethod("Struct", *this, &PtXmlRpcTest::Struct);
        }

        void setUp()
        {
            _loop = new Pt::System::EventLoop();
            _loop->setIdleTimeout(1000);
            connect(_loop->timeout, *_loop, &Pt::System::EventLoop::exit);

            _server = new Pt::Net::HttpServer(*_loop, "127.0.0.1", 8001);
        }

        void tearDown()
        {
            delete _loop;
            delete _server;
        }

        void Nothing()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyNothing);
            _server->addService("/calc", service);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onNothingFinished );

            multiply.begin();

            _loop->run();
        }

        void onNothingFinished(const bool& r)
        {
            PT_UNIT_ASSERT_EQUALS(r, false)

            _loop->exit();
        }

        void Boolean()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyBoolean);
            _server->addService("/calc", service);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<bool, bool, bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onBooleanFinished );

            multiply.begin(true, true);

            _loop->run();
        }

        void onBooleanFinished(const bool& r)
        {
            PT_UNIT_ASSERT_EQUALS(r, true)

            _loop->exit();
        }

        void Integer()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyInt);
            _server->addService("/calc", service);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<int, int, int> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onIntegerFinished );

            multiply.begin(2, 3);

            _loop->run();
        }

        void onIntegerFinished(const int& r)
        {
            PT_UNIT_ASSERT_EQUALS(r, 6)

            _loop->exit();
        }

        void Double()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyDouble);
            _server->addService("/calc", service);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<double, double, double> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onDoubleFinished );

            multiply.begin(2.0, 3.0);

            _loop->run();
        }

        void onDoubleFinished(const double& r)
        {
            PT_UNIT_ASSERT_EQUALS(r, 6.0)

            _loop->exit();
        }

        void String()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyString);
            _server->addService("/calc", service);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<std::string, std::string, std::string> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onStringFinished );

            multiply.begin("2", "3");

            _loop->run();
        }

        void onStringFinished(const std::string& r)
        {
            PT_UNIT_ASSERT_EQUALS(r, "6")

            _loop->exit();
        }

        void Array()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyVector);
            _server->addService("/calc", service);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onArrayFinished );

            std::vector<int> vec;
            vec.push_back(10);
            vec.push_back(20);

            multiply.begin(vec, vec);

            _loop->run();
        }

        void onArrayFinished(const std::vector<int>& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.size(), 2)
            PT_UNIT_ASSERT_EQUALS(r.at(0), 100)
            PT_UNIT_ASSERT_EQUALS(r.at(1), 400)

            _loop->exit();
        }

        void Struct()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyColor);
            _server->addService("/calc", service);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure< Color, Color, Color > multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onStuctFinished );

            Color a;
            a.red = 2;
            a.green = 3;
            a.blue = 4;

            Color b;
            b.red = 3;
            b.green = 4;
            b.blue = 5;

            multiply.begin(a, b);

            _loop->run();
        }

        void onStuctFinished(const Color& color)
        {
            PT_UNIT_ASSERT_EQUALS(color.red, 6)
            PT_UNIT_ASSERT_EQUALS(color.green, 12)
            PT_UNIT_ASSERT_EQUALS(color.blue, 20)

            _loop->exit();
        }

        bool multiplyNothing()
        {
            return false;
        }

        bool multiplyBoolean(bool a, bool b)
        {
            PT_UNIT_ASSERT_EQUALS(a, true)
            PT_UNIT_ASSERT_EQUALS(b, true)
            return true;
        }

        int multiplyInt(int a, int b)
        {
            return a*b;
        }

        double multiplyDouble(double a, double b)
        {
            return a*b;
        }

        std::string multiplyString(std::string a, std::string b)
        {
            PT_UNIT_ASSERT_EQUALS(a, "2")
            PT_UNIT_ASSERT_EQUALS(b, "3")
            return "6";
        }

        std::vector<int> multiplyVector(const std::vector<int>& a, const std::vector<int>& b)
        {
            std::vector<int> r;
            r.push_back( a.at(0) * b.at(0) );
            r.push_back( a.at(1) * b.at(1) );
            return r;
        }

        Color multiplyColor(const Color& a, const Color& b)
        {
            Color color;
            color.red = a.red * b.red;
            color.green = a.green * b.green;
            color.blue = a.blue * b.blue;
            return color;
        }
};

Pt::Unit::RegisterTest<PtXmlRpcTest> register_PtXmlRpcTest;
