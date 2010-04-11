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
#include "Pt/XmlRpc/HttpClient.h"
#include "Pt/XmlRpc/Fault.h"
#include "Pt/XmlRpc/RemoteProcedure.h"
#include "Pt/Http/Server.h"
#include "Pt/System/EventLoop.h"
#include "Pt/System/Clock.h"

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
        unsigned _count;

    public:
        PtXmlRpcTest()
        : Pt::Unit::TestSuite("Pt-XmlRpc-Test")
        {
            registerMethod("Fault", *this, &PtXmlRpcTest::Fault);
            registerMethod("Exception", *this, &PtXmlRpcTest::Exception);
            registerMethod("CallbackException", *this, &PtXmlRpcTest::CallbackException);
            registerMethod("ConnectError", *this, &PtXmlRpcTest::ConnectError);
            registerMethod("Nothing", *this, &PtXmlRpcTest::Nothing);
            registerMethod("Boolean", *this, &PtXmlRpcTest::Boolean);
            registerMethod("Integer", *this, &PtXmlRpcTest::Integer);
            registerMethod("Double", *this, &PtXmlRpcTest::Double);
            registerMethod("String", *this, &PtXmlRpcTest::String);
            registerMethod("EmptyValues", *this, &PtXmlRpcTest::EmptyValues);
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
            _loop = new Pt::System::EventLoop();
            _loop->setIdleTimeout(2000);
            connect(_loop->timeout, *this, &PtXmlRpcTest::failTest);
            connect(_loop->timeout, *_loop, &Pt::System::EventLoop::exit);

            _server = new Pt::Http::Server(*_loop, "127.0.0.1", 8001);
        }

        void tearDown()
        {
            delete _loop;
            delete _server;
        }

        ////////////////////////////////////////////////////////////
        // Fault
        //
        void Fault()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::throwFault);
            _server->addService("/calc", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/calc");
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

        bool throwFault()
        {
            throw Pt::XmlRpc::Fault("Fault", 7);
            return false;
        }

        ////////////////////////////////////////////////////////////
        // Exception
        //
        void Exception()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::throwException);
            _server->addService("/calc", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<bool> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onException );
            multiply.begin();

            _loop->run();
        }

        void onException(const Pt::XmlRpc::Result<bool>& result)
        {
            try
            {
                //bool v =
                    result.get();
                PT_UNIT_ASSERT(false);
            }
            catch (const Pt::XmlRpc::Fault& e)
            {
                PT_UNIT_ASSERT_EQUALS(e.rc(), 0)
                PT_UNIT_ASSERT_EQUALS(e.text(), "Exception")
            }

            _loop->exit();
        }

        bool throwException()
        {
            throw std::runtime_error("Exception");
            return false;
        }

        ////////////////////////////////////////////////////////////
        // Nothing
        //
        void Nothing()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyNothing);
            _server->addService("/calc", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/calc");
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

        bool multiplyNothing()
        {
            return false;
        }

        ////////////////////////////////////////////////////////////
        // CallbackException
        //
        void CallbackException()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyNothing);
            _server->addService("/calc", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/calc");
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

        ////////////////////////////////////////////////////////////
        // ConnectError
        //
        void ConnectError()
        {
            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8002, "/calc");
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

        ////////////////////////////////////////////////////////////
        // Boolean
        //
        void Boolean()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyBoolean);
            _server->addService("/calc", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/calc");
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

        bool multiplyBoolean(bool a, bool b)
        {
            PT_UNIT_ASSERT_EQUALS(a, true)
            PT_UNIT_ASSERT_EQUALS(b, true)
            return true;
        }

        ////////////////////////////////////////////////////////////
        // Integer
        //

        Pt::XmlRpc::RemoteProcedure<int, int, int>* _multiply;

        void Integer()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyInt);
            _server->addService("/calc", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure<int, int, int> multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onIntegerFinished );

            multiply.begin(2, 3);
            _multiply= &multiply;

            _loop->run();
        }

        void onIntegerFinished(const Pt::XmlRpc::Result<int>& r)
        {
            static int ttt = 0;
            PT_UNIT_ASSERT_EQUALS(r.get(), 6)

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
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyDouble);
            _server->addService("/calc", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/calc");
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

        double multiplyDouble(double a, double b)
        {
            return a*b;
        }

        ////////////////////////////////////////////////////////////
        // String
        //
        void String()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("echoString", *this, &PtXmlRpcTest::echoString);
            _server->addService("/foo", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/foo");
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

        std::string echoString(std::string a)
        {
            return a;
        }

        ////////////////////////////////////////////////////////////
        // EmptyValues
        //
        void EmptyValues()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyEmpty);
            _server->addService("/calc", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/calc");
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

        std::string multiplyEmpty(std::string a, std::string b)
        {
            PT_UNIT_ASSERT_EQUALS(a, "")
            PT_UNIT_ASSERT_EQUALS(b, "")
            return "4";
        }

        ////////////////////////////////////////////////////////////
        // Array
        //
        void Array()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyVector);
            _server->addService("/calc", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > multiply(client, "multiply");
            connect( multiply.finished, *this, &PtXmlRpcTest::onArrayFinished );

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

        void onArrayFinished(const Pt::XmlRpc::Result<std::vector<int> >& r)
        {
            PT_UNIT_ASSERT_EQUALS(r.get().size(), 2)
            PT_UNIT_ASSERT_EQUALS(r.get().at(0), 100)
            PT_UNIT_ASSERT_EQUALS(r.get().at(1), 400)

            _loop->exit();
        }

        ////////////////////////////////////////////////////////////
        // ArrayBenchmark
        //
        Pt::XmlRpc::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> >* benchmarkProc;
        std::vector<int> benchmarkVec;
        std::vector<int> benchmarkResult;

        void ArrayBenchmark()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("mergeVector", *this, &PtXmlRpcTest::mergeVector);
            _server->addService("/calc", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/calc");
            Pt::XmlRpc::RemoteProcedure< std::vector<int>, std::vector<int>, std::vector<int> > proc(client, "mergeVector");
            connect( proc.finished, *this, &PtXmlRpcTest::onArrayBenchmarkFinished );

            benchmarkProc = &proc;

            benchmarkVec.push_back(1);
            benchmarkVec.push_back(2);
            benchmarkVec.push_back(3);
            benchmarkVec.push_back(4);

            proc.begin(benchmarkVec, benchmarkVec);

            Pt::System::Clock clock;
            clock.start();
            _loop->run();
            Pt::Timespan ts = clock.stop();
            std::cerr << "Time   : " << ts.toUSecs() <<  std::endl;
            std::cerr << "Req/Sec: " << ((10000.0/ts.toUSecs())*1000000) <<  std::endl;
        }

        void onArrayBenchmarkFinished(const Pt::XmlRpc::Result<std::vector<int> >& r)
        {
            //PT_UNIT_ASSERT_EQUALS(r.get().size(), 8)

            r.get();
            static unsigned benchmarkCounter = 0;

            if(++benchmarkCounter >= 10000)
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
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyVector);
            _server->addService("/calc", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/calc");
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

        ////////////////////////////////////////////////////////////
        // Struct
        //
        void Struct()
        {
            Pt::XmlRpc::Service service;
            service.registerMethod("multiply", *this, &PtXmlRpcTest::multiplyColor);
            _server->addService("/calc", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/calc");
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
            Pt::XmlRpc::Service service;
            service.registerMethod("multiplyset", *this, &PtXmlRpcTest::multiplySet);
            _server->addService("/test", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/test");
            Pt::XmlRpc::RemoteProcedure<IntSet, IntSet, int> multiply(client, "multiplyset");
            connect( multiply.finished, *this, &PtXmlRpcTest::onSetFinished );

            IntSet myset;
            myset.insert(4);
            myset.insert(5);
            myset.insert(11);
            myset.insert(5);

            multiply.begin(myset, 2);

            _loop->run();
        }

        void onSetFinished(const Pt::XmlRpc::Result<IntSet>& result)
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
            Pt::XmlRpc::Service service;
            service.registerMethod("multiplyset", *this, &PtXmlRpcTest::multiplyMultiset);
            _server->addService("/test", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/test");
            Pt::XmlRpc::RemoteProcedure<IntMultiset, IntMultiset, int> multiply(client, "multiplyset");
            connect( multiply.finished, *this, &PtXmlRpcTest::onMultisetFinished );

            IntMultiset myset;
            myset.insert(4);
            myset.insert(5);
            myset.insert(11);
            myset.insert(5);

            multiply.begin(myset, 2);

            _loop->run();
        }

        void onMultisetFinished(const Pt::XmlRpc::Result<IntMultiset>& result)
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
            Pt::XmlRpc::Service service;
            service.registerMethod("multiplymap", *this, &PtXmlRpcTest::multiplyMap);

            _server->addService("/test", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/test");
            Pt::XmlRpc::RemoteProcedure<IntMap, IntMap, int> multiply(client, "multiplymap");
            connect( multiply.finished, *this, &PtXmlRpcTest::onMultiplyMapFinished );

            IntMap mymap;
            mymap[2] = 4;
            mymap[7] = 7;
            mymap[1] = -1;

            multiply.begin(mymap, 2);

            _loop->run();
        }

        void onMultiplyMapFinished(const Pt::XmlRpc::Result<IntMap>& result)
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
            Pt::XmlRpc::Service service;
            service.registerMethod("multiplymultimap", *this, &PtXmlRpcTest::multiplyMultimap);

            _server->addService("/test", service);

            Pt::XmlRpc::HttpClient client(*_loop, "127.0.0.1", 8001, "/test");
            Pt::XmlRpc::RemoteProcedure<IntMultimap, IntMultimap, int> multiply(client, "multiplymultimap");
            connect( multiply.finished, *this, &PtXmlRpcTest::onMultiplyMultimapFinished );

            IntMultimap mymap;
            mymap.insert(IntMultimap::value_type(2, 4));
            mymap.insert(IntMultimap::value_type(7, 7));
            mymap.insert(IntMultimap::value_type(7, 8));
            mymap.insert(IntMultimap::value_type(1, -1));

            multiply.begin(mymap, 2);

            _loop->run();
        }

        void onMultiplyMultimapFinished(const Pt::XmlRpc::Result<IntMultimap>& result)
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
