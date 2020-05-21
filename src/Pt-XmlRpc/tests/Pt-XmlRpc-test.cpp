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
#include "Pt/SmartPtr.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Soap/HttpService.h"
#include "Pt/Soap/HttpClient.h"
#include "Pt/Soap/ServiceDeclaration.h"
#include "Pt/XmlRpc/HttpService.h"
#include "Pt/XmlRpc/HttpClient.h"
#include "Pt/XmlRpc/Fault.h"
#include "Pt/Remoting/RemoteProcedure.h"
#include "Pt/Http/Server.h"
#include "Pt/Http/Servlet.h"
#include "Pt/Http/Request.h" // soap experiments
#include "Pt/Http/Reply.h" // soap experiments
#include "Pt/Net/Endpoint.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Clock.h"
#include "Pt/System/Logger.h"
#include <memory>

namespace Pt {

template <typename T>
class BasicComposer< std::vector<T> > : public Composer
{
    public:
        BasicComposer(SerializationContext* context = 0)
        : _type(0)
        {
            _elemComposer.setParent(this);
        }

        void begin(std::vector<T>& type)
        {
            type.clear();
            type.reserve(5);
            _type = &type;
        }

        virtual void onSetId(const char* id, std::size_t len)
        { }

        virtual Pt::Composer* onBeginElement()
        {
            _type->push_back( T() );
            _elemComposer.begin( _type->back() );
            return &_elemComposer;
        }

    private:
        std::vector<T>* _type;
        BasicComposer<T> _elemComposer;
};

template <typename T>
class BasicDecomposer< std::vector<T> > : public Decomposer
{
    public:
        BasicDecomposer(SerializationContext* context = 0)
        : _type(0)
        { 
            _elemDecomposer.setParent(this);
        }

        void begin(const std::vector<T>& type, const char* name)
        {
            _type = &type;
            _name = name;
        }

        void onBeginFormat(Formatter& formatter)
        {
            formatter.beginSequence(_name.c_str(), "std::vector", "");
            _it = _type->begin();
        }

        Decomposer* onAdvanceFormat(Formatter& formatter)
        {
            if( _it != _type->begin() )
            {
                formatter.finishElement();
            }

            if( _it == _type->end() )
            {
                formatter.finishSequence();
                return this->parent();
            }

            formatter.beginElement();
            _elemDecomposer.begin(*_it, "");
            _elemDecomposer.beginFormat(formatter);
            ++_it;

            return &_elemDecomposer;
        }

    private:
        std::string _name;
        const std::vector<T>* _type;
        BasicDecomposer<T> _elemDecomposer;
        typename std::vector<T>::const_iterator _it;
};

}

struct Color
{
    int red;
    int green;
    int blue;
};


typedef std::set<int> IntSet;
typedef std::multiset<int> IntMultiset;
typedef std::map<int, int> IntMap;
typedef std::multimap<int, int> IntMultimap;


void operator >>=(const Pt::SerializationInfo& si, Color& color)
{
    si.getMember("red") >>= color.red;
    si.getMember("green") >>= color.green;
    si.getMember("blue") >>= color.blue;
}


void operator <<=(Pt::SerializationInfo& si, const Color& color)
{
    si.addMember("red") <<= color.red;
    si.addMember("green") <<= color.green;
    si.addMember("blue") <<= color.blue;
}


