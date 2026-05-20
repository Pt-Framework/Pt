/*
 * Copyright (C) 2020-2026 by Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <Pt/Unit/TestMain.h>
#include <Pt/JsonRpc/HttpService.h>
#include <Pt/JsonRpc/HttpClient.h>
#include <Pt/JsonRpc/ServiceDeclaration.h>
#include <Pt/JsonRpc/Fault.h>
#include <Pt/Remoting/ServiceDefinition.h>
#include <Pt/Remoting/RemoteProcedure.h>
#include <Pt/Http/Server.h>
#include <Pt/Http/Servlet.h>
#include <Pt/Net/Endpoint.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/Logger.h>


namespace Pt {

namespace JsonRpc {

class HttpTest : public Pt::Unit::TestSuite
               , public Pt::Connectable
{
  private:
    Pt::System::Timer _exitTimer;
    Pt::System::MainLoop* _loop;
    Pt::Http::Server* _server;

  public:
    HttpTest()
    : Pt::Unit::TestSuite("Pt::JsonRpc::HttpTest")
    {
        Pt::System::Logger::setLogLevel("Pt.JsonRpc", Pt::System::Error);

        registerMethod("Multiply", *this, &HttpTest::Multiply);
    }

    void failTest()
    {
        throw Pt::Unit::Assertion("test timed out", PT_SOURCEINFO);
    }

    void setUp()
    {
        _loop = new Pt::System::MainLoop();

        _exitTimer.setActive(*_loop);
        _exitTimer.start(10000);
        _exitTimer.timeout() += Pt::slot(*this, &HttpTest::failTest);
        _exitTimer.timeout() += Pt::slot(*_loop, &Pt::System::MainLoop::exit);

        Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Any(8077);
        _server = new Pt::Http::Server(*_loop, ep);
    }

    void tearDown()
    {
        delete _server;
        _server = 0;
        delete _loop;
        _loop = 0;
    }

    ////////////////////////////////////////////////////////////
    // Multiply
    //
    void Multiply()
    {
        Pt::Remoting::ServiceDefinition remotingService;
        remotingService.registerProcedure("multiply", *this, &HttpTest::multiplyInt);

        Pt::JsonRpc::ServiceDeclaration decl;
        Pt::JsonRpc::HttpService httpService(decl, remotingService);

        Pt::Http::MapUrl servlet("/jsonrpc", httpService);
        _server->addServlet(servlet);

        Pt::JsonRpc::HttpClient client(*_loop);
        Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback(8077);
        client.setTarget(ep, "/jsonrpc");

        Pt::Remoting::RemoteProcedure<int, int, int> multiply(client, "multiply");
        multiply.finished() += Pt::slot(*this, &HttpTest::onMultiplyFinished);

        multiply.begin(6, 7);

        _loop->run();

        delete _server;
        _server = 0;
    }

    void onMultiplyFinished(const Pt::Remoting::Result<int>& r)
    {
        PT_UNIT_ASSERT_EQUALS(r.get(), 42);
        _loop->exit();
    }

    int multiplyInt(int a, int b)
    {
        return a * b;
    }
};

Pt::Unit::RegisterTest<HttpTest> register_HttpTest;

} // namespace JsonRpc

} // namespace Pt
