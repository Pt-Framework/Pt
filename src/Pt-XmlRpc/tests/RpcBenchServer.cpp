/*
 * Copyright (C) 2011 Tommi Maekitalo
 * Copyright (C) 2013 by Marc Duerner
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

#include <Pt/XmlRpc/HttpService.h>
#include <Pt/Http/Server.h>
#include <Pt/Http/Servlet.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Arg.h>
#include <Pt/Main.h>
#include <iostream>

class EchoService;

class AsyncEcho : public Pt::XmlRpc::ActiveProcedure<std::string, std::string>
                , public Pt::Connectable
{
    public:   
        AsyncEcho(Pt::XmlRpc::Responder& resp, EchoService& srv)
        : Pt::XmlRpc::ActiveProcedure<std::string, std::string>(resp)
        , _srv(&srv)
        {
            _timer.timeout() += Pt::slot(*this, &AsyncEcho::onTimeout);
        }

    protected:
        virtual void onInvoke(Pt::System::EventLoop& loop, const std::string& msg)
        {
            _r = msg;
            _timer.setActive(loop);
            _timer.start(1000);         
        }

        virtual const std::string& onResult()
        {
            std::cerr << " " << _r << " ";
            return _r;
        }

        void onTimeout()
        {
            //_timer.stop();
            //_timer.detach();
            this->setReady();
        }

    private:
        std::string _r;
        Pt::System::Timer _timer;
        EchoService* _srv;
};


class AsyncHello : public Pt::XmlRpc::ActiveProcedure<std::string>
                 , public Pt::Connectable
{
    public:   
        AsyncHello(Pt::XmlRpc::Responder& resp, const char* helloTxt)
        : Pt::XmlRpc::ActiveProcedure<std::string>(resp)
        , _hello(helloTxt)
        {
            _timer.timeout() += Pt::slot(*this, &AsyncHello::setReady);
        }

    protected:
        virtual void onInvoke(Pt::System::EventLoop& loop)
        {
            _timer.setActive(loop);
            _timer.start(1000);         
        }

        virtual const std::string& onResult()
        {
            return _hello;
        }

    private:
        std::string _hello;
        Pt::System::Timer _timer;
};


class EchoService : public Pt::XmlRpc::ServiceDefinition
{
    public:
        EchoService()
        {
            registerProcedure("echo2", *this, &EchoService::echo);
            
            registerActiveProcedure("echo", *this, &EchoService::asyncEcho);
            registerActiveProcedure("hello", *this, &EchoService::asyncHello);
        }

        std::string echo(const std::string& msg)
        {
            return msg;
        }

        AsyncEcho* asyncEcho(Pt::XmlRpc::Responder& r)
        {
            return new AsyncEcho(r, *this);
        }

        AsyncHello* asyncHello(Pt::XmlRpc::Responder& r)
        {
            return new AsyncHello(r, "Hello world!");
        }
};


int main(int argc, char* argv[])
{
  try
  {
    Pt::Arg<std::string> ip(argc, argv, 'i');
    Pt::Arg<unsigned short> port(argc, argv, 'p', 7002);
    Pt::Arg<unsigned> threads(argc, argv, 't', 4);

    std::cout << "rpc echo server running on port " << port.get() << "\n"
                 "using " << threads.get() << " thread(s)\n\n"
                 "options:\n\n"
                 "   -i ip      set listen address (default: all interfaces)\n"
                 "   -p number  set listen port (default: 7002)\n"
                 "   -t number  set number of server threads (default: 4)\n"
              << std::endl;

    Pt::System::MainLoop loop;
    Pt::Http::Server server(loop);

    Pt::Http::Server::Options options;
    options.setMaxThreads( threads.get() );
    server.listen(ip.get(), port.get(), options);
    
    EchoService service;

    Pt::XmlRpc::HttpService httpService(service);
    Pt::Http::MapUrl servlet("/myservice", httpService);
    server.addServlet(servlet);

    loop.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