class PtXmlRpcTest : public Pt::Unit::TestSuite
                   , public Pt::Connectable
{
    private:
        Pt::System::Timer _exitTimer;
        Pt::System::MainLoop* _loop;
        Pt::Http::Server* _server;
        unsigned _count;

    public:
        PtXmlRpcTest()
        : Pt::Unit::TestSuite("Pt-XmlRpc-Test")
        {
            Pt::System::Logger::setLogLevel("", Pt::System::Error);

            registerMethod("NotFound", *this, &PtXmlRpcTest::NotFound);
            registerMethod("Fault", *this, &PtXmlRpcTest::Fault);
            registerMethod("CallbackException", *this, &PtXmlRpcTest::CallbackException);
            registerMethod("ConnectError", *this, &PtXmlRpcTest::ConnectError);
            registerMethod("Nothing", *this, &PtXmlRpcTest::Nothing);
            registerMethod("Boolean", *this, &PtXmlRpcTest::Boolean);
            registerMethod("Integer", *this, &PtXmlRpcTest::Integer);
            registerMethod("Double", *this, &PtXmlRpcTest::Double);
            registerMethod("String", *this, &PtXmlRpcTest::String);
            registerMethod("EmptyValues", *this, &PtXmlRpcTest::EmptyValues);
            registerMethod("SoapMap", *this, &PtXmlRpcTest::SoapMap);
            registerMethod("SoapArray", *this, &PtXmlRpcTest::SoapArray);
            registerMethod("Array", *this, &PtXmlRpcTest::Array);
            registerMethod("ArrayBenchmark", *this, &PtXmlRpcTest::ArrayBenchmark);
            registerMethod("EmptyArray", *this, &PtXmlRpcTest::EmptyArray);
            registerMethod("Struct", *this, &PtXmlRpcTest::Struct);
            registerMethod("Set", *this, &PtXmlRpcTest::Set);
            registerMethod("Multiset", *this, &PtXmlRpcTest::Multiset);
            registerMethod("Map", *this, &PtXmlRpcTest::Map);
            registerMethod("Multimap", *this, &PtXmlRpcTest::Multimap);
        }

        void failTest()
        {
            throw Pt::Unit::Assertion("test timed out", PT_SOURCEINFO);
        }

        void setUp()
        {
            _loop = new Pt::System::MainLoop();
            Pt::AutoPtr<Pt::System::MainLoop> loopPtr(_loop);

            _exitTimer.setActive(*_loop);
            _exitTimer.start(20000);
            _exitTimer.timeout() += Pt::slot(*this, &PtXmlRpcTest::failTest);
            _exitTimer.timeout() += Pt::slot(*_loop, &Pt::System::MainLoop::exit);

            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Any(8001);
            _server = new Pt::Http::Server(*_loop, ep);

            loopPtr.release();
        }

        void tearDown()
        {
            delete _server;
            delete _loop;
        }

        ////////////////////////////////////////////////////////////
        // NotFound
        //
        void NotFound()
        {
            Pt::Remoting::ServiceDefinition service;
            Pt::XmlRpc::HttpService httpService(service);
            
            Pt::Http::MapUrl servlet("/calc", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/calc");
            
            Pt::Remoting::RemoteProcedure<bool> multiply(client, "multiply");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onNotFound);
            multiply.begin();

            _loop->run();
        }

        void onNotFound(const Pt::Remoting::Result<bool>& result)
        {
            try
            {
                result.get();
                PT_UNIT_ASSERT_MSG(false, "Pt::XmlRpc::Fault exception expected");
            }
            catch (const Pt::XmlRpc::Fault& e)
            {
                PT_UNIT_ASSERT_EQUALS(e.rc(), Pt::XmlRpc::Fault::MethodNotFound);
            }

            _loop->exit();
        }

        ////////////////////////////////////////////////////////////
        // Fault
        //
        void Fault()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("multiply", *this, &PtXmlRpcTest::throwFault);

            Pt::XmlRpc::HttpService httpService(service);
            
            Pt::Http::MapUrl servlet("/calc", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/calc");
            
            Pt::Remoting::RemoteProcedure<bool> multiply(client, "multiply");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onFault);
            multiply.begin();

            _loop->run();
        }

        void onFault(const Pt::Remoting::Result<bool>& result)
        {
            try
            {
                result.get();
                PT_UNIT_ASSERT_MSG(false, "Pt::XmlRpc::Fault exception expected");
            }
            catch (const Pt::XmlRpc::Fault& e)
            {
                PT_UNIT_ASSERT_EQUALS(e.rc(), 7);
                PT_UNIT_ASSERT_EQUALS( std::string(e.what()), "test fault message");
            }

            _loop->exit();
        }

        bool throwFault()
        {
            throw Pt::XmlRpc::Fault("test fault message", 7);
            return false;
        }

        ////////////////////////////////////////////////////////////
        // Nothing
        //
        void Nothing()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("multiply", *this, &PtXmlRpcTest::multiplyNothing);

            Pt::XmlRpc::HttpService httpService(service);
            
            Pt::Http::MapUrl servlet("/calc", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/calc");
            
            Pt::Remoting::RemoteProcedure<bool> multiply(client, "multiply");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onNothingFinished);

            multiply.begin();

            _loop->run();
        }

        void onNothingFinished(const Pt::Remoting::Result<bool>& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get(), false);

            _loop->exit();
        }

        bool multiplyNothing()
        {
            return false;
        }

        ////////////////////////////////////////////////////////////
        // CallbackException
        //
        void CallbackException()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("multiply", *this, &PtXmlRpcTest::multiplyNothing);

            Pt::XmlRpc::HttpService httpService(service);
            
            Pt::Http::MapUrl servlet("/calc", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/calc");
            
            Pt::Remoting::RemoteProcedure<bool> multiply(client, "multiply");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onExceptionCallback);

            multiply.begin();

            _count = 0;
            PT_UNIT_ASSERT_THROW(_loop->run(), std::runtime_error);
            PT_UNIT_ASSERT_EQUALS(_count, 1);

            // service must live longer than server
            delete _server;
            _server = 0;
        }

        void onExceptionCallback(const Pt::Remoting::Result<bool>& r)
        {
            ++_count;
            _loop->exit();
            throw std::runtime_error("my error");
        }

        ////////////////////////////////////////////////////////////
        // ConnectError
        //
        void ConnectError()
        {
            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8002);
            client.setTarget(ep, "/calc");
            
            Pt::Remoting::RemoteProcedure<bool> multiply(client, "multiply");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onConnectErrorCallback);
            try
            {
                multiply.begin();
            }
            catch (const std::exception&)
            {
                return;
            }

            try
            {
                _loop->run();
            }
            catch (const std::exception& e)
            {
                PT_UNIT_ASSERT_MSG(false, std::string("unexpected exception ") + ": " + e.what());
            }
        }

        void onConnectErrorCallback(const Pt::Remoting::Result<bool>& r)
        {
            _loop->exit();
            PT_UNIT_ASSERT_THROW(r.get(), std::exception);
        }

        ////////////////////////////////////////////////////////////
        // Boolean
        //
        void Boolean()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("multiply", *this, &PtXmlRpcTest::multiplyBoolean);

            Pt::XmlRpc::HttpService httpService(service);
            
            Pt::Http::MapUrl servlet("/calc", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/calc");
            
            Pt::Remoting::RemoteProcedure<bool, bool, bool> multiply(client, "multiply");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onBooleanFinished );

            multiply.begin(true, true);

            _loop->run();
        }

        void onBooleanFinished(const Pt::Remoting::Result<bool>& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get(), true);

            _loop->exit();
        }

        bool multiplyBoolean(bool a, bool b)
        {
            PT_UNIT_ASSERT_EQUALS(a, true);
            PT_UNIT_ASSERT_EQUALS(b, true);
            return true;
        }

        ////////////////////////////////////////////////////////////
        // Integer
        //

        Pt::Remoting::RemoteProcedure<int, int, int>* _multiply;

        void Integer()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("multiply", *this, &PtXmlRpcTest::multiplyInt);

            Pt::XmlRpc::HttpService httpService(service);

            Pt::Http::MapUrl servlet("/calc", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/calc");
            
            Pt::Remoting::RemoteProcedure<int, int, int> multiply(client, "multiply");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onIntegerFinished );

            multiply.begin(2, 3);
            _multiply= &multiply;

            _loop->run();

            delete _server;
            _server = 0;
        }

        void onIntegerFinished(const Pt::Remoting::Result<int>& r)
        {
            static int ttt = 0;
            PT_UNIT_ASSERT_EQUALS(r.get(), 6);

            if(ttt++ == 1)
                _loop->exit();

            _multiply->begin(1, 6);
        }

        int multiplyInt(int a, int b)
        {
            return a*b;
        }

        ////////////////////////////////////////////////////////////
        // Double
        //
        void Double()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("multiply", *this, &PtXmlRpcTest::multiplyDouble);

            Pt::XmlRpc::HttpService httpService(service);

            Pt::Http::MapUrl servlet("/calc", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/calc");
            
            Pt::Remoting::RemoteProcedure<double, double, double> multiply(client, "multiply");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onDoubleFinished );

            multiply.begin(2.0, 3.0);

            _loop->run();
        }

        void onDoubleFinished(const Pt::Remoting::Result<double>& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get(), 6.0);

            _loop->exit();
        }

        double multiplyDouble(double a, double b)
        {
            return a*b;
        }

        ////////////////////////////////////////////////////////////
        // String
        //
        void String()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("echoString", *this, &PtXmlRpcTest::echoString);

            Pt::XmlRpc::HttpService httpService(service);

            Pt::Http::MapUrl servlet("/foo", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/foo");

            Pt::Remoting::RemoteProcedure<std::string, std::string> echo(client, "echoString");
            echo.finished() += Pt::slot(*this, &PtXmlRpcTest::onStringEchoFinished);

            echo.begin("Hello'\"&<> foo?");

            _loop->run();
        }

        void onStringEchoFinished(const Pt::Remoting::Result<std::string>& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get(), "Hello'\"&<> foo?");

            _loop->exit();
        }

        std::string echoString(std::string a)
        {
            return a;
        }

        ////////////////////////////////////////////////////////////
        // EmptyValues
        //
        void EmptyValues()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("multiply", *this, &PtXmlRpcTest::multiplyEmpty);

            Pt::XmlRpc::HttpService httpService(service);

            Pt::Http::MapUrl servlet("/calc", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/calc");
            
            Pt::Remoting::RemoteProcedure<std::string, std::string, std::string> multiply(client, "multiply");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onEmptyFinished );

            multiply.begin("", "");

            _loop->run();
        }

        void onEmptyFinished(const Pt::Remoting::Result<std::string>& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get(), "4");
            _loop->exit();
        }

        std::string multiplyEmpty(std::string a, std::string b)
        {
            PT_UNIT_ASSERT_EQUALS(a, "");
            PT_UNIT_ASSERT_EQUALS(b, "");
            return "4";
        }

        ////////////////////////////////////////////////////////////
        // Soap
        //
        class CalcSoapServiceDeclaration: public Pt::Soap::ServiceDeclaration
        {
            public:
                class ArrayMultiply : public Pt::Soap::Operation
                {
                    public:
                        ArrayMultiply()
                        : Pt::Soap::Operation("multiply", "multiplyResponse")
                        , _intArrayType("IntArrayType")
                        {
                            _intArrayType.setElement("number", intType());

                            addInput("a", _intArrayType);
                            addInput("b", _intArrayType);

                            setOutput("multiplyResult", _intArrayType);
                        }

                    private:
                        Pt::Soap::ArrayType _intArrayType;
                };

                class MapMultiply : public Pt::Soap::Operation
                {
                    public:
                        MapMultiply()
                        : Pt::Soap::Operation("multiplyMap", "multiplyMapResponse")
                        , _intDictType("IntDictType", "IntDictElementType")
                        {
                            _intDictType.setElement("elem",
                                                    "first", intType(),
                                                    "second", intType() );

                            addInput("a", _intDictType);

                            setOutput("multiplyMapResult", intType());
                        }

                    private:
                        Pt::Soap::DictType _intDictType;
                };

                CalcSoapServiceDeclaration()
                : Pt::Soap::ServiceDeclaration("calc")
                {
                    setTargetNamespace("http://tempuri.org/");
                    addOperation(_arrayMultiply);
                    addOperation(_mapMultiply);
                }
            
            private:
                ArrayMultiply _arrayMultiply;
                MapMultiply   _mapMultiply;
        };

        ////////////////////////////////////////////////////////////
        // SoapMap
        //
        void SoapMap()
        {
            CalcSoapServiceDeclaration serviceDecl;

            Pt::Remoting::ServiceDefinition serviceDef;
            serviceDef.registerProcedure("multiplyMap", *this, &PtXmlRpcTest::multiplySoapMap);

            Pt::Soap::HttpService httpService(serviceDecl, serviceDef);
            Pt::Http::MapUrl servlet("/" + serviceDecl.name(), httpService);
            _server->addServlet(servlet);

            Pt::Soap::HttpClient client(serviceDecl, *_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/calc");

            Pt::Remoting::RemoteProcedure<int, std::map<int, int> > multiply(client, "multiplyMap");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onMapMultiplyFinished);

            std::map<int, int> m;
            m[6] = 7;

            multiply.begin(m);
            
            _loop->run();
        }

        int multiplySoapMap(const std::map<int, int>& a)
        {
            int r = 0;

            std::map<int, int>::const_iterator it;
            for(it = a.begin(); it != a.end(); ++it)
            {
              r += it->first * it->second;
            }

            return r;
        }

        void onMapMultiplyFinished(const Pt::Remoting::Result<int>& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get(), 42);
            _loop->exit();
        }

        ////////////////////////////////////////////////////////////
        // SoapArray
        //
        void SoapArray()
        {
            CalcSoapServiceDeclaration serviceDecl;

            Pt::Remoting::ServiceDefinition serviceDef;
            serviceDef.registerProcedure("multiply", *this, &PtXmlRpcTest::multiplyVector);

            Pt::Soap::HttpService httpService(serviceDecl, serviceDef);
            Pt::Http::MapUrl servlet("/" + serviceDecl.name(), httpService);
            _server->addServlet(servlet);

            Pt::Soap::HttpClient client(serviceDecl, *_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/calc");

            Pt::Remoting::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > multiply(client, "multiply");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onArrayFinished);

            std::vector<int> vec;
            vec.push_back(10);
            vec.push_back(20);

            multiply.begin(vec, vec);
            
            _loop->run();
        }

        ////////////////////////////////////////////////////////////
        // Array
        //
        void Array()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("multiply", *this, &PtXmlRpcTest::multiplyVector);

            Pt::XmlRpc::HttpService httpService(service);

            Pt::Http::MapUrl servlet("/calc", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/calc");
            
            Pt::Remoting::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > multiply(client, "multiply");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onArrayFinished);

            std::vector<int> vec;
            vec.push_back(10);
            vec.push_back(20);

            multiply.begin(vec, vec);

            _loop->run();
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

        void onArrayFinished(const Pt::Remoting::Result<std::vector<int> >& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get().size(), 2);
            PT_UNIT_ASSERT_EQUALS(r.get().at(0), 100);
            PT_UNIT_ASSERT_EQUALS(r.get().at(1), 400);

            _loop->exit();
        }

        ////////////////////////////////////////////////////////////
        // ArrayBenchmark
        //
        Pt::Remoting::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> >* benchmarkProc;
        std::vector<int> benchmarkVec;
        std::vector<int> benchmarkResult;

        void ArrayBenchmark()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("mergeVector", *this, &PtXmlRpcTest::mergeVector);

            Pt::XmlRpc::HttpService httpService(service);

            Pt::Http::MapUrl mapurl("/calc", httpService);
            _server->addServlet(mapurl);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/calc");
            client.setKeepAlive();
            
            Pt::Remoting::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > proc(client, "mergeVector");
            proc.finished() += Pt::slot(*this, &PtXmlRpcTest::onArrayBenchmarkFinished);

            benchmarkProc = &proc;

            benchmarkVec.push_back(1);
            benchmarkVec.push_back(2);
            benchmarkVec.push_back(3);
            benchmarkVec.push_back(4);

            proc.begin(benchmarkVec, benchmarkVec);

            Pt::System::Clock clock;
            clock.start();
            _loop->run();

            client.close();
            Pt::Timespan ts = clock.stop();
            std::cerr << "Time   : " << ts.toUSecs() <<  std::endl;
            std::cerr << "Req/Sec: " << ((1000.0/ts.toUSecs())*1000000) <<  std::endl;

            delete _server;
            _server = 0;
        }

        void onArrayBenchmarkFinished(const Pt::Remoting::Result<std::vector<int> >& r)
        {
            //PT_UNIT_ASSERT_EQUALS(r.get().size(), 8);

            r.get();
            static unsigned benchmarkCounter = 0;

            if(++benchmarkCounter >= 1000)
                _loop->exit();

            benchmarkProc->begin(benchmarkVec, benchmarkVec);
        }

        std::vector<int> mergeVector(const std::vector<int>& a, const std::vector<int>& b)
        {
            benchmarkResult = a;
            std::vector<int>::const_iterator it;
            for(it = b.begin(); it != b.end(); ++it)
            {
                benchmarkResult.push_back(*it);
            }

            return benchmarkResult;
        }

        ////////////////////////////////////////////////////////////
        // EmptyArray
        //
        void EmptyArray()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("multiply", *this, &PtXmlRpcTest::multiplyVector);

            Pt::XmlRpc::HttpService httpService(service);

            Pt::Http::MapUrl servlet("/calc", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/calc");
            
            Pt::Remoting::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > multiply(client, "multiply");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onEmptyArrayFinished);

            std::vector<int> vec;
            multiply.begin(vec, vec);

            _loop->run();
        }

        void onEmptyArrayFinished(const Pt::Remoting::Result<std::vector<int> >& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get().size(), 0);

            _loop->exit();
        }

        ////////////////////////////////////////////////////////////
        // Struct
        //
        void Struct()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("multiply", *this, &PtXmlRpcTest::multiplyColor);

            Pt::XmlRpc::HttpService httpService(service);

            Pt::Http::MapUrl servlet("/calc", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/calc");
            
            Pt::Remoting::RemoteProcedure< Color, Color, Color > multiply(client, "multiply");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onStuctFinished);

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

        void onStuctFinished(const Pt::Remoting::Result<Color>& color)
        {
            PT_UNIT_ASSERT_EQUALS(color.get().red, 6);
            PT_UNIT_ASSERT_EQUALS(color.get().green, 12);
            PT_UNIT_ASSERT_EQUALS(color.get().blue, 20);

            _loop->exit();
        }

        Color multiplyColor(const Color& a, const Color& b)
        {
            Color color;
            color.red = a.red * b.red;
            color.green = a.green * b.green;
            color.blue = a.blue * b.blue;
            return color;
        }

        ////////////////////////////////////////////////////////////
        // Set
        //
        void Set()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("multiplyset", *this, &PtXmlRpcTest::multiplySet);

            Pt::XmlRpc::HttpService httpService(service);

            Pt::Http::MapUrl servlet("/test", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/test");
            
            Pt::Remoting::RemoteProcedure<IntSet, IntSet, int> multiply(client, "multiplyset");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onSetFinished);

            IntSet myset;
            myset.insert(4);
            myset.insert(5);
            myset.insert(11);
            myset.insert(5);

            multiply.begin(myset, 2);

            _loop->run();
        }

        void onSetFinished(const Pt::Remoting::Result<IntSet>& result)
        {
            const IntSet& v = result.get();
            PT_UNIT_ASSERT_EQUALS(v.size(), 3);
            PT_UNIT_ASSERT(v.find(8) != v.end());
            PT_UNIT_ASSERT(v.find(10) != v.end());
            PT_UNIT_ASSERT(v.find(22) != v.end());

            _loop->exit();
        }

        IntSet multiplySet(const IntSet& s, int f)
        {
            IntSet ret;
            for (IntSet::const_iterator it = s.begin(); it != s.end(); ++it)
                ret.insert(*it * f);
            return ret;
        }

        ////////////////////////////////////////////////////////////
        // Multiset
        //
        void Multiset()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("multiplyset", *this, &PtXmlRpcTest::multiplyMultiset);

            Pt::XmlRpc::HttpService httpService(service);

            Pt::Http::MapUrl servlet("/test", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/test");
            
            Pt::Remoting::RemoteProcedure<IntMultiset, IntMultiset, int> multiply(client, "multiplyset");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onMultisetFinished);

            IntMultiset myset;
            myset.insert(4);
            myset.insert(5);
            myset.insert(11);
            myset.insert(5);

            multiply.begin(myset, 2);

            _loop->run();
        }

        void onMultisetFinished(const Pt::Remoting::Result<IntMultiset>& result)
        {
            const IntMultiset& v = result.get();
            PT_UNIT_ASSERT_EQUALS(v.size(), 4);
            PT_UNIT_ASSERT_EQUALS(v.count(8), 1);
            PT_UNIT_ASSERT_EQUALS(v.count(10), 2);
            PT_UNIT_ASSERT_EQUALS(v.count(22), 1);

            _loop->exit();
        }

        IntMultiset multiplyMultiset(const IntMultiset& s, int f)
        {
            IntMultiset ret;
            for (IntMultiset::const_iterator it = s.begin(); it != s.end(); ++it)
                ret.insert(*it * f);
            return ret;
        }

        ////////////////////////////////////////////////////////////
        // Map
        //
        void Map()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("multiplymap", *this, &PtXmlRpcTest::multiplyMap);
            
            Pt::XmlRpc::HttpService httpService(service);

            Pt::Http::MapUrl servlet("/test", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/test");
            
            Pt::Remoting::RemoteProcedure<IntMap, IntMap, int> multiply(client, "multiplymap");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onMultiplyMapFinished);

            IntMap mymap;
            mymap[2] = 4;
            mymap[7] = 7;
            mymap[1] = -1;

            multiply.begin(mymap, 2);

            _loop->run();
        }

        void onMultiplyMapFinished(const Pt::Remoting::Result<IntMap>& result)
        {
            const IntMap& v = result.get();
            PT_UNIT_ASSERT_EQUALS(v.size(), 3);
            PT_UNIT_ASSERT(v.find(2) != v.end());
            PT_UNIT_ASSERT_EQUALS(v.find(2)->second, 8);
            PT_UNIT_ASSERT(v.find(7) != v.end());
            PT_UNIT_ASSERT_EQUALS(v.find(7)->second, 14);
            PT_UNIT_ASSERT(v.find(1) != v.end());
            PT_UNIT_ASSERT_EQUALS(v.find(1)->second, -2);

            _loop->exit();
        }

        IntMap multiplyMap(const IntMap& m, int f)
        {
            IntMap ret;
            for (IntMap::const_iterator it = m.begin(); it != m.end(); ++it)
            {
                ret[it->first] = it->second * f;
            }

            return ret;
        }

        ////////////////////////////////////////////////////////////
        // Multimap
        //
        void Multimap()
        {
            Pt::Remoting::ServiceDefinition service;
            service.registerProcedure("multiplymultimap", *this, &PtXmlRpcTest::multiplyMultimap);

            Pt::XmlRpc::HttpService httpService(service);

            Pt::Http::MapUrl servlet("/test", httpService);
            _server->addServlet(servlet);

            Pt::XmlRpc::HttpClient client(*_loop);
            Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8001);
            client.setTarget(ep, "/test");
            
            Pt::Remoting::RemoteProcedure<IntMultimap, IntMultimap, int> multiply(client, "multiplymultimap");
            multiply.finished() += Pt::slot(*this, &PtXmlRpcTest::onMultiplyMultimapFinished);

            IntMultimap mymap;
            mymap.insert(IntMultimap::value_type(2, 4));
            mymap.insert(IntMultimap::value_type(7, 7));
            mymap.insert(IntMultimap::value_type(7, 8));
            mymap.insert(IntMultimap::value_type(1, -1));

            multiply.begin(mymap, 2);

            _loop->run();
        }

        void onMultiplyMultimapFinished(const Pt::Remoting::Result<IntMultimap>& result)
        {
            const IntMultimap& v = result.get();
            PT_UNIT_ASSERT_EQUALS(v.size(), 4);
            PT_UNIT_ASSERT(v.lower_bound(2) != v.end());
            PT_UNIT_ASSERT_EQUALS(v.lower_bound(2)->second, 8);
            PT_UNIT_ASSERT(v.lower_bound(7) != v.end());
            PT_UNIT_ASSERT_EQUALS(v.lower_bound(7)->second, 14);
            IntMultimap::const_iterator it = v.lower_bound(7);
            ++it;
            PT_UNIT_ASSERT(it != v.end());
            PT_UNIT_ASSERT_EQUALS(it->first, 7);
            PT_UNIT_ASSERT_EQUALS(it->second, 16);
            PT_UNIT_ASSERT(v.lower_bound(1) != v.end());
            PT_UNIT_ASSERT_EQUALS(v.lower_bound(1)->second, -2);

            _loop->exit();
        }

        IntMultimap multiplyMultimap(const IntMultimap& m, int f)
        {
            IntMultimap ret;
            for (IntMultimap::const_iterator it = m.begin(); it != m.end(); ++it)
            {
                ret.insert(IntMultimap::value_type(it->first, it->second * f));
            }

            return ret;
        }

};

Pt::Unit::RegisterTest<PtXmlRpcTest> register_PtXmlRpcTest;
