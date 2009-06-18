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
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/XmlRpc/Service.h"
#include "Pt/XmlRpc/Client.h"
#include "Pt/XmlRpc/Fault.h"
#include "Pt/XmlRpc/RemoteProcedure.h"
#include "Pt/Http/Server.h"
#include "Pt/System/EventLoop.h"
#include "Pt/System/Thread.h"


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
        Pt::Http::Server* _server;
        Pt::System::AttachedThread* _serverThread;
        unsigned _count;

    public:
        PtXmlRpcTest()
        : Pt::Unit::TestSuite("Pt-XmlRpc-Test")
        {
            this->registerMethod("Fault", *this, &PtXmlRpcTest::Fault);
            this->registerMethod("Exception", *this, &PtXmlRpcTest::Exception);
            this->registerMethod("CallbackException", *this, &PtXmlRpcTest::CallbackException);
            this->registerMethod("ConnectError", *this, &PtXmlRpcTest::ConnectError);
            this->registerMethod("Nothing", *this, &PtXmlRpcTest::Nothing);
            this->registerMethod("Boolean", *this, &PtXmlRpcTest::Boolean);
            this->registerMethod("Integer", *this, &PtXmlRpcTest::Integer);
            this->registerMethod("Double", *this, &PtXmlRpcTest::Double);
            this->registerMethod("String", *this, &PtXmlRpcTest::String);
            this->registerMethod("EmptyValues", *this, &PtXmlRpcTest::EmptyValues);
            this->registerMethod("Array", *this, &PtXmlRpcTest::Array);
            this->registerMethod("EmptyArray", *this, &PtXmlRpcTest::EmptyArray);
            this->registerMethod("Struct", *this, &PtXmlRpcTest::Struct);
        }

        void failTest()
        {
            throw Pt::Unit::Assertion("test timed out", PT_SOURCEINFO);
        }

        void setUp()
        {
            _loop = new Pt::System::EventLoop();
            _loop->setIdleTimeout(2000);
            connect(_loop->timeout, *this, &PtXmlRpcTest::failTest);
            connect(_loop->timeout, *_loop, &Pt::System::EventLoop::exit);

            _server = new Pt::Http::Server("127.0.0.1", 8001);
            _serverThread = new Pt::System::AttachedThread( Pt::callable(*_server, &Pt::Http::Server::run) );
        }

        void tearDown()
        {
            delete _loop;
            delete _server;
            delete _serverThread;
        }

        void Fault()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::throwFault);
            _server->addService("/calc", service);

            _serverThread->start();
            Pt::System::Thread::sleep(500);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onFault );
            multiply.begin();

            _loop->run();
        }

        void onFault(const Pt::XmlRpc::Result<bool>& result)
        {
            try
            {
                result.get();
                PT_UNIT_ASSERT_MSG(false, "Pt::XmlRpc::Fault exception expected");
            }
            catch (const Pt::XmlRpc::Fault& e)
            {
                PT_UNIT_ASSERT_EQUALS(e.rc(), 7)
                PT_UNIT_ASSERT_EQUALS(e.text(), "Fault")
            }

            _loop->exit();
        }

        void Exception()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::throwException);
            _server->addService("/calc", service);

            _serverThread->start();
            Pt::System::Thread::sleep(500);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onException );
            multiply.begin();

            _loop->run();
        }

        void onException(const Pt::XmlRpc::Result<bool>& result)
        {
            try
            {
                bool v = result.get();
                PT_UNIT_ASSERT(false);
            }
            catch (const Pt::XmlRpc::Fault& e)
            {
                PT_UNIT_ASSERT_EQUALS(e.rc(), 0)
                PT_UNIT_ASSERT_EQUALS(e.text(), "Exception")
            }

            _loop->exit();
        }

        void Nothing()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyNothing);
            _server->addService("/calc", service);

            _serverThread->start();
            Pt::System::Thread::sleep(500);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onNothingFinished );

            multiply.begin();

            _loop->run();
        }

        void onNothingFinished(const Pt::XmlRpc::Result<bool>& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get(), false)

            _loop->exit();
        }

        void CallbackException()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyNothing);
            _server->addService("/calc", service);

            _serverThread->start();
            Pt::System::Thread::sleep(500);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onExceptionCallback );

            multiply.begin();

            _count = 0;
            PT_UNIT_ASSERT_THROW(_loop->run(), std::runtime_error);
            PT_UNIT_ASSERT_EQUALS(_count, 1);
        }

        void onExceptionCallback(const Pt::XmlRpc::Result<bool>& r)
        {
            ++_count;
            _loop->exit();
            throw std::runtime_error("my error");
        }

        void ConnectError()
        {
            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onConnectErrorCallback );

            multiply.begin();

            try
            {
                _loop->run();
            }
            catch (const std::exception& e)
            {
                PT_UNIT_ASSERT_MSG(false, std::string("unexpected exception ") + typeid(e).name() + ": " + e.what());
            }
        }

        void onConnectErrorCallback(const Pt::XmlRpc::Result<bool>& r)
        {
            _loop->exit();
            PT_UNIT_ASSERT_THROW(r.get(), std::exception);
        }

        void Boolean()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyBoolean);
            _server->addService("/calc", service);

            _serverThread->start();
            Pt::System::Thread::sleep(500);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<bool, bool, bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onBooleanFinished );

            multiply.begin(true, true);

            _loop->run();
        }

        void onBooleanFinished(const Pt::XmlRpc::Result<bool>& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get(), true)

            _loop->exit();
        }

        void Integer()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyInt);
            _server->addService("/calc", service);

            _serverThread->start();
            Pt::System::Thread::sleep(500);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<int, int, int> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onIntegerFinished );

            multiply.begin(2, 3);

            _loop->run();
        }

        void onIntegerFinished(const Pt::XmlRpc::Result<int>& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get(), 6)

            _loop->exit();
        }

        void Double()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyDouble);
            _server->addService("/calc", service);

            _serverThread->start();
            Pt::System::Thread::sleep(500);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<double, double, double> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onDoubleFinished );

            multiply.begin(2.0, 3.0);

            _loop->run();
        }

        void onDoubleFinished(const Pt::XmlRpc::Result<double>& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get(), 6.0)

            _loop->exit();
        }

        void String()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("echoString", *this, &PtXmlRpcTest::echoString);
            _server->addService("/foo", service);

            _serverThread->start();
            Pt::System::Thread::sleep(500);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/foo");
            Pt::XmlRpc::RemoteProcedure<std::string, std::string> echo(client, "echoString");
            connect( echo.finished, *this, &PtXmlRpcTest::onStringEchoFinished );

            echo.begin("\xc3\xaf\xc2\xbb\xc2\xbf'\"&<> foo?");

            _loop->run();
        }

        void onStringEchoFinished(const Pt::XmlRpc::Result<std::string>& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get(), "\xc3\xaf\xc2\xbb\xc2\xbf'\"&<> foo?")

            _loop->exit();
        }

        void EmptyValues()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyEmpty);
            _server->addService("/calc", service);

            _serverThread->start();
            Pt::System::Thread::sleep(500);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<std::string, std::string, std::string> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onEmptyFinished );

            multiply.begin("", "");

            _loop->run();
        }

        void onEmptyFinished(const Pt::XmlRpc::Result<std::string>& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get(), "4")
            _loop->exit();
        }

        void Array()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyVector);
            _server->addService("/calc", service);

            _serverThread->start();
            Pt::System::Thread::sleep(500);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onArrayFinished );

            std::vector<int> vec;
            vec.push_back(10);
            vec.push_back(20);

            multiply.begin(vec, vec);

            _loop->run();
        }

        void onArrayFinished(const Pt::XmlRpc::Result<std::vector<int> >& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get().size(), 2)
            PT_UNIT_ASSERT_EQUALS(r.get().at(0), 100)
            PT_UNIT_ASSERT_EQUALS(r.get().at(1), 400)

            _loop->exit();
        }

        void EmptyArray()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyVector);
            _server->addService("/calc", service);

            _serverThread->start();
            Pt::System::Thread::sleep(500);

            Pt::XmlRpc::Client client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onEmptyArrayFinished );

            std::vector<int> vec;
            multiply.begin(vec, vec);

            _loop->run();
        }

        void onEmptyArrayFinished(const Pt::XmlRpc::Result<std::vector<int> >& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get().size(), 0)

            _loop->exit();
        }

        void Struct()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyColor);
            _server->addService("/calc", service);

            _serverThread->start();
            Pt::System::Thread::sleep(500);

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

        void onStuctFinished(const Pt::XmlRpc::Result<Color>& color)
        {
            PT_UNIT_ASSERT_EQUALS(color.get().red, 6)
            PT_UNIT_ASSERT_EQUALS(color.get().green, 12)
            PT_UNIT_ASSERT_EQUALS(color.get().blue, 20)

            _loop->exit();
        }

        bool throwFault()
        {
            throw Pt::XmlRpc::Fault("Fault", 7);
            return false;
        }

        bool throwException()
        {
            throw std::runtime_error("Exception");
            return false;
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

        std::string echoString(std::string a)
        {
            return a;
        }

        std::string multiplyEmpty(std::string a, std::string b)
        {
            PT_UNIT_ASSERT_EQUALS(a, "")
            PT_UNIT_ASSERT_EQUALS(b, "")
            return "4";
        }

        std::vector<int> multiplyVector(const std::vector<int>& a, const std::vector<int>& b)
        {
            std::vector<int> r;
            if( a.size() )
            {
                r.push_back( a.at(0) * b.at(0) );
                r.push_back( a.at(1) * b.at(1) );
            }

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
