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

#include <Pt/XmlRpc/RemoteProcedure.h>
#include <Pt/XmlRpc/HttpClient.h>
#include <Pt/Net/Endpoint.h>
#include <Pt/System/Application.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Clock.h>
#include <Pt/System/Logger.h>
#include <Pt/Timespan.h>
#include <Pt/Atomicity.h>
#include <Pt/Main.h>
#include <Pt/Arg.h>
#include <iostream>

#ifndef WINCE
#include <signal.h>
#endif

#ifndef SIGPIPE
#define SIGPIPE -1
#endif

class BenchClient
{
  public:
    explicit BenchClient(const Pt::Net::Endpoint& ep)
      : client(),
        echo(client, "echo"),
        thread()
    {  
        thread.init( Pt::callable(*this, &BenchClient::exec) );
        client.setTarget(ep, "/myservice");
    }

    static unsigned numRequests()
    { return _numRequests; }

    static void numRequests(unsigned n)
    { _numRequests = n; }

    static unsigned requestsStarted()
    { return static_cast<unsigned>(Pt::atomicGet(_requestsStarted)); }

    static unsigned requestsFinished()
    { return static_cast<unsigned>(Pt::atomicGet(_requestsFinished)); }

    static unsigned requestsFailed()
    { return static_cast<unsigned>(Pt::atomicGet(_requestsFailed)); }

    void start()
    { thread.start(); }

    void join()
    { thread.join(); }

  private:
    void exec()
    {
        while (static_cast<unsigned>(Pt::atomicIncrement(_requestsStarted)) <= _numRequests)
        {
            try
            {
                std::cout << echo("hi") << " ";
                Pt::atomicIncrement(_requestsFinished);
            }
            catch(const std::exception& e)
            {
                {
                    Pt::System::MutexLock lock(mutex);
                    std::cerr << "request failed: " << e.what() << std::endl;
                    client.cancel();
                }

                Pt::atomicIncrement(_requestsFailed);
            }
        }
    }

    Pt::XmlRpc::HttpClient client;
    Pt::XmlRpc::RemoteProcedure<std::string, std::string> echo;
    Pt::System::AttachedThread thread;

    static unsigned _numRequests;
    static Pt::atomic_t _requestsStarted;
    static Pt::atomic_t _requestsFinished;
    static Pt::atomic_t _requestsFailed;
    static Pt::System::Mutex mutex;
};

Pt::atomic_t BenchClient::_requestsStarted(0);

Pt::atomic_t BenchClient::_requestsFinished(0);

Pt::atomic_t BenchClient::_requestsFailed(0);

unsigned BenchClient::_numRequests = 0;

Pt::System::Mutex BenchClient::mutex;


int main(int argc, char* argv[])
{
  typedef std::vector<BenchClient*> BenchClients;

  BenchClients clients;

  try
  {
    Pt::System::Application app;
    app.ignoreSystemSignal(SIGPIPE);

    Pt::System::Logger::setLogLevel("", Pt::System::Error);

    Pt::Arg<std::string> ip(argc, argv, 'i');
    Pt::Arg<unsigned short> port(argc, argv, 'p', 7002);
    Pt::Arg<unsigned> threads(argc, argv, 't', 4);
    BenchClient::numRequests( Pt::Arg<unsigned>(argc, argv, 'n', 10000).get() );

    std::cout << "call " << BenchClient::numRequests() << " requests with " 
              << threads.get() << " threads\n\n"
                 "options:\n"
                 "   -i ip      set ip address of server (default: IP4 loopback)\n"
                 "   -p number  set port number of server (default: 7002)\n"
                 "   -t number  set number of threads (default: 4)\n"
                 "   -n number  set number of requests (default: 10000)\n"
              << std::endl;

    Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Loopback( port.get() );
    
    if( ip.isSet() )
        ep = Pt::Net::Endpoint( ip.get(), port.get() );

    while (clients.size() < threads.get())
      clients.push_back( new BenchClient(ep) );

    Pt::System::Clock cl;
    cl.start();

    for (BenchClients::iterator it = clients.begin(); it != clients.end(); ++it)
      (*it)->start();

    for (BenchClients::iterator it = clients.begin(); it != clients.end(); ++it)
      (*it)->join();

    Pt::Timespan t = cl.stop();

    std::cout << "--- DONE ---" << std::endl;
    std::cout << BenchClient::numRequests() << " requests in "
              << t.toMSecs()/1e3 << " s => " 
              << BenchClient::requestsStarted() / t.toSeconds() << "#/s\n"
              << BenchClient::requestsFinished() << " finished " 
              << BenchClient::requestsFailed() << " failed" << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }

  for (BenchClients::iterator it = clients.begin(); it != clients.end(); ++it)
    delete *it;
}

