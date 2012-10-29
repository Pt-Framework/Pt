/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <iostream>
#include <Pt/Arg.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Http/Server.h>
#include <Pt/Http/Servlet.h>
#include <Pt/XmlRpc/Service.h>
#include <Pt/Main.h>

class EchoService;

class AsyncEcho : public Pt::XmlRpc::AsyncServiceProcedure<std::string, std::string>
{
    public:   
        AsyncEcho(Pt::SerializationContext& ctx, EchoService& srv)
        : Pt::XmlRpc::AsyncServiceProcedure<std::string, std::string>(ctx)
        , _srv(&srv)
        {}

    protected:
        virtual void onBeginCall(const std::string& msg)
        {
            std::cerr << "beginEcho " << this << std::endl;
            this->loop();
            this->setResult(msg);
        }

    private:
        EchoService* _srv;
};


class AsyncEcho2 : public Pt::XmlRpc::AsyncCall<std::string>
{
    public:   
        AsyncEcho2(Pt::XmlRpc::AsyncResult& result, EchoService& srv, const std::string& msg)
        : Pt::XmlRpc::AsyncCall<std::string>(result)
        , _srv(&srv)
        {}

        virtual void beginCall()
        {
            std::cerr << "beginEcho " << this << std::endl;
        }

    private:
        EchoService* _srv;
};


class EchoService : public Pt::XmlRpc::Service
{
    public:
        EchoService()
        {
            registerProcedure("echo", new Pt::XmlRpc::AsyncDef<EchoService, AsyncEcho>(*this));

            /// registerAsync("echo", *this, &EchoService::beginEcho);
            registerMethod("echo2", *this, &EchoService::echo);
        }

        std::string echo(const std::string& msg)
        {
            return msg;
        }

        Pt::XmlRpc::AsyncCall<std::string>* beginEcho(Pt::XmlRpc::AsyncResult& result, const std::string& msg)
        {
            return new AsyncEcho2(result, *this, msg);
        }
};


int main(int argc, char* argv[])
{
  try
  {
    Pt::Arg<std::string> ip(argc, argv, 'i');
    Pt::Arg<unsigned short> port(argc, argv, 'p', 7002);
    Pt::Arg<unsigned> threads(argc, argv, 't', 4);

    std::cout << "rpc echo server running on port " << port.getValue() << "\n"
                 "using " << threads.getValue() << " thread(s)\n\n"
                 "options:\n\n"
                 "   -i ip      set listen address (default: all interfaces)\n"
                 "   -p number  set listen port (default: 7002)\n"
                 "   -t number  set number of server threads (default: 4)\n"
              << std::endl;

    Pt::System::MainLoop loop;
    
    EchoService service;
    //service.registerFunction("echo", echo);
    
    Pt::Http::Server server(loop);

    Pt::Http::Server::Options options;
    options.setMaxThreads(threads);
    server.listen(ip, port, options);
    
    Pt::Http::MapUrl servlet("/myservice", service);
    server.addServlet(servlet);

    //loop.setIdleTimeout(5000);
    //loop.timeout() += Pt::slot(loop, &Pt::System::EventLoop::exit);
    loop.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

